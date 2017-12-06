#include "EventTypeImpl.h"

namespace CoreDiam
{

CEventTypeImpl::CEventTypeImpl(IAVP* _pAVP) :
	m_pAVP(_pAVP),
	m_SIPMethod(""),
	m_Event(""),
	m_Expires(-1)
{

}

CEventTypeImpl::CEventTypeImpl(const unsigned char* _Data, unsigned int _Size) :
	m_SIPMethod(""),
	m_Event(""),
	m_Expires(-1)
{
	const unsigned char* AVPData;
	unsigned int AVPSize;

	AVPData = GetAVPData(_Data, _Size, 824, AVPSize);
	if (AVPData)
	{
		const unsigned char* pEnd = AVPData + AVPSize;
		while(AVPData != pEnd)
			m_SIPMethod.push_back(*(const char*)(AVPData++));
	}

	AVPData = GetAVPData(_Data, _Size, 825, AVPSize);
	if (AVPData)
	{
		const unsigned char* pEnd = AVPData + AVPSize;
		while(AVPData != pEnd)
			m_Event.push_back(*(const char*)(AVPData++));
	}

	AVPData = GetAVPData(_Data, _Size, 888, AVPSize);
	if (AVPData)
		m_Expires = ntohl(*(int*)AVPData);

}

CEventTypeImpl::~CEventTypeImpl()
{
}

const char* CEventTypeImpl::GetSIPMethod()
{
	return m_SIPMethod.c_str();
}

const char* CEventTypeImpl::GetEvent()
{
	return m_Event.c_str();
}

unsigned int CEventTypeImpl::GetExpires()
{
	return m_Expires;
}

bool CEventTypeImpl::SetSIPMethod(const char* _SIPMethod)
{
	m_SIPMethod.assign(_SIPMethod);
	return true;
}

bool CEventTypeImpl::SetEvent(const char* _Event)
{
	m_Event.assign(_Event);
	m_pAVP->SetAVP(new CDiameterAVP(IAVP::BASIC, (const unsigned char*)m_Event.c_str(), static_cast<unsigned int>(m_Event.size()), 825, 0xC0, 10415));
	return true;
}

bool CEventTypeImpl::SetExpires(unsigned int _Expires)
{
	m_Expires = _Expires;
	m_pAVP->SetAVP(new CDiameterAVP(IAVP::BASIC, (int)m_Expires, 888, 0xC0, 10415));
	return true;
}

} // namespace CoreDiam
