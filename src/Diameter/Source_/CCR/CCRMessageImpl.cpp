#include "CCRMessageImpl.h"

namespace CoreDiam
{

CCCRMessageImpl::CCCRMessageImpl():
	m_RequestType(INITIAL),
    m_TrafficCase(TC_UNDEFINED),
	m_EventTimestamp(-1),
	m_TerminationCause(-1),
	m_SessionId(""),
	m_OriginHost(""),
	m_OriginRealm(""),
	m_DestinationHost(""),
	m_DestinationRealm(""),
	m_AuthApplicationId(-1),
	m_ServiceContextId(""),
	m_RequestNumber(-1),
	m_ServiceIdentifier(-1),
	m_RequestedAction(-1),
	m_pUserEquipmentInfo(NULL),
	m_pServiceInformation(NULL),
    m_pRequestedServiceUnits(NULL),
    m_OtherPartyId(NULL),
    m_pUsedServiceUnits(NULL)
{
	m_pAVP = new CDiameterAVP(IAVP::BASE, NULL, 0, 0, 0x00, 0);
	m_pRawMessage = new CRawDiameterMessage(m_pAVP, new CDiameterHeader());
	
	m_pMSCCAVP = new CDiameterAVPGroup(IAVP::GROUPEDSET, 456);
	m_pAVP->SetAVP(m_pMSCCAVP);

    m_pSPIAVP = new CDiameterAVPGroup(IAVP::GROUPEDSET, 440);
    m_pAVP->SetAVP(m_pSPIAVP);

	m_pSubscriptionIdAVP = new CDiameterAVPGroup(IAVP::GROUPEDSET, 443);
	m_pAVP->SetAVP(m_pSubscriptionIdAVP);
}

CCCRMessageImpl::CCCRMessageImpl(const unsigned char* _Data, unsigned int _Size, IDiameterMessage* _pRawMessage):
	m_RequestType(INITIAL),
	m_TrafficCase(TC_UNDEFINED),
	m_EventTimestamp(-1),
	m_TerminationCause(-1),
	m_Timezone(-1),
	m_SessionId(""),
	m_OriginHost(""),
	m_OriginRealm(""),
	m_DestinationHost(""),
	m_DestinationRealm(""),
	m_AuthApplicationId(-1),
	m_ServiceContextId(""),
	m_RequestNumber(-1),
	m_ServiceIdentifier(-1),
	m_RequestedAction(-1),
	m_pUserEquipmentInfo(NULL),
	m_pServiceInformation(NULL),
    m_pRequestedServiceUnits(NULL),
	m_OtherPartyId(NULL),
	m_pRawMessage(_pRawMessage)
{
	const unsigned char* AVPData;
	unsigned int AVPSize;
	
	m_pAVP = m_pRawMessage->GetAVPs();
	
	AVPData = GetAVPData(_Data, _Size, 263, AVPSize, true);
	if (AVPData)
		m_SessionId.assign((const char*)AVPData, AVPSize);
	
	AVPData = GetAVPData(_Data, _Size, 264, AVPSize, true);
	if (AVPData)
		m_OriginHost.assign((const char*)AVPData, AVPSize);

    AVPData = GetAVPData(_Data, _Size, 23, AVPSize, true);
    if(AVPData)
        m_Timezone = ntohl(*(int *)AVPData);

	
	AVPData = GetAVPData(_Data, _Size, 296, AVPSize, true);
	if (AVPData)
		m_OriginRealm.assign((const char*)AVPData, AVPSize);

	AVPData = GetAVPData(_Data, _Size, 293, AVPSize, true);
	if (AVPData)
		m_DestinationHost.assign((const char*)AVPData, AVPSize);

	AVPData = GetAVPData(_Data, _Size, 283, AVPSize, true);
	if (AVPData)
		m_DestinationRealm.assign((const char*)AVPData, AVPSize);

	AVPData = GetAVPData(_Data, _Size, 1074, AVPSize, true);
	if (AVPData)
		m_SubscriptionIdLocation.assign((const char*)AVPData, AVPSize);
	
	AVPData = GetAVPData(_Data, _Size, 258, AVPSize, true);
	if (AVPData)
		m_AuthApplicationId = ntohl(*(int*)AVPData);
	
	AVPData = GetAVPData(_Data, _Size, 461, AVPSize, true);
	if (AVPData)
		m_ServiceContextId.assign((const char*)AVPData, AVPSize);
	
	AVPData = GetAVPData(_Data, _Size, 416, AVPSize, true);
	if (AVPData)
		m_RequestType = (RequestType)ntohl(*(int*)AVPData);

	AVPData = GetAVPData(_Data, _Size, 1082, AVPSize, true);
	if (AVPData)
		m_TrafficCase = (TrafficCase)ntohl(*(int*)AVPData);

	AVPData = GetAVPData(_Data, _Size, 415, AVPSize, true);
	if (AVPData)
		m_RequestNumber = ntohl(*(int*)AVPData);
	
	AVPData = GetAVPData(_Data, _Size, 55, AVPSize, true);
	if (AVPData)
		m_EventTimestamp = ntohl(*(unsigned int*)AVPData);

	AVPData = GetAVPData(_Data, _Size, 295, AVPSize, true);
	if(AVPData)
		m_TerminationCause = ntohl(*(int *)AVPData);

	AVPData = GetAVPData(_Data, _Size, 436, AVPSize, true);
	if(AVPData)
		m_RequestedAction = ntohl(*(int *)AVPData);


    AVPData = GetAVPData(_Data, _Size, 439, AVPSize, true);
    if (AVPData)
        m_ServiceIdentifier = ntohl(*(int*)AVPData);

    AVPData = GetAVPData(_Data, _Size, 1075, AVPSize, true);
    if (AVPData)
        m_OtherPartyId = new COtherPartyIdImpl(AVPData, AVPSize);

	AVPData = _Data + 20;
	AVPSize = 0;
	while ((AVPData = GetAVPData(AVPData + AVPSize, _Size - static_cast<unsigned int>(AVPData + AVPSize - _Data), 443, AVPSize)) != NULL)
		m_vecSubscriptionID.push_back(new CSubscriptionIdImpl(AVPData, AVPSize));
	
	AVPData = GetAVPData(_Data, _Size, 458, AVPSize, true);
	if (AVPData)
		m_pUserEquipmentInfo = new CUserEquipmentInfoImpl(AVPData, AVPSize);

    AVPData = GetAVPData(_Data, _Size, 437, AVPSize, true);
    if (AVPData)
        m_pRequestedServiceUnits = new CUnitsImpl(AVPData, AVPSize);

    AVPData = GetAVPData(_Data, _Size, 446, AVPSize, true);
    if (AVPData)
        m_pUsedServiceUnits = new CUnitsImpl(AVPData, AVPSize);
	
	AVPData = _Data + 20;
	AVPSize = 0;
	while ((AVPData = GetAVPData(AVPData + AVPSize, _Size - static_cast<unsigned int>(AVPData + AVPSize - _Data), 456, AVPSize)) != NULL)
		m_vecMSCC.push_back(new CMSCCImpl(AVPData, AVPSize));

	AVPData = _Data + 20;
	AVPSize = 0;
	while ((AVPData = GetAVPData(AVPData + AVPSize, _Size - static_cast<unsigned int>(AVPData + AVPSize - _Data), 440, AVPSize)) != NULL)
		m_vecSPI.push_back(new CServiceParameterInfoImpl(AVPData, AVPSize));
	
	AVPData = GetAVPData(_Data, _Size, 873, AVPSize, true);
	if (AVPData)
		m_pServiceInformation = new CServiceInformationImpl(AVPData, AVPSize);
}

CCCRMessageImpl::~CCCRMessageImpl()
{
	if (m_pRawMessage)
		delete m_pRawMessage;

    if (m_pRequestedServiceUnits)
        delete m_pRequestedServiceUnits;

    m_pRequestedServiceUnits = NULL;

	VEC_SUBSCRIPTIONID::iterator bsubs = m_vecSubscriptionID.begin(), esubs = m_vecSubscriptionID.end();
	for (; bsubs != esubs; bsubs++)
		delete *bsubs;
	m_vecSubscriptionID.clear();
	
	if (m_pUserEquipmentInfo)
		delete m_pUserEquipmentInfo;

	VEC_MSCC::iterator bmscc = m_vecMSCC.begin(), emscc = m_vecMSCC.end();
	for (; bmscc != emscc; bmscc++)
		delete *bmscc;
	m_vecMSCC.clear();

	if (m_pServiceInformation)
		delete m_pServiceInformation;
}

const char* CCCRMessageImpl::GetSessionId()
{
	return m_SessionId.c_str();
}
COtherPartyId* CCCRMessageImpl::GetOtherPartyId()
{
	return m_OtherPartyId;
}
const char* CCCRMessageImpl::GetSubscriptionIdLocation()
{
	return m_SubscriptionIdLocation.c_str();
}

const char* CCCRMessageImpl::GetOriginHost()
{
	return m_OriginHost.c_str();
}

const char* CCCRMessageImpl::GetOriginRealm()
{
	return m_OriginRealm.c_str();
}

const char* CCCRMessageImpl::GetDestinationHost()
{
    return m_DestinationHost.c_str();
}

const char* CCCRMessageImpl::GetDestinationRealm()
{
	return m_DestinationRealm.c_str();
}

int CCCRMessageImpl::GetAuthApplicationId()
{
	return m_AuthApplicationId;
}

const char* CCCRMessageImpl::GetServiceContextId()
{
	return m_ServiceContextId.c_str();
}

int CCCRMessageImpl::GetRequestNumber()
{
	return m_RequestNumber;
}

RequestType CCCRMessageImpl::GetRequestType()
{
	return m_RequestType;
}
TrafficCase CCCRMessageImpl::GetTrafficCase()
{
	return m_TrafficCase;
}
int CCCRMessageImpl::GetServiceIdentifier()
{
    return m_ServiceIdentifier;
}

unsigned int CCCRMessageImpl::GetEventTimestamp()
{
	return m_EventTimestamp;
}

CSubscriptionId* CCCRMessageImpl::GetSubscriptionId(int _Index)
{
	if (_Index < (int)m_vecSubscriptionID.size())
		return m_vecSubscriptionID[_Index];
	return NULL;
}

CUserEquipmentInfo* CCCRMessageImpl::GetUserEquipmentInfo()
{
	return m_pUserEquipmentInfo;
}

CMSCC* CCCRMessageImpl::GetMSCC(int _Index)
{
	if (_Index < (int)m_vecMSCC.size())
		return m_vecMSCC[_Index];
	return NULL;
}

CServiceParameterInfo * CCCRMessageImpl::GetSPI(int _Index)
{
	if (_Index < (int)m_vecSPI.size())
    	return m_vecSPI[_Index];
	return NULL;
}

CServiceInformation* CCCRMessageImpl::GetServiceInformation()
{
	return m_pServiceInformation;
}

bool CCCRMessageImpl::SetSessionId(const char* _SessionId)
{
	if (_SessionId)
		m_SessionId.assign(_SessionId);
	else
	return false;
	m_pAVP->SetAVP(new CDiameterAVP(IAVP::BASIC, (const unsigned char*)m_SessionId.c_str(), static_cast<unsigned int>(m_SessionId.size()), 263, 0x40, 10415));
	return true;
}


COtherPartyId* CCCRMessageImpl::CreateOtherPartyId()
{
	if (m_OtherPartyId)
		return NULL;
	IAVP* UEIAVP = new CDiameterAVP(IAVP::GROUPED, NULL, 0, 1075, 0xC0, 193);
	m_OtherPartyId = new COtherPartyIdImpl(UEIAVP);
	m_pAVP->SetAVP(UEIAVP);

	return m_OtherPartyId;
}
bool CCCRMessageImpl::SetSubscriptionIdLocation(const char *_SubscriptionIdLocation)
{
	if (_SubscriptionIdLocation)
		m_SubscriptionIdLocation.assign(_SubscriptionIdLocation);
	else
		return false;
	m_pAVP->SetAVP(new CDiameterAVP(IAVP::BASIC, (const unsigned char*)m_SubscriptionIdLocation.c_str(), static_cast<unsigned int>(m_SubscriptionIdLocation.size()), 1074, 0xC0, 193));
	return true;
}

bool CCCRMessageImpl::SetOriginHost(const char* _OriginHost)
{
	m_OriginHost.assign(_OriginHost);
	m_pAVP->SetAVP(new CDiameterAVP(IAVP::BASIC, (const unsigned char*)m_OriginHost.c_str(), static_cast<unsigned int>(m_OriginHost.size()), 264, 0x40, 10415));
	return true;
}

bool CCCRMessageImpl::SetOriginRealm(const char* _OriginRealm)
{
	m_OriginRealm.assign(_OriginRealm);
	m_pAVP->SetAVP(new CDiameterAVP(IAVP::BASIC, (const unsigned char*)m_OriginRealm.c_str(), static_cast<unsigned int>(m_OriginRealm.size()), 296, 0x40, 10415));
	return true;
}

bool CCCRMessageImpl::SetDestinationHost(const char* _DestinationHost)
{
    m_DestinationHost.assign(_DestinationHost);
    m_pAVP->SetAVP(new CDiameterAVP(IAVP::BASIC, (const unsigned char*)m_DestinationHost.c_str(), static_cast<unsigned int>(m_DestinationHost.size()), 293, 0x40, 10415));
    return true;
}

bool CCCRMessageImpl::SetDestinationRealm(const char* _DestinationRealm)
{
	m_DestinationRealm.assign(_DestinationRealm);
	m_pAVP->SetAVP(new CDiameterAVP(IAVP::BASIC, (const unsigned char*)m_DestinationRealm.c_str(), static_cast<unsigned int>(m_DestinationRealm.size()), 283, 0x40, 10415));
	return true;
}

bool CCCRMessageImpl::SetAuthApplicationId(int _AuthApplicationId)
{
	m_AuthApplicationId = _AuthApplicationId;
	m_pAVP->SetAVP(new CDiameterAVP(IAVP::BASIC, m_AuthApplicationId, 258, 0x40, 10415));
	return true;
}

bool CCCRMessageImpl::SetServiceContextId(const char* _ServiceContextId)
{
	if (_ServiceContextId)
		m_ServiceContextId.assign(_ServiceContextId);
	else
		return false;
	m_pAVP->SetAVP(new CDiameterAVP(IAVP::BASIC, (const unsigned char*)m_ServiceContextId.c_str(), static_cast<unsigned int>(m_ServiceContextId.size()), 461, 0x40, 10415));
	return true;
}

bool CCCRMessageImpl::SetRequestNumber(int _RequestNumber)
{
	m_RequestNumber = _RequestNumber;
	m_pAVP->SetAVP(new CDiameterAVP(IAVP::BASIC, m_RequestNumber, 415, 0x40, 10415));
	return true;
}

bool CCCRMessageImpl::SetServiceIdentifier(int _ServiceIdentifier)
{
    m_ServiceIdentifier = _ServiceIdentifier;
    m_pAVP->SetAVP(new CDiameterAVP(IAVP::BASIC, m_ServiceIdentifier, 439, 0x40, 10415));
    return true;
}

bool CCCRMessageImpl::SetRequestType(RequestType _RequestType)
{
	m_RequestType = _RequestType;
	m_pAVP->SetAVP(new CDiameterAVP(IAVP::BASIC, m_RequestType, 416, 0x40, 10415));
	return true;
}

bool CCCRMessageImpl::SetTrafficCase(TrafficCase _TrafficCase)
{
	m_TrafficCase = _TrafficCase;
    m_pAVP->SetAVP(new CDiameterAVP(IAVP::BASIC, m_TrafficCase, 1082, 0xC0, 193));
	return true;
}

bool CCCRMessageImpl::SetEventTimestamp(unsigned int _EventTimestamp)
{
	m_EventTimestamp = _EventTimestamp;
	m_pAVP->SetAVP(new CDiameterAVP(IAVP::BASIC, (int)m_EventTimestamp, 55, 0x40, 10415));
	return true;
}

CSubscriptionId* CCCRMessageImpl::CreateSubscriptionId(int _Index)
{
	if(_Index == (int)m_vecSubscriptionID.size())
	{
		IAVP* SubscrIDAVP = new CDiameterAVP(IAVP::GROUPED, NULL, 0, 443, 0x40, 10415);
		CSubscriptionIdImpl* SubscrID = new CSubscriptionIdImpl(SubscrIDAVP);
		m_vecSubscriptionID.push_back(SubscrID);
		m_pSubscriptionIdAVP->SetAVP(SubscrIDAVP, _Index);
		return m_vecSubscriptionID[_Index];
	}
	return NULL;
}

CUserEquipmentInfo* CCCRMessageImpl::CreateUserEquipmentInfo()
{
	if (m_pUserEquipmentInfo)
		return NULL;
	IAVP* UEIAVP = new CDiameterAVP(IAVP::GROUPED, NULL, 0, 458, 0x00, 10415);
	m_pUserEquipmentInfo = new CUserEquipmentInfoImpl(UEIAVP);
	m_pAVP->SetAVP(UEIAVP);

	return m_pUserEquipmentInfo;
}

CMSCC * CCCRMessageImpl::CreateMSCC(int _Index)
{
	if(static_cast<VEC_MSCC::size_type>(_Index) == m_vecMSCC.size())
	{
		IAVP *MSCCAVP = new CDiameterAVP(IAVP::GROUPED, NULL, 0, 456, 0x40, 10415);
		m_vecMSCC.push_back(new CMSCCImpl(MSCCAVP));
		m_pMSCCAVP->SetAVP(MSCCAVP, _Index);
		return m_vecMSCC[_Index];
	}

	return NULL;
}

    CServiceParameterInfo * CCCRMessageImpl::CreateSPI(int _Index)
    {
        if(static_cast<VEC_SPI::size_type>(_Index) == m_vecSPI.size())
        {
            IAVP *SPIAVP = new CDiameterAVP(IAVP::GROUPED, NULL, 0, 440, 0x40, 10415);
            m_vecSPI.push_back(new CServiceParameterInfoImpl(SPIAVP));
            m_pSPIAVP->SetAVP(SPIAVP, _Index);
            return m_vecSPI[_Index];
        }

        return NULL;
    }

bool CCCRMessageImpl::DeleteMSCC(int _Index)//m_pMSCCAVP
{
	if(_Index >= static_cast<int>(m_vecMSCC.size()) || _Index < -1)
		return false;

	if(_Index == -1)
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

CServiceInformation * CCCRMessageImpl::CreateServiceInformation()
{
	if (m_pServiceInformation)
		return NULL;
	IAVP* SIAVP = new CDiameterAVP(IAVP::GROUPED, NULL, 0, 873, 0xC0, 10415);
	m_pServiceInformation = new CServiceInformationImpl(SIAVP);
	m_pAVP->SetAVP(SIAVP);
	
	return m_pServiceInformation;
}

IDiameterHeader* CCCRMessageImpl::GetHeader()
{
	if (m_pRawMessage)
		return m_pRawMessage->GetHeader();
	return NULL;
}

IAVP* CCCRMessageImpl::GetAVPs()
{
	if (m_pRawMessage)
		return m_pRawMessage->GetAVPs();
	return NULL;
}

CUnits * CCCRMessageImpl::CreateRequestedServiceUnits()
{
    if (m_pRequestedServiceUnits)
        delete m_pRequestedServiceUnits;
    IAVP* pAVP = new CDiameterAVP(IAVP::GROUPED, NULL, 0, 437, 0x40, 10415);
    m_pAVP->SetAVP(pAVP);
    m_pRequestedServiceUnits = new CUnitsImpl(pAVP);
    return m_pRequestedServiceUnits;
}

CUnits* CCCRMessageImpl::GetRequestedServiceUnits()
{
    return m_pRequestedServiceUnits;
}

CUnits * CCCRMessageImpl::CreateUsedServiceUnits()
{
    if (m_pUsedServiceUnits)
        delete m_pUsedServiceUnits;
    IAVP* pAVP = new CDiameterAVP(IAVP::GROUPED, NULL, 0, 446, 0x40, 10415);
    m_pAVP->SetAVP(pAVP);
    m_pUsedServiceUnits = new CUnitsImpl(pAVP);
    return m_pUsedServiceUnits;
}

CUnits* CCCRMessageImpl::GetUsedServiceUnits()
{
    return m_pUsedServiceUnits;
}

void CCCRMessageImpl::Dump(std::ostream &_OutStream)
{
	m_pRawMessage->Dump(_OutStream);
}

int CCCRMessageImpl::GetRequestedAction()
{
	return m_RequestedAction;
}

int CCCRMessageImpl::GetTerminationCause()
{
	return m_TerminationCause;
}

bool CCCRMessageImpl::SetTerminationCause(int _terminationCause)
{
	m_TerminationCause = _terminationCause;
	m_pAVP->SetAVP(new CDiameterAVP(IAVP::BASIC, m_TerminationCause, 295, 0x40, 10415));
	return true;
}

bool CCCRMessageImpl::SetTimezone(int _timezone)
{
    /*
    m_Timezone = _timezone;
    m_pAVP->SetAVP(new CDiameterAVP(IAVP::BASIC, m_Timezone, 23, 0xC0, 10415));
    return true;

    */
    size_t len = 2;
    unsigned char* Buf = new unsigned char[len];
    Buf[0] = 2;
    Buf[1] = 0;
    m_pAVP->SetAVP(new CDiameterAVP(IAVP::BASIC, Buf, len, 23, 0xC0, 10415));
	return true;
}

bool CCCRMessageImpl::SetRequestedAction(int _requestedAction)
{
	m_RequestedAction = _requestedAction;
	m_pAVP->SetAVP(new CDiameterAVP(IAVP::BASIC, m_RequestedAction, 436, 0x40, 10415));
	return true;
}



}  // namespace CoreDiam
