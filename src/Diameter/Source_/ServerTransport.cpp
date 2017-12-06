#include "ServerTransport.h"

#include <new>
#include <arpa/inet.h>
#include <Logger/logger.h>

/*/
#if !defined SOCKET_ERROR && !defined _WINDOWS && !defined _WIN32 && !defined WIN32
	#define SOCKET_ERROR    -1
#endif

#if !defined INVALID_SOCKET && !defined _WINDOWS && !defined _WIN32 && !defined WIN32
	#define INVALID_SOCKET  -1
#endif

#ifndef SOL_TCP
	#define SOL_TCP IPPROTO_TCP
#endif
//*/

namespace CoreDiam {

//----------------------------------------------------------------------------
// ClientSocket
//----------------------------------------------------------------------------
/*/
class TransportClient
{
private:
	int                       id_;
	DiamSocket::SocketHandle  handle_;
	CDiameterFormer          *former_;
};
//*/

//----------------------------------------------------------------------------
// CServerTransport
//----------------------------------------------------------------------------
CServerTransport::CServerTransport(ITransportEvents *_pManager, int _QueueSize, int _rcvBufferSize, int _sndBufferSize, bool _isBlocking):
	m_ListenHandle(DiamSocket::DS_INVALID_SOCKET),
	m_pManager(_pManager),
	m_bExit(false),
	m_Id(0),
	m_unsentPacketsCount(0),
	m_havingWaiters(false),
	m_maxQueueSize(_QueueSize),
	m_rcvBufferSize(_rcvBufferSize),
	m_sndBufferSize(_sndBufferSize),
	m_isBlocking(_isBlocking),
	m_rcvBuffer(0)
{

	//ACE::init();

	m_rcvBuffer   = new char[m_rcvBufferSize];
	m_pSendThread = new CSendThread(this);
}
//----------------------------------------------------------------------------
CServerTransport::~CServerTransport()
{

	m_pSendThread->interrupt();
	m_SendQueueEvent.signal();
	delete m_pSendThread;
	delete[] m_rcvBuffer;
	Clear();
}
//----------------------------------------------------------------------------
int CServerTransport::Start(const char* _Host, unsigned short _Port, int _MaxCount, ITransportEvents *_Events)
{
	m_Host.assign(_Host);
	m_Port = _Port;
	m_bExit = false;
	activate();
	m_StartEvent.wait();
	if(m_ListenHandle != DiamSocket::DS_INVALID_SOCKET)
		return 0;
	return -1;
}
//----------------------------------------------------------------------------
void CServerTransport::Stop()
{
	m_bExit = true;
	wait();
}
//----------------------------------------------------------------------------
void CServerTransport::Release()
{
	delete this;
}
//----------------------------------------------------------------------------
inline int CServerTransport::FillConnectedSet(fd_set &_Set, int *_MaxValue)
{

	int Count = 0;
	m_Mutex.lock();

	MAP_INT::iterator begin = m_mapHandleId.begin(), end = m_mapHandleId.end();
	for (; begin != end; begin++, Count++)
	{
		FD_SET(begin->first, &_Set);
		if(_MaxValue)
		{
			if(begin->first > *_MaxValue)
				*_MaxValue = begin->first;
		}
	}
	m_Mutex.unlock();
	return Count;
}
//----------------------------------------------------------------------------
SendResult CServerTransport::Send(int _ClientID, const char *_sid, size_t _sidLen, const unsigned char *_Data, unsigned int _Len, int &_ErrNo)
{

	_ErrNo = 0;
	int Handle = -1;

	m_Mutex.lock();

	MAP_INT::iterator it = m_mapIdHandle.find(_ClientID);
	if(it == m_mapIdHandle.end())
	{
		m_Mutex.unlock();
		return UNKNOWN_CONNECTION;
	}

	Handle = it->second;

	m_Mutex.unlock();

	m_PacketsMutex.lock();

	MAP_INTPACKETS::iterator itp = m_mapHandleToPackets.find(Handle);
	if(itp == m_mapHandleToPackets.end())
	{
		m_PacketsMutex.unlock();
		//m_Mutex.release();
		return UNKNOWN_CONNECTION;
	}

	LST_PACKET &LstPack = itp->second;

	if(LstPack.size() < m_maxQueueSize)
	{
		CPacket *packet = new(std::nothrow) CPacket(_Data, _Len, _sid, _sidLen, _ClientID);
		if(NULL == packet)
		{
			m_PacketsMutex.unlock();
			return QUEUE_ERROR;
		}

		if(!packet->Data() || !packet->Size())
		{
			delete packet;
			m_PacketsMutex.unlock();
			return QUEUE_ERROR;
		}

		LstPack.push_back(packet);
		size_t count = ++m_unsentPacketsCount;
		_ErrNo = (int)LstPack.size();

		bool havingWaiters = m_havingWaiters;
		if(m_havingWaiters)
			m_havingWaiters = false;

		m_PacketsMutex.unlock();
		//m_Mutex.release();

		if(1 == count)
		{
			//m_pSendThread->signal();
			if(havingWaiters)
				m_SendQueueEvent.signal();
		}

		return PUT_TO_QUEUE;
	}

	m_PacketsMutex.unlock();
	//m_Mutex.release();
	return QUEUE_OVERFLOW;

	/*/
	if (LstPack.size() > 0)
	{
		if ( (LstPack.size() < m_QueueSize))
		{
			LstPack.push_back(new CPacket(_Data, _Len));
			_ErrNo = LstPack.size();

			m_PacketsMutex.release();
			m_Mutex.release();
			return PUT_TO_QUEUE;
		}

		m_PacketsMutex.release();
			m_Mutex.release();
			return QUEUE_OVERFLOW;
		}

	int Bytes = send(Handle, (const char*)_Data, _Len, 0);
	if (Bytes == SOCKET_ERROR)
	{
		Bytes = ClientSocket::getLastError();
		if (Bytes == EWOULDBLOCK || Bytes == ENOMEM || Bytes == ENOBUFS)
			Bytes = 0;
		else
		{
			m_Mutex.release();
			_ErrNo = Bytes;
			return SEND_ERROR;
		}
	}
	if (Bytes != _Len)
	{
		LstPack.push_back(new CPacket(_Data + Bytes, _Len - Bytes));
		//m_pSendThread->AddClient(Handle);
		_ErrNo = LstPack.size();
		m_Mutex.release();
		return PUT_TO_QUEUE;
	}
	m_Mutex.release();
	return OK;
	//*/
}
//----------------------------------------------------------------------------
size_t CServerTransport::SendAsync()
{

	m_PacketsMutex.lock();

	m_PacketsMutex.unlock();

	return 0;
}
//----------------------------------------------------------------------------
size_t CServerTransport::SendFromQueue(int _socketHandle)
{

	m_PacketsMutex.lock();

	//m_Mutex.acquire();

	MAP_INTPACKETS::iterator it = m_mapHandleToPackets.find(_socketHandle);
	if(it == m_mapHandleToPackets.end())
	{
		//m_Mutex.release();
		m_PacketsMutex.unlock();
		return 0;
	}

	LST_PACKET &LstPack = it->second;

	//m_Mutex.release();
	size_t packetsCount = LstPack.size();

	if(packetsCount == 0)
	{
		//m_Mutex.release();
		m_PacketsMutex.unlock();
		return 0;
	}

	CPacket *Pack = LstPack.front();

	if(!Pack->Data() || !Pack->Size())
	{
		LstPack.pop_front();
		delete Pack;

		packetsCount--;
		m_unsentPacketsCount--;

		m_PacketsMutex.unlock();
		return packetsCount;
	}

	int Bytes = DiamSocket::send(_socketHandle, (const char *)Pack->Data(), Pack->Size());

	if(Bytes == DiamSocket::DS_SOCKET_ERROR)
	{
		Bytes = DiamSocket::getLastError();
		if(Bytes == EWOULDBLOCK || Bytes == ENOMEM || Bytes == ENOBUFS)
		{
			// no error - just need to wait a while
			//Bytes = 0;
			//m_pSendThread->signal();
		}
		else
		{
			// we've got the error
			//m_Mutex.release();

			//m_PacketsMutex.release();
			//return;
		}

		m_pSendThread->signal();
		m_PacketsMutex.unlock();
		return packetsCount;
	}
	/*/
if(Bytes <= 0)
{
	//m_Mutex.release();
	m_PacketsMutex.release();
	return;
}
//*/

	if(Bytes == (int)Pack->Size())
	{
		LstPack.pop_front();

		packetsCount--;
		m_unsentPacketsCount--;

		m_PacketsMutex.unlock();

		m_pManager->OnSent(Pack->clientId(), Pack->sid(), Pack->sidLength(), static_cast<size_t>(Bytes));
		delete Pack;

		return packetsCount;
	}

	//LstPack.push_front(new CPacket(Pack->Data() + Bytes, Pack->Size() - Bytes));
	(*Pack) += Bytes;
	m_pSendThread->signal();

	if(LstPack.size() == 0)
	{
		//m_pSendThread->RemoveClient(_socketHandle);
		//m_pManager->OnEmptyQueue(_socketHandle);
	}

	//m_Mutex.release();
	m_PacketsMutex.unlock();
	//delete Pack;

	return packetsCount;
}
//----------------------------------------------------------------------------
size_t CServerTransport::GetQueueSize(int _socketHandle)
{

	m_PacketsMutex.lock();

	MAP_INTPACKETS::iterator it = m_mapHandleToPackets.find(_socketHandle);
	if(it == m_mapHandleToPackets.end())
	{
		m_PacketsMutex.unlock();
		return 0;
	}

	LST_PACKET &LstPack = it->second;
	size_t result = LstPack.size();

	m_PacketsMutex.unlock();
	return result;
}
//----------------------------------------------------------------------------
size_t CServerTransport::GetUnsentPacketsCount()
{

	m_PacketsMutex.lock();
	size_t result = m_unsentPacketsCount;
	m_PacketsMutex.unlock();
	return result;
}
//----------------------------------------------------------------------------
bool CServerTransport::WaitForSendQueue()
{

	m_PacketsMutex.lock();
	size_t count = m_unsentPacketsCount;
	m_PacketsMutex.unlock();

	if(!count)
	{
		for(int i = 8; i && !count; i--)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(20));
			m_PacketsMutex.lock();
			count = m_unsentPacketsCount;
			m_havingWaiters = !count;
			m_PacketsMutex.unlock();
		}
	}

	if(count)
		return true;

	/*m_PacketsMutex.acquire();
	count = m_unsentPacketsCount;
	m_havingWaiters = !count;
	m_PacketsMutex.release();

	if(count)
		return true; */
	m_SendQueueEvent.wait();
	return true;
}
//----------------------------------------------------------------------------
bool CServerTransport::StartListen()
{

	m_ListenHandle = -1;

	sockaddr_in Addr;
	Addr.sin_family = AF_INET;
	Addr.sin_port = htons(m_Port);
	Addr.sin_addr.s_addr = inet_addr(m_Host.c_str());

	if (Addr.sin_addr.s_addr == INADDR_NONE)
		return false;

	m_ListenHandle = socket(AF_INET, SOCK_STREAM, 0);
	if(m_ListenHandle == -1)
		return false;

	//#if defined(SO_REUSEPORT) && (!NO_SO_REUSEPORT)
	//int option = SO_REUSEADDR | SO_REUSEPORT;
	//#else
	//int option = SO_REUSEADDR;
	//#endif
	//
	//int ReuseAddress = 1;
	//int res = ::setsockopt(m_ListenHandle, SOL_SOCKET, option, (const char*)&ReuseAddress, sizeof(ReuseAddress));

	int soEnable  = 1;
	int res;

	res = ::setsockopt(m_ListenHandle, SOL_SOCKET, SO_REUSEADDR, (const char *)&soEnable, sizeof(soEnable));
	if(0 != res)
        {
	}

	#if defined(SO_REUSEPORT)  && (!NO_SO_REUSEPORT)
	res = ::setsockopt(m_ListenHandle, SOL_SOCKET, SO_REUSEPORT, (const char *)&soEnable, sizeof(soEnable));
	if(0 != res)
        {
	}
	#endif
	
	//int Len = sizeof(Addr);
	res = ::bind(m_ListenHandle, (sockaddr *)&Addr, sizeof(Addr));
	if(res == 0)
	{
		res = ::listen(m_ListenHandle, 100);
		if(res == 0)
			return true;
		else
                {
		}
	}
	else
        {
	}

	DiamSocket::close(m_ListenHandle);
	m_ListenHandle = DiamSocket::DS_INVALID_SOCKET;
	return false;
}
//----------------------------------------------------------------------------
inline void CServerTransport::Clear()
{

	m_mapIdHandle.clear();
	m_mapHandleId.clear();
	MAP_FORMER::iterator begin = m_mapFormer.begin(), end = m_mapFormer.end();
	for (; begin != end; begin++)
		delete begin->second;
	m_mapFormer.clear();
	m_mapHandleToPackets.clear();
}
//----------------------------------------------------------------------------
inline void CServerTransport::AddConnection(int _Handle, int _Id)
{

	m_Mutex.lock();

	MAP_INT::iterator it = m_mapIdHandle.find(_Id);
	if (it != m_mapIdHandle.end())
		m_mapIdHandle.erase(it);
	m_mapIdHandle.insert(m_mapIdHandle.lower_bound(_Id), MAP_INT::value_type(_Id, _Handle));

	it = m_mapHandleId.find(_Handle);
	if (it != m_mapHandleId.end())
		m_mapHandleId.erase(it);
	m_mapHandleId.insert(m_mapHandleId.lower_bound(_Handle), MAP_INT::value_type(_Handle, _Id));

	MAP_FORMER::iterator itf = m_mapFormer.find(_Handle);
	if (itf != m_mapFormer.end())
	{
		delete itf->second;
		m_mapFormer.erase(itf);
	}
	m_mapFormer.insert(m_mapFormer.lower_bound(_Handle), MAP_FORMER::value_type(_Handle, new CDiameterFormer(m_rcvBufferSize)));

	m_PacketsMutex.lock();
	MAP_INTPACKETS::iterator itp = m_mapHandleToPackets.find(_Handle);
	if (itp != m_mapHandleToPackets.end())
		m_mapHandleToPackets.erase(itp);
	m_mapHandleToPackets.insert(m_mapHandleToPackets.lower_bound(_Handle), MAP_INTPACKETS::value_type(_Handle, LST_PACKET()));
	m_pSendThread->AddClient(_Handle);
	m_PacketsMutex.unlock();

	m_Mutex.unlock();
}
//----------------------------------------------------------------------------
inline void CServerTransport::DelConnection(int _Handle)
{

	m_Mutex.lock();

	m_pSendThread->RemoveClient(_Handle);

	MAP_INT::iterator it = m_mapHandleId.find(_Handle);
	if (it != m_mapHandleId.end())
	{
		MAP_INT::iterator it2 = m_mapIdHandle.find(it->second);
		if (it2 != m_mapIdHandle.end())
			m_mapIdHandle.erase(it2);
		m_mapHandleId.erase(it);
	}

	MAP_FORMER::iterator itf = m_mapFormer.find(_Handle);
	if (itf != m_mapFormer.end())
	{
		delete itf->second;
		m_mapFormer.erase(itf);
	}

	m_PacketsMutex.lock();
	MAP_INTPACKETS::iterator itp = m_mapHandleToPackets.find(_Handle);
	if (itp != m_mapHandleToPackets.end())
	{
		LST_PACKET &packets = itp->second;
		m_unsentPacketsCount -= packets.size();
		m_mapHandleToPackets.erase(itp);
	}
	m_PacketsMutex.unlock();

	m_Mutex.unlock();
}
//----------------------------------------------------------------------------
inline int CServerTransport::FindClientByHandle(int _Handle)
{

	MAP_INT::iterator it = m_mapHandleId.find(_Handle);
	if(it != m_mapHandleId.end())
		return it->second;
	return -1;
}
//----------------------------------------------------------------------------
void CServerTransport::svc(void)
{

	if(!DiamSocket::initialize())
		return ;

	int retriesCount = 12;
	for(; retriesCount; --retriesCount)
	{
		if(StartListen())
			break;
		sleep(5);  // sleep in seconds
	}

	m_StartEvent.signal();

	if(!retriesCount)
		return ;

	struct timeval TimeOut = {0, 50000};

	fd_set FdSet;
	int    MaxHandleValue = 0;

	while(true)
	{
		if(m_bExit)
		{
			break;
			//MAP_INT::iterator begin = m_mapHandleId.begin(), end = m_mapHandleId.end();
			//for (; begin != end; begin++)
			//{
			//	m_pManager->SendDPR(begin->second);
			//}
		}

		FD_ZERO(&FdSet);

		FD_SET(m_ListenHandle, &FdSet);
		MaxHandleValue = (int)m_ListenHandle;

		FillConnectedSet(FdSet, &MaxHandleValue);

		TimeOut.tv_sec  = 1;
		TimeOut.tv_usec = 0;

		int Res = select(MaxHandleValue + 1, &FdSet, NULL, NULL, &TimeOut);
		switch(Res)
		{
		case 0:
			for(MAP_INT::iterator it = m_mapHandleId.begin(); it != m_mapHandleId.end(); /**/)
			{
				int handle   = it->first;
				int clientId = it->second;

				++it;  // increment here, because connection can be deleted and iterator can become invalid

				if(!m_pManager->OnNoDataKeepClient(clientId))
				{
					m_pManager->OnBreak(clientId/*FindClientByHandle(Handle)*/, CoreDiam::PEER_INACTIVITY);
					DelConnection(handle);
					DiamSocket::close(handle);
				}
			}
			break;

		case -1:
			m_bExit = true;
			break;

		default:
			/*/
			int count = 0;
			m_Mutex.acquire();
			MAP_INT::iterator end = m_mapHandleId.end();
			for(MAP_INT::iterator it = m_mapHandleId.begin(); it != end; it++, count++)
			{
				//FD_SET(it->first, &_Set);
			}
			m_Mutex.release();
			//*/

			if(FD_ISSET(m_ListenHandle, &FdSet))
			{
				sockaddr_in Addr;

				DiamSocket::SocketHandle Client = DiamSocket::accept(m_ListenHandle, m_isBlocking, &Addr);
				if(DiamSocket::DS_INVALID_SOCKET == Client)
				{
					continue;
				}

				DiamSocket::setBufferSizes(Client, m_rcvBufferSize, m_sndBufferSize);

				int Id = ++m_Id;
				AddConnection(Client, Id);

				if(!m_pManager->OnConnect(Id, inet_ntoa(Addr.sin_addr), Addr.sin_port))
				{
					DelConnection(Client);
					DiamSocket::close(Client);
				}
			}

			//m_Mutex.acquire_read();

			for(MAP_INT::iterator it = m_mapHandleId.begin(); it != m_mapHandleId.end(); /**/)
			{
				int Handle   = it->first;
				int clientId = it->second;

				++it;

				if(!FD_ISSET(Handle, &FdSet))
				{
					if(!m_pManager->OnNoDataKeepClient(clientId))
					{
						m_pManager->OnBreak(clientId/*FindClientByHandle(Handle)*/, CoreDiam::PEER_INACTIVITY);
						DelConnection(Handle);
						DiamSocket::close(Handle);
					}
				}
				else
				{
					//char Buffer[1024*32];
					//VTUNE_TASK_BEGIN(vtuneAmpHolder.hSockReceiveTask);
					int Result = DiamSocket::recv(Handle, m_rcvBuffer, m_rcvBufferSize);

					if(Result == DiamSocket::DS_SOCKET_ERROR)
					{
						int errorCode = DiamSocket::getLastError();
						if(errorCode == EWOULDBLOCK || errorCode == EAGAIN)
							continue;
					}

					if(Result == 0 || Result == DiamSocket::DS_SOCKET_ERROR)
					{
						m_pManager->OnBreak(clientId/*FindClientByHandle(Handle)*/, CoreDiam::PEER_DISCONNECT);
						DelConnection(Handle);
						DiamSocket::close(Handle);
					}
					else
					{
						/*
						int Id = -1;
						
						//m_Mutex.acquire();
						MAP_INT::iterator it = m_mapHandleId.find(Handle);
						if(it != m_mapHandleId.end())
							Id = it->second;
						else
						{
							//m_Mutex.release();
							//m_pManager->OnBreak(Handle); // peter: no sense without id
							DelConnection(Handle);
							DiamSocket::close(Handle);
							continue;
						}
						*/
						int Id = clientId;

						MAP_FORMER::iterator itf = m_mapFormer.find(Handle);
						if(itf == m_mapFormer.end())
						{
							//m_Mutex.release();
							m_pManager->OnBreak(Id, CoreDiam::FRAMEWORK_FAILURE); //m_pManager->OnBreak(Handle);
							DelConnection(Handle);
							DiamSocket::close(Handle);
							continue;
						}

						CDiameterFormer *pFormer = itf->second;
						//m_Mutex.release();

						unsigned int   Size = Result;
						for(unsigned char *Pack = pFormer->ReceiveData((unsigned char *)m_rcvBuffer, Size); Pack; Pack = pFormer->ReceiveData(NULL, Size))
						{
							m_pManager->OnReceive(Id, Pack, Size);
							Size = 0;
						}

						if(pFormer->isOutOfSpace())
						{
							m_pManager->OnBreak(FindClientByHandle(Handle), CoreDiam::PEER_MESSAGE_TOO_LARGE);
							DelConnection(Handle);
							DiamSocket::close(Handle);
						}
					}
				}
			}  // for(int Handle = 0; Handle < MaxHandleValue + 1; Handle++)
			break;
		}  // switch()
	}  // while(true)

	MAP_INT::iterator begin = m_mapHandleId.begin(), end = m_mapHandleId.end();
	for (; begin != end; begin++)
	{
		DiamSocket::shutdown(begin->first);
		DiamSocket::close(begin->first);
	}

	DiamSocket::shutdown(m_ListenHandle);
	DiamSocket::close(m_ListenHandle);

	m_ListenHandle = -1;

	Clear();
	return ;
}
//----------------------------------------------------------------------------
int CServerTransport::getMaxQueueSize()
{
	return m_maxQueueSize;
}
//----------------------------------------------------------------------------
uint32_t CServerTransport::getHost() const
{
	// Linux:
	//struct in_addr inAddr;
	//if(inet_aton(m_Host.c_str(), &inAddr))
	//	return inAddr.s_addr; // ? unsigned long

	sockaddr_in Addr;
	Addr.sin_addr.s_addr = inet_addr(m_Host.c_str());
	if(Addr.sin_addr.s_addr != INADDR_NONE)
		return Addr.sin_addr.s_addr;

	return 0;
}


}  // namespace CoreDiam

