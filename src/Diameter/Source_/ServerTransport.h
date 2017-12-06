#ifndef __SERVERTRANSPORT_H__
#define __SERVERTRANSPORT_H__

#pragma warning (disable:4786)

//#ifdef FD_SETSIZE
//#  undef FD_SETSIZE
//#endif
//#define FD_SETSIZE 65536

#define MYVAL_SOMAXCONN 200

#ifdef WIN32
#  include <Winsock2.h>
#else
#  include <sys/socket.h>
#  include <sys/types.h>
#  include <netinet/tcp.h>
#endif

#include "../Interface/DiamcoreInterfaces.h"

#include <list>
#include <map>

#ifdef FD_SETSIZE
#  undef FD_SETSIZE
#endif
#define FD_SETSIZE 65536

#include "DiameterFormer.h"
#include "DiameterSocket.h"

#include "TransportEvents.h"
#include "SendThread.h"


namespace CoreDiam {

class CServerTransport: public Task, public ITransport
{
private:
	typedef std::list<int>                    LST_INT;
	typedef std::map<int, int>                MAP_INT;
	typedef std::map<int, CDiameterFormer *>  MAP_FORMER;
	typedef std::map<int, LST_PACKET>         MAP_INTPACKETS;

protected:
	DiamSocket::SocketHandle  m_ListenHandle;
	ITransportEvents         *m_pManager;
	bool                      m_bExit;
	std::string               m_Host;
	unsigned short            m_Port;
protected:
	long                      m_Id;

	MAP_INT                   m_mapIdHandle;
	MAP_INT                   m_mapHandleId;
	MAP_FORMER                m_mapFormer;
	MAP_INTPACKETS            m_mapHandleToPackets;
	//LST_PACKET                m_Packets;

	std::mutex              m_Mutex;
	std::mutex              m_PacketsMutex;
    Event                 m_StartEvent;

    Event                 m_SendQueueEvent;

	CSendThread              *m_pSendThread;
	size_t                    m_unsentPacketsCount;
	bool                      m_havingWaiters;

protected:
	unsigned int              m_maxQueueSize;
	int                       m_rcvBufferSize;
	int                       m_sndBufferSize;
	bool                      m_isBlocking;
	char                     *m_rcvBuffer;

protected:
	bool               StartListen();
	inline void        Clear();
	inline int         FillConnectedSet(fd_set &_Set, int *_MaxValue = NULL);
	inline void        AddConnection(int _Handle, int _Id);
	inline void        DelConnection(int _Handle);
	inline int         FindClientByHandle(int _Handle);

public:
	CServerTransport(ITransportEvents *_pManager, int _QueueSize, int _rcvBufferSize, int _sndBufferSize, bool _isBlocking);
	virtual ~CServerTransport();

	// ACE_Task_Base:
	virtual void        svc(void);

	// ITransport:
	virtual SendResult Send(int _ClientID, const char *_sid, size_t _sidLen, const unsigned char *_Data, unsigned int _Len, int &_ErrNo);
	virtual size_t     SendFromQueue(int _socketHandle);
	virtual size_t     SendAsync();
	virtual size_t     GetQueueSize(int _socketHandle);
	virtual size_t     GetUnsentPacketsCount();
	virtual bool       WaitForSendQueue();

	int                Start(const char* _Host, unsigned short _Port, int _MaxCount, ITransportEvents *_Events);
	void               Stop();
	void               Release();
	int                getMaxQueueSize();

	uint32_t           getHost() const;

	unsigned short     getPort() const {return m_Port;}
};

}  // namespace CoreDiam

#endif //__SERVERTRANSPORT_H__
