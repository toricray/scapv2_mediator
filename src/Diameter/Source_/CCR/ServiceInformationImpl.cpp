#include "ServiceInformationImpl.h"

namespace CoreDiam
{
	
CServiceInformationImpl::CServiceInformationImpl(IAVP* _pAVP) :
	m_pIMSInformation(NULL),
	m_pSMSInformation(NULL),
	m_pPSInformation(NULL),
	m_pMMSInformation(NULL),
	m_pAVP(_pAVP)
{
}

CServiceInformationImpl::CServiceInformationImpl(const unsigned char* _Data, unsigned int _Size) :
	m_pIMSInformation(NULL),
	m_pSMSInformation(NULL),
	m_pPSInformation(NULL),
	m_pMMSInformation(NULL),
	m_pAVP(NULL)
{
	const unsigned char* AVPData;
	unsigned int AVPSize;

	AVPData = GetAVPData(_Data, _Size, 876, AVPSize);
	if (AVPData)
		m_pIMSInformation = new CIMSInformationImpl(AVPData, AVPSize);

	AVPData = GetAVPData(_Data, _Size, 2000, AVPSize);
	if (AVPData)
		m_pSMSInformation = new CSMSInformationImpl(AVPData, AVPSize);
	
	AVPData = GetAVPData(_Data, _Size, 874, AVPSize);
	if (AVPData)
		m_pPSInformation = new CPSInformationImpl(AVPData, AVPSize);

	AVPData = GetAVPData(_Data, _Size, 877, AVPSize);
	if (AVPData)
		m_pMMSInformation = new CMMSInformationImpl(AVPData, AVPSize);
}

CServiceInformationImpl::~CServiceInformationImpl()
{
	if (m_pIMSInformation)
		delete m_pIMSInformation;
	if (m_pSMSInformation)
		delete m_pSMSInformation;
	if (m_pPSInformation)
		delete m_pPSInformation;
	if (m_pMMSInformation)
		delete m_pMMSInformation;
}

CIMSInformation* CServiceInformationImpl::GetIMSInformation()
{
	return m_pIMSInformation;
}

CSMSInformation* CServiceInformationImpl::GetSMSInformation()
{
	return m_pSMSInformation;
}

CPSInformation* CServiceInformationImpl::GetPSInformation()
{
	return m_pPSInformation;
}

CMMSInformation* CServiceInformationImpl::GetMMSInformation()
{
	return m_pMMSInformation;
}

CIMSInformation* CServiceInformationImpl::CreateIMSInformation()
{
	if (m_pIMSInformation)
		return NULL;

	IAVP* InfAVP = new CDiameterAVP(IAVP::GROUPED, NULL, 0, 876, 0xC0, 10415);
	m_pAVP->SetAVP(InfAVP);

	m_pIMSInformation = new CIMSInformationImpl(InfAVP);

	return m_pIMSInformation;
}

CSMSInformation* CServiceInformationImpl::CreateSMSInformation()
{
	if (m_pSMSInformation)
		return NULL;
	
	IAVP* InfAVP = new CDiameterAVP(IAVP::GROUPED, NULL, 0, 2000, 0xC0, 10415);
	m_pAVP->SetAVP(InfAVP);
	
	m_pSMSInformation = new CSMSInformationImpl(InfAVP);
	return m_pSMSInformation;
}

CPSInformation* CServiceInformationImpl::CreatePSInformation()
{
	if (m_pPSInformation)
		return NULL;
	
	IAVP* InfAVP = new CDiameterAVP(IAVP::GROUPED, NULL, 0, 874, 0xC0, 10415);
	m_pAVP->SetAVP(InfAVP);
	
	m_pPSInformation = new CPSInformationImpl(InfAVP);
	return m_pPSInformation;
}

CMMSInformation* CServiceInformationImpl::CreateMMSInformation()
{
	if (m_pMMSInformation)
		return NULL;
	
	IAVP* InfAVP = new CDiameterAVP(IAVP::GROUPED, NULL, 0, 877, 0xC0, 10415);
	m_pAVP->SetAVP(InfAVP);
	
	m_pMMSInformation = new CMMSInformationImpl(InfAVP);
	return m_pMMSInformation;
}

}
