#ifndef __DIAMETERCLIENT_H__
#define __DIAMETERCLIENT_H__

#include "../Interface/DiamcoreInterfaces.h"

#include "ClientTransport.h"

#include "Coder/DiameterDecoder.h"
#include "Coder/DiameterCoder.h"

#include "CCR/CCAMessageImpl.h"

#include "SessionManager.h"

namespace CoreDiam
{

class DIAMCORE_API CDiameterClient: public IDiameterClient, public ITransportEvents
{
private:
	struct ClientInfo
	{
		int         ClientID;
		std::string ipAddressStr;
		int         port;
	};

	typedef std::map<int, ClientInfo>  MAP_CLIENTS;

protected:
	IDiameterEvents  * m_pDiameterEvents;
	ICCAEvents       * m_pCCAEvents;
	ISettings        * m_pSettings;
	bool               m_bStarted;
	CClientTransport * m_pTransport;
	int                m_Client;

	int                m_SessCounterLo;
	int                m_SessCounterHi;
	int                m_HopByHopId;
	int                m_EndToEndId;
	bool               m_bOverload;
    bool               m_isProxy;
protected:
	//int  m_QueueSize;
protected:
	CDiameterDecoder *m_pDecoder;
	CDiameterCoder   *m_pCoder;

	CSessionManager  *m_pSessionManager;

    std::mutex      m_Mutex;

protected:
	//ACE_RW_Mutex      m_ClientMutex;
    std::mutex      m_SessionMutex;

    std::string hostAddress;
	std::string originHostStr;
	std::string originRealmStr;
    std::string destinationRealmStr;

    std::mutex      m_OverloadMutex;

	void               SendCER();
	IDiameterMessage * CreateCER();

	IDiameterMessage * CreateDPR();
    IDiameterMessage * CreateDWR();

	void               InitData();
	inline void        FillMessage(IDiameterMessage* _pMessage);
	inline int         SelectClient(IDiameterMessage* _pMessage);

public:
	CDiameterClient(IDiameterEvents* _pDiameterEvents, ICCAEvents* _pCCAEvents, ISettings* _pSettings,  int _QueueSize, bool isProxy);
	virtual ~CDiameterClient();

	virtual void       Start();
	virtual void       StopInput() {}
	virtual void       Stop();
	virtual void       Release();

	virtual bool       Connect(const char *_Addresses, const char* originHost, const char* originRealm, const char* dstRealm);
	virtual SendResult Send(IDiameterMessage* _pMessage, unsigned long *_sendQueueSize = NULL);

	virtual bool       OnConnect(int, std::string _IP, int port);
	virtual void       OnReceive(int, unsigned char *_Data, unsigned int _Len);
	virtual void       OnSent(int,  const char *_sid, size_t _sidLen, int _sendResult);
	virtual void       OnBreak(int, DiamDisconnectionReason reason);
	virtual void       OnEmptyQueue(int);
	virtual bool       OnNoDataKeepClient(int);

	void               SendDPR();
    void               SendDWR();
};

}

#endif //__DIAMETERCLIENT_H__
