#include "FinalUnitIndicationImpl.h"

namespace CoreDiam
{
	
CFinalUnitIndicationImpl::CFinalUnitIndicationImpl(IAVP* _pAVP) :
	m_pAVP(_pAVP),
	m_FinalUnitAction((FinalUnitAction)-1),
	m_pRedirectServer(NULL)
{
}

CFinalUnitIndicationImpl::CFinalUnitIndicationImpl(const unsigned char* _Data, unsigned int _Size) :
	m_pAVP(NULL),
	m_FinalUnitAction((FinalUnitAction)-1),
	m_pRedirectServer(NULL)
{
	const unsigned char* AVPData;
	unsigned int AVPSize;
	
	AVPData = GetAVPData(_Data, _Size, 449, AVPSize);
	if (AVPData)
		m_FinalUnitAction = (FinalUnitAction)ntohl(*(int*)AVPData);
	
	AVPData = GetAVPData(_Data, _Size, 434, AVPSize);
	if (AVPData)
		m_pRedirectServer = new CRedirectServerImpl(AVPData, AVPSize);
}

CFinalUnitIndicationImpl::~CFinalUnitIndicationImpl()
{
	if (m_pRedirectServer)
		delete m_pRedirectServer;
}

FinalUnitAction CFinalUnitIndicationImpl::GetFinalUnitAction()
{
	return m_FinalUnitAction;
}

CRedirectServer* CFinalUnitIndicationImpl::GetRedirectServer()
{
	return m_pRedirectServer;
}

bool CFinalUnitIndicationImpl::SetFinalUnitAction(FinalUnitAction _FUI)
{
	m_FinalUnitAction = _FUI;
	m_pAVP->SetAVP(new CDiameterAVP(IAVP::BASIC, m_FinalUnitAction, 449, 0x40, 10415));
	return true;
}

CRedirectServer* CFinalUnitIndicationImpl::CreateRedirectServer()
{
	if (m_pRedirectServer)
		delete m_pRedirectServer;
	IAVP* pAVP = new CDiameterAVP(IAVP::GROUPED, NULL, 0, 434, 0x40, 10415);
	m_pAVP->SetAVP(pAVP);
	m_pRedirectServer = new CRedirectServerImpl(pAVP);
	return m_pRedirectServer;
}

}
