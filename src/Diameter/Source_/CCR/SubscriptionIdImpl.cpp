#include "SubscriptionIdImpl.h"

namespace CoreDiam
{

CSubscriptionIdImpl::CSubscriptionIdImpl(IAVP* _pAVP) :
	m_pAVP(_pAVP)
{
	m_SubscriptionIdType = (SubscriptionIdType)-1;
}

CSubscriptionIdImpl::CSubscriptionIdImpl(const unsigned char* _Data, unsigned int _Size) :
	m_pAVP(NULL)
{
	const unsigned char* AVPData;
	unsigned int AVPSize;
	
	AVPData = GetAVPData(_Data, _Size, 450, AVPSize);
	if (AVPData)
		m_SubscriptionIdType = (SubscriptionIdType)ntohl(*(int*)AVPData);
	
	AVPData = GetAVPData(_Data, _Size, 444, AVPSize);
	if (AVPData)
	{
		const unsigned char* EndPtr = AVPData + AVPSize;
		while (AVPData != EndPtr)
			m_vecSubscriptionIdData.push_back(*(AVPData++));//!!!
	}
}

CSubscriptionIdImpl::~CSubscriptionIdImpl()
{
}

SubscriptionIdType CSubscriptionIdImpl::GetSubscriptionIdType()
{
	return m_SubscriptionIdType;
}

const unsigned char* CSubscriptionIdImpl::GetSubscriptionIdData(unsigned int& _Len)
{
	_Len = static_cast<unsigned int>(m_vecSubscriptionIdData.size());
	return m_vecSubscriptionIdData.data();
}

bool CSubscriptionIdImpl::SetSubscriptionIdType(SubscriptionIdType _SubscriptionIdType)
{
	m_SubscriptionIdType = _SubscriptionIdType;
	m_pAVP->SetAVP(new CDiameterAVP(IAVP::BASIC, m_SubscriptionIdType, 450, 0x40, 10415));
	return true;
}

bool CSubscriptionIdImpl::SetSubscriptionIdData(const unsigned char* _SubscriptionIdValue, unsigned int _Len)
{
	m_vecSubscriptionIdData.clear();
	m_pAVP->SetAVP(new CDiameterAVP(IAVP::BASIC, _SubscriptionIdValue, _Len, 444, 0x40, 10415));
	const unsigned char* EndPtr = _SubscriptionIdValue + _Len;
	m_vecSubscriptionIdData.clear();
	while (_SubscriptionIdValue != EndPtr)
		m_vecSubscriptionIdData.push_back(*(_SubscriptionIdValue++));
	return true;
}


    COtherPartyIdImpl::COtherPartyIdImpl(IAVP* _pAVP) :
			m_pAVP(_pAVP)
	{
		m_OtherPartyIdType = (SubscriptionIdType)-1;
	}

    COtherPartyIdImpl::COtherPartyIdImpl(const unsigned char* _Data, unsigned int _Size) :
			m_pAVP(NULL)
	{
		const unsigned char* AVPData;
		unsigned int AVPSize;

		AVPData = GetAVPData(_Data, _Size, 1078, AVPSize);
		if (AVPData)
			m_OtherPartyIdType = (int)ntohl(*(int*)AVPData);

		AVPData = GetAVPData(_Data, _Size, 1076, AVPSize);
		if (AVPData)
			m_OtherPartyIdNature = (int)ntohl(*(int*)AVPData);

		AVPData = GetAVPData(_Data, _Size, 1077, AVPSize);
		if (AVPData)
		{
			const unsigned char* EndPtr = AVPData + AVPSize;
			while (AVPData != EndPtr)
				m_OtherPartyIdData.push_back(*(AVPData++));
		}
	}

    COtherPartyIdImpl::~COtherPartyIdImpl()
	{
	}

	int COtherPartyIdImpl::GetOtherPartyIdType()
	{
		return m_OtherPartyIdType;
	}

    int COtherPartyIdImpl::GetOtherPartyIdNature()
    {
        return m_OtherPartyIdNature;
    }

	const char* COtherPartyIdImpl::GetOtherPartyIdData(unsigned int& _Len)
	{
		_Len = static_cast<unsigned int>(m_OtherPartyIdData.size());
		return m_OtherPartyIdData.c_str();
	}

	bool COtherPartyIdImpl::SetOtherPartyIdType(int _SubscriptionIdType)
	{
        m_OtherPartyIdType = _SubscriptionIdType;
		m_pAVP->SetAVP(new CDiameterAVP(IAVP::BASIC, m_OtherPartyIdType, 1078, 0xC0, 193));
		return true;
	}

    bool COtherPartyIdImpl::SetOtherPartyIdNature(int _OtherPartyIdNature)
    {
        m_OtherPartyIdNature = _OtherPartyIdNature;
        m_pAVP->SetAVP(new CDiameterAVP(IAVP::BASIC, m_OtherPartyIdNature, 1076, 0xC0, 193));
        return true;
    }

	bool COtherPartyIdImpl::SetOtherPartyIdData(const unsigned char* _SubscriptionIdValue, unsigned int _Len)
	{
        m_OtherPartyIdData.clear();
		m_pAVP->SetAVP(new CDiameterAVP(IAVP::BASIC, _SubscriptionIdValue, _Len, 1077, 0xC0, 193));
		const unsigned char* EndPtr = _SubscriptionIdValue + _Len;
        m_OtherPartyIdData.clear();
		while (_SubscriptionIdValue != EndPtr)
            m_OtherPartyIdData.push_back(*(_SubscriptionIdValue++));
		return true;
	}

}
