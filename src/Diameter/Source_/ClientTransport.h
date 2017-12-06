#ifndef __CLIENT_TRANSPORT__
#define __CLIENT_TRANSPORT__

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

#include "TransportEvents.h"

#include "DiameterFormer.h"
#include "DiameterSocket.h"

#include "SendThread.h"

namespace CoreDiam {

class CClientTransport : public Task, public ITransport
{
protected:
	DiamSocket::SocketHandle  m_Handle;
	ITransportEvents         *m_pManager;
	bool                      m_bExit;
	std::string               m_Host;
	unsigned short            m_Port;
protected:
	CDiameterFormer          *m_pFormer;
	LST_PACKET                m_lstSendQueue;

    std::mutex              m_PacketsMutex;
    Event   m_StartEvent;
    Event   m_SendQueueEvent;
	Event   m_StopEvent;
	CSendThread              *m_pSendThread;
	size_t                    m_unsentPacketsCount;
	bool                      m_havingWaiters;

protected:
	unsigned int              m_maxQueueSize;
	int                       m_rcvBufferSize;
	int                       m_sndBufferSize;
	bool                      m_isBlocking;
	char                     *m_rcvBuffer;

	bool               connect();

public:
	CClientTransport(ITransportEvents *_pManager, int _QueueSize, int _rcvBufferSize, int _sndBufferSize, bool _isBlocking);
	~CClientTransport();

	// ACE_Task_Base:
	virtual void        svc(void);

	// ITransport:
	virtual SendResult Send(int _ClientID, const char *_sid, size_t _sidLen, const unsigned char *_Data, unsigned int _Len, int &_ErrNo);
	virtual size_t     SendFromQueue(int _socketHandle);
	virtual size_t     GetQueueSize(int _socketHandle);
	virtual size_t     GetUnsentPacketsCount();
	virtual bool       WaitForSendQueue();

	int                Start(const char* _Addresses, ITransportEvents *_Events);
	void               Stop();
	void               Release();
	int                getMaxQueueSize();
};

}

#endif //__CLIENT_TRANSPORT__
