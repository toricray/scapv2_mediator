#include "CCAMessageImpl.h"

namespace CoreDiam
{

CCCAMessageImpl::CCCAMessageImpl() :
	m_SessionId(""),
	m_ErrorMessage(""),
	m_ResultCode(-1),
    m_ResultCodeExt(-1),
	m_OriginHost(""),
	m_OriginRealm(""),
	m_AuthApplicationId(-1),
	m_ServiceContextId(""),
	m_RequestNumber(-1),
	m_RequestType(INITIAL)
{
	m_pAVP = new CDiameterAVP(IAVP::BASE, NULL, 0, 0, 0x00, 0);
	m_pRawMessage = new CRawDiameterMessage(m_pAVP, new CDiameterHeader());

	m_pMSCCAVP = new CDiameterAVPGroup(IAVP::GROUPEDSET, 456);
	m_pAVP->SetAVP(m_pMSCCAVP);
}

CCCAMessageImpl::CCCAMessageImpl(CCCRMessage *_pCCRMessage, const char *originHost):
	m_ResultCode(2001),
    m_ResultCodeExt(-1),
	m_OriginHost(""),
	m_OriginRealm(""),
	m_ServiceContextId("")
{
	m_pAVP = new CDiameterAVP(IAVP::BASE, NULL, 0, 0, 0x00, 0);
	m_pRawMessage = new CRawDiameterMessage(m_pAVP, new CDiameterHeader(*(CDiameterHeader*)_pCCRMessage->GetHeader()));
	m_pRawMessage->GetHeader()->SetCommandFlags(0x00);

	m_SessionId = _pCCRMessage->GetSessionId();
	m_pAVP->SetAVP(new CDiameterAVP(IAVP::BASIC, (const unsigned char*)m_SessionId.c_str(), static_cast<unsigned int>(m_SessionId.size()), 263, 0x40, 10415));

	m_pAVP->SetAVP(new CDiameterAVP(IAVP::BASIC, m_ResultCode, 268, 0x40, 10415));
    m_pAVP->SetAVP(new CDiameterAVP(IAVP::BASIC, m_ResultCodeExt, 1067, 0x40, 193));

	if(originHost)
	{
		m_OriginHost.assign(originHost);
	}
	else
	{
		IAVP *pAVP = _pCCRMessage->GetAVPs()->GetAVPByCode(293);
		if(pAVP)
		{
			unsigned int Size;
			const unsigned char *Data = pAVP->GetData(Size);
			if(Data)
				m_OriginHost.assign((const char *)Data, Size);
		}
	}

	m_pAVP->SetAVP(new CDiameterAVP(IAVP::BASIC, (const unsigned char*)m_OriginHost.c_str(), static_cast<unsigned int>(m_OriginHost.size()), 264, 0x40, 10415));

	m_OriginRealm = _pCCRMessage->GetDestinationRealm();
	m_pAVP->SetAVP(new CDiameterAVP(IAVP::BASIC, (const unsigned char*)m_OriginRealm.c_str(), static_cast<unsigned int>(m_OriginRealm.size()), 296, 0x40, 10415));

	m_ServiceContextId = _pCCRMessage->GetServiceContextId();
	m_pAVP->SetAVP(new CDiameterAVP(IAVP::BASIC, (const unsigned char*)m_ServiceContextId.c_str(), static_cast<unsigned int>(m_ServiceContextId.size()), 461, 0x40, 10415));

	m_RequestType = _pCCRMessage->GetRequestType();
	m_pAVP->SetAVP(new CDiameterAVP(IAVP::BASIC, m_RequestType, 416, 0x40, 10415));

	m_RequestNumber = _pCCRMessage->GetRequestNumber();
	m_pAVP->SetAVP(new CDiameterAVP(IAVP::BASIC, m_RequestNumber, 415, 0x40, 10415));

	m_AuthApplicationId = _pCCRMessage->GetAuthApplicationId();
	m_pAVP->SetAVP(new CDiameterAVP(IAVP::BASIC, m_AuthApplicationId, 258, 0x40, 10415));
	
	m_pMSCCAVP = new CDiameterAVPGroup(IAVP::GROUPEDSET, 456);
	m_pAVP->SetAVP(m_pMSCCAVP);

	CMSCC *ccrMSCC = NULL;
	for(int msccIndex = 0; NULL != (ccrMSCC = _pCCRMessage->GetMSCC(msccIndex)); msccIndex++)
	{
		CMSCC *ccaMSCC = CreateMSCC(msccIndex);
		if(ccaMSCC != NULL)
		{
			if(-1 != ccrMSCC->GetRatingGroup())
				ccaMSCC->SetRatingGroup(ccrMSCC->GetRatingGroup());

			int sid = 0;
			for(int sidIndex = 0; -1 != (sid = ccrMSCC->GetServiceIdentifier(sidIndex)); sidIndex++)
				ccaMSCC->SetServiceIdentifier(sid, sidIndex);
		}
	}
}

CCCAMessageImpl::CCCAMessageImpl(const unsigned char* _Data, unsigned int _Size, IDiameterMessage* _pRawMessage) :
	m_SessionId(""),
	m_ErrorMessage(""),
	m_ResultCode(-1),
    m_ResultCodeExt(-1),
	m_OriginHost(""),
	m_OriginRealm(""),
	m_AuthApplicationId(-1),
	m_ServiceContextId(""),
	m_RequestNumber(-1),
	m_RequestType(INITIAL),
	m_pRawMessage(_pRawMessage)
{
	const unsigned char* AVPData;
	unsigned int AVPSize;
	
	m_pAVP = m_pRawMessage->GetAVPs();

	AVPData = GetAVPData(_Data, _Size, 263, AVPSize, true);
	if (AVPData)
		m_SessionId.assign((const char*)AVPData, AVPSize);


	AVPData = GetAVPData(_Data, _Size, 281, AVPSize, true);
	if (AVPData)
		m_ErrorMessage.assign((const char*)AVPData, AVPSize);

	
	AVPData = GetAVPData(_Data, _Size, 268, AVPSize, true);
	if (AVPData)
		m_ResultCode = ntohl(*(int*)AVPData);

    AVPData = GetAVPData(_Data, _Size, 1067, AVPSize, true);
    if (AVPData)
        m_ResultCodeExt = ntohl(*(int*)AVPData);

	AVPData = GetAVPData(_Data, _Size, 264, AVPSize, true);
	if (AVPData)
		m_OriginHost.assign((const char*)AVPData, AVPSize);
	
	AVPData = GetAVPData(_Data, _Size, 296, AVPSize, true);
	if (AVPData)
		m_OriginRealm.assign((const char*)AVPData, AVPSize);
	
	AVPData = GetAVPData(_Data, _Size, 258, AVPSize, true);
	if (AVPData)
		m_AuthApplicationId = ntohl(*(int*)AVPData);
	
	AVPData = GetAVPData(_Data, _Size, 461, AVPSize, true);
	if (AVPData)
		m_ServiceContextId.assign((const char*)AVPData, AVPSize);
	
	AVPData = GetAVPData(_Data, _Size, 416, AVPSize, true);
	if (AVPData)
		m_RequestType = (RequestType)ntohl(*(int*)AVPData);
	
	AVPData = GetAVPData(_Data, _Size, 415, AVPSize, true);
	if (AVPData)
		m_RequestNumber = ntohl(*(int*)AVPData);
	
	AVPData = _Data + 20;
	AVPSize = 0;
	while ((AVPData = GetAVPData(AVPData + AVPSize, _Size - static_cast<unsigned int>(AVPData + AVPSize - _Data), 456, AVPSize)) != NULL)
		m_vecMSCC.push_back(new CMSCCImpl(AVPData, AVPSize));
}

CCCAMessageImpl::~CCCAMessageImpl()
{
	if (m_pRawMessage)
		delete m_pRawMessage;
	VEC_MSCC::iterator begin = m_vecMSCC.begin(), end = m_vecMSCC.end();
	for (; begin != end; begin++)
		delete *begin;
	m_vecMSCC.clear();
}

IDiameterHeader* CCCAMessageImpl::GetHeader()
{
	return m_pRawMessage->GetHeader();
}

IAVP* CCCAMessageImpl::GetAVPs()
{
	return m_pRawMessage->GetAVPs();
}

const char* CCCAMessageImpl::GetSessionId() const
{
	return m_SessionId.c_str();
}


const char* CCCAMessageImpl::GetErrorMessage() const
{
	return m_ErrorMessage.c_str();
}

int CCCAMessageImpl::GetResultCode() const
{
	return m_ResultCode;
}

    int CCCAMessageImpl::GetResultCodeExt() const
    {
        return m_ResultCodeExt;
    }
const char* CCCAMessageImpl::GetOriginHost() const
{
	return m_OriginHost.c_str();
}

const char* CCCAMessageImpl::GetOriginRealm() const
{
	return m_OriginRealm.c_str();
}

int CCCAMessageImpl::GetAuthApplicationId() const
{
	return m_AuthApplicationId;
}

const char* CCCAMessageImpl::GetServiceContextId() const
{
	return m_ServiceContextId.c_str();
}

int CCCAMessageImpl::GetRequestNumber() const
{
	return m_RequestNumber;
}

RequestType CCCAMessageImpl::GetRequestType() const
{
	return m_RequestType;
}

CMSCC * CCCAMessageImpl::GetMSCC(int _Index) const
{
	if (_Index < (int)m_vecMSCC.size())
		return m_vecMSCC[_Index];
	return NULL;
}

bool CCCAMessageImpl::SetSessionId(const char* _SessionId)
{
	m_SessionId.assign(_SessionId);
	if (m_pAVP->SetAVP(new CDiameterAVP(IAVP::BASIC, (const unsigned char*)m_SessionId.c_str(), static_cast<unsigned int>(m_SessionId.size()), 263, 0x40, 10415)))
		return true;
	return false;
}

bool CCCAMessageImpl::SetErrorMessage(const char* _errorMsg)
{
	m_ErrorMessage.assign(_errorMsg);
	if (m_pAVP->SetAVP(new CDiameterAVP(IAVP::BASIC, (const unsigned char*)m_ErrorMessage.c_str(), static_cast<unsigned int>(m_ErrorMessage.size()), 281, 0x40, 10415)))
		return true;
	return false;
}

bool CCCAMessageImpl::SetResultCode(int _ResultCode)
{
	m_ResultCode = _ResultCode;
	if (m_pAVP->SetAVP(new CDiameterAVP(IAVP::BASIC, m_ResultCode, 268, 0x40, 10415)))
		return true;
	return false;
}
    bool CCCAMessageImpl::SetResultCodeExt(int _ResultCodeExt)
    {
        m_ResultCodeExt = _ResultCodeExt;
        if (m_pAVP->SetAVP(new CDiameterAVP(IAVP::BASIC, m_ResultCodeExt, 268, 0x40, 10415)))
            return true;
        return false;
    }
bool CCCAMessageImpl::SetOriginHost(const char* _OriginHost)
{
	m_OriginHost.assign(_OriginHost);
	if (m_pAVP->SetAVP(new CDiameterAVP(IAVP::BASIC, (const unsigned char*)m_OriginHost.c_str(), static_cast<unsigned int>(m_OriginHost.size()), 264, 0x40, 10415)))
		return true;
	return false;
}

bool CCCAMessageImpl::SetOriginRealm(const char* _OriginRealm)
{
	m_OriginRealm.assign(_OriginRealm);
	if (m_pAVP->SetAVP(new CDiameterAVP(IAVP::BASIC, (const unsigned char*)m_OriginRealm.c_str(), static_cast<unsigned int>(m_OriginRealm.size()), 296, 0x40, 10415)))
		return true;
	return false;
}

bool CCCAMessageImpl::SetAuthApplicationId(int _AuthApplicationId)
{
	m_AuthApplicationId = _AuthApplicationId;
	if (m_pAVP->SetAVP(new CDiameterAVP(IAVP::BASIC, m_AuthApplicationId, 258, 0x40, 10415)))
		return true;
	return false;
}

bool CCCAMessageImpl::SetServiceContextId(const char* _ServiceContextId)
{
	m_ServiceContextId.assign(_ServiceContextId);
	if (m_pAVP->SetAVP(new CDiameterAVP(IAVP::BASIC, (const unsigned char*)m_ServiceContextId.c_str(), static_cast<unsigned int>(m_ServiceContextId.size()), 461, 0x40, 10415)))
		return true;
	return false;
}

bool CCCAMessageImpl::SetRequestNumber(int _RequestNumber)
{
	m_RequestNumber = _RequestNumber;
	if (m_pAVP->SetAVP(new CDiameterAVP(IAVP::BASIC, m_RequestNumber, 415, 0x40, 10415)))
		return true;
	return false;
}

bool CCCAMessageImpl::SetRequestType(RequestType _RequestType)
{
	m_RequestType = _RequestType;
	if (m_pAVP->SetAVP(new CDiameterAVP(IAVP::BASIC, m_RequestType, 416, 0x40, 10415)))
		return true;
	return false;
}

CMSCC * CCCAMessageImpl::CreateMSCC(int _Index)
{
	if(static_cast<size_t>(_Index) != m_vecMSCC.size())
		return NULL;

	IAVP *MSCCAVP = new CDiameterAVP(IAVP::GROUPED, NULL, 0, 456, 0x40, 10415);
	CMSCCImpl* MSCC = new CMSCCImpl(MSCCAVP);
	m_vecMSCC.push_back(MSCC);
	m_pMSCCAVP->SetAVP(MSCCAVP, _Index);

	//MSCC->AddTriggerType(CHANGE_IN_RAT);

	return MSCC;
}

bool CCCAMessageImpl::DeleteMSCC(int _Index)//m_pMSCCAVP
{
	if (_Index >= static_cast<int>(m_vecMSCC.size()) || _Index < -1)
		return false;
	if (_Index == -1)
	{
		VEC_MSCC::iterator begin = m_vecMSCC.begin(), end = m_vecMSCC.end();
		for (; begin != end; begin++)
			delete *begin;
		m_vecMSCC.clear();
	}
	else
	{
		delete m_vecMSCC[_Index];
		for (int i = _Index; i < (static_cast<int>(m_vecMSCC.size()) - 1); i++)
			m_vecMSCC[i] = m_vecMSCC[i+1];
		m_vecMSCC.pop_back();
	}
	return true;
}

void CCCAMessageImpl::Dump(std::ostream &_OutStream)
{
	m_pRawMessage->Dump(_OutStream);
}

}
