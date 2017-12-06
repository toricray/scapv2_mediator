#include "ServiceSpecificInfoImpl.h"

namespace CoreDiam
{

CServiceSpecificInfoImpl::CServiceSpecificInfoImpl(IAVP* _pAVP) :
	m_pAVP(_pAVP)
{
}

CServiceSpecificInfoImpl::CServiceSpecificInfoImpl(const unsigned char* _Data, unsigned int _Size) :
	m_pAVP(NULL)
{
	const unsigned char* AVPData;
	unsigned int AVPSize;
	
	AVPData = GetAVPData(_Data, _Size, 1257, AVPSize);
	if (AVPData)
		m_ServiceSpecificInfoType = ntohl(*(int*)AVPData);
	
	AVPData = GetAVPData(_Data, _Size, 863, AVPSize);
	if (AVPData)
	{
		const unsigned char* EndPtr = AVPData + AVPSize;
		while (AVPData != EndPtr)
			m_vecServiceSpecificInfoData.push_back(*(AVPData++));
	}
}

CServiceSpecificInfoImpl::~CServiceSpecificInfoImpl()
{
}

int CServiceSpecificInfoImpl::GetServiceSpecificInfoType()
{
	return m_ServiceSpecificInfoType;
}

const unsigned char* CServiceSpecificInfoImpl::GetServiceSpecificInfoData(unsigned int& _Len)
{
	_Len = static_cast<unsigned int>(m_vecServiceSpecificInfoData.size());
	return m_vecServiceSpecificInfoData.data();
}

bool CServiceSpecificInfoImpl::SetServiceSpecificInfoType(int _ServiceParameterInfoType)
{
	m_ServiceSpecificInfoType = _ServiceParameterInfoType;
	m_pAVP->SetAVP(new CDiameterAVP(IAVP::BASIC, m_ServiceSpecificInfoType, 1257, 0xC0, 10415));
	return true;
}

bool CServiceSpecificInfoImpl::SetServiceSpecificInfoData(const unsigned char* _ServiceParameterInfoData, unsigned int _Len)
{
	m_vecServiceSpecificInfoData.clear();
	m_pAVP->SetAVP(new CDiameterAVP(IAVP::BASIC, _ServiceParameterInfoData, _Len, 863, 0xC0, 10415));
	const unsigned char* EndPtr = _ServiceParameterInfoData + _Len;
	m_vecServiceSpecificInfoData.clear();
	while (_ServiceParameterInfoData != EndPtr)
		m_vecServiceSpecificInfoData.push_back(*(_ServiceParameterInfoData++));
	return true;
}
	
}
