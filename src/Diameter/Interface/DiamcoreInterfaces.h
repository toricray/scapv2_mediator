#ifndef __DIAMCOREINTERFACES_H__
#define __DIAMCOREINTERFACES_H__

#include <string>

#include "Settings.h"
#include "DiameterMessage.h"
#include "CCRCCAMessage.h"

namespace CoreDiam {

typedef enum
{
	PEER_DISCONNECT = 0,
	PEER_INACTIVITY = 1,
	PEER_MESSAGE_TOO_LARGE = 2,
	FRAMEWORK_FAILURE = 3,
	STOP = 4
} DiamDisconnectionReason;

class ICommon
{
public:
	virtual ~ICommon() {}
	virtual void Start() = 0;
	virtual void StopInput() = 0;
	virtual void Stop() = 0;
	virtual void Release() = 0;
};

class IDiameterEvents
{
public:
	virtual ~IDiameterEvents() {}
	virtual void OnReadyConnection(int _Count, int _clientId) = 0;
	virtual void OnBreakConnection(int _Count, int _clientId, DiamDisconnectionReason reason) = 0;
	virtual void OnReceive(IDiameterMessage* _pDiameterMessage, int _clientId) = 0;
	virtual void OnResponseTimeout(const char* _SessionID, int _clientId) = 0;
	virtual void OnSessionTimeout(const char* _SessionID, int _clientId) = 0;
	virtual void OnOverload(int _clientId) = 0;
	virtual void OnEmptyQueue(int _clientId) = 0;
};

class ICCREvents
{
public:
	virtual ~ICCREvents() {}

	virtual void OnReceiveCCR(CCCRMessage *_CCRMessage, int _ClientId) = 0;
    virtual void OnSentCCA(const char *_sessionId, size_t _sessionIdLength, int _ClientId, int _sendResult) = 0;
};

class ICCAEvents
{
public:
	virtual ~ICCAEvents() {}
	virtual void OnReceiveCCA(CCCAMessage* _CCAMessage, int _ClientId) = 0;
};

#define MAX_CLIENT_ADDR_SIZE 511

struct DiamClientInfo
{
	int  id;
	int  port;
	char address[MAX_CLIENT_ADDR_SIZE + 1];
	char ipStr[16];
	union
	{
		char          ipChar[4];
		unsigned int  ipInt32;
	};
};

typedef enum
{
	NOT_STARTED        = -7,
	QUEUE_ERROR        = -6,
	QUEUE_OVERFLOW     = -5,
	UNKNOWN_CONNECTION = -4,
	CODER_ERROR        = -3,
	SEND_ERROR         = -2,
	TRANS_ERROR        = -1,
	OK                 =  0,
	PUT_TO_QUEUE       =  1
} SendResult;

class IDiameterClient : public ICommon
{
public:
	virtual bool       Connect(const char *_Addresses, const char* originHost, const char* originRealm, const char* dstRealm) = 0;
	virtual SendResult Send(IDiameterMessage *_pMessage, unsigned long *_sendQueueSize = NULL) = 0;
    virtual void SendDWR() = 0;
};

class IDiameterServer : public ICommon
{
public:
	virtual bool       Listen(unsigned short _Port, const char *_Address) = 0;
	virtual SendResult Send(IDiameterMessage *_pMessage, unsigned long *_sendQueueSize = NULL) = 0;
	virtual SendResult Send(int _clientId, IDiameterMessage* _pMessage, unsigned long *_sendQueueSize = NULL) = 0;
	virtual bool       GetClientInfo(int _clientId, DiamClientInfo *_info) = 0;
	virtual void       Disconnect() = 0;

	virtual size_t     GetTimerCount()          = 0;
	virtual size_t     GetTimerCreatedCount()   = 0;
	virtual size_t     GetTimerCancelledCount() = 0;
	virtual size_t     GetTimerTriggeredCount() = 0;
};

}

#endif //__DIAMCOREINTERFACES_H__
