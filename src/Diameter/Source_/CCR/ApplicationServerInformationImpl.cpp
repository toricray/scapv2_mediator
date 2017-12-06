#include "ApplicationServerInformationImpl.h"

namespace CoreDiam
{

CApplicationServerInformationImpl::CApplicationServerInformationImpl(IAVP* _pAVP) :
	m_pAVP(_pAVP)
{

}

CApplicationServerInformationImpl::CApplicationServerInformationImpl(const unsigned char* _Data, unsigned int _Size)
{
	const unsigned char* AVPData;
	unsigned int AVPSize;

	AVPData = GetAVPData(_Data, _Size, 836, AVPSize);
	if (AVPData)
		m_ApplicationServer.assign((const char*)AVPData, AVPSize);

	AVPData = GetAVPData(_Data, _Size, 837, AVPSize);
	if (AVPData)
		m_ApplicationProvidedCalledPartyAddress.assign((const char*)AVPData, AVPSize);
}

CApplicationServerInformationImpl::~CApplicationServerInformationImpl()
{
}

const char* CApplicationServerInformationImpl::GetApplicationServer()
{
	return m_ApplicationServer.c_str();
}

const char* CApplicationServerInformationImpl::GetApplicationProvidedCalledPartyAddress()
{
	return m_ApplicationProvidedCalledPartyAddress.c_str();
}

bool CApplicationServerInformationImpl::SetApplicationServer(const char* _ApplicationServer)
{
	m_ApplicationServer.assign(_ApplicationServer);
	m_pAVP->SetAVP(new CDiameterAVP(IAVP::BASIC, (const unsigned char*)m_ApplicationServer.c_str(), static_cast<unsigned int>(m_ApplicationServer.size()), 836, 0xC0, 10415));
	return true;
}

bool CApplicationServerInformationImpl::SetApplicationProvidedCalledPartyAddress(const char* _ApplicationProvidedCalledPartyAddress)
{
	m_ApplicationProvidedCalledPartyAddress.assign(_ApplicationProvidedCalledPartyAddress);
	m_pAVP->SetAVP(new CDiameterAVP(IAVP::BASIC, (const unsigned char*)m_ApplicationProvidedCalledPartyAddress.c_str(), static_cast<unsigned int>(m_ApplicationProvidedCalledPartyAddress.size()), 837, 0xC0, 10415));
	return true;
}

}