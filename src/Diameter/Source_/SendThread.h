#ifndef __SENDTHREAD_H__
#define __SENDTHREAD_H__

#include "TransportEvents.h"

#include <list>
#include <vector>
#include <set>
#include <new>
#include <cstring>
#include "Utills/utills.h"

namespace CoreDiam {

#define SENDQUEUESIZE 1000

class CPacket
{
protected:
	unsigned char *m_Buffer;
	unsigned int   m_Size;
	unsigned int   m_Offset;
	std::string    m_sid;
    int            m_ClientId;
	//int            m_SocketHandle;
	//bool           m_IsPartial;
public:
	CPacket(const unsigned char *_Data, unsigned int _Size, const char *_sid = NULL, size_t _sidLen = 0, int _clientId = -1, int _socketHandle = 0)
		: m_Size(_Size)
		//, m_SocketHandle(_socketHandle)
		, m_Offset(0)
		, m_ClientId(_clientId)
	{
		m_Buffer = new(std::nothrow) unsigned char[m_Size];
		if(m_Buffer)
			memcpy(&m_Buffer[0], _Data, m_Size);
		else
			m_Size = 0;

		if(NULL != _sid && _sidLen)
			m_sid.assign(_sid, _sidLen);
		else
			m_sid = "";
	}

	~CPacket()
	{
		if(m_Buffer)
			delete[] m_Buffer;
	}

public:
	const unsigned char * Data() {
		return m_Buffer + m_Offset;
	}

	unsigned int Size() {
		return m_Size - m_Offset;
	}

	//int socketHandle()
	//{
	//	return m_SocketHandle;
	//}

	const char * sid() {
		//if(!m_sid.length())
		//	return NULL;
		return m_sid.c_str();
	}

    size_t sidLength() {
	    //if(!m_sid.length())
	    //	return NULL;
	    return m_sid.length();
    }

	int   clientId() {
		return m_ClientId;
	}

	friend CPacket & operator +=(CPacket &lhs, unsigned int rhs) {
		if(lhs.m_Offset + rhs <= lhs.m_Size)
			lhs.m_Offset += rhs;
		else
			lhs.m_Offset = lhs.m_Size;
		return lhs;
	}
};

/*
struct DelayedPacket
{
	CPacket        *packet;
	ACE_Time_Value  timePoint;

	DelayedPacket(CPacket *_packet, int _seconds = 0): packet(_packet) {
		timePoint = ACE_OS::gettimeofday() + ACE_Time_Value(_seconds, 0);
	}
};

struct DelayedPacketLessOperator
{
	bool operator()(const DelayedPacket &lhs, const DelayedPacket &rhs) const {
		return (lhs.timePoint < rhs.timePoint);
	}
};

typedef std::multiset<DelayedPacket, DelayedPacketLessOperator>  DelayedPacketsMSet;
*/

typedef std::list<CPacket *> LST_PACKET;


class CSendThread: public Task
{
private:
	bool           m_bExit;
	typedef std::set<int>    SET_INT;
	typedef std::vector<int> VEC_INT;

    Event       m_StartEvent;
    Event       m_QueueEvent;
	ITransport    *m_pTransport;
	Semaphore  		m_Semaphore;



	std::mutex               m_Mutex; //ACE_Recursive_Thread_Mutex m_Mutex;
	SET_INT                    m_setHandles;
	VEC_INT                    m_vecHandles;

	inline int FillConnectedSet(fd_set &_Set, int *_MaxValue, VEC_INT *_vecHandles);
	void waitQueue();

protected:
	virtual void svc(void);

public:
	CSendThread(ITransport *_pTransport);
	~CSendThread();

	void AddClient(int _Handle);
	void RemoveClient(int _Handle);

	void signal();
	//void cancel(unsigned int count);
	void interrupt();
};

}

#endif //__SENDTHREAD_H__
