#include "RedirectServerImpl.h"

namespace CoreDiam
{
	
CRedirectServerImpl::CRedirectServerImpl(IAVP* _pAVP) :
	m_pAVP(_pAVP),
	m_RedirectAddressType((RedirectAddressType)-1)
{
}

CRedirectServerImpl::CRedirectServerImpl(const unsigned char* _Data, unsigned int _Size) :
	m_pAVP(NULL),
	m_RedirectAddressType((RedirectAddressType)-1)
{
	const unsigned char* AVPData;
	unsigned int AVPSize;
	
	AVPData = GetAVPData(_Data, _Size, 433, AVPSize);
	if (AVPData)
		m_RedirectAddressType = (RedirectAddressType)ntohl(*(int*)AVPData);
	
	AVPData = GetAVPData(_Data, _Size, 435, AVPSize);
	if (AVPData)
	{
		const unsigned char* EndPtr = AVPData + AVPSize;
		while (AVPData != EndPtr)
			m_vecRedirectServerAddress.push_back(*(AVPData++));
	}
}

CRedirectServerImpl::~CRedirectServerImpl()
{
}

RedirectAddressType CRedirectServerImpl::GetRedirectAddressType()
{
	return m_RedirectAddressType;
}

const unsigned char* CRedirectServerImpl::GetRedirectServerAddress(unsigned int& _Len)
{
	_Len = static_cast<unsigned int>(m_vecRedirectServerAddress.size());
	return m_vecRedirectServerAddress.data();
}

bool CRedirectServerImpl::SetRedirectAddressType(RedirectAddressType _RedirectAddressType)
{
	m_RedirectAddressType = _RedirectAddressType;
	if (m_pAVP)
		m_pAVP->SetAVP(new CDiameterAVP(IAVP::BASIC, m_RedirectAddressType, 433, 0x40, 10415));
	return true;
}

bool CRedirectServerImpl::SetRedirectServerAddress(const unsigned char* _RedirectServerAddress, unsigned int _Len)
{
	if (m_pAVP)
		m_pAVP->SetAVP(new CDiameterAVP(IAVP::BASIC, _RedirectServerAddress, _Len, 435, 0x40, 10415));
	const unsigned char* EndPtr = _RedirectServerAddress + _Len;
	m_vecRedirectServerAddress.clear();
	while (_RedirectServerAddress != EndPtr)
			m_vecRedirectServerAddress.push_back(*(_RedirectServerAddress++));
	return true;
}

}
