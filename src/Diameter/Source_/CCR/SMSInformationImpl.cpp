#include "SMSInformationImpl.h"

namespace CoreDiam
{

CSMSInformationImpl::CSMSInformationImpl(IAVP* _pAVP) :
	m_SMSNode((SMSNode)-1),
	m_SMSCAddressFamily((unsigned short)-1),
	m_pOriginatorReceivedAddress(NULL),
	m_pAVP(_pAVP)
{
	m_pRecipientInfoAVP = new CDiameterAVPGroup(IAVP::GROUPEDSET, 2026);
	m_pAVP->SetAVP(m_pRecipientInfoAVP);
}

CSMSInformationImpl::CSMSInformationImpl(const unsigned char* _Data, unsigned int _Size) :
	m_SMSNode((SMSNode)-1),
	m_SMSCAddressFamily((unsigned short)-1),
	m_pOriginatorReceivedAddress(NULL)
{
	const unsigned char* AVPData;
	unsigned int AVPSize;

	AVPData = GetAVPData(_Data, _Size, 2016, AVPSize);
	if (AVPData)
		m_SMSNode = (SMSNode)ntohl(*(int*)AVPData);
	
	AVPData = GetAVPData(_Data, _Size, 2017, AVPSize);
	if (AVPData && AVPSize >= 2)
	{
		m_SMSCAddressFamily = ntohs(*(unsigned short*)AVPData);
		const unsigned char* EndPtr = AVPData + AVPSize;
		AVPData += 2;
		while (AVPData != EndPtr)
			m_vecSMSCAddress.push_back(*(AVPData++));
	}

	AVPData = GetAVPData(_Data, _Size, 2008, AVPSize);
	if (AVPData)
	{
		const unsigned char* EndPtr = AVPData + AVPSize;
		while (AVPData != EndPtr)
			m_vecOriginatorSCCPAddress.push_back(*(AVPData++));
	}

	AVPData = _Data;
	AVPSize = 0;
	while ((AVPData = GetAVPData(AVPData + AVPSize, _Size - static_cast<unsigned int>(AVPData + AVPSize - _Data), 2026, AVPSize)) != NULL)
		m_vecRecipientInfo.push_back(new CRecipientInfoImpl(AVPData, AVPSize));

	AVPData = GetAVPData(_Data, _Size, 2027, AVPSize);
	if (AVPData)
		m_pOriginatorReceivedAddress = new COriginatorReceivedAddressImpl(AVPData, AVPSize);
}

CSMSInformationImpl::~CSMSInformationImpl()
{
	VEC_RECIPIENTINFO::iterator begin = m_vecRecipientInfo.begin(), end = m_vecRecipientInfo.end();
	for (; begin != end; begin++)
		delete *begin;
	m_vecRecipientInfo.clear();

	if (m_pOriginatorReceivedAddress)
		delete m_pOriginatorReceivedAddress;
}

SMSNode CSMSInformationImpl::GetSMSNode()
{
	return m_SMSNode;
}

const unsigned char* CSMSInformationImpl::GetSMSCAddress(unsigned short& _AddressFamily, unsigned int& _Len)
{
	_AddressFamily = m_SMSCAddressFamily;
	_Len = static_cast<unsigned int>(m_vecSMSCAddress.size());
	return m_vecSMSCAddress.data();
}

const unsigned char* CSMSInformationImpl::GetOriginatorSCCPAddress(unsigned int& _Len)
{
	_Len = static_cast<unsigned int>(m_vecOriginatorSCCPAddress.size());
	return m_vecOriginatorSCCPAddress.data();
}

CRecipientInfo* CSMSInformationImpl::GetRecipientInfo(int _Index)
{
	if(_Index < static_cast<int>(m_vecRecipientInfo.size()))
		return m_vecRecipientInfo[_Index];
	return NULL;
}

COriginatorReceivedAddress* CSMSInformationImpl::GetOriginatorReceivedAddress()
{
	return m_pOriginatorReceivedAddress;
}

bool CSMSInformationImpl::SetSMSNode(SMSNode _SMSNode)
{
	m_SMSNode = _SMSNode;
	if (m_pAVP)
		m_pAVP->SetAVP(new CDiameterAVP(IAVP::BASIC, m_SMSNode, 2016, 0xC0, 10415));
	return true;
}

bool CSMSInformationImpl::SetSMSCAddress(unsigned short _AddressFamily, const unsigned char* _SMSCAddress, unsigned int _Len)
{
	if (m_pAVP)
	{
		unsigned char* Buf = new unsigned char[_Len + 2];
		unsigned short NetFamily = htons(_AddressFamily);
		memcpy(Buf, &NetFamily, sizeof(unsigned short));
		memcpy(&Buf[2], _SMSCAddress, _Len);
		m_pAVP->SetAVP(new CDiameterAVP(IAVP::BASIC, Buf, _Len + 2, 2017, 0xC0, 10415));
		delete[] Buf;
	}
	const unsigned char* EndPtr = _SMSCAddress + _Len;
	m_vecSMSCAddress.clear();
	while (_SMSCAddress != EndPtr)
		m_vecSMSCAddress.push_back(*(_SMSCAddress++));
	m_SMSCAddressFamily = _AddressFamily;
	return true;
}

bool CSMSInformationImpl::SetOriginatorSCCPAddress(const unsigned char* _OriginatorSCCPAddress, unsigned int _Len)
{
	if (m_pAVP)
		m_pAVP->SetAVP(new CDiameterAVP(IAVP::BASIC, _OriginatorSCCPAddress, _Len, 2008, 0xC0, 10415));
	const unsigned char* EndPtr = _OriginatorSCCPAddress + _Len;
	while (_OriginatorSCCPAddress != EndPtr)
		m_vecOriginatorSCCPAddress.push_back(*(_OriginatorSCCPAddress++));
	return true;
}

CRecipientInfo* CSMSInformationImpl::CreateRecipientInfo(int _Index)
{
	if (_Index == (int)m_vecRecipientInfo.size())
	{
		IAVP* RIAVP = new CDiameterAVP(IAVP::GROUPED, NULL, 0, 2026, 0xC0, 10415);
		CRecipientInfoImpl* RI = new CRecipientInfoImpl(RIAVP);
		m_vecRecipientInfo.push_back(RI);
		m_pRecipientInfoAVP->SetAVP(RIAVP, _Index);
		return m_vecRecipientInfo[_Index];
	}
	return NULL;
}

COriginatorReceivedAddress* CSMSInformationImpl::CreateOriginatorReceivedAddress()
{
	if (m_pOriginatorReceivedAddress)
		return NULL;
	IAVP* ORAAVP = new CDiameterAVP(IAVP::GROUPED, NULL, 0, 2027, 0xC0, 10415);
	m_pOriginatorReceivedAddress = new COriginatorReceivedAddressImpl(ORAAVP);
	m_pAVP->SetAVP(ORAAVP);
	
	return m_pOriginatorReceivedAddress;
}

}
