#include "PSFurnishChargingInformationImpl.h"

namespace CoreDiam
{

CPSFurnishChargingInformationImpl::CPSFurnishChargingInformationImpl(IAVP* _pAVP) :
	m_pAVP(_pAVP),
	m_PSAppendFreeFormatData((PSAppendFreeFormatData)-1)
{
}

CPSFurnishChargingInformationImpl::CPSFurnishChargingInformationImpl(const unsigned char* _Data, unsigned int _Size) :
	m_pAVP(NULL),
	m_PSAppendFreeFormatData((PSAppendFreeFormatData)-1)
{
	const unsigned char* AVPData;
	unsigned int AVPSize;
	
	AVPData = GetAVPData(_Data, _Size, 2, AVPSize);
	if (AVPData)
	{
		const unsigned char* EndPtr = AVPData + AVPSize;
		while (AVPData != EndPtr)
			m_vec3GPPChargingId.push_back(*(AVPData++));
	}
	
	AVPData = GetAVPData(_Data, _Size, 866, AVPSize);
	if (AVPData)
	{
		const unsigned char* EndPtr = AVPData + AVPSize;
		while (AVPData != EndPtr)
			m_vecPSFreeFormatData.push_back(*(AVPData++));
	}
	
	AVPData = GetAVPData(_Data, _Size, 867, AVPSize);
	if (AVPData)
		m_PSAppendFreeFormatData = (PSAppendFreeFormatData)ntohl(*(int*)AVPData);
}

CPSFurnishChargingInformationImpl::~CPSFurnishChargingInformationImpl()
{
}

const unsigned char* CPSFurnishChargingInformationImpl::GetGPRSChargingId(unsigned int& _Len)
{
	_Len = static_cast<unsigned int>(m_vec3GPPChargingId.size());
	return m_vec3GPPChargingId.data();
}

const unsigned char* CPSFurnishChargingInformationImpl::GetPSFreeFormatData(unsigned int& _Len)
{
	_Len = static_cast<unsigned int>(m_vecPSFreeFormatData.size());
	return m_vecPSFreeFormatData.data();
}

PSAppendFreeFormatData CPSFurnishChargingInformationImpl::GetPSAppendFreeFormatData()
{
	return m_PSAppendFreeFormatData;
}

bool CPSFurnishChargingInformationImpl::SetGPRSChargingId(const unsigned char* _GPRSChargingId, unsigned int _Len)
{
	if(m_pAVP)
		m_pAVP->SetAVP(new CDiameterAVP(IAVP::BASIC, _GPRSChargingId, _Len, 2, 0xC0, 10415));
	const unsigned char* EndPtr = _GPRSChargingId + _Len;
	m_vec3GPPChargingId.clear();
	while (_GPRSChargingId != EndPtr)
		m_vec3GPPChargingId.push_back(*(_GPRSChargingId++));
	return true;
}

bool CPSFurnishChargingInformationImpl::SetPSFreeFormatData(const unsigned char* _PSFreeFormatData, unsigned int _Len)
{
	if (m_pAVP)
		m_pAVP->SetAVP(new CDiameterAVP(IAVP::BASIC, _PSFreeFormatData, _Len, 866, 0xC0, 10415));
	const unsigned char *EndPtr = _PSFreeFormatData + _Len;
	m_vecPSFreeFormatData.clear();
	while (_PSFreeFormatData != EndPtr)
		m_vecPSFreeFormatData.push_back(*(_PSFreeFormatData++));
	return true;
}

bool CPSFurnishChargingInformationImpl::SetPSAppendFreeFormatData(PSAppendFreeFormatData _PSAppendFreeFormatData)
{
	m_PSAppendFreeFormatData = _PSAppendFreeFormatData;
	if(m_pAVP)
		m_pAVP->SetAVP(new CDiameterAVP(IAVP::BASIC, m_PSAppendFreeFormatData, 867, 0xC0, 10415));

	return true;
}

}