#include <arpa/inet.h>
#include "DiameterClient.h"

namespace CoreDiam
{

CDiameterClient::CDiameterClient(IDiameterEvents* _pDiameterEvents, ICCAEvents* _pCCAEvents, ISettings* _pSettings, int _QueueSize, bool isProxy) :
	m_pDiameterEvents(_pDiameterEvents),
	m_pCCAEvents(_pCCAEvents),
	m_pSettings(_pSettings),
	m_bOverload(false),
	m_isProxy(isProxy)
{
	int  queueSize       = SENDQUEUESIZE;
	int  rcvBufferSize   = 0;
	int  sndBufferSize   = 0;
	bool isBlocking      = true;

	int  sessionTimeout  = 0;
	int  responseTimeout = 0;

	if (m_pSettings)
	{
		unsigned int Size = 0;
		const unsigned char* Data = NULL;

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
		if (Data && Size == sizeof(int))
			sessionTimeout = *(int*)Data;

		m_pSettings->TakeSetting("ResponseTimeout", Data, Size);
		if (Data && Size == sizeof(int))
			responseTimeout = *(int*)Data;
	}

	m_pTransport      = new CClientTransport(this, queueSize, rcvBufferSize, sndBufferSize, isBlocking);

	m_pSessionManager = new CSessionManager(_pDiameterEvents);
	m_pSessionManager->SetTimeout(sessionTimeout, responseTimeout);

	m_pDecoder = new CDiameterDecoder();
	m_pCoder   = new CDiameterCoder();

	InitData();
}
//----------------------------------------------------------------------------
CDiameterClient::~CDiameterClient()
{

	Stop();
	m_pTransport->Release();
	delete m_pSessionManager;
	delete m_pCoder;
	delete m_pDecoder;
}
//----------------------------------------------------------------------------
void CDiameterClient::InitData()
{
	void *thisVoid   = reinterpret_cast<void *>(this);
	void **thisVoid2 = &thisVoid;

	m_SessCounterHi = *reinterpret_cast<int *>(thisVoid2);

	m_SessCounterLo = 0;
	m_HopByHopId    = 0;
	m_EndToEndId    = time(NULL);
}
//----------------------------------------------------------------------------
void CDiameterClient::Start()
{
	m_bStarted = false;
}
//----------------------------------------------------------------------------
void CDiameterClient::Stop()
{
	m_bStarted = false;
	m_pTransport->Stop();
}
//----------------------------------------------------------------------------
void CDiameterClient::Release()
{
	delete this;
}
//----------------------------------------------------------------------------
bool CDiameterClient::Connect(const char *_Addresses, const char* originHost, const char* originRealm, const char* dstRealm)
{
	if(!originHost || ! originRealm)
		return false;

	originHostStr = originHost;
	originRealmStr = originRealm;
	destinationRealmStr = dstRealm;

	const char* C = strstr(_Addresses, ":");
	if(!C)
		return false;

	hostAddress.assign(_Addresses, C);

	Start();
	if(m_pTransport->Start(_Addresses, this) == 0)
		return true;
	return false;
}
//----------------------------------------------------------------------------
bool CDiameterClient::OnConnect(int, std::string _IP, int port)
{

	SendCER();
	return true;
}
//----------------------------------------------------------------------------
void CDiameterClient::OnBreak(int, CoreDiam::DiamDisconnectionReason reason)
{
	m_pDiameterEvents->OnBreakConnection(0, 0, reason);

	m_Mutex.lock();
	m_pSessionManager->EraseSessions(0);
	m_Mutex.unlock();

	m_OverloadMutex.lock();
	m_bOverload = false;
	m_OverloadMutex.unlock();
}
//----------------------------------------------------------------------------
void CDiameterClient::OnEmptyQueue(int)
{
	m_OverloadMutex.lock();
	if (m_bOverload)
	{
		m_bOverload = false;
		m_pDiameterEvents->OnEmptyQueue(0);
	}
	m_OverloadMutex.unlock();
}
//----------------------------------------------------------------------------
void CDiameterClient::OnReceive(int, unsigned char *_Data, unsigned int _Len)
{
	//ACE_Guard<ACE_Recursive_Thread_Mutex> Guard(m_ClientMutex);

	CRawDiameterMessage *pMessage = new CRawDiameterMessage();
	if (m_pDecoder->Decode(_Data, _Len, pMessage) == 0)
	{
		switch (pMessage->GetHeader()->GetCommandCode())
		{
		case 272://CCA
			{
				m_pSessionManager->OnAnswerMessage(pMessage, 0);
				if (m_pCCAEvents)
				{
					CCCAMessageImpl* pCCAMessage = new CCCAMessageImpl(_Data, _Len, pMessage);
					m_pCCAEvents->OnReceiveCCA(pCCAMessage, 0);
					delete pCCAMessage;
					return;
				}
			}
		case 257://CEA
			{
				if (!m_bStarted)
				{
					m_bStarted = true;
					m_pDiameterEvents->OnReadyConnection(1, 0);
				}
				delete pMessage;
				return;
			}
		case 280://DWA
			{
				delete pMessage;
				return;
			}
		}
		m_pDiameterEvents->OnReceive(pMessage, 0);
	}
	delete pMessage;
}
//----------------------------------------------------------------------------
void CDiameterClient::OnSent(int, const char *_sid, size_t _sidLen, int _sendResult)
{
	//
}
//----------------------------------------------------------------------------
SendResult CDiameterClient::Send(IDiameterMessage* _pMessage, unsigned long *_sendQueueSize)
{
	if (!m_bStarted)
		return NOT_STARTED;

	FillMessage(_pMessage);

	unsigned char Buffer[65535];
	unsigned int  Len      = sizeof(Buffer)/sizeof(unsigned char);
	unsigned int  PackSize = 0;

	if(!m_pCoder->Encode(Buffer, Len, PackSize, _pMessage))
	{
		int ClientId = SelectClient(_pMessage); // client can be only one
		if(ClientId < 0)
		{
			delete _pMessage;
			return TRANS_ERROR;
		}

		int ErrNo;
		SendResult Res = m_pTransport->Send(ClientId, NULL, 0, Buffer, PackSize, ErrNo);
		bool flagSkipPacket;
		if(Res == OK || Res == PUT_TO_QUEUE)
{
			m_pSessionManager->OnRequestMessage(_pMessage, ClientId, &flagSkipPacket);
		}

		delete _pMessage;

		if(Res == PUT_TO_QUEUE)
		{
			if(_sendQueueSize != NULL)
				*_sendQueueSize = ErrNo;

			m_OverloadMutex.lock();
			if((ErrNo >= (m_pTransport->getMaxQueueSize() * 0.8)) && !m_bOverload)
{
				m_OverloadMutex.unlock();
				m_OverloadMutex.lock();

				if((ErrNo >= (m_pTransport->getMaxQueueSize() * 0.8)) && !m_bOverload)
	{
					m_bOverload = true;
					m_pDiameterEvents->OnOverload(ClientId);
				}
	}

			m_OverloadMutex.unlock();
		}
		else
	{
			if(_sendQueueSize != NULL)
				*_sendQueueSize = 0;
	}

		return Res;
}

	delete _pMessage;
	return CODER_ERROR;
}
//----------------------------------------------------------------------------
void CDiameterClient::SendCER()
{

	IDiameterMessage* pCERMessage = CreateCER();
	if (!pCERMessage)
		return;

	unsigned char Buffer[65535];
	unsigned int Len = sizeof(Buffer)/sizeof(unsigned char);
	unsigned int PackSize = 0;
	if (m_pCoder->Encode(Buffer, Len, PackSize, pCERMessage) == 0)
	{
		int ErrNo;
		m_pTransport->Send(0, NULL, 0, Buffer, PackSize, ErrNo);
	}

	delete pCERMessage;
}
//----------------------------------------------------------------------------
IDiameterMessage* CDiameterClient::CreateCER()
{

	IDiameterMessage* pMessage = new CRawDiameterMessage();

	IDiameterHeader* pHeader = pMessage->GetHeader();
	if (!pHeader)
	{
		delete pMessage;
		return NULL;
	}

	pHeader->SetApplicationID(0);
	pHeader->SetCommandCode(257);
	pHeader->SetCommandFlags(0x80);
	pHeader->SetEndToEndIdentifier(m_EndToEndId);
	pHeader->SetHopByHopIdentifier(m_HopByHopId++);

	IAVP* pAVP = pMessage->GetAVPs();
	if (!pAVP)
	{
		delete pMessage;
		return NULL;
	}

	CDiameterAVP* AVP;

	AVP = new CDiameterAVP(IAVP::BASIC, (const unsigned char*)originHostStr.c_str(), static_cast<unsigned int>(originHostStr.size()), 264, 0x40, 10415);//Origin-Host
	pAVP->SetAVP(AVP);

	AVP = new CDiameterAVP(IAVP::BASIC, (const unsigned char*)originRealmStr.c_str(), static_cast<unsigned int>(originRealmStr.size()), 296, 0x40, 10415);//Origin-Realm
	pAVP->SetAVP(AVP);

	size_t len = 6;
	in_addr maskAddr;
	inet_aton(hostAddress.c_str(), &maskAddr);
	unsigned char* Buf = new unsigned char[len];
	Buf[0] = 0x00;
	Buf[1] = 0x01;
	memcpy(Buf+2, &maskAddr.s_addr, len);
	AVP = new CDiameterAVP(IAVP::BASIC, Buf, len, 257, 0x40, 10415);//Host-IP-Address
	pAVP->SetAVP(AVP);

	AVP = new CDiameterAVP(IAVP::BASIC, 10415, 266, 0x40, 10415);//Vendor-Id
	pAVP->SetAVP(AVP);

	AVP = new CDiameterAVP(IAVP::BASIC, 4, 258, 0x40, 10415);//Auth-Application-Id
	pAVP->SetAVP(AVP);


	AVP = new CDiameterAVP(IAVP::BASIC, (const unsigned char*)("DiameterClient"), static_cast<unsigned int>(strlen("DiameterClient")), 269, 0x00, 10415);//Product-Name
	pAVP->SetAVP(AVP);


	return pMessage;
}
//----------------------------------------------------------------------------
void CDiameterClient::SendDPR()
{

	IDiameterMessage* pCERMessage = CreateDPR();
	if (!pCERMessage)
		return;

	unsigned char Buffer[65535];
	unsigned int Len = sizeof(Buffer)/sizeof(unsigned char);
	unsigned int PackSize = 0;
	if (m_pCoder->Encode(Buffer, Len, PackSize, pCERMessage) == 0)
	{
		int ErrNo;
		m_pTransport->Send(0, NULL, 0, Buffer, PackSize, ErrNo);
	}

	delete pCERMessage;
}

    //----------------------------------------------------------------------------
    void CDiameterClient::SendDWR()
    {
        IDiameterMessage* pCERMessage = CreateDWR();
        if (!pCERMessage)
            return;

        unsigned char Buffer[65535];
        unsigned int Len = sizeof(Buffer)/sizeof(unsigned char);
        unsigned int PackSize = 0;
        if (m_pCoder->Encode(Buffer, Len, PackSize, pCERMessage) == 0)
        {
            int ErrNo;
            m_pTransport->Send(0, NULL, 0, Buffer, PackSize, ErrNo);
        }

        delete pCERMessage;
    }
//----------------------------------------------------------------------------
IDiameterMessage* CDiameterClient::CreateDPR()
{

	IDiameterMessage* pMessage = new CRawDiameterMessage();

	IDiameterHeader* pHeader = pMessage->GetHeader();
	if (!pHeader)
	{
		delete pMessage;
		return NULL;
	}

	pHeader->SetApplicationID(0);
	pHeader->SetCommandCode(282); // Diameter DPR (Disconnect-Peer-Request) code
	pHeader->SetCommandFlags(0x80); // 'R' (Request) flag
	pHeader->SetEndToEndIdentifier(m_EndToEndId);
	pHeader->SetHopByHopIdentifier(m_HopByHopId++);

	IAVP* pAVP = pMessage->GetAVPs();
	if (!pAVP)
	{
		delete pMessage;
		return NULL;
	}

	CDiameterAVP* AVP;

	AVP = new CDiameterAVP(IAVP::BASIC, (const unsigned char*)originHostStr.c_str(), static_cast<unsigned int>(originHostStr.size()), 264, 0x40, 10415);//Origin-Host
	pAVP->SetAVP(AVP);

	AVP = new CDiameterAVP(IAVP::BASIC, (const unsigned char*)originRealmStr.c_str(), static_cast<unsigned int>(originRealmStr.size()), 296, 0x40, 10415);//Origin-Realm
	pAVP->SetAVP(AVP);

	AVP = new CDiameterAVP(IAVP::BASIC, 0, 273, 0x40, 10415); // AVP Disconnect-Cause (273) code REBOOTING (0) MUST include flag (0x40)
	pAVP->SetAVP(AVP);

	return pMessage;
}

IDiameterMessage* CDiameterClient::CreateDWR()
{
    IDiameterMessage* pMessage = new CRawDiameterMessage();

    IDiameterHeader* pHeader = pMessage->GetHeader();
    if (!pHeader)
    {
        delete pMessage;
        return NULL;
    }

    pHeader->SetApplicationID(0);
    pHeader->SetCommandCode(280); // Diameter DWR (Diameter-Watchdog-Request) code
    pHeader->SetCommandFlags(0x80); // 'R' (Request) flag
    pHeader->SetEndToEndIdentifier(m_EndToEndId);
    pHeader->SetHopByHopIdentifier(m_HopByHopId++);

    IAVP* pAVP = pMessage->GetAVPs();
    if (!pAVP)
    {
        delete pMessage;
        return NULL;
    }

    CDiameterAVP* AVP;

    AVP = new CDiameterAVP(IAVP::BASIC, (const unsigned char*)originHostStr.c_str(), static_cast<unsigned int>(originHostStr.size()), 264, 0x40, 10415);//Origin-Host
    pAVP->SetAVP(AVP);

    AVP = new CDiameterAVP(IAVP::BASIC, (const unsigned char*)originRealmStr.c_str(), static_cast<unsigned int>(originRealmStr.size()), 296, 0x40, 10415);//Origin-Realm
    pAVP->SetAVP(AVP);



    return pMessage;
    }
//----------------------------------------------------------------------------
inline void CDiameterClient::FillMessage(IDiameterMessage* _pMessage)
{

	IDiameterHeader* pHeader = _pMessage->GetHeader();
	if (pHeader->GetApplicationID() == -1)
		pHeader->SetApplicationID(4);
	if (pHeader->GetCommandCode() == -1)
		pHeader->SetCommandCode(272);
	pHeader->SetCommandFlags(m_isProxy?0xC0:0x80);
	if (pHeader->GetEndToEndIdentifier() == -1)
		pHeader->SetEndToEndIdentifier(m_EndToEndId);
	if (pHeader->GetHopByHopIdentifier() == -1)
		pHeader->SetHopByHopIdentifier(m_HopByHopId++);
}

//----------------------------------------------------------------------------
inline int CDiameterClient::SelectClient(IDiameterMessage* _pMessage)
{
	return 0;
}

bool CDiameterClient::OnNoDataKeepClient(int)
{
	return true;
}



};
