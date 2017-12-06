#include <netinet/in.h>
#include "SessionManager.h"

#include "DiameterMessage.h"


namespace CoreDiam {

struct CCAInfo
{
	const char   *sid;
	unsigned int  sidLength;
	int           requestType;
	int           resultCode;
	bool          hasMSCC;
	bool          hasFUI;
};


class AutoWriteLocker
{
private:
	std::mutex &lock_;
public:
	AutoWriteLocker(std::mutex &lock): lock_(lock) {
		lock_.lock();
	}
	~AutoWriteLocker() {
		lock_.unlock();
	}
};

bool getCCAInfo(IDiameterMessage *msg, CCAInfo *ccaInfo)
{

	ccaInfo->hasMSCC = false;
	ccaInfo->hasFUI  = false;

	IAVP *avp = msg->GetAVPs()->GetAVPByCode(avpcode::CC_REQUEST_TYPE);
	if(!avp)
		return false;

	unsigned int  dataSize;
	const char   *data = (const char *)avp->GetData(dataSize);
	if((!data) || (dataSize != sizeof(int)))
		return false;

	ccaInfo->requestType = ntohl(*(int *)data);


	avp = msg->GetAVPs()->GetAVPByCode(avpcode::SESSION_ID);
	if(!avp)
		return false;

	ccaInfo->sid = (const char *)avp->GetData(ccaInfo->sidLength);
	if(!ccaInfo->sid || !ccaInfo->sidLength)
		return false;


	avp = msg->GetAVPs()->GetAVPByCode(avpcode::RESULT_CODE);
	if(!avp)
		return false;

	data = (const char *)avp->GetData(dataSize);
	if((!data) || (dataSize != sizeof(int)))
		return false;

	ccaInfo->resultCode = ntohl(*(int *)data);


	avp = msg->GetAVPs()->GetAVPByCode(avpcode::MULTIPLE_SERVICES_CREDIT_CONTROL);
	if(!avp)
		return true;

	if(avp->GetType() != IAVP::GROUPEDSET)
		return true;

	avp = avp->GetAVPByCode(avpcode::MULTIPLE_SERVICES_CREDIT_CONTROL, 0);
	if(!avp || avp->GetType() != IAVP::GROUPED)
		return true;

	ccaInfo->hasMSCC = true;

	avp = avp->GetAVPByCode(avpcode::FINAL_UNIT_INDICATION);
	ccaInfo->hasFUI = (avp != NULL);

	return true;
}

//----------------------------------------------------------------------------
// CSessionManager
//----------------------------------------------------------------------------
CSessionManager::CSessionManager(IDiameterEvents* _pDiameterEvents):
	m_SessionTimeout(120000),
	m_ResponseTimeout(10000),
	m_Mutex(),
	m_pDiameterEvents(_pDiameterEvents),
	m_tmLock(m_Mutex)
{

	m_pTimerManager = new CTimerManager(this, m_tmLock, &m_smTimerDataFactory);
	m_LockedSession = m_mapSessions.end();
}
//----------------------------------------------------------------------------
CSessionManager::~CSessionManager()
{

	m_pTimerManager->Release();
}
//----------------------------------------------------------------------------
void CSessionManager::SetTimeout(int _SessionTimeout, int _ResponseTimeout)
{

	m_SessionTimeout  = _SessionTimeout;
	m_ResponseTimeout = _ResponseTimeout;
}
//----------------------------------------------------------------------------
bool CSessionManager::OnTimer(TimerData *_Timer)
{

	if(_Timer == NULL)
		return false;

	SessionNotificationInfo snInfo;

	MAP_STRSESSINFO::iterator it = reinterpret_cast<SMTimerData *>(_Timer)->m_SMData;
	if(it != m_mapSessions.end())
	{
		snInfo.clientId     = it->second.ClientId;
		snInfo.sessionId    = it->first;
		snInfo.sessionState = !it->second.isActive? SESS_STATE_TERMINATED: (it->second.Response? SESS_STATE_ANSWER_TURN: SESS_STATE_REQUEST_TURN);

		m_mapSessions.erase(it);

		m_SessionNotifications.push_back(snInfo);

		return true;
	}

	return false;
}
//----------------------------------------------------------------------------
bool CSessionManager::OnDPATimer(TimerData *_Timer)
{

	if(_Timer == NULL)
		return false;

	for(MAP_DISCONNECTCLIENTINFO::iterator it = m_mapDisconnectClients.begin(); it != m_mapDisconnectClients.end(); it++)
	{
		if(*it->second.TimerEl == _Timer)
		{
			it->second.TimerEl = m_pTimerManager->nullTimer(); //NULL;
			it->second.Disconnected = true; // we don't wait for normal closing of channel in case of timeout
			return true;
		}
	}
	return false;
}
//----------------------------------------------------------------------------
void CSessionManager::Flush()
{
	for(VEC_SESSION_NOTIFY::iterator it = m_SessionNotifications.begin(); it != m_SessionNotifications.end(); ++it)
	{
		switch(it->sessionState)
		{
		case SESS_STATE_ANSWER_TURN : m_pDiameterEvents->OnResponseTimeout(it->sessionId.c_str(), it->clientId); break;
		case SESS_STATE_REQUEST_TURN: m_pDiameterEvents->OnSessionTimeout(it->sessionId.c_str(), it->clientId); break;
		case SESS_STATE_TERMINATED  : m_pDiameterEvents->OnSessionTimeout(it->sessionId.c_str(), it->clientId); break;
		default:
			m_pDiameterEvents->OnSessionTimeout(it->sessionId.c_str(), it->clientId);
		}
	}

	m_SessionNotifications.clear();
}
//----------------------------------------------------------------------------
int CSessionManager::FindClient(IDiameterMessage* _pMessage, bool _OnRequest)
{

	// for DiameterServer is called in Send() method

	const char   *s;
	unsigned int  len;
	int           requestType;
	if(!getSessionIdAndRequestType(_pMessage, &s, &len, &requestType))
		return -1;

	std::string SessionId(s, len);
	AutoWriteLocker autoLocker(m_Mutex); //m_Mutex.acquire_read();

	MAP_STRSESSINFO::iterator it = m_mapSessions.find(SessionId);
	if (it == m_mapSessions.end())
	{
		return -1;
	}

	return it->second.ClientId; //RetVal;
}
//----------------------------------------------------------------------------
int CSessionManager::FindClientAndLockSession(IDiameterMessage *_pMessage)
{

	// for DiameterServer is called in Send() method

	const char   *s;
	unsigned int  len;
	int           requestType;
	if(!getSessionIdAndRequestType(_pMessage, &s, &len, &requestType))
		return -1;

	std::string SessionId;
	SessionId.assign((const char *)s, (const char*)(s + len));

	int RetVal = -1;

	MAP_STRSESSINFO::iterator it;

	m_Mutex.lock();

	it = m_mapSessions.find(SessionId);
	if(it == m_mapSessions.end())
	{
		m_Mutex.unlock();
		return -1;
	}

	m_LockedSession = it;
	RetVal = it->second.ClientId;

	//m_Mutex.release();

	
	return RetVal;
}
//----------------------------------------------------------------------------
void CSessionManager::OnRequestMessage(IDiameterMessage* _pMessage, int _ClientId, bool *_flagSkipPacket)
{

	// for DiameterServer is called in OnReceive() method
	// for DiameterClient is called in Send() method

	const char   *s;
	unsigned int  len;
	int           requestType;
	if(!getSessionIdAndRequestType(_pMessage, &s, &len, &requestType))
		return;

	std::string SessionId;
	SessionId.assign((const char *)s, (const char*)(s + len));

	AutoWriteLocker autoLocker(m_Mutex); //

	//INITIAL_REQUEST     = 1
	//UPDATE_REQUEST      = 2
	//TERMINATION_REQUEST = 3
	//EVENT_REQUEST       = 4
	if(requestType == CoreDiam::INITIAL || requestType == CoreDiam::EVENT)
	{

		MAP_STRSESSINFO::iterator it = m_mapSessions.find(SessionId);
		if (it != m_mapSessions.end())
		{
			*_flagSkipPacket = true;
			return;
		}

		it = m_mapSessions.insert(m_mapSessions.lower_bound(SessionId), MAP_STRSESSINFO::value_type(SessionId, SessionInfo()));
		it->second.ClientId = _ClientId;
		it->second.Response = true;

		if(!m_ResponseTimeout)
		{
			it->second.TimerEl = m_pTimerManager->nullTimer(); //NULL;
		}
		else
		{
			it->second.TimerEl = m_pTimerManager->SetTimer(m_ResponseTimeout);
			reinterpret_cast<SMTimerData *>(*(it->second.TimerEl))->m_SMData = it;
		}
	}
	else
	{
		MAP_STRSESSINFO::iterator it = m_mapSessions.find(SessionId);
		if (it == m_mapSessions.end())
		{
			*_flagSkipPacket = true;
			return;
		}

		if (it->second.ClientId != _ClientId)
		{
			*_flagSkipPacket = true;
			return;
		}

		it->second.Response = true;

		if(m_ResponseTimeout)
		{
			if(it->second.TimerEl != m_pTimerManager->nullTimer()) //NULL)
				m_pTimerManager->UpdateTimer(it->second.TimerEl, m_ResponseTimeout);
			else
			{
				// ? is it an error? no - session timeout could be zero
				it->second.TimerEl = m_pTimerManager->SetTimer(m_ResponseTimeout);
				reinterpret_cast<SMTimerData *>(*(it->second.TimerEl))->m_SMData = it;
			}
		}
		else
		{
			if(it->second.TimerEl != m_pTimerManager->nullTimer()) //NULL)
			{
				m_pTimerManager->CancelTimer(it->second.TimerEl);
				it->second.TimerEl = m_pTimerManager->nullTimer(); //NULL;
			}
		}
	}

	*_flagSkipPacket = false;
}
//----------------------------------------------------------------------------
void CSessionManager::OnAnswerMessage(IDiameterMessage *_pMessage, int _ClientId, bool _Locked /*= false*/)
{
	CCAInfo ccaInfo;
	if(!getCCAInfo(_pMessage, &ccaInfo))
		return;

	std::string SessionId;
	SessionId.assign((const char *)ccaInfo.sid, (const char *)(ccaInfo.sid + ccaInfo.sidLength));

	MAP_STRSESSINFO::iterator it;

	if(!_Locked)
	{
		m_Mutex.lock();
		it = m_mapSessions.find(SessionId);
	}
	else
	{
		if(m_LockedSession == m_mapSessions.end())
			it = m_mapSessions.find(SessionId);
		else
		{
			it = m_LockedSession;
			if(!it->first.compare(SessionId) || it->second.ClientId != _ClientId)
				it = m_mapSessions.find(SessionId);
		}
	}

	if(it == m_mapSessions.end())
	{
		if(!_Locked)
			m_Mutex.unlock();  // must not enter
		return;
	}

	if(!it->second.Response)
	{
		if(!_Locked)
			m_Mutex.unlock();  // must not enter
		return;
	}

	it->second.Response = false;

	//INITIAL_REQUEST     = 1
	//UPDATE_REQUEST      = 2
	//TERMINATION_REQUEST = 3
	//EVENT_REQUEST       = 4
	if(ccaInfo.requestType == CoreDiam::INITIAL || ccaInfo.requestType == CoreDiam::UPDATE)
	{
		if(ccaInfo.resultCode != 2001 && !ccaInfo.hasFUI)
		{
			if(it->second.TimerEl != m_pTimerManager->nullTimer()) //NULL)
			{
				m_pTimerManager->CancelTimer(it->second.TimerEl);
				it->second.TimerEl = m_pTimerManager->nullTimer(); //NULL;
			}

			m_mapSessions.erase(it);
		}
		else
		{
			if(!m_SessionTimeout)
			{
				if(it->second.TimerEl != m_pTimerManager->nullTimer()) //NULL)
				{
					m_pTimerManager->CancelTimer(it->second.TimerEl);
					it->second.TimerEl = m_pTimerManager->nullTimer(); //NULL;
				}
			}
			else
			{
				if(it->second.TimerEl != m_pTimerManager->nullTimer())
				{
					if(-1 == m_pTimerManager->UpdateTimer(it->second.TimerEl, m_SessionTimeout))
						it->second.TimerEl = m_pTimerManager->nullTimer();  // somebody has deleted it
				}

				if(it->second.TimerEl == m_pTimerManager->nullTimer())
					it->second.TimerEl = m_pTimerManager->SetTimer(m_SessionTimeout);
			}
		}
	}
	else
	{
		// terminate answer
		if(it->second.TimerEl != m_pTimerManager->nullTimer()) //NULL)
		{
			m_pTimerManager->CancelTimer(it->second.TimerEl);
			it->second.TimerEl = m_pTimerManager->nullTimer(); //NULL;
		}

		m_mapSessions.erase(it);
	}

	if(!_Locked)
		m_Mutex.unlock();
}
//----------------------------------------------------------------------------
void CSessionManager::OnAnswerMessage(IDiameterMessage *_pMessage, int *_ClientId, SessionStateStatus *_sessionStateStatus)
{

	if(_ClientId)
		*_ClientId = -1;

	if(_sessionStateStatus)
		*_sessionStateStatus = SESSION_STATE_STATUS_UNDEFINED;

	if(!_ClientId || !_sessionStateStatus)
		return;

	CCAInfo ccaInfo;
	if(!getCCAInfo(_pMessage, &ccaInfo))
		return;

	std::string SessionId;
	SessionId.assign((const char *)ccaInfo.sid, (const char *)(ccaInfo.sid + ccaInfo.sidLength));

	AutoWriteLocker autoLocker(m_Mutex);

	MAP_STRSESSINFO::iterator it = m_mapSessions.find(SessionId);

	if(it == m_mapSessions.end())
	{
		*_sessionStateStatus = SESSION_STATE_STATUS_UNDEFINED;
		return;
	}

	if(!it->second.Response)
	{
		*_sessionStateStatus = SESSION_STATE_STATUS_CORRUPTED;
		return;
	}

	*_ClientId = it->second.ClientId;
	*_sessionStateStatus = SESSION_STATE_STATUS_VALID;

	it->second.Response = false;

	if(ccaInfo.requestType == CoreDiam::INITIAL || ccaInfo.requestType == CoreDiam::UPDATE)
	{
		if(ccaInfo.resultCode != 2001 && !ccaInfo.hasFUI)
		{
			if(it->second.TimerEl != m_pTimerManager->nullTimer())
			{
				m_pTimerManager->CancelTimer(it->second.TimerEl);
				it->second.TimerEl = m_pTimerManager->nullTimer();
			}

			m_mapSessions.erase(it);
		}
		else
		{
			if(!m_SessionTimeout)
			{
				if(it->second.TimerEl != m_pTimerManager->nullTimer())
				{
					m_pTimerManager->CancelTimer(it->second.TimerEl);
					it->second.TimerEl = m_pTimerManager->nullTimer();
				}
			}
			else
			{
				if(it->second.TimerEl != m_pTimerManager->nullTimer())
				{
					if(-1 == m_pTimerManager->UpdateTimer(it->second.TimerEl, m_SessionTimeout))
						it->second.TimerEl = m_pTimerManager->nullTimer();  // somebody has deleted it
				}

				if(it->second.TimerEl == m_pTimerManager->nullTimer())
					it->second.TimerEl = m_pTimerManager->SetTimer(m_SessionTimeout);
			}
		}
	}
	else
	{
		if(it->second.TimerEl != m_pTimerManager->nullTimer())
		{
			m_pTimerManager->CancelTimer(it->second.TimerEl);
			it->second.TimerEl = m_pTimerManager->nullTimer();
		}

		m_mapSessions.erase(it);
	}
}
//----------------------------------------------------------------------------
void CSessionManager::EraseSessions(int _ClientID)
{

	//LST_STR sessions;

	AutoWriteLocker autoLocker(m_Mutex);

	for(MAP_STRSESSINFO::iterator it = m_mapSessions.begin(); it != m_mapSessions.end();)
	{
		if(it->second.ClientId != _ClientID)
			++it;
		else
		{
			if(it->second.TimerEl != m_pTimerManager->nullTimer())
			{
				//m_pTimerManager->CancelTimer(it->second.TimerEl);
				m_pTimerManager->UpdateTimer(it->second.TimerEl, 0);
				it->second.isActive = false;
				++it;
			}
			else
			{
				m_mapSessions.erase(it++);
			}
		}
	}
}
//----------------------------------------------------------------------------
void CSessionManager::EraseSession(IDiameterMessage *_pMessage)
{

	CCAInfo ccaInfo;
	if(!getCCAInfo(_pMessage, &ccaInfo))
		return;

	std::string sessionId;
	sessionId.assign((const char *)ccaInfo.sid, (const char *)(ccaInfo.sid + ccaInfo.sidLength));

	m_Mutex.lock();

	MAP_STRSESSINFO::iterator it = m_mapSessions.find(sessionId);

	if(it != m_mapSessions.end())
	{
		if(it->second.TimerEl != m_pTimerManager->nullTimer())
		{
			// session should be deleted by timer signal
			if(-1 == m_pTimerManager->UpdateTimer(it->second.TimerEl, 0))
			{
				// probably some timeout is already triggered
			}

			it->second.isActive = false;
		}
		else
		{
			m_mapSessions.erase(it);
		}
	}

	m_Mutex.unlock();
}
//----------------------------------------------------------------------------
bool CSessionManager::getSessionIdAndRequestType(IDiameterMessage *_pMessage, const char **sid, unsigned int *len, int *requestType)
{

	IAVP *pAVP = _pMessage->GetAVPs()->GetAVPByCode(416);
	if(!pAVP)
		return false;

	unsigned int  dataSize;
	const char   *data = (const char *)pAVP->GetData(dataSize);
	if((!data) || (dataSize != sizeof(int)))
		return false;

	*requestType = ntohl(*(int *)data);

	pAVP = _pMessage->GetAVPs()->GetAVPByCode(263);
	if(!pAVP)
		return false;

	*sid = (const char *)pAVP->GetData(*len);
	if(!*sid)
		return false;

	return true;
}
//----------------------------------------------------------------------------
void CSessionManager::OnRequestDisconnect(IDiameterMessage* _pMessage, int _ClientId)
{

	// on sending DPR to server

	// new disconnect client
	AutoWriteLocker autoLocker(m_Mutex);

	MAP_DISCONNECTCLIENTINFO::iterator it = m_mapDisconnectClients.find(_ClientId);

	if(it != m_mapDisconnectClients.end())
	{
		return;
	}

	CTimerManager::TimerReference ptimer = m_pTimerManager->SetTimer(m_ResponseTimeout, DPA);
	if(ptimer == m_pTimerManager->nullTimer()) // NULL)
	{
		//m_DisconnectClientsMutex.release();
		return; // error
	}

	it = m_mapDisconnectClients.insert(m_mapDisconnectClients.lower_bound(_ClientId), MAP_DISCONNECTCLIENTINFO::value_type(_ClientId, DisconnectInfo(ptimer)));

	//m_DisconnectClientsMutex.release();
}
//----------------------------------------------------------------------------
void CSessionManager::OnAnswerDisconnect(IDiameterMessage *_pMessage, int _ClientId)
{

	// on receive DPA message

	AutoWriteLocker autoLocker(m_Mutex); //m_DisconnectClientsMutex.acquire_write();

	MAP_DISCONNECTCLIENTINFO::iterator it = m_mapDisconnectClients.find(_ClientId);
	if(it == m_mapDisconnectClients.end())
	{
		return; // error
	}

	if(it->second.TimerEl != m_pTimerManager->nullTimer()) //NULL)
	{
		m_pTimerManager->CancelTimer(it->second.TimerEl);
		it->second.TimerEl = m_pTimerManager->nullTimer(); //NULL;
		it->second.Disconnected = true;
	}
}
//----------------------------------------------------------------------------
bool CSessionManager::Disconnected(const unsigned long &clients_number)
{

	bool result = true;
	AutoWriteLocker autoLocker(m_Mutex);

	if(m_mapDisconnectClients.size() == clients_number)
	{
		for(MAP_DISCONNECTCLIENTINFO::iterator i=m_mapDisconnectClients.begin(); i!=m_mapDisconnectClients.end(); i++)
		if(!i->second.Disconnected)
		{
			result = false;
			break;
		}
	}
	return result;
}
//----------------------------------------------------------------------------
void CSessionManager::CleanDisconnectMap()
{

	AutoWriteLocker autoLocker(m_Mutex);
	m_mapDisconnectClients.clear();
}
//----------------------------------------------------------------------------
size_t CSessionManager::GetTimerCount()
{
	return m_pTimerManager->GetCount();
}
//----------------------------------------------------------------------------
size_t CSessionManager::GetTimerCreatedCount()
{
	return m_pTimerManager->GetCreatedCount();
}
//----------------------------------------------------------------------------
size_t CSessionManager::GetTimerCancelledCount()
{
	return m_pTimerManager->GetCancelledCount();
}
//----------------------------------------------------------------------------
size_t CSessionManager::GetTimerTriggeredCount()
{
	return m_pTimerManager->GetTriggeredCount();
}

}
