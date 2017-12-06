#include "OriginatorReceivedAddressImpl.h"

namespace CoreDiam
{
	
COriginatorReceivedAddressImpl::COriginatorReceivedAddressImpl(IAVP* _pAVP) :
	m_AddressType((AddressType)-1),
	m_AddressData(""),
	m_pAVP(_pAVP)
{

}

COriginatorReceivedAddressImpl::COriginatorReceivedAddressImpl(const unsigned char* _Data, unsigned int _Size) :
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

COriginatorReceivedAddressImpl::~COriginatorReceivedAddressImpl()
{
}

AddressType COriginatorReceivedAddressImpl::GetAddressType()
{
	return m_AddressType;
}

const char* COriginatorReceivedAddressImpl::GetAddressData()
{
	return m_AddressData.c_str();
}

bool COriginatorReceivedAddressImpl::SetAddressType(AddressType _AddressType)
{
	m_AddressType = _AddressType;
	m_pAVP->SetAVP(new CDiameterAVP(IAVP::BASIC, m_AddressType, 899, 0xC0, 10415));
	return true;
}

bool COriginatorReceivedAddressImpl::SetAddressData(const char* _AddressData)
{
	m_AddressData.assign(_AddressData);
	m_pAVP->SetAVP(new CDiameterAVP(IAVP::BASIC, (const unsigned char*)m_AddressData.c_str(), static_cast<unsigned int>(m_AddressData.size()), 897, 0xC0, 10415));
	return true;
}

}
