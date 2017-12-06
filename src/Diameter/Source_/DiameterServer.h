#ifndef __DIAMETERSERVER_H__
#define __DIAMETERSERVER_H__

#include "../Interface/DiamcoreInterfaces.h"

#include "ServerTransport.h"

#include "SessionManager.h"

#include "Coder/DiameterDecoder.h"
#include "Coder/DiameterCoder.h"

#include "CCR/CCRMessageImpl.h"

#include <string>
#include <map>


namespace CoreDiam {

class DIAMCORE_API CDiameterServer: public IDiameterServer, public ITransportEvents
{
private:
	struct ClientInfo
	{
		int             ClientID;
		std::string     ipAddressStr;
		int             port;
		long  lastActionPoint;
	};

	typedef std::map<int, ClientInfo>  MAP_CLIENTS;
	typedef std::map<int, bool>        MAP_INTBOOL;

	template<typename T> class AutoReadLocker
	{
	private:
		T &lock_;
	public:
		AutoReadLocker(T &lock): lock_(lock) {
			lock_.lock();
		}
		AutoReadLocker(T *lock): lock_(*lock) {
			lock_.lock();
		}
		~AutoReadLocker() {
			lock_.unlock();
		}
	};

	template<typename T> class AutoWriteLocker
	{
	private:
		T &lock_;
	public:
		AutoWriteLocker(T &lock): lock_(lock) {
			lock_.lock();
		}
		AutoWriteLocker(T *lock): lock_(*lock) {
			lock_.lock();
		}
		~AutoWriteLocker() {
			lock_.unlock();
		}
	};

	typedef AutoReadLocker<std::mutex>   AutoReadLocker_;
	typedef AutoWriteLocker<std::mutex>  AutoWriteLocker_;

protected:
	IDiameterEvents * m_pDiameterEvents;
	ICCREvents      * m_pCCREvents;
	ISettings       * m_pSettings;
	bool              m_bStarted;
	CServerTransport* m_pTransport;

	CDiameterDecoder* m_pDecoder;
	CDiameterCoder  * m_pCoder;

	CSessionManager * m_pSessionManager;

	MAP_CLIENTS       m_mapClients;
	std::mutex      m_Mutex; //ACE_Recursive_Thread_Mutex m_Mutex;

	bool              m_FlagReceiveOn; // flag of receiving CCR/CCA messages, when true, messages are processed
	std::mutex      m_mtx_FlagReceiveOn;

	//int m_QueueSize;

	std::mutex      m_OverloadMutex;
	MAP_INTBOOL       m_mapOverload;

	std::string       m_Host;
	std::string       m_Realm;

	int               m_InactivityTimeout;
	//ACE_Time_Value    m_LastRecvPoint;

protected:
	bool                      HandleDefaultMessages(int _ClientID, unsigned char *_Data, unsigned int _Len, const CDiameterHeader &diamHeader);

	inline void               SendCEA(int _ClientID, IDiameterMessage* _pCERMessage, unsigned char *_Data, unsigned int _Len);
	inline void               SendDWA(int _ClientID, IDiameterMessage* _pDWRMessage, unsigned char *_Data, unsigned int _Len);
	void                      SendDPR(int _ClientID);

	inline IDiameterMessage * CreateCEA(IDiameterMessage *_pCERMessage);
	inline IDiameterMessage * CreateDWA(IDiameterMessage *_pDWRMessage);
	IDiameterMessage*         CreateDPR();

	inline bool               CheckDeliver(int _ClientID, CCCRMessageImpl *pCCRMessage);

public:
	CDiameterServer(IDiameterEvents *_pDiameterEvents, ICCREvents *_pCCREvents, ISettings *_pSettings,  int _QueueSizeconst, const bool &FlagReceiveOn);
	virtual ~CDiameterServer();

	virtual void       Start();
	virtual void       Stop();
	virtual void       StopInput();
	virtual void       Release();

	// IDiameterServer
	virtual bool       Listen(unsigned short _Port, const char *_Address);
	virtual SendResult Send(IDiameterMessage *_pMessage, unsigned long *_sendQueueSize = NULL);
    virtual SendResult Send(int _clientId, IDiameterMessage* _pMessage, unsigned long *_sendQueueSize = NULL);
    virtual bool       GetClientInfo(int _clientId, DiamClientInfo *_info);

	// ITransportEvents
	virtual bool       OnConnect(int _ClientID, std::string _IP, int port);
	virtual void       OnReceive(int _ClientID, unsigned char *_Data, unsigned int _Len);
    virtual void       OnSent(int _clientID, const char *_sid, size_t _sidLen, int _sendResult);
    virtual void       OnBreak(int _ClientID, DiamDisconnectionReason reason);
	virtual void       OnEmptyQueue(int _ClientID);
	virtual bool       OnNoDataKeepClient(int _ClientID);

	void               SetReceiving(const bool &flag_receive_on);
	virtual void       Disconnect();

	virtual size_t     GetTimerCount();
	virtual size_t     GetTimerCreatedCount();
	virtual size_t     GetTimerCancelledCount();
	virtual size_t     GetTimerTriggeredCount();
};

}

#endif //__DIAMETERSERVER_H__
