#include "IMSInformationImpl.h"

namespace CoreDiam
{

CIMSInformationImpl::CIMSInformationImpl(IAVP* _pAVP) :
	m_pEventType(NULL),
	m_NodeFunctionality((NodeFunctionality)-1),
	m_CallingPartyAddress(""),
	m_CalledPartyAddress(""),
	m_RequestedPartyAddress(""),
	m_pTimeStamps(NULL),
	m_pApplicationServerInformation(NULL),
	m_ServiceId(""),
	m_pAVP(_pAVP)
{
	m_pSSInfoAVP = new CDiameterAVPGroup(IAVP::GROUPEDSET, 1249);
	m_pAVP->SetAVP(m_pSSInfoAVP);
}

CIMSInformationImpl::CIMSInformationImpl(const unsigned char* _Data, unsigned int _Size) :
	m_pEventType(NULL),
	m_NodeFunctionality((NodeFunctionality)-1),
	m_CallingPartyAddress(""),
	m_CalledPartyAddress(""),
	m_RequestedPartyAddress(""),
	m_pTimeStamps(NULL),
	m_pApplicationServerInformation(NULL),
	m_ServiceId(""),
	m_pAVP(NULL),
	m_pSSInfoAVP(NULL)
{
	const unsigned char* AVPData;
	unsigned int AVPSize;
	
	AVPData = GetAVPData(_Data, _Size, 823, AVPSize);
	if (AVPData)
		m_pEventType = new CEventTypeImpl(AVPData, AVPSize);

	AVPData = GetAVPData(_Data, _Size, 862, AVPSize);
	if (AVPData)
		m_NodeFunctionality = (NodeFunctionality)ntohl(*(int*)AVPData);

	AVPData = GetAVPData(_Data, _Size, 831, AVPSize);
	if (AVPData)
		m_CallingPartyAddress.assign((const char*)AVPData, AVPSize);
	
	AVPData = GetAVPData(_Data, _Size, 832, AVPSize);
	if (AVPData)
		m_CalledPartyAddress.assign((const char*)AVPData, AVPSize);
	
	AVPData = GetAVPData(_Data, _Size, 1251, AVPSize);
	if (AVPData)
		m_RequestedPartyAddress.assign((const char*)AVPData, AVPSize);

	AVPData = GetAVPData(_Data, _Size, 833, AVPSize);
	if (AVPData)
		m_pTimeStamps = new CTimeStampsImpl(AVPData, AVPSize);
	
	AVPData = GetAVPData(_Data, _Size, 850, AVPSize);
	if (AVPData)
		m_pApplicationServerInformation = new CApplicationServerInformationImpl(AVPData, AVPSize);

	AVPData = GetAVPData(_Data, _Size, 854, AVPSize);
	if (AVPData)
	{
		const unsigned char* EndPtr = AVPData + AVPSize;
		while (AVPData != EndPtr)
			m_vecBearerService.push_back(*(AVPData++));
	}
	
	AVPData = GetAVPData(_Data, _Size, 855, AVPSize);
	if (AVPData)
		m_ServiceId.assign((const char*)AVPData, AVPSize);

	AVPData = _Data;
	AVPSize = 0;
	while ((AVPData = GetAVPData(AVPData + AVPSize, _Size - static_cast<unsigned int>(AVPData + AVPSize - _Data), 1249, AVPSize)) != NULL)
		m_vecServiceSpecificInfo.push_back(new CServiceSpecificInfoImpl(AVPData, AVPSize));	
}

CIMSInformationImpl::~CIMSInformationImpl()
{
	if (m_pEventType)
		delete m_pEventType;
	if (m_pTimeStamps)
		delete m_pTimeStamps;
	if (m_pApplicationServerInformation)
		delete m_pApplicationServerInformation;

	VEC_SERVICESPECIFICINFO::iterator bssinfo = m_vecServiceSpecificInfo.begin(), essinfo = m_vecServiceSpecificInfo.end();
	for (; bssinfo != essinfo; bssinfo++)
		delete *bssinfo;
	m_vecServiceSpecificInfo.clear();
}

CEventType* CIMSInformationImpl::GetEventType()
{
	return m_pEventType;
}

NodeFunctionality CIMSInformationImpl::GetNodeFunctionality()
{
	return (NodeFunctionality)-1;
}

const char* CIMSInformationImpl::GetCallingPartyAddress()
{
	return m_CallingPartyAddress.c_str();
}

const char* CIMSInformationImpl::GetCalledPartyAddress()
{
	return m_CalledPartyAddress.c_str();
}

const char* CIMSInformationImpl::GetRequestedPartyAddress()
{
	return m_RequestedPartyAddress.c_str();
}

CTimeStamps* CIMSInformationImpl::GetTimeStamps()
{
	return m_pTimeStamps;
}

CApplicationServerInformation* CIMSInformationImpl::GetApplicationServerInformation()
{
	return m_pApplicationServerInformation;
}

const unsigned char* CIMSInformationImpl::GetBearerService(unsigned int& _Len)
{
	_Len = (unsigned int)m_vecBearerService.size();
	return m_vecBearerService.data();
}

const char* CIMSInformationImpl::GetServiceId()
{
	return m_ServiceId.c_str();
}

CServiceSpecificInfo* CIMSInformationImpl::GetServiceSpecificInfo(int _Index)
{
	if (_Index < (int)m_vecServiceSpecificInfo.size())
		return m_vecServiceSpecificInfo[_Index];
	return NULL;
}

CEventType* CIMSInformationImpl::CreateEventType()
{
	if (m_pEventType)
	return NULL;
	IAVP* pAVP = new CDiameterAVP(IAVP::GROUPED, NULL, 0, 823, 0xC0, 10415);
	m_pAVP->SetAVP(pAVP);
	m_pEventType = new CEventTypeImpl(pAVP);
	return m_pEventType;
}

bool CIMSInformationImpl::SetNodeFunctionality(NodeFunctionality _NodeFunctionality)
{
	m_NodeFunctionality = _NodeFunctionality;
	if (m_pAVP->SetAVP(new CDiameterAVP(IAVP::BASIC, m_NodeFunctionality, 862, 0xC0, 10415)))
		return true;
	return false;
}

bool CIMSInformationImpl::SetCallingPartyAddress(const char* _CallingPartyAddress)
{
	m_CallingPartyAddress.assign(_CallingPartyAddress);
	m_pAVP->SetAVP(new CDiameterAVP(IAVP::BASIC, (const unsigned char*)m_CallingPartyAddress.c_str(), static_cast<unsigned int>(m_CallingPartyAddress.size()), 831, 0xC0, 10415));
	return true;
}

bool CIMSInformationImpl::SetCalledPartyAddress(const char* _CalledPartyAddress)
{
	m_CalledPartyAddress.assign(_CalledPartyAddress);
	m_pAVP->SetAVP(new CDiameterAVP(IAVP::BASIC, (const unsigned char*)m_CalledPartyAddress.c_str(), static_cast<unsigned int>(m_CalledPartyAddress.size()), 832, 0xC0, 10415));
	return true;
}

bool CIMSInformationImpl::SetRequestedPartyAddress(const char* _RequestedPartyAddress)
{
	m_RequestedPartyAddress.assign(_RequestedPartyAddress);
	m_pAVP->SetAVP(new CDiameterAVP(IAVP::BASIC, (const unsigned char*)m_RequestedPartyAddress.c_str(), static_cast<unsigned int>(m_RequestedPartyAddress.size()), 1251, 0xC0, 10415));
	return true;
}

CTimeStamps* CIMSInformationImpl::CreateTimeStamps()
{
	if (m_pTimeStamps)
	return NULL;
	IAVP* pAVP = new CDiameterAVP(IAVP::GROUPED, NULL, 0, 833, 0xC0, 10415);
	m_pAVP->SetAVP(pAVP);
	m_pTimeStamps = new CTimeStampsImpl(pAVP);
	return m_pTimeStamps;
}

CApplicationServerInformation* CIMSInformationImpl::CreateApplicationServerInformation()
{
	if (m_pApplicationServerInformation)
	return NULL;
	IAVP* pAVP = new CDiameterAVP(IAVP::GROUPED, NULL, 0, 850, 0xC0, 10415);
	m_pAVP->SetAVP(pAVP);
	m_pApplicationServerInformation = new CApplicationServerInformationImpl(pAVP);
	return m_pApplicationServerInformation;
}

bool CIMSInformationImpl::SetBearerService(const unsigned char* _BearerService, unsigned int _Len)
{
	if(m_pAVP)
		m_pAVP->SetAVP(new CDiameterAVP(IAVP::BASIC, _BearerService, _Len, 854, 0xC0, 10415));
	const unsigned char* EndPtr = _BearerService + _Len;
	while (_BearerService != EndPtr)
		m_vecBearerService.push_back(*(_BearerService++));
	return true;
}

bool CIMSInformationImpl::SetServiceId(const char* _ServiceId)
{
	m_ServiceId.assign(_ServiceId);
	m_pAVP->SetAVP(new CDiameterAVP(IAVP::BASIC, (const unsigned char*)m_ServiceId.c_str(), static_cast<unsigned int>(m_ServiceId.size()), 855, 0xC0, 10415));
	return true;
}

CServiceSpecificInfo* CIMSInformationImpl::CreateServiceSpecificInfo(int _Index)
{
	if (_Index == (int)m_vecServiceSpecificInfo.size())
	{
		IAVP *SSIAVP = new CDiameterAVP(IAVP::GROUPED, NULL, 0, 1249, 0xC0, 10415);
		m_vecServiceSpecificInfo.push_back(new CServiceSpecificInfoImpl(SSIAVP));
		m_pSSInfoAVP->SetAVP(SSIAVP, _Index);
		return m_vecServiceSpecificInfo[_Index];
	}
	return NULL;
}

size_t CIMSInformationImpl::GetServiceSpecificInfoCount()
{
	return m_vecServiceSpecificInfo.size();
}


}