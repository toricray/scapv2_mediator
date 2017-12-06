#include "TimeStampsImpl.h"

namespace CoreDiam
{

CTimeStampsImpl::CTimeStampsImpl(IAVP* _pAVP) :
	m_pAVP(_pAVP)
{

}

CTimeStampsImpl::CTimeStampsImpl(const unsigned char* _Data, unsigned int _Size) :
	m_SIPResponseTimeStamp(-1)
{
	const unsigned char* AVPData;
	unsigned int AVPSize;

	AVPData = GetAVPData(_Data, _Size, 835, AVPSize);
	if (AVPData)
		m_SIPResponseTimeStamp = ntohl(*(unsigned int*)AVPData);
}

CTimeStampsImpl::~CTimeStampsImpl()
{
}

unsigned int CTimeStampsImpl::GetSIPResponseTimeStamp()
{
	return m_SIPResponseTimeStamp;
}

bool CTimeStampsImpl::SetSIPResponseTimeStamp(unsigned int _SIPResponseTimeStamp)
{
	m_SIPResponseTimeStamp = _SIPResponseTimeStamp;
	m_pAVP->SetAVP(new CDiameterAVP(IAVP::BASIC, (int)m_SIPResponseTimeStamp, 835, 0xC0, 10415));
	return true;
}

}
