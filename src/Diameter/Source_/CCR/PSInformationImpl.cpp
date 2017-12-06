#include "PSInformationImpl.h"

namespace CoreDiam
{

CPSInformationImpl::CPSInformationImpl(IAVP* _pAVP) :
	m_3GPPChargingId((unsigned int)-1),
	m_PDPType((PDPType)-1),
	m_PDPAddressFamily((unsigned short)-1),
	m_DynamicAddressFlag((DynamicAddressFlag)-1),
	m_SGSNAddressFamily((unsigned short)-1),
	m_GGSNAddressFamily((unsigned short)-1),
	m_3GPP_IMSI_MCCMNC(""),
	m_3GPP_GGSN_MCCMNC(""),
	m_CalledStationId(""),
	m_3GPP_SGSN_MCCMNC(""),
	m_StartTime(-1),
	m_StopTime(-1),
	m_GeographicLocationType((unsigned char)-1),
	m_pAVP(_pAVP)
{
}

CPSInformationImpl::CPSInformationImpl(const unsigned char* _Data, unsigned int _Size) :
	m_3GPPChargingId((unsigned int)-1),
	m_PDPType((PDPType)-1),
	m_PDPAddressFamily((unsigned short)-1),
	m_DynamicAddressFlag((DynamicAddressFlag)-1),
	m_SGSNAddressFamily((unsigned short)-1),
	m_GGSNAddressFamily((unsigned short)-1),
	m_3GPP_IMSI_MCCMNC(""),
	m_3GPP_GGSN_MCCMNC(""),
	m_CalledStationId(""),
	m_3GPP_SGSN_MCCMNC(""),
	m_StartTime(-1),
	m_StopTime(-1),
	m_GeographicLocationType((unsigned char)-1),
	m_pAVP(NULL)
{
	const unsigned char* AVPData;
	unsigned int AVPSize;

	AVPData = GetAVPData(_Data, _Size, 2, AVPSize);
	if (AVPData && (AVPSize >= sizeof(unsigned int)))
	{
		memcpy(&m_3GPPChargingId, AVPData + (AVPSize - sizeof(unsigned int)), sizeof(unsigned int));
		m_3GPPChargingId = ntohl(m_3GPPChargingId);
	}

	AVPData = GetAVPData(_Data, _Size, 2064, AVPSize);
	if (AVPData)
	{
		const unsigned char* EndPtr = AVPData + AVPSize;
		while (AVPData != EndPtr)
			m_vecNodeId.push_back(*(AVPData++));
	}

	AVPData = GetAVPData(_Data, _Size, 3, AVPSize);
	if (AVPData)
		m_PDPType = (PDPType)ntohl(*(int*)AVPData);

	AVPData = GetAVPData(_Data, _Size, 1227, AVPSize);
	if (AVPData && AVPSize >= 2)
	{
		m_PDPAddressFamily = ntohs(*(unsigned short*)AVPData);
		const unsigned char* EndPtr = AVPData + AVPSize;
		AVPData += 2;
		while (AVPData != EndPtr)
			m_vecPDPAddress.push_back(*(AVPData++));
	}

	AVPData = GetAVPData(_Data, _Size, 2051, AVPSize);
	if (AVPData)
		m_DynamicAddressFlag = (DynamicAddressFlag)ntohl(*(int*)AVPData);

	AVPData = GetAVPData(_Data, _Size, 1228, AVPSize);
	if (AVPData && AVPSize >= 2)
	{
		m_SGSNAddressFamily = ntohs(*(unsigned short*)AVPData);
		const unsigned char* EndPtr = AVPData + AVPSize;
		AVPData += 2;
		while (AVPData != EndPtr)
			m_vecSGSNAddress.push_back(*(AVPData++));
	}

	AVPData = GetAVPData(_Data, _Size, 847, AVPSize);
	if (AVPData && AVPSize >= 2)
	{
		m_GGSNAddressFamily = ntohs(*(unsigned short*)AVPData);
		const unsigned char* EndPtr = AVPData + AVPSize;
		AVPData += 2;
		while (AVPData != EndPtr)
			m_vecGGSNAddress.push_back(*(AVPData++));
	}

	AVPData = GetAVPData(_Data, _Size, 8, AVPSize);
	if(AVPData)
		m_3GPP_IMSI_MCCMNC.assign((const char *)AVPData, AVPSize);

	AVPData = GetAVPData(_Data, _Size, 9, AVPSize);
	if(AVPData)
		m_3GPP_GGSN_MCCMNC.assign((const char *)AVPData, AVPSize);

	AVPData = GetAVPData(_Data, _Size, 30, AVPSize);
	if (AVPData)
		m_CalledStationId.assign((const char*)AVPData, AVPSize);

	AVPData = GetAVPData(_Data, _Size, 18, AVPSize);
	if(AVPData)
		m_3GPP_SGSN_MCCMNC.assign((const char *)AVPData, AVPSize);

	AVPData = GetAVPData(_Data, _Size, 22, AVPSize);
	if (AVPData && AVPSize >= 1)
	{
		m_GeographicLocationType = *AVPData;
		const unsigned char* EndPtr = AVPData + AVPSize;
		AVPData++;
		while (AVPData != EndPtr)
			m_vec3GPPUserLocationInfo.push_back(*(AVPData++));
	}

	AVPData = GetAVPData(_Data, _Size, 21, AVPSize);
	if(AVPData)
		m_RATType = *AVPData;

	AVPData = GetAVPData(_Data, _Size, 2041, AVPSize);
	if (AVPData)
		m_StartTime = ntohl(*(unsigned int*)AVPData);

	AVPData = GetAVPData(_Data, _Size, 2042, AVPSize);
	if (AVPData)
		m_StopTime = ntohl(*(unsigned int*)AVPData);

}

CPSInformationImpl::~CPSInformationImpl()
{
}

unsigned int CPSInformationImpl::Get3GPPChargingId()
{
	return m_3GPPChargingId;
}

const unsigned char* CPSInformationImpl::GetNodeId(unsigned int& _Len)
{
	_Len = static_cast<unsigned int>(m_vecNodeId.size());
	return m_vecNodeId.data();
}

PDPType CPSInformationImpl::Get3GPPPDPType()
{
	return m_PDPType;
}

const unsigned char* CPSInformationImpl::GetPDPAddress(unsigned short& _AddressFamily, unsigned int& _Len, std::string& _StrAddress)
{
	if (m_PDPAddressFamily == 1 && m_vecPDPAddress.size() == 4)
	{
		_StrAddress.resize(16);
		sprintf((char*)_StrAddress.c_str(), "%u.%u.%u.%u", m_vecPDPAddress[0], m_vecPDPAddress[1], m_vecPDPAddress[2], m_vecPDPAddress[3]);
	}
	_AddressFamily = m_PDPAddressFamily;
	_Len = static_cast<unsigned int>(m_vecPDPAddress.size());
	return m_vecPDPAddress.data();
}

DynamicAddressFlag CPSInformationImpl::GetDynamicAddressFlag()
{
	return m_DynamicAddressFlag;
}

const unsigned char* CPSInformationImpl::GetSGSNAddress(unsigned short& _AddressFamily, unsigned int& _Len, std::string& _StrAddress)
{
	if (m_SGSNAddressFamily == 1 && m_vecSGSNAddress.size() == 4)
	{
		_StrAddress.resize(16);
		sprintf((char*)_StrAddress.c_str(), "%u.%u.%u.%u", m_vecSGSNAddress[0], m_vecSGSNAddress[1], m_vecSGSNAddress[2], m_vecSGSNAddress[3]);
	}
	_AddressFamily = m_SGSNAddressFamily;
	_Len = static_cast<unsigned int>(m_vecSGSNAddress.size());
	return m_vecSGSNAddress.data();
}

const unsigned char* CPSInformationImpl::GetGGSNAddress(unsigned short& _AddressFamily, unsigned int& _Len, std::string& _StrAddress)
{
	if (m_GGSNAddressFamily == 1 && m_vecGGSNAddress.size() == 4)
	{
		_StrAddress.resize(16);
		sprintf((char*)_StrAddress.c_str(), "%u.%u.%u.%u", m_vecGGSNAddress[0], m_vecGGSNAddress[1], m_vecGGSNAddress[2], m_vecGGSNAddress[3]);
	}
	_AddressFamily = m_GGSNAddressFamily;
	_Len = static_cast<unsigned int>(m_vecGGSNAddress.size());
	return m_vecGGSNAddress.data();
}
//----------------------------------------------------------------------------
const char * CPSInformationImpl::Get3GPP_IMSI_MCCMNC(unsigned int &_Len)
{
	_Len = m_3GPP_IMSI_MCCMNC.length();
	return m_3GPP_IMSI_MCCMNC.c_str();
}
//----------------------------------------------------------------------------
const char * CPSInformationImpl::Get3GPP_GGSN_MCCMNC(unsigned int &_Len)
{
	_Len = m_3GPP_GGSN_MCCMNC.length();
	return m_3GPP_GGSN_MCCMNC.c_str();
}
//----------------------------------------------------------------------------
const char* CPSInformationImpl::GetCalledStationId()
{
	return m_CalledStationId.c_str();
}
//----------------------------------------------------------------------------
const char * CPSInformationImpl::Get3GPP_SGSN_MCCMNC(unsigned int &_Len)
{
	_Len = m_3GPP_SGSN_MCCMNC.length();
	return m_3GPP_SGSN_MCCMNC.c_str();
}
//----------------------------------------------------------------------------
const unsigned char* CPSInformationImpl::Get3GPPUserLocationInfo(unsigned char& _GeographicLocationType, unsigned int& _Len)
{
	_GeographicLocationType = m_GeographicLocationType;
	_Len = static_cast<unsigned int>(m_vec3GPPUserLocationInfo.size());
	return m_vec3GPPUserLocationInfo.data();
}

unsigned char CPSInformationImpl::Get3GPPRATType()
{
	return m_RATType;
}

unsigned int CPSInformationImpl::GetStartTime()
{
	return m_StartTime;
}

unsigned int CPSInformationImpl::GetStopTime()
{
	return m_StopTime;
}

bool CPSInformationImpl::Set3GPPChargingId(unsigned int _3GPPChargingId)
{
	m_3GPPChargingId = _3GPPChargingId;
	if (m_pAVP)
	{
		unsigned char* Buf = new unsigned char[2 + sizeof(unsigned int)];
		Buf[0] = 0x02;
		Buf[1] = 0x06;
		_3GPPChargingId = htonl(_3GPPChargingId);
		memcpy(&Buf[2], &_3GPPChargingId, sizeof(unsigned int));

		m_pAVP->SetAVP(new CDiameterAVP(IAVP::BASIC, Buf, 6, 2, 0xC0, 10415));
		delete[] Buf;
	}
	return true;
}

bool CPSInformationImpl::SetNodeId(const unsigned char* _NodeId, unsigned int _Len)
{
	if (m_pAVP)
		m_pAVP->SetAVP(new CDiameterAVP(IAVP::BASIC, _NodeId, _Len, 2064, 0xC0, 10415));
	const unsigned char* EndPtr = _NodeId + _Len;
	m_vecNodeId.clear();
	while (_NodeId != EndPtr)
		m_vecNodeId.push_back(*(_NodeId++));
	return true;
}

bool CPSInformationImpl::Set3GPPPDPType(PDPType _PDPType)
{
	m_PDPType = _PDPType;
	m_pAVP->SetAVP(new CDiameterAVP(IAVP::BASIC, m_PDPType, 3, 0xC0, 10415));
	return false;
}

bool CPSInformationImpl::SetPDPAddress(unsigned short _AddressFamily, const unsigned char* _PDPAddress, unsigned int _Len)
{
	if (m_pAVP)
	{
		unsigned char* Buf = new unsigned char[_Len + 2];
		unsigned short NetFamily = htons(_AddressFamily);
		memcpy(Buf, &NetFamily, sizeof(unsigned short));
		memcpy(&Buf[2], _PDPAddress, _Len);
		m_pAVP->SetAVP(new CDiameterAVP(IAVP::BASIC, Buf, _Len + 2, 1227, 0xC0, 10415));
		delete[] Buf;
	}
	const unsigned char* EndPtr = _PDPAddress + _Len;
	m_vecPDPAddress.clear();
	while (_PDPAddress != EndPtr)
		m_vecPDPAddress.push_back(*(_PDPAddress++));
	m_PDPAddressFamily = _AddressFamily;
	return true;
}

bool CPSInformationImpl::SetDynamicAddressFlag(DynamicAddressFlag _DynamicAddressFlag)
{
	m_DynamicAddressFlag = _DynamicAddressFlag;
	m_pAVP->SetAVP(new CDiameterAVP(IAVP::BASIC, m_DynamicAddressFlag, 2051, 0xC0, 10415));
	return false;
}

bool CPSInformationImpl::SetSGSNAddress(unsigned short _AddressFamily, const unsigned char* _SGSNAddress, unsigned int _Len)
{
	if (m_pAVP)
	{
		unsigned char* Buf = new unsigned char[_Len + 2];
		unsigned short NetFamily = htons(_AddressFamily);
		memcpy(Buf, &NetFamily, sizeof(unsigned short));
		memcpy(&Buf[2], _SGSNAddress, _Len);
		m_pAVP->SetAVP(new CDiameterAVP(IAVP::BASIC, Buf, _Len + 2, 1228, 0xC0, 10415));
		delete[] Buf;
	}
	const unsigned char* EndPtr = _SGSNAddress + _Len;
	m_vecSGSNAddress.clear();
	while (_SGSNAddress != EndPtr)
		m_vecSGSNAddress.push_back(*(_SGSNAddress++));
	m_SGSNAddressFamily = _AddressFamily;
	return true;
}

bool CPSInformationImpl::SetGGSNAddress(unsigned short _AddressFamily, const unsigned char* _GGSNAddress, unsigned int _Len)
{
	if (m_pAVP)
	{
		unsigned char* Buf = new unsigned char[_Len + 2];
		unsigned short NetFamily = htons(_AddressFamily);
		memcpy(Buf, &NetFamily, sizeof(unsigned short));
		memcpy(&Buf[2], _GGSNAddress, _Len);
		m_pAVP->SetAVP(new CDiameterAVP(IAVP::BASIC, Buf, _Len + 2, 847, 0xC0, 10415));
		delete[] Buf;
	}
	const unsigned char* EndPtr = _GGSNAddress + _Len;
	m_vecGGSNAddress.clear();
	while (_GGSNAddress != EndPtr)
		m_vecGGSNAddress.push_back(*(_GGSNAddress++));
	m_GGSNAddressFamily = _AddressFamily;
	return true;
}
//----------------------------------------------------------------------------
bool CPSInformationImpl::Set3GPP_IMSI_MCCMNC(const char *_mccMNC, unsigned int &_Len)
{
	if(!_mccMNC)
		return false;

	m_3GPP_IMSI_MCCMNC.assign(_mccMNC, _Len);

	return m_pAVP->SetAVP(new CDiameterAVP(IAVP::BASIC, (const unsigned char *)m_3GPP_IMSI_MCCMNC.c_str(), static_cast<unsigned int>(m_3GPP_IMSI_MCCMNC.size()), 8, 0xC0, 10415));
}
//----------------------------------------------------------------------------
bool CPSInformationImpl::Set3GPP_GGSN_MCCMNC(const char *_mccMNC, unsigned int &_Len)
{
	if(!_mccMNC)
		return false;

	m_3GPP_GGSN_MCCMNC.assign(_mccMNC, _Len);

	return m_pAVP->SetAVP(new CDiameterAVP(IAVP::BASIC, (const unsigned char *)m_3GPP_GGSN_MCCMNC.c_str(), static_cast<unsigned int>(m_3GPP_GGSN_MCCMNC.size()), 9, 0xC0, 10415));
}
//----------------------------------------------------------------------------
bool CPSInformationImpl::SetCalledStationId(const char* _CalledStationId)
{
	if (_CalledStationId)
		m_CalledStationId.assign(_CalledStationId);
	else
		return false;
	m_pAVP->SetAVP(new CDiameterAVP(IAVP::BASIC, (const unsigned char*)m_CalledStationId.c_str(), static_cast<unsigned int>(m_CalledStationId.size()), 30, 0x40, 10415));
	return true;
}
//----------------------------------------------------------------------------
bool CPSInformationImpl::Set3GPP_SGSN_MCCMNC(const char *_mccMNC, unsigned int &_Len)
{
	if(!_mccMNC)
		return false;

	m_3GPP_SGSN_MCCMNC.assign(_mccMNC, _Len);

	return m_pAVP->SetAVP(new CDiameterAVP(IAVP::BASIC, (const unsigned char *)m_3GPP_SGSN_MCCMNC.c_str(), static_cast<unsigned int>(m_3GPP_SGSN_MCCMNC.size()), 18, 0xC0, 10415));
}
//----------------------------------------------------------------------------
bool CPSInformationImpl::Set3GPPUserLocationInfo(unsigned char _GeographicLocationType, const unsigned char* _3GPPUserLocationInfo, unsigned int _Len)
{
	if (m_pAVP)
	{
		unsigned char* Buf = new unsigned char[_Len + 1];
		memcpy(Buf, &_GeographicLocationType, sizeof(unsigned char));
		memcpy(&Buf[1], _3GPPUserLocationInfo, _Len);
		m_pAVP->SetAVP(new CDiameterAVP(IAVP::BASIC, Buf, _Len + 1, 22, 0xC0, 10415));
		delete[] Buf;
	}
	const unsigned char* EndPtr = _3GPPUserLocationInfo + _Len;
	m_vec3GPPUserLocationInfo.clear();
	while (_3GPPUserLocationInfo != EndPtr)
		m_vec3GPPUserLocationInfo.push_back(*(_3GPPUserLocationInfo++));
	m_GeographicLocationType = _GeographicLocationType;
	return true;
}

bool CPSInformationImpl::Set3GPPRATType(unsigned char _ratType)
{
	m_RATType = _ratType;
	return m_pAVP->SetAVP(new CDiameterAVP(IAVP::BASIC, &m_RATType, sizeof(m_RATType), 21, 0xC0, 10415));
}

bool CPSInformationImpl::SetStartTime(unsigned int _StartTime)
{
	m_StartTime = _StartTime;
	m_pAVP->SetAVP(new CDiameterAVP(IAVP::BASIC, (int)m_StartTime, 2041, 0xC0, 10415));
	return true;
}

bool CPSInformationImpl::SetStopTime(unsigned int _StopTime)
{
	m_StopTime = _StopTime;
	m_pAVP->SetAVP(new CDiameterAVP(IAVP::BASIC, (int)m_StopTime, 2042, 0x40, 10415));
	return true;
}

}
