#include "DiameterServer.h"

#include "DiameterMessage.h"


#ifdef USE_STACK_TRACE
#  include "StackDumper.h"
#else
#  ifndef OCS_STACK_TRACE
#    define OCS_STACK_TRACE(...)
#  endif
#  ifndef OCS_STACK_TRACE_MSG
#    define OCS_STACK_TRACE_MSG(ARG)
#  endif
#endif


namespace CoreDiam {

CDiameterServer::CDiameterServer(IDiameterEvents* _pDiameterEvents, ICCREvents* _pCCREvents, ISettings* _pSettings,   int _QueueSize, const bool &FlagReceiveOn) :
	m_pDiameterEvents(_pDiameterEvents),
	m_pCCREvents(_pCCREvents),
	m_pSettings(_pSettings),
	m_pSessionManager(0),
	m_FlagReceiveOn(FlagReceiveOn),
	m_InactivityTimeout(120000)
{
	OCS_STACK_TRACE();

	int  queueSize         = SENDQUEUESIZE;
	int  rcvBufferSize     = 0;
	int  sndBufferSize     = 0;
	bool isBlocking        = true;
	int  sessionTimeout    = 0;
	int  responseTimeout   = 0;
	bool useSessionManager = true;

	if(m_pSettings)
	{
		unsigned int         Size = 0;
		const unsigned char *Data = NULL;

		m_pSettings->TakeSetting("QueueSize", Data, Size);
		if(Data && Size == sizeof(int))
			queueSize = *(int *)Data;

		m_pSettings->TakeSetting("RcvBufferSize", Data, Size);
		if(Data && Size == sizeof(int))
			rcvBufferSize = *(int *)Data;

		m_pSettings->TakeSetting("SndBufferSize", Data, Size);
		if(Data && Size == sizeof(int))
			sndBufferSize = *(int *)Data;

		m_pSettings->TakeSetting("IsBlocking", Data, Size);
		if(Data && Size == sizeof(int))
			isBlocking = (0 != *(int *)Data);

		m_pSettings->TakeSetting("SessionTimeout", Data, Size);
		if(Data && Size == sizeof(int))
			sessionTimeout = *(int *)Data;

		m_pSettings->TakeSetting("ResponseTimeout", Data, Size);
		if(Data && Size == sizeof(int))
			responseTimeout = *(int *)Data;

		m_pSettings->TakeSetting("eventTimeout", Data, Size);
		if(Data && Size == sizeof(int))
		{
			if(*(int *)Data > responseTimeout)
				responseTimeout = *(int *)Data;
		}

		m_pSettings->TakeSetting("inactivityTimeout", Data, Size);
		if(Data && Size == sizeof(int))
			m_InactivityTimeout = *(int*)Data;

		m_pSettings->TakeSetting("useSessionManager", Data, Size);
		if(Data && Size == sizeof(int))
			useSessionManager = (0 != *(int *)Data);

		m_pSettings->TakeSetting("host", Data, Size);
		if(Data && Size > 0)
			m_Host = reinterpret_cast<const char *>(Data);

		m_pSettings->TakeSetting("realm", Data, Size);
		if(Data && Size > 0)
			m_Realm = reinterpret_cast<const char *>(Data);
	}

	//m_pTransport      = new CServerTransport(this, m_QueueSize);
	m_pTransport      = new CServerTransport(this, queueSize, rcvBufferSize, sndBufferSize, isBlocking);

	if(useSessionManager)
	{
		m_pSessionManager = new CSessionManager(_pDiameterEvents);
		m_pSessionManager->SetTimeout(sessionTimeout, responseTimeout);
	}

	m_pDecoder = new CDiameterDecoder();
	m_pCoder   = new CDiameterCoder();
}
//----------------------------------------------------------------------------
CDiameterServer::~CDiameterServer()
{
	OCS_STACK_TRACE();
	StopInput();
	Stop();
	m_pTransport->Release();
	if(m_pSessionManager)
		delete m_pSessionManager;
	delete m_pCoder;
	delete m_pDecoder;
}
//----------------------------------------------------------------------------
void CDiameterServer::Start()
{
	OCS_STACK_TRACE();
	m_bStarted = true;
	SetReceiving(true);
}
//----------------------------------------------------------------------------
void CDiameterServer::StopInput()
{
	OCS_STACK_TRACE();
	if(m_bStarted)
	{
		SetReceiving(false);
	}
}
//----------------------------------------------------------------------------
void CDiameterServer::Stop()
{
	OCS_STACK_TRACE();
	if(m_bStarted)
	{
		m_bStarted = false;
		m_pTransport->Stop();
	}
}
//----------------------------------------------------------------------------
void CDiameterServer::Release()
{
	OCS_STACK_TRACE();
	delete this;
}
//----------------------------------------------------------------------------
bool CDiameterServer::Listen(unsigned short _Port, const char *_Address)
{
	OCS_STACK_TRACE();
	if(m_bStarted)
	{
		StopInput();
		Stop();
	}

	Start();

	return (0 == m_pTransport->Start(_Address, _Port, 99, this));
}
//----------------------------------------------------------------------------
bool CDiameterServer::OnConnect(int _ClientID, std::string _IP, int port)
{
	OCS_STACK_TRACE();

	int count = 0;

	{
		AutoWriteLocker_ locker(m_Mutex);

		MAP_CLIENTS::iterator it = m_mapClients.find(_ClientID);
		if(it != m_mapClients.end())
			return false; //must not enter

		it = m_mapClients.insert(m_mapClients.lower_bound(_ClientID), MAP_CLIENTS::value_type(_ClientID, ClientInfo()));
		it->second.ClientID = _ClientID;
		it->second.ipAddressStr = _IP;
		it->second.port = port;
		it->second.lastActionPoint = nowMSec();

		count = static_cast<int>(m_mapClients.size());
	}

	{
		AutoWriteLocker_ locker(m_OverloadMutex);

		MAP_INTBOOL::iterator it2 = m_mapOverload.find(_ClientID);
		if(it2 != m_mapOverload.end())
			return false;//must not enter

		m_mapOverload.insert(m_mapOverload.lower_bound(_ClientID), MAP_INTBOOL::value_type(_ClientID, false));
	}

	m_pDiameterEvents->OnReadyConnection(count, _ClientID);
	return true;
}
//----------------------------------------------------------------------------
void CDiameterServer::OnBreak(int _ClientID, DiamDisconnectionReason reason)
{
	OCS_STACK_TRACE();
	int count = 0;

	{
		AutoReadLocker_ locker(m_Mutex);
		count = static_cast<int>(m_mapClients.size());
		//MAP_CLIENTS::iterator it = m_mapClients.find(_ClientID);
		//if(it != m_mapClients.end())
		//	--Count;
	}

	m_pDiameterEvents->OnBreakConnection(count - 1, _ClientID, reason);

	{
		AutoWriteLocker_ locker(m_Mutex);
		MAP_CLIENTS::iterator it = m_mapClients.find(_ClientID);
		if(it != m_mapClients.end())
			m_mapClients.erase(it);
		//int Count = static_cast<int>(m_mapClients.size());
	}

	if(m_pSessionManager)
		m_pSessionManager->EraseSessions(_ClientID);

	{
		AutoWriteLocker_ locker(m_OverloadMutex);
		MAP_INTBOOL::iterator it2 = m_mapOverload.find(_ClientID);
		if(it2 != m_mapOverload.end())
			m_mapOverload.erase(it2);
	}
}
//----------------------------------------------------------------------------
void CDiameterServer::OnEmptyQueue(int _ClientID)
{
	OCS_STACK_TRACE();
	/*/
	std::string IP;
	m_Mutex.acquire();
	MAP_CLIENTS::iterator it = m_mapClients.find(_ClientID);
	if (it == m_mapClients.end())
	{
		m_Mutex.release();
		return;
	}

	IP = it->second.IpAddress;
	m_Mutex.release();
	//*/

	AutoWriteLocker_ locker(m_OverloadMutex);

	MAP_INTBOOL::iterator it2 = m_mapOverload.find(_ClientID);
	if(it2 == m_mapOverload.end())
		return;

	if(it2->second == true)
	{
		it2->second = false;
		m_pDiameterEvents->OnEmptyQueue(_ClientID);
	}
}
//----------------------------------------------------------------------------
bool CDiameterServer::HandleDefaultMessages(int _ClientID, unsigned char *_Data, unsigned int _Len, const CDiameterHeader &diamHeader)
{
	OCS_STACK_TRACE();
	//CRawDiameterMessage *pMessage = 0;

	switch(diamHeader.GetCommandCode())
	{
	case 257: // CER
	case 280: // DWR
		break;
	default:
		return false;
	}

	CRawDiameterMessage *pMessage = new CRawDiameterMessage(diamHeader);
	IAVP *pAVP = pMessage->GetAVPs();
	if(!pAVP)
	{
		delete pMessage;
		return true;
	}

	int decodeResult = m_pDecoder->DecodeAVPS(_Data, _Len, pAVP);
	if(0 != decodeResult)
	{
		delete pMessage;
		return true;
	}

	IAVP *originHost = NULL;

	switch(diamHeader.GetCommandCode())
	{
	case 257: //CER
		originHost = pAVP->GetAVPByCode(264);
		if(originHost)
		{
			//unsigned int dataSize;
			//const char *data = (const char *)originHost->GetData(dataSize);
		}

		SendCEA(_ClientID, pMessage, _Data, _Len);
		break;

	case 280://DWR
		SendDWA(_ClientID, pMessage, _Data, _Len);
		break;

	case 282: //DPA
		// todo: count client answers here
		break;
	}

	delete pMessage;
	return true;
}
//----------------------------------------------------------------------------
void CDiameterServer::OnReceive(int _ClientID, unsigned char *_Data, unsigned int _Len)
{
	OCS_STACK_TRACE();
	//CRawDiameterMessage *pMessage = new CRawDiameterMessage();
	//int decodeResult = m_pDecoder->Decode(_Data, _Len, pMessage);

	CDiameterHeader diamHeader;
	unsigned int    diamHeaderSize = -1;

	int decodeResult = m_pDecoder->DecodeHeader(_Data, _Len, &diamHeader, &diamHeaderSize);

	if(decodeResult != 0)
	{
		return;
	}

	if((diamHeader.GetCommandFlags() & 0xA0) != 0x80)  // RPETrrrr: for request bits must be 1?0?????
		return;

	{
		AutoWriteLocker_ locker(m_Mutex);

		MAP_CLIENTS::iterator it = m_mapClients.find(_ClientID);
		if(it == m_mapClients.end())
			return; //must not enter

		it->second.lastActionPoint = nowMSec();
	}

	if(HandleDefaultMessages(_ClientID, _Data + diamHeaderSize, _Len - diamHeaderSize, diamHeader))
		return;

	if(!m_pCCREvents)
		return;

	switch(diamHeader.GetCommandCode())
	{
	case 272:  // CCR/CCA
		{

			CRawDiameterMessage *pMessage   = new CRawDiameterMessage(diamHeader);
			IAVP *pAVP = pMessage->GetAVPs();
			if(!pAVP)
			{
				delete pMessage;
				return;
			}

			decodeResult = m_pDecoder->DecodeAVPS(_Data + diamHeaderSize, _Len - diamHeaderSize, pAVP);
			if(0 != decodeResult)
			{
				delete pMessage;
				return;
			}

			CCCRMessageImpl *pCCRMessage = new CCCRMessageImpl(_Data, _Len, pMessage);

			bool flagSkipPacket;
			if(m_pSessionManager)
				m_pSessionManager->OnRequestMessage(pMessage, _ClientID, &flagSkipPacket);
			if(!flagSkipPacket)
			{
				if(CheckDeliver(_ClientID, pCCRMessage))
				{
					m_pCCREvents->OnReceiveCCR(pCCRMessage, _ClientID);
				}
			}

			delete pCCRMessage;
			return;
		}
		break;

	//default:
	//	return;
	}
}
//----------------------------------------------------------------------------
#ifdef COREDIAM_USE_OLD_SEND
/*/
SendResult CDiameterServer::Send(IDiameterMessage *_pMessage, unsigned long *_sendQueueSize)
{
	VTUNE_TASK_BEGIN(CoreDiam::profiling::hSendTask);
	if(!m_bStarted)
	{
		VTUNE_TASK_END();
		return NOT_STARTED;
	}

	unsigned char Buffer[65535];
	unsigned int  Len      = sizeof(Buffer)/sizeof(unsigned char);
	unsigned int  PackSize = 0;

	int encodeResult = m_pCoder->Encode(Buffer, Len, PackSize, _pMessage);

	if(0 != encodeResult)
	{
		delete _pMessage;
		VTUNE_TASK_END();
		return CODER_ERROR;
	}

	int ClientId = m_pSessionManager->FindClient(_pMessage);
	if(ClientId < 0)
	{
		delete _pMessage;
		VTUNE_TASK_END();
		return TRANS_ERROR;
	}

	int ErrNo;
	SendResult Res = m_pTransport->Send(ClientId, Buffer, PackSize, ErrNo);
	if(Res == OK || Res == PUT_TO_QUEUE)
	{
		VTUNE_TASK_BEGIN(CoreDiam::profiling::hTimeoutsOnAnsTask);
		m_pSessionManager->OnAnswerMessage(_pMessage, ClientId);
		VTUNE_TASK_END();
	}

	delete _pMessage;

	if(Res == PUT_TO_QUEUE)
	{
		if(_sendQueueSize != NULL)
			*_sendQueueSize = ErrNo;

		m_OverloadMutex.acquire_read();
		MAP_INTBOOL::iterator it2 = m_mapOverload.find(ClientId);
		if (it2 == m_mapOverload.end())
		{
			m_OverloadMutex.release();
			VTUNE_TASK_END();
			return UNKNOWN_CONNECTION;
		}

		if((ErrNo >= (m_pTransport->getMaxQueueSize() * 0.8)) && !it2->second)
		{
			m_OverloadMutex.release();
			m_OverloadMutex.acquire_write();

			it2 = m_mapOverload.find(ClientId);
			if (it2 == m_mapOverload.end())
			{
				m_OverloadMutex.release();
				VTUNE_TASK_END();
				return UNKNOWN_CONNECTION;
			}

			if((ErrNo >= (m_pTransport->getMaxQueueSize() * 0.8)) && !it2->second)
			{
				it2->second = true;
				m_pDiameterEvents->OnOverload(ClientId);
			}
			//m_OverloadMutex.release();
		}

		m_OverloadMutex.release();
	}
	else
	{
		if(_sendQueueSize != NULL)
			*_sendQueueSize = 0;
	}

	VTUNE_TASK_END();
	return Res;
}
//*/
//----------------------------------------------------------------------------
#else  // COREDIAM_USE_OLD_SEND
//----------------------------------------------------------------------------
SendResult CDiameterServer::Send(IDiameterMessage *_pMessage, unsigned long *_sendQueueSize)
{
	OCS_STACK_TRACE();

	if(!m_bStarted)
	{
		return NOT_STARTED;
	}

	unsigned char Buffer[65535];
	unsigned int  Len      = sizeof(Buffer)/sizeof(unsigned char);
	unsigned int  PackSize = 0;

	int encodeResult = m_pCoder->Encode(Buffer, Len, PackSize, _pMessage);
	if(0 != encodeResult || !PackSize)
	{
		delete _pMessage;
		return CODER_ERROR;
	}

	int                ClientId = -1;
	SessionStateStatus sessionStateStatus = SESSION_STATE_STATUS_UNDEFINED;

	if(m_pSessionManager)
		m_pSessionManager->OnAnswerMessage(_pMessage, &ClientId, &sessionStateStatus);

	//if(sessionStateStatus == SESSION_STATE_STATUS_UNDEFINED)
	//else if(sessionStateStatus == SESSION_STATE_STATUS_CORRUPTED)

	if(ClientId < 0)
	{
		delete _pMessage;
		return TRANS_ERROR;
	}

	const char   *sid       = NULL;
	unsigned int  sidLength = 0;

	IAVP *avp = _pMessage->GetAVPs()->GetAVPByCode(avpcode::SESSION_ID);
	if(avp)
	{
		sid = (const char *)avp->GetData(sidLength);
		if(sid && sidLength)
		{
			//
		}
	}

	int        ErrNo;
	SendResult Res = m_pTransport->Send(ClientId, sid, sidLength, Buffer, PackSize, ErrNo);
	if(Res != OK && Res != PUT_TO_QUEUE)
	{
		// Rollback session update: delete session and 
		// Res: SEND_ERROR, UNKNOWN_CONNECTION
		if(m_pSessionManager)
			m_pSessionManager->EraseSession(_pMessage);
	}

	delete _pMessage;

	if(Res != PUT_TO_QUEUE)
	{
		if(_sendQueueSize != NULL)
			*_sendQueueSize = 0;
	}
	else
	{
		if(_sendQueueSize != NULL)
			*_sendQueueSize = ErrNo;

		bool isOverloaded = false;

		{
			AutoReadLocker_ locker(m_OverloadMutex);

			MAP_INTBOOL::iterator it2 = m_mapOverload.find(ClientId);
			if(it2 == m_mapOverload.end())
			{
				return UNKNOWN_CONNECTION;
			}

			isOverloaded = it2->second;
		}

		if((ErrNo >= (m_pTransport->getMaxQueueSize() * 0.8)) && !isOverloaded)
		{
			AutoWriteLocker_ locker(m_OverloadMutex);

			MAP_INTBOOL::iterator it2 = m_mapOverload.find(ClientId);
			if(it2 == m_mapOverload.end())
			{
				return UNKNOWN_CONNECTION;
			}

			if((ErrNo >= (m_pTransport->getMaxQueueSize() * 0.8)) && !it2->second)
			{
				it2->second = true;
				m_pDiameterEvents->OnOverload(ClientId);
			}
		}
	}
	return Res;
}
#endif  // COREDIAM_OLD_SEND
//----------------------------------------------------------------------------
SendResult CDiameterServer::Send(int _clientId, IDiameterMessage* _pMessage, unsigned long *_sendQueueSize)
{
	OCS_STACK_TRACE();

	if(!m_bStarted)
	{
		return NOT_STARTED;
	}

	unsigned char Buffer[65535];
	unsigned int  Len      = sizeof(Buffer)/sizeof(unsigned char);
	unsigned int  PackSize = 0;

	int encodeResult = m_pCoder->Encode(Buffer, Len, PackSize, _pMessage);
	if(0 != encodeResult || !PackSize)
	{
		delete _pMessage;
		return CODER_ERROR;
	}

	int                ClientId           = _clientId; //-1;
	SessionStateStatus sessionStateStatus = SESSION_STATE_STATUS_UNDEFINED;

	if(m_pSessionManager)
	{
		m_pSessionManager->OnAnswerMessage(_pMessage, &ClientId, &sessionStateStatus);
		if(_clientId >= 0 && _clientId != ClientId)
		{
			delete _pMessage;
			return TRANS_ERROR;
		}
	}

	//if(sessionStateStatus == SESSION_STATE_STATUS_UNDEFINED)
	//else if(sessionStateStatus == SESSION_STATE_STATUS_CORRUPTED)

	if(ClientId < 0)
	{
		delete _pMessage;
		return TRANS_ERROR;
	}

	const char   *sid       = NULL;
	unsigned int  sidLength = 0;

	IAVP *avp = _pMessage->GetAVPs()->GetAVPByCode(avpcode::SESSION_ID);
	if(avp)
	{
		sid = (const char *)avp->GetData(sidLength);
		if(sid && sidLength)
		{
			//
		}
	}

	int        ErrNo;
	SendResult Res = m_pTransport->Send(ClientId, sid, sidLength, Buffer, PackSize, ErrNo);
	if(Res != OK && Res != PUT_TO_QUEUE)
	{
		// Rollback session update: delete session and
		// Res: SEND_ERROR, UNKNOWN_CONNECTION
		if(m_pSessionManager)
			m_pSessionManager->EraseSession(_pMessage);
	}

	delete _pMessage;

	if(Res != PUT_TO_QUEUE)
	{
		if(_sendQueueSize != NULL)
			*_sendQueueSize = 0;
	}
	else
	{
		if(_sendQueueSize != NULL)
			*_sendQueueSize = ErrNo;

		bool isOverloaded = false;

		{
			AutoReadLocker_ locker(m_OverloadMutex);

			MAP_INTBOOL::iterator it2 = m_mapOverload.find(ClientId);
			if(it2 == m_mapOverload.end())
			{
				return UNKNOWN_CONNECTION;
			}

			isOverloaded = it2->second;
		}

		if((ErrNo >= (m_pTransport->getMaxQueueSize() * 0.8)) && !isOverloaded)
		{
			AutoWriteLocker_ locker(m_OverloadMutex);

			MAP_INTBOOL::iterator it2 = m_mapOverload.find(ClientId);
			if(it2 == m_mapOverload.end())
			{
				return UNKNOWN_CONNECTION;
			}

			if((ErrNo >= (m_pTransport->getMaxQueueSize() * 0.8)) && !it2->second)
			{
				it2->second = true;
				m_pDiameterEvents->OnOverload(ClientId);
			}
		}
	}
	return Res;
}
//----------------------------------------------------------------------------
void CDiameterServer::SendCEA(int _ClientID, IDiameterMessage* _pCERMessage, unsigned char *_Data, unsigned int _Len)
{
	OCS_STACK_TRACE();
	IDiameterMessage* pCEAMessage = CreateCEA(_pCERMessage);
	if (!pCEAMessage)
		return;

	unsigned char Buffer[65535];
	unsigned int Len = sizeof(Buffer)/sizeof(unsigned char);
	unsigned int PackSize = 0;
	if(m_pCoder->Encode(Buffer, Len, PackSize, pCEAMessage) == 0)
	{
		


		int ErrNo;
		m_pTransport->Send(_ClientID, NULL, 0, Buffer, PackSize, ErrNo);
	}

	delete pCEAMessage;
}
//----------------------------------------------------------------------------
void CDiameterServer::SendDWA(int _ClientID, IDiameterMessage* _pDWRMessage, unsigned char *_Data, unsigned int _Len)
{
	OCS_STACK_TRACE();
	IDiameterMessage* pDWAMessage = CreateDWA(_pDWRMessage);
	if (!pDWAMessage)
		return;

	unsigned char Buffer[65535];
	unsigned int Len = sizeof(Buffer)/sizeof(unsigned char);
	unsigned int PackSize = 0;
	if (m_pCoder->Encode(Buffer, Len, PackSize, pDWAMessage) == 0)
	{
		int ErrNo;
		m_pTransport->Send(_ClientID, NULL, 0, Buffer, PackSize, ErrNo);
	}

	delete pDWAMessage;
}
//----------------------------------------------------------------------------
void CDiameterServer::SendDPR(int _ClientID)
{
	OCS_STACK_TRACE();
	IDiameterMessage* pDPRMessage = CreateDPR();
	if (!pDPRMessage)
		return;

	unsigned char Buffer[65535];
	unsigned int Len = sizeof(Buffer)/sizeof(unsigned char);
	unsigned int PackSize = 0;
	if (m_pCoder->Encode(Buffer, Len, PackSize, pDPRMessage) == 0)
	{
		int ErrNo;
		m_pTransport->Send(_ClientID, NULL, 0, Buffer, PackSize, ErrNo);
	}

	delete pDPRMessage;
}
//----------------------------------------------------------------------------
IDiameterMessage * CDiameterServer::CreateCEA(IDiameterMessage *_pCERMessage)
{
	OCS_STACK_TRACE();
	IDiameterMessage* pMessage = new CRawDiameterMessage();

	IDiameterHeader* pHeader = pMessage->GetHeader();
	if (!pHeader)
	{
		delete pMessage;
		return NULL;
	}

	pHeader->SetApplicationID(0);
	pHeader->SetCommandCode(257);
	pHeader->SetCommandFlags(0x00);
	pHeader->SetEndToEndIdentifier(_pCERMessage->GetHeader()->GetEndToEndIdentifier());
	pHeader->SetHopByHopIdentifier(_pCERMessage->GetHeader()->GetHopByHopIdentifier());

	IAVP *pAVP = pMessage->GetAVPs();
	if(!pAVP)
	{
		delete pMessage;
		return NULL;
	}

	CDiameterAVP *AVP = new CDiameterAVP(IAVP::BASIC, 2001, 268, 0x40, 10415); // Result-Code
	pAVP->SetAVP(AVP);

	AVP = new CDiameterAVP(IAVP::BASIC, (const unsigned char *)m_Host.c_str(), static_cast<unsigned int>(m_Host.size()), 264, 0x40, 10415); // Origin-Host
	pAVP->SetAVP(AVP);

	AVP = new CDiameterAVP(IAVP::BASIC, (const unsigned char *)m_Realm.c_str(), static_cast<unsigned int>(m_Realm.size()), 296, 0x40, 10415); // Origin-Realm
	pAVP->SetAVP(AVP);

	unsigned char HostIPAddress[] = {0x00, 0x01, 0x00, 0x00, 0x00, 0x00};
	uint32_t host = m_pTransport->getHost();
	const unsigned char *pByte = (const unsigned char *)&host;
	for(int i = 0; i < 4; i++)
		HostIPAddress[i + 2] = *pByte++;
	AVP = new CDiameterAVP(IAVP::BASIC, HostIPAddress, 6, 257, 0x40, 10415); // Host-IP-Address
	pAVP->SetAVP(AVP);

	AVP = new CDiameterAVP(IAVP::BASIC, 10415, 266, 0x40, 10415); // Vendor-Id
	pAVP->SetAVP(AVP);

	AVP = new CDiameterAVP(IAVP::BASIC, (const unsigned char *)("DiameterServer"), static_cast<unsigned int>(strlen("DiameterServer")), 269, 0x40, 10415);//Product-Name
	pAVP->SetAVP(AVP);

	AVP = new CDiameterAVP(IAVP::BASIC, 0, 278, 0x40, 10415);  // Origin-State-Id
	pAVP->SetAVP(AVP);

	AVP = new CDiameterAVP(IAVP::BASIC, 4, 258, 0x40, 10415);  // Auth-Application-Id: 4 == Diameter Credit Control
	pAVP->SetAVP(AVP);

	/*/
	Auth-Application-Id *   258
	Error-Message        [] 281
	Failed-AVP          *[] 279
	Firmware-Revision    [] 267
	Host-IP-Address     Gmb 257
	Origin-Host         Gmb 264
	Origin-Realm        Gmb 296
	Origin-State-Id      [] 278
	Product-Name        Gmb 269
	Result-Code         Gmb 268
	Supported-Vendor-Id *[] 265
	Vendor-Id           Gmb 266
	Vendor-Specific-Application-Id *[] 260
	/*/

	return pMessage;
}
//----------------------------------------------------------------------------
IDiameterMessage * CDiameterServer::CreateDWA(IDiameterMessage* _pDWRMessage)
{
	OCS_STACK_TRACE();
	IDiameterMessage* pMessage = new CRawDiameterMessage();

	IDiameterHeader* pHeader = pMessage->GetHeader();
	if (!pHeader)
	{
		delete pMessage;
		return NULL;
	}

	pHeader->SetApplicationID(0);
	pHeader->SetCommandCode(280);
	pHeader->SetCommandFlags(0x00);
	pHeader->SetEndToEndIdentifier(_pDWRMessage->GetHeader()->GetEndToEndIdentifier());
	pHeader->SetHopByHopIdentifier(_pDWRMessage->GetHeader()->GetHopByHopIdentifier());

	IAVP* pAVP = pMessage->GetAVPs();
	if (!pAVP)
	{
		delete pMessage;
		return NULL;
	}

	CDiameterAVP* AVP = new CDiameterAVP(IAVP::BASIC, 2001, 268, 0x40, 10415);//Result-Code
	pAVP->SetAVP(AVP);

	AVP = new CDiameterAVP(IAVP::BASIC, (const unsigned char*)("test-host"), static_cast<unsigned int>(strlen("test-host")), 264, 0x40, 10415);//Origin-Host
	pAVP->SetAVP(AVP);

	AVP = new CDiameterAVP(IAVP::BASIC, (const unsigned char*)("test-realm"), static_cast<unsigned int>(strlen("test-realm")), 296, 0x40, 10415);//Origin-Realm
	pAVP->SetAVP(AVP);

	return pMessage;
}
//----------------------------------------------------------------------------
IDiameterMessage * CDiameterServer::CreateDPR()
{
	OCS_STACK_TRACE();
	IDiameterMessage* pMessage = new CRawDiameterMessage();

	IDiameterHeader* pHeader = pMessage->GetHeader();
	if (!pHeader)
	{
		delete pMessage;
		return NULL;
	}

	pHeader->SetApplicationID(4);
	pHeader->SetCommandCode(282);
	pHeader->SetCommandFlags(0x80); // 'R' (Request) flag on
	// voluntarily use zero identifiers here, since it's the only call to server before restart
	pHeader->SetEndToEndIdentifier(0);
	pHeader->SetHopByHopIdentifier(0);

	IAVP* pAVP = pMessage->GetAVPs();
	if (!pAVP)
	{
		delete pMessage;
		return NULL;
	}

	// Origin-Host
	CDiameterAVP *AVP = new CDiameterAVP(IAVP::BASIC, (const unsigned char*)("test-host"), static_cast<unsigned int>(strlen("test-host")), 264, 0x40, 10415);//Origin-Host
	pAVP->SetAVP(AVP);

	// Origin-Realm
	AVP = new CDiameterAVP(IAVP::BASIC, (const unsigned char*)("test-realm"), static_cast<unsigned int>(strlen("test-realm")), 296, 0x40, 10415);//Origin-Realm
	pAVP->SetAVP(AVP);

	// Disconnect-Cause
	AVP = new CDiameterAVP(IAVP::BASIC, CoreDiam::REBOOTING, 273, 0x40, 10415); // vendor id is not used (flag 0x40 means that AVP is mandatory)
	pAVP->SetAVP(AVP);

	return pMessage;
}
//----------------------------------------------------------------------------
bool CDiameterServer::CheckDeliver(int _ClientID, CCCRMessageImpl* pCCRMessage)
{
	OCS_STACK_TRACE();
	AutoReadLocker_ locker(m_OverloadMutex);

	if(!m_FlagReceiveOn)
		return false; // don't accept message when m_FlagReceivingOn is set to false

	MAP_INTBOOL::iterator it2 = m_mapOverload.find(_ClientID);
	if(it2 != m_mapOverload.end())
	{
		if(it2->second && pCCRMessage->GetRequestType() == INITIAL)
			return false;
	}

	return true;
}
//----------------------------------------------------------------------------
bool CDiameterServer::GetClientInfo(int _clientId, DiamClientInfo *_info)
{
	OCS_STACK_TRACE();
	if(_info == NULL)
		return false;

	//m_Mutex.acquire();
	AutoWriteLocker_ locker(m_Mutex);

	{
		MAP_CLIENTS::iterator it = m_mapClients.find(_clientId);
		if(it == m_mapClients.end())
		{
			_info->id         = _clientId;
			_info->address[0] = '\0';
			_info->ipInt32    = 0x00000000;
			_info->ipStr[0]   = '\0';
			_info->port       = 0;

			return false;
		}

		_info->id         = _clientId;
		_info->address[0] = '\0';
		_info->ipInt32    = 0x00000000;
		strncpy(_info->ipStr, it->second.ipAddressStr.c_str(), 16); //_info->ipStr      = it->second.IpAddress;
		_info->port       = it->second.port;
	}

	return true;
}
//----------------------------------------------------------------------------
void CDiameterServer::SetReceiving(const bool &FlagReceiveOn)
{
	OCS_STACK_TRACE();
	AutoWriteLocker_ locker(m_mtx_FlagReceiveOn);
	m_FlagReceiveOn = FlagReceiveOn;
}
//----------------------------------------------------------------------------
void CDiameterServer::Disconnect()
{
	OCS_STACK_TRACE();

	{
		AutoWriteLocker_ locker(m_Mutex);
		for(MAP_CLIENTS::iterator i = m_mapClients.begin(); i != m_mapClients.end(); i++)
		{
			SendDPR(i->first);
		}
	}


	if(m_pSessionManager)
	{
		while(!m_pSessionManager->Disconnected(m_mapClients.size()))
		{
			sleep(1);
		}
		m_pSessionManager->CleanDisconnectMap();
	}
}
//----------------------------------------------------------------------------
bool CDiameterServer::OnNoDataKeepClient(int _ClientID)
{
	OCS_STACK_TRACE();
	AutoWriteLocker_ locker(m_Mutex);

	MAP_CLIENTS::iterator it = m_mapClients.find(_ClientID);
	if(it == m_mapClients.end())
		return false;

	if(it->second.lastActionPoint + m_InactivityTimeout < nowMSec())
		return false;

	return true;
}
//----------------------------------------------------------------------------
size_t CDiameterServer::GetTimerCount()
{
	if(!m_pSessionManager)
		return 0;
	return m_pSessionManager->GetTimerCount();
}
//----------------------------------------------------------------------------
size_t CDiameterServer::GetTimerCreatedCount()
{
	if(!m_pSessionManager)
		return 0;
	return m_pSessionManager->GetTimerCreatedCount();
}
//----------------------------------------------------------------------------
size_t CDiameterServer::GetTimerCancelledCount()
{
	if(!m_pSessionManager)
		return 0;
	return m_pSessionManager->GetTimerCancelledCount();
}
//----------------------------------------------------------------------------
size_t CDiameterServer::GetTimerTriggeredCount()
{
	if(!m_pSessionManager)
		return 0;
	return m_pSessionManager->GetTimerTriggeredCount();
}

//----------------------------------------------------------------------------
void CDiameterServer::OnSent(int _clientID, const char *_sid, size_t _sidLen, int _sendResult)
{
	if(m_pCCREvents && _sid && _sidLen)
		m_pCCREvents->OnSentCCA(_sid, _sidLen, _clientID, _sendResult);
}

}
