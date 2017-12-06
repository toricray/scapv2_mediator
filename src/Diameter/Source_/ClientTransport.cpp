#include <arpa/inet.h>
#include <Logger/logger.h>
#include "ClientTransport.h"
#if !defined SOCKET_ERROR && !defined _WINDOWS && !defined _WIN32 && !defined WIN32
	#define SOCKET_ERROR    -1
#endif

#if !defined INVALID_SOCKET && !defined _WINDOWS && !defined _WIN32 && !defined WIN32
	#define INVALID_SOCKET  -1
#endif


namespace CoreDiam {

CClientTransport::CClientTransport(ITransportEvents *_pManager, int _QueueSize, int _rcvBufferSize, int _sndBufferSize, bool _isBlocking):
	m_Handle(DiamSocket::DS_INVALID_SOCKET),
	m_pManager(_pManager),
	m_bExit(false),
	m_unsentPacketsCount(0),
	m_havingWaiters(false),
	m_maxQueueSize(_QueueSize),
	m_rcvBufferSize(_rcvBufferSize),
	m_sndBufferSize(_sndBufferSize),
	m_isBlocking(_isBlocking),
	m_rcvBuffer(0)
{

	//ACE::init();

	m_pFormer     = new CDiameterFormer();
	m_pSendThread = new CSendThread(this);
	m_rcvBuffer   = new char[m_rcvBufferSize];
}
//----------------------------------------------------------------------------
CClientTransport::~CClientTransport()
{

	m_pSendThread->interrupt();
	m_SendQueueEvent.signal();

	delete   m_pFormer;
	delete   m_pSendThread;
	delete[] m_rcvBuffer;
}
//----------------------------------------------------------------------------
int CClientTransport::Start(const char *_Addresses, ITransportEvents *_Events)
{
    m_StartEvent.reset();
	const char* C = strstr(_Addresses, ":");
	if(!C)
		return -1;

	m_Host.assign(_Addresses, C);
	m_Port  = atoi(C + 1);
	if(m_Port < 1024 || m_Port > 65535)
		return -1;

	m_bExit = false;
	activate();
	m_StartEvent.wait();
	if(m_Handle != DiamSocket::DS_INVALID_SOCKET)
		return 0;

	return -1;
}
//----------------------------------------------------------------------------
void CClientTransport::Stop()
{
	m_bExit = true;
    m_StopEvent.wait();
}
//----------------------------------------------------------------------------
void CClientTransport::Release()
{
	delete this;
}
//----------------------------------------------------------------------------
size_t CClientTransport::GetQueueSize(int _socketHandle)
{
	m_PacketsMutex.lock();
	size_t result = m_lstSendQueue.size();
	m_PacketsMutex.unlock();
	return result;
}
//----------------------------------------------------------------------------
size_t CClientTransport::GetUnsentPacketsCount()
{
	m_PacketsMutex.lock();
	size_t result = m_unsentPacketsCount;
	m_PacketsMutex.unlock();
	return result;
}
//----------------------------------------------------------------------------
bool CClientTransport::WaitForSendQueue()
{

	m_PacketsMutex.lock();
	size_t count = m_unsentPacketsCount;
	m_PacketsMutex.unlock();

	if(!count)
	{
		for(int i = 8; i && !count; i--)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(30));
			m_PacketsMutex.lock();
			count = m_unsentPacketsCount;
			//m_havingWaiters = !count;
			m_PacketsMutex.unlock();
		}
	}

	if(count)
		return true;

	m_PacketsMutex.lock();
	count = m_unsentPacketsCount;
	m_havingWaiters = !count;
	m_PacketsMutex.unlock();

	if(count)
		return true;

	m_SendQueueEvent.wait();
    return true;
}
//----------------------------------------------------------------------------
SendResult CClientTransport::Send(int _ClientID, const char *_sid, size_t _sidLen, const unsigned char *_Data, unsigned int _Len, int &_ErrNo)
{
	_ErrNo     = 0;
	//int Handle = m_Handle;

	m_PacketsMutex.lock();

	LST_PACKET &LstPack = m_lstSendQueue;

	if(LstPack.size() < m_maxQueueSize)
	{
		LstPack.push_back(new CPacket(_Data, _Len));
		size_t count = ++m_unsentPacketsCount;
		_ErrNo = (int)LstPack.size();
		bool havingWaiters = m_havingWaiters;
		if(m_havingWaiters)
			m_havingWaiters = false;

		m_PacketsMutex.unlock();

		if(1 == count)
		{
			if(havingWaiters)
				m_SendQueueEvent.signal();
		}

		return PUT_TO_QUEUE;
	}

	m_PacketsMutex.unlock();
	return QUEUE_OVERFLOW;
}
//----------------------------------------------------------------------------
size_t CClientTransport::SendFromQueue(int _socketHandle)
{

	m_PacketsMutex.lock();

	size_t packetsCount = m_lstSendQueue.size();

	if(packetsCount == 0)
	{
		m_PacketsMutex.unlock();
		return 0;
	}

	CPacket* Pack = m_lstSendQueue.front();

	int Bytes = DiamSocket::send(_socketHandle, (const char *)Pack->Data(), Pack->Size());

	if(Bytes == DiamSocket::DS_SOCKET_ERROR)
	{
		Bytes = DiamSocket::getLastError();
		if (Bytes == EWOULDBLOCK || Bytes == ENOMEM || Bytes == ENOBUFS)
		{
			// ?
		}
		else
		{
			// ?
		}

		m_pSendThread->signal();
		m_PacketsMutex.unlock();
		return packetsCount;
	}

	m_lstSendQueue.pop_front();

	if(Bytes == (int)Pack->Size())
	{
		packetsCount--;
		m_unsentPacketsCount--;
	}
	else
	{
		m_lstSendQueue.push_front(new CPacket(Pack->Data() + Bytes, Pack->Size() - Bytes));
		m_pSendThread->signal();
	}

	m_PacketsMutex.unlock();
	delete Pack;

	return packetsCount;
}
//----------------------------------------------------------------------------
bool CClientTransport::connect()
{

	m_Handle = socket(AF_INET, SOCK_STREAM, 0);

	if(m_Handle == DiamSocket::DS_INVALID_SOCKET)
		return false;

#if defined (SO_REUSEPORT)
	int option = SO_REUSEADDR | SO_REUSEPORT;
#else
	int option = SO_REUSEADDR;
#endif

	int yes = 1;
	if(0 == ::setsockopt(m_Handle, SOL_SOCKET, option, (const char *)&yes, sizeof(yes)))
	{
		// TODO: seems not working for windows
	}

	if(0 == ::setsockopt(m_Handle, IPPROTO_TCP, TCP_NODELAY, (const char *)&yes, sizeof(yes)))
	{

	}

	DiamSocket::setBufferSizes(m_Handle, m_rcvBufferSize, m_sndBufferSize);

	sockaddr_in Addr;

	Addr.sin_family      = AF_INET;
	Addr.sin_port        = htons(m_Port);
	Addr.sin_addr.s_addr = inet_addr(m_Host.c_str());
	if(INADDR_NONE != Addr.sin_addr.s_addr)
	{
		int connectResult = ::connect(m_Handle, (sockaddr *)&Addr, sizeof(Addr));
		if(DiamSocket::DS_SOCKET_ERROR != connectResult)
		{
			if(!DiamSocket::setBlocking(m_Handle, m_isBlocking))
			{
				int lastError = DiamSocket::getLastError();
				if(lastError == EWOULDBLOCK || lastError == EAGAIN)
					return true;
			}

			return true;
		}
		
	}

	DiamSocket::close(m_Handle);
	m_Handle = DiamSocket::DS_INVALID_SOCKET;
	return false;
}
//----------------------------------------------------------------------------
void CClientTransport::svc(void)
{

	if(!DiamSocket::initialize())
		return ;

    bool connectResult = connect();

	m_StartEvent.signal();
    if (!connectResult)
        return;

	m_pSendThread->AddClient(m_Handle);

	m_pManager->OnConnect((int)m_Handle, m_Host, m_Port);

	struct timeval selectTimeout = {0, 500000};

	fd_set FdSet;
	int    nfds = (int)m_Handle + 1;

	while(true)
	{
		if(m_bExit)
			break;

		FD_ZERO(&FdSet);
		FD_SET(m_Handle, &FdSet);

		selectTimeout.tv_sec  = 0;
		selectTimeout.tv_usec = 500000;

		int Res = ::select(nfds, &FdSet, NULL, NULL, &selectTimeout);
		switch (Res)
		{
		case 0:
			break;

		case -1:
			m_bExit = true;
			break;

		default:
			if(FD_ISSET(m_Handle, &FdSet))
			{
				//char Buffer[1024];
				int Result = DiamSocket::recv(m_Handle, m_rcvBuffer, m_rcvBufferSize);
				if(Result == 0 || Result == DiamSocket::DS_SOCKET_ERROR)
				{
					if(Result == DiamSocket::DS_SOCKET_ERROR)
					{
						int lastError = DiamSocket::getLastError();
						if(lastError == EWOULDBLOCK || lastError == EAGAIN)
							continue;
					}

					//m_pManager->OnBreak((int)m_Handle);
					m_bExit = true;
				}
				else
				{
					int Id = (int)m_Handle;
					unsigned int Size = Result;
					unsigned char *Pack = m_pFormer->ReceiveData((unsigned char *)m_rcvBuffer, Size);
					if (Pack)
					{
						m_pManager->OnReceive(Id, Pack, Size);
						Size = 0;
						while ((Pack = m_pFormer->ReceiveData(NULL, Size)) != NULL)
							m_pManager->OnReceive(Id, Pack, Size);
					}
				}
			}
			break;
		}  // switch
	}  // while(true)


	if(m_Handle != DiamSocket::DS_INVALID_SOCKET)
	{
		m_pSendThread->RemoveClient((int)m_Handle);

		m_pManager->OnBreak((int)m_Handle, CoreDiam::STOP);

		DiamSocket::shutdown(m_Handle);
		DiamSocket::close(m_Handle);
		m_Handle = DiamSocket::DS_INVALID_SOCKET;
	}
    m_StopEvent.signal();
	return;
}
//----------------------------------------------------------------------------
int CClientTransport::getMaxQueueSize()
{
	return m_maxQueueSize;
}


}
