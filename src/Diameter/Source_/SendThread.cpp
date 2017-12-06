#include "SendThread.h"

namespace CoreDiam {

CSendThread::CSendThread(ITransport *_pTransport) :
	m_bExit(false),
	m_QueueEvent(),
	m_pTransport(_pTransport),m_Semaphore(0)
{

	activate();
	m_StartEvent.wait();
}
//----------------------------------------------------------------------------
CSendThread::~CSendThread()
{

	m_bExit = true;
	signal(); //m_Semaphore.release();
	wait();
}
//----------------------------------------------------------------------------
void CSendThread::interrupt()
{
	m_bExit = true;
}
//----------------------------------------------------------------------------
    void CSendThread::svc(void)
{

	m_StartEvent.signal();

	struct timeval TimeOut = {0, 50000};

	fd_set sndFDS;
	int    MaxHandleValue = 0;

	size_t sheduledPacketsCount = m_pTransport->GetUnsentPacketsCount();

	while(!m_bExit)
	{
		//if(!m_pTransport->GetUnsentPacketsCount()) //if(!sheduledPacketsCount)
		//	waitQueue(); //m_Semaphore.acquire();

		m_pTransport->WaitForSendQueue();

		if(m_bExit)
			break;

		FD_ZERO(&sndFDS);

		MaxHandleValue = 0;

		FillConnectedSet(sndFDS, &MaxHandleValue, &m_vecHandles);

		TimeOut.tv_sec  = 1;
		TimeOut.tv_usec = 0;

		int Res = select(MaxHandleValue + 1, NULL, &sndFDS, NULL, &TimeOut);

		sheduledPacketsCount = 0;

		switch (Res)
		{
		case 0:
			//for(unsigned int i = 0; i < m_vecHandles.size(); i++)
			//{
			//	int handle = m_vecHandles.at(i);
			//	sheduledPacketsCount += m_pTransport->GetQueueSize(handle);
			//}

			//sheduledPacketsCount = m_pTransport->GetUnsentPacketsCount();
			break;

		case -1:
			break;

		default:
			/*/
			for (int Handle = 0; Handle < MaxHandleValue + 1; Handle++)
			{
				if(FD_ISSET(Handle, &FdSet))
				{
					//m_Mutex.acquire();
					m_pTransport->SendFromQueue(Handle);
					//m_Mutex.release();
				}
			}
			//*/
			for(unsigned int i = 0; i < m_vecHandles.size(); i++)
			{
				int handle = m_vecHandles.at(i);
				if(FD_ISSET(handle, &sndFDS))
				{
					//m_Mutex.acquire();
					sheduledPacketsCount += m_pTransport->SendFromQueue(handle);
					//m_Mutex.release();
				}
				else
				{
					sheduledPacketsCount += m_pTransport->GetQueueSize(handle);
				}
			}
			break;
		}

		//m_Mutex.acquire();
		//if (m_setHandles.size() != 0)
		//	m_Semaphore.release();
		//m_Mutex.release();
	}

	return ;
}
//----------------------------------------------------------------------------
inline int CSendThread::FillConnectedSet(fd_set &_Set, int *_MaxValue, VEC_INT *_vecHandles)
{

	int Count = 0;
	_vecHandles->clear();

	m_Mutex.lock();

	SET_INT::iterator begin = m_setHandles.begin(), end = m_setHandles.end();
	for (; begin != end; begin++, Count++)
	{
		FD_SET(*begin, &_Set);
		_vecHandles->push_back(*begin);
		if(_MaxValue)
		{
			if(*begin > *_MaxValue)
				*_MaxValue = *begin;
		}
	}

	m_Mutex.unlock();
	return Count;
}
//----------------------------------------------------------------------------
void CSendThread::waitQueue()
{
	m_QueueEvent.wait();
}
//----------------------------------------------------------------------------
void CSendThread::AddClient(int _Handle)
{

	m_Mutex.lock();
	SET_INT::iterator it = m_setHandles.find(_Handle);
	if (it == m_setHandles.end())
	{
		m_setHandles.insert(_Handle);
		//if (m_setHandles.size() == 1)
		//	m_Semaphore.release();
	}
	m_Mutex.unlock();
}
//----------------------------------------------------------------------------
void CSendThread::RemoveClient(int _Handle)
{

	m_Mutex.lock();
	SET_INT::iterator it = m_setHandles.find(_Handle);
	if (it != m_setHandles.end())
		m_setHandles.erase(it);
	m_Mutex.unlock();
}
//----------------------------------------------------------------------------
void CSendThread::signal()
{

	//m_Semaphore.release();
	m_QueueEvent.signal();
}

//void CSendThread::cancel(unsigned int count)
//{
//	for(unsigned int i = count; i > 0; i--)
//		m_Semaphore.acquire();
//}

}
