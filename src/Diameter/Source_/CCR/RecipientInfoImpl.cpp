#include "RecipientInfoImpl.h"

namespace CoreDiam
{

CRecipientInfoImpl::CRecipientInfoImpl(IAVP* _pAVP) :
	m_pAVP(_pAVP)
{
	m_pRecipientAddressAVP = new CDiameterAVPGroup(IAVP::GROUPEDSET, 1201);
	m_pAVP->SetAVP(m_pRecipientAddressAVP);
}

CRecipientInfoImpl::CRecipientInfoImpl(const unsigned char* _Data, unsigned int _Size)
{
	const unsigned char* AVPData;
	unsigned int AVPSize;
	
	AVPData = GetAVPData(_Data, _Size, 2010, AVPSize);
	if (AVPData)
	{
		const unsigned char* EndPtr = AVPData + AVPSize;
		while (AVPData != EndPtr)
			m_vecRecipientSCCPAddress.push_back(*(AVPData++));
	}
	
	AVPData = _Data;
	AVPSize = 0;
	while ((AVPData = GetAVPData(AVPData + AVPSize, _Size - static_cast<unsigned int>(AVPData + AVPSize - _Data), 1201, AVPSize)) != NULL)
		m_vecRecipientAddress.push_back(new CRecipientAddressImpl(AVPData, AVPSize));
}

CRecipientInfoImpl::~CRecipientInfoImpl()
{
	VEC_RECIPIENTADDRESS::iterator begin = m_vecRecipientAddress.begin(), end = m_vecRecipientAddress.end();
	for (; begin != end; begin++)
		delete *begin;
}

CRecipientAddress* CRecipientInfoImpl::GetRecipientAddress(int _Index)
{
	if (_Index < (int)m_vecRecipientAddress.size())
		return m_vecRecipientAddress[_Index];
	return NULL;
}

const unsigned char* CRecipientInfoImpl::GetRecipientSCCPAddress(unsigned int& _Len)
{
	_Len = static_cast<unsigned int>(m_vecRecipientSCCPAddress.size());
	return m_vecRecipientSCCPAddress.data();
}

CRecipientAddress* CRecipientInfoImpl::CreateRecipientAddress(int _Index)
{
	if (static_cast<VEC_RECIPIENTADDRESS::size_type>(_Index) == m_vecRecipientAddress.size())
	{
		IAVP* RAAVP = new CDiameterAVP(IAVP::GROUPED, NULL, 0, 1201, 0xC0, 10415);
		m_vecRecipientAddress.push_back(new CRecipientAddressImpl(RAAVP));
		m_pRecipientAddressAVP->SetAVP(RAAVP, _Index);
		return m_vecRecipientAddress[_Index];
	}
	return NULL;
}

bool CRecipientInfoImpl::SetRecipientSCCPAddress(const unsigned char* _RecipientSCCPAddress, unsigned int _Len)
{
	if (m_pAVP)
		m_pAVP->SetAVP(new CDiameterAVP(IAVP::BASIC, _RecipientSCCPAddress, _Len, 2010, 0xC0, 10415));
	const unsigned char* EndPtr = _RecipientSCCPAddress + _Len;
	m_vecRecipientSCCPAddress.clear();
	while (_RecipientSCCPAddress != EndPtr)
		m_vecRecipientSCCPAddress.push_back(*(_RecipientSCCPAddress++));
	return true;
}

}