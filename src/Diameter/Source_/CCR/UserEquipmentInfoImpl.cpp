#include "UserEquipmentInfoImpl.h"

namespace CoreDiam
{
	
CUserEquipmentInfoImpl::CUserEquipmentInfoImpl(IAVP* _pAVP) :
	m_pAVP(_pAVP)
{

}

CUserEquipmentInfoImpl::CUserEquipmentInfoImpl(const unsigned char* _Data, unsigned int _Size) :
	m_pAVP(NULL)
{
	const unsigned char* AVPData;
	unsigned int AVPSize;
	
	AVPData = GetAVPData(_Data, _Size, 459, AVPSize);
	if (AVPData)
		m_UserEquipmentInfoType = (UserEquipmentInfoType)ntohl(*(int*)AVPData);
	
	AVPData = GetAVPData(_Data, _Size, 460, AVPSize);
	if (AVPData)
	{
		const unsigned char* EndPtr = AVPData + AVPSize;
		while (AVPData != EndPtr)
			m_vecUserEquipmentInfoValue.push_back(*(AVPData++));
	}
}

CUserEquipmentInfoImpl::~CUserEquipmentInfoImpl()
{
}

UserEquipmentInfoType CUserEquipmentInfoImpl::GetUserEquipmentInfoType()
{
	return m_UserEquipmentInfoType;
}

const unsigned char* CUserEquipmentInfoImpl::GetUserEquipmentInfoValue(unsigned int& _Len)
{
	_Len = static_cast<unsigned int>(m_vecUserEquipmentInfoValue.size());
	return m_vecUserEquipmentInfoValue.data();
}

bool CUserEquipmentInfoImpl::SetUserEquipmentInfoType(UserEquipmentInfoType _UserEquipmentInfoType)
{
	m_UserEquipmentInfoType = _UserEquipmentInfoType;
	m_pAVP->SetAVP(new CDiameterAVP(IAVP::BASIC, m_UserEquipmentInfoType, 459, 0x00, 10415));
	return true;
}

bool CUserEquipmentInfoImpl::SetUserEquipmentInfoValue(const unsigned char* _UserEquipmentInfoValue, unsigned int _Len)
{
	m_vecUserEquipmentInfoValue.clear();
	m_pAVP->SetAVP(new CDiameterAVP(IAVP::BASIC, _UserEquipmentInfoValue, _Len, 460, 0x00, 10415));
	const unsigned char* EndPtr = _UserEquipmentInfoValue + _Len;
	m_vecUserEquipmentInfoValue.clear();
	while (_UserEquipmentInfoValue != EndPtr)
		m_vecUserEquipmentInfoValue.push_back(*(_UserEquipmentInfoValue++));
	return true;
}

}
