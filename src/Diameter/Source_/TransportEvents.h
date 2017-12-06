#ifndef __ITRANSPORTEVENTS_H__
#define __ITRANSPORTEVENTS_H__

#include "../Interface/DiamcoreInterfaces.h"

#include <string>

class ITransportEvents
{
public:
	virtual ~ITransportEvents() {}

	virtual bool OnConnect(int _ClientID, std::string _IP, int port) = 0;
	virtual void OnReceive(int _ClientID, unsigned char *_Data, unsigned int _Len) = 0;
    virtual void OnSent(int _clientID, const char *_sid, size_t _sidLen, int _sendResult) = 0;
	virtual void OnBreak(int _ClientID, CoreDiam::DiamDisconnectionReason reason) = 0;

	virtual void OnEmptyQueue(int _ClientID) = 0;

	virtual bool OnNoDataKeepClient(int _ClientID) = 0;
};

class ITransport
{
public:
	virtual ~ITransport() {}

	virtual CoreDiam::SendResult Send(int _ClientID, const char *_sid, size_t _sidLen, const unsigned char *_Data, unsigned int _Len, int &_ErrNo) = 0;
	virtual size_t               SendFromQueue(int _socketHandle) = 0;

	virtual size_t               SendAsync() {return 0;}
	virtual size_t               GetQueueSize(int _socketHandle) {return 0;}
	virtual size_t               GetUnsentPacketsCount() {return 0;}
	virtual bool                 WaitForSendQueue() {return true;}
};

#endif //__ITRANSPORTEVENTS_H__
