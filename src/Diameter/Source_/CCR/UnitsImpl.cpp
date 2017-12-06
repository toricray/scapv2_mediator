#include "UnitsImpl.h"

namespace CoreDiam
{

CUnitsImpl::CUnitsImpl(IAVP* _pAVP) :
	m_Time(-1),
	m_pMoney(NULL),
	m_TotalOctets((uint64_t)-1),
	m_InputOctets((uint64_t)-1),
	m_OutputOctets((uint64_t)-1),
	m_ServiceSpecificUnits((uint64_t)-1),
	m_EventChargingTimeStamp(-1),
	m_pAVP(_pAVP)
{
}

CUnitsImpl::CUnitsImpl(const unsigned char* _Data, unsigned int _Size) :
	m_Time(-1),
	m_pMoney(NULL),
	m_TotalOctets((uint64_t)-1),
	m_InputOctets((uint64_t)-1),
	m_OutputOctets((uint64_t)-1),
	m_ServiceSpecificUnits((uint64_t)-1),
	m_EventChargingTimeStamp(-1),
	m_pAVP(NULL)
{
	const unsigned char* AVPData;
	unsigned int AVPSize;
	
	AVPData = GetAVPData(_Data, _Size, 420, AVPSize);
	if (AVPData)
		m_Time = ntohl(*(int*)AVPData);

	AVPData = GetAVPData(_Data, _Size, 413, AVPSize);
	if (AVPData)
		m_pMoney = new CMoneyImpl(AVPData, AVPSize);

	AVPData = GetAVPData(_Data, _Size, 421, AVPSize);
	if (AVPData)
	{
		for (unsigned int i = 0; i < sizeof(uint64_t); i++)
			*(((unsigned char*)&m_TotalOctets) + i) = *(AVPData + sizeof(uint64_t) - i - 1);
	}

	AVPData = GetAVPData(_Data, _Size, 412, AVPSize);
	if (AVPData)
	{
		for (unsigned int i = 0; i < sizeof(uint64_t); i++)
			*(((unsigned char*)&m_InputOctets) + i) = *(AVPData + sizeof(uint64_t) - i - 1);
	}

	AVPData = GetAVPData(_Data, _Size, 414, AVPSize);
	if (AVPData)
	{
		for (unsigned int i = 0; i < sizeof(uint64_t); i++)
			*(((unsigned char*)&m_OutputOctets) + i) = *(AVPData + sizeof(uint64_t) - i - 1);
	}

	AVPData = GetAVPData(_Data, _Size, 417, AVPSize);
	if (AVPData)
	{
		for (unsigned int i = 0; i < sizeof(uint64_t); i++)
			*(((unsigned char*)&m_ServiceSpecificUnits) + i) = *(AVPData + sizeof(uint64_t) - i - 1);
	}
	
	AVPData = GetAVPData(_Data, _Size, 1258, AVPSize);
	if (AVPData)
		m_EventChargingTimeStamp = ntohl(*(unsigned int*)AVPData);
}

CUnitsImpl::~CUnitsImpl()
{
	if (m_pMoney)
		delete m_pMoney;
}

int CUnitsImpl::GetTime()
{
	return m_Time;
}

CMoney* CUnitsImpl::GetMoney()
{
	return m_pMoney;
}

uint64_t CUnitsImpl::GetTotalOctets()
{
	return m_TotalOctets;
}

uint64_t CUnitsImpl::GetInputOctets()
{
	return m_InputOctets;
}

uint64_t CUnitsImpl::GetOutputOctets()
{
	return m_OutputOctets;
}

uint64_t CUnitsImpl::GetServiceSpecificUnits()
{
	return m_ServiceSpecificUnits;
}

unsigned int CUnitsImpl::GetEventChargingTimeStamp()
{
	return m_EventChargingTimeStamp;
}

bool CUnitsImpl::SetTime(int _Time)
{
	m_Time = _Time;
	m_pAVP->SetAVP(new CDiameterAVP(IAVP::BASIC, m_Time, 420, 0x40, 10415));
	return true;
}

CMoney* CUnitsImpl::CreateMoney()
{
	IAVP* pAVP = new CDiameterAVP(IAVP::GROUPED, NULL, 0, 420, 0x40, 10415);
	m_pMoney = new CMoneyImpl(pAVP);
	m_pAVP->SetAVP(pAVP);
	return m_pMoney;
}

bool CUnitsImpl::SetTotalOctets(uint64_t _TotalOctets)
{
	m_TotalOctets = _TotalOctets;
	if (m_pAVP)
	{
		for (unsigned int i = 0; i < sizeof(uint64_t); i++)
			*(((unsigned char*)&_TotalOctets) + i) = *((unsigned char*)&m_TotalOctets + sizeof(uint64_t) - i - 1);
		m_pAVP->SetAVP(new CDiameterAVP(IAVP::BASIC, (const unsigned char*)&_TotalOctets, sizeof(uint64_t), 421, 0x40, 10415));
	}
	return true;
}

bool CUnitsImpl::SetInputOctets(uint64_t _InputOctets)
{
	m_InputOctets = _InputOctets;
	if (m_pAVP)
	{
		for (unsigned int i = 0; i < sizeof(uint64_t); i++)
			*(((unsigned char*)&_InputOctets) + i) = *((unsigned char*)&m_InputOctets + sizeof(uint64_t) - i - 1);
		m_pAVP->SetAVP(new CDiameterAVP(IAVP::BASIC, (const unsigned char*)&_InputOctets, sizeof(uint64_t), 412, 0x40, 10415));
	}
	return true;
}

bool CUnitsImpl::SetOutputOctets(uint64_t _OutputOctets)
{
	m_OutputOctets = _OutputOctets;
	if (m_pAVP)
	{
		for (unsigned int i = 0; i < sizeof(uint64_t); i++)
			*(((unsigned char*)&_OutputOctets) + i) = *((unsigned char*)&m_OutputOctets + sizeof(uint64_t) - i - 1);
		m_pAVP->SetAVP(new CDiameterAVP(IAVP::BASIC, (const unsigned char*)&_OutputOctets, sizeof(uint64_t), 414, 0x40, 10415));
	}
	return true;
}

bool CUnitsImpl::SetServiceSpecificUnits(uint64_t _ServiceSpecificUnits)
{
	m_ServiceSpecificUnits = _ServiceSpecificUnits;
	if (m_pAVP)
	{
		for (unsigned int i = 0; i < sizeof(uint64_t); i++)
			*(((unsigned char*)&_ServiceSpecificUnits) + i) = *((unsigned char*)&m_ServiceSpecificUnits + sizeof(uint64_t) - i - 1);
		m_pAVP->SetAVP(new CDiameterAVP(IAVP::BASIC, (const unsigned char*)&_ServiceSpecificUnits, sizeof(uint64_t), 417, 0x40, 10415));
	}
	return true;
}

bool CUnitsImpl::SetEventChargingTimeStamp(unsigned int _EventChargingTimeStamp)
{
	m_EventChargingTimeStamp = _EventChargingTimeStamp;
	m_pAVP->SetAVP(new CDiameterAVP(IAVP::BASIC, m_EventChargingTimeStamp, 1258, 0xC0, 10415));
	return true;
}

}
