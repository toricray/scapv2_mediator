#ifndef __SESSIONMANAGER_H__
#define __SESSIONMANAGER_H__

#include "../Interface/DiamcoreInterfaces.h"

#include "TimerManager.h"
#include "DiameterMessage.h"

#include <map>
#include <string>

namespace CoreDiam
{

enum SessionStateStatus {
	SESSION_STATE_STATUS_VALID     = 0,
	SESSION_STATE_STATUS_CORRUPTED = 1,
	SESSION_STATE_STATUS_UNDEFINED = 2
};

class CSessionManager : public ITimerCallback
{
private:
	enum SessionState {
		SESS_STATE_UNKNOWN,
		SESS_STATE_REQUEST_TURN,
		SESS_STATE_ANSWER_TURN,
		SESS_STATE_TERMINATED
	};

	struct SessionInfo
	{
		int        ClientId;
		CTimerManager::TimerReference TimerEl;   //TimerData *TimerEl;
		bool       Response;
		bool                          isActive;

		SessionInfo(): isActive(true) {}
	};

	struct SessionNotificationInfo
	{
		int          clientId;
		std::string  sessionId;
		SessionState sessionState;

		SessionNotificationInfo()
			: clientId(-1)
			, sessionId("")
			, sessionState(SESS_STATE_UNKNOWN)
		{}

		SessionNotificationInfo(int _clientId, const std::string &_sessionId, SessionState _sessionState)
			: clientId(_clientId)
			, sessionId(_sessionId)
			, sessionState(_sessionState)
		{}

		SessionNotificationInfo(const SessionNotificationInfo &from)
			: clientId(from.clientId)
			, sessionId(from.sessionId)
			, sessionState(from.sessionState)
		{
		}

		SessionNotificationInfo & operator = (const SessionNotificationInfo &from)
		{
			if(&from == this)
				return *this;

			clientId     = from.clientId;
			sessionId    = from.sessionId;
			sessionState = from.sessionState;

			return *this;
		}
	};

	struct DisconnectInfo
	{
		int ClientId;
		CTimerManager::TimerReference TimerEl;  //TimerData  *TimerEl;
		bool Disconnected;

		DisconnectInfo(CTimerManager::TimerReference &ptimer): ClientId(0), TimerEl(ptimer), Disconnected(false) {}
	};

	typedef std::map<std::string, SessionInfo> MAP_STRSESSINFO;
	typedef std::map<int, DisconnectInfo>      MAP_DISCONNECTCLIENTINFO;
	typedef std::list<std::string>             LST_STR;
	typedef std::vector<SessionNotificationInfo> VEC_SESSION_NOTIFY;

	struct SMTimerData: public TimerData
	{
		MAP_STRSESSINFO::iterator m_SMData;

		SMTimerData(const timer_type &type = COMMON): TimerData(type) {}
	};

	class SMTimerDataFactory: public TimerDataFactory
	{
	public:
		SMTimerDataFactory(): TimerDataFactory() {}
		virtual ~SMTimerDataFactory() {}

		virtual TimerData * acquire(timer_type ttype) {
			return new SMTimerData(ttype);
		}
		virtual void        release(TimerData *td) {
			delete td;
		}
	};

	class SMTimerManagerLock: public ITimerManagerLock
	{
	private:
		std::mutex  &m_Mutex;
	public:
		SMTimerManagerLock(std::mutex &mutex): m_Mutex(mutex) {}
		virtual ~SMTimerManagerLock() {}
		virtual void enter() {
			m_Mutex.lock();
		}
		virtual void leave() {
			m_Mutex.unlock();
		}
	};

	MAP_STRSESSINFO::iterator  m_LockedSession;

	bool getSessionIdAndRequestType(IDiameterMessage *_pMessage, const char **sid, unsigned int *len, int *requestType);

protected:
	int              m_SessionTimeout;
	int              m_ResponseTimeout;
	MAP_STRSESSINFO  m_mapSessions;

	std::mutex     m_Mutex;

	MAP_DISCONNECTCLIENTINFO	m_mapDisconnectClients;
	//ACE_RW_Mutex              m_DisconnectClientsMutex;

	VEC_SESSION_NOTIFY        m_SessionNotifications;

	CTimerManager   *m_pTimerManager;
	IDiameterEvents *m_pDiameterEvents;

	SMTimerDataFactory        m_smTimerDataFactory;
	SMTimerManagerLock        m_tmLock;

	bool OnTimer(TimerData *_Timer);
	bool OnDPATimer(TimerData *_Timer);
	void Flush();

public:
	CSessionManager(IDiameterEvents *_pDiameterEvents);
	virtual ~CSessionManager();

	int  FindClient(IDiameterMessage *_pMessage, bool _OnRequest = false);
	int  FindClientAndLockSession(IDiameterMessage *_pMessage);

	void OnRequestMessage(IDiameterMessage *_pMessage, int _ClientId, bool *_flagSkipPacket); // _flagSkipPacket is set to ignore duplicate sessions numbers with different client id

	void OnAnswerMessage(IDiameterMessage *_pMessage, int _ClientId, bool _Locked = false);

	// 0. success
	// 1. no session
	// 2. no request for the session
	void OnAnswerMessage(IDiameterMessage *_pMessage, int *_ClientId, SessionStateStatus *_sessionStateStatus);

	void OnRequestDisconnect(IDiameterMessage *_pMessage, int _ClientID);
	void OnAnswerDisconnect(IDiameterMessage *_pMessage, int _ClientID);

	void EraseSessions(int _ClientID);
	void EraseSession(IDiameterMessage *_pMessage);

	void SetTimeout(int _SessionTimeout, int _ResponseTimeout);

	bool Disconnected(const unsigned long &clients_number);
	void CleanDisconnectMap();

	size_t              GetTimerCount();
	size_t              GetTimerCreatedCount();
	size_t              GetTimerCancelledCount();
	size_t              GetTimerTriggeredCount();
	size_t              GetTimerMinPoint();
	size_t              GetTimerMaxPoint();
	size_t              GetPoint();
};

}

#endif //__SESSIONMANAGER_H__

