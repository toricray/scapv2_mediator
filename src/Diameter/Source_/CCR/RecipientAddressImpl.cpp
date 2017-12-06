#include "RecipientAddressImpl.h"

namespace CoreDiam
{
	
CRecipientAddressImpl::CRecipientAddressImpl(IAVP* _pAVP) :
	m_AddressType((AddressType)-1),
	m_AddressData(""),
	m_pAVP(_pAVP)
{

}

CRecipientAddressImpl::CRecipientAddressImpl(const unsigned char* _Data, unsigned int _Size) :
	m_AddressType((AddressType)-1),
	m_AddressData(""),
	m_pAVP(NULL)
{
	const unsigned char* AVPData;
	unsigned int AVPSize;

	AVPData = GetAVPData(_Data, _Size, 897, AVPSize);
	if (AVPData)
		m_AddressData.assign((const char*)AVPData, AVPSize);
	
	AVPData = GetAVPData(_Data, _Size, 899, AVPSize);
	if (AVPData)
		m_AddressType = (AddressType)ntohl(*(int*)AVPData);
}

CRecipientAddressImpl::~CRecipientAddressImpl()
{
}

AddressType CRecipientAddressImpl::GetAddressType()
{
	return m_AddressType;
}

const char* CRecipientAddressImpl::GetAddressData()
{
	return m_AddressData.c_str();
}

bool CRecipientAddressImpl::SetAddressType(AddressType _AddressType)
{
	m_AddressType = _AddressType;
	m_pAVP->SetAVP(new CDiameterAVP(IAVP::BASIC, m_AddressType, 899, 0xC0, 10415));
	return true;
}

bool CRecipientAddressImpl::SetAddressData(const char* _AddressData)
{
	m_AddressData.assign(_AddressData);
	m_pAVP->SetAVP(new CDiameterAVP(IAVP::BASIC, (const unsigned char*)m_AddressData.c_str(), static_cast<unsigned int>(m_AddressData.size()), 897, 0xC0, 10415));
	return true;
}

}
