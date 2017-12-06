#include "MessageClassImpl.h"

namespace CoreDiam
{

CMessageClassImpl::CMessageClassImpl(IAVP* _pAVP) :
	m_ClassIdentifier((ClassIdentifier)-1),
	m_pAVP(_pAVP)
{

}

CMessageClassImpl::CMessageClassImpl(const unsigned char* _Data, unsigned int _Size) :
	m_ClassIdentifier((ClassIdentifier)-1),
	m_pAVP(NULL)
{
	const unsigned char* AVPData;
	unsigned int AVPSize;
	
	AVPData = GetAVPData(_Data, _Size, 1214, AVPSize);
	if (AVPData)
		m_ClassIdentifier = (ClassIdentifier)ntohl(*(int*)AVPData);
}

CMessageClassImpl::~CMessageClassImpl()
{

}

ClassIdentifier CMessageClassImpl::GetClassIdentifier()
{
	return m_ClassIdentifier;
}

bool CMessageClassImpl::SetClassIdentifier(ClassIdentifier _ClassIdentifier)
{
	m_ClassIdentifier = _ClassIdentifier;
	m_pAVP->SetAVP(new CDiameterAVP(IAVP::BASIC, m_ClassIdentifier, 1214, 0xC0, 10415));
	return true;
}

}