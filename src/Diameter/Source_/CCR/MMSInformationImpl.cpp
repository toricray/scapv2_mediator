#include "MMSInformationImpl.h"

namespace CoreDiam
{

CMMSInformationImpl::CMMSInformationImpl(IAVP* _pAVP) :
	m_pOriginatorAddress(NULL),
	m_MessageId(""),
	m_MessageType((MessageType)-1),
	m_MessageSize(-1),
    m_pMessageClass(NULL),
	m_pAVP(_pAVP)
{
	m_pRecipientAddrAVP = new CDiameterAVPGroup(IAVP::GROUPEDSET, 1201);
	m_pAVP->SetAVP(m_pRecipientAddrAVP);
}

CMMSInformationImpl::CMMSInformationImpl(const unsigned char* _Data, unsigned int _Size) :
	m_pOriginatorAddress(NULL),
	m_MessageId(""),
	m_MessageType((MessageType)-1),
	m_MessageSize(-1),
    m_pMessageClass(NULL),
	m_pAVP(NULL),
	m_pRecipientAddrAVP(NULL)
{
	const unsigned char* AVPData;
	unsigned int AVPSize;

	AVPData = GetAVPData(_Data, _Size, 886, AVPSize);
	if (AVPData)
		m_pOriginatorAddress = new COriginatorAddressImpl(AVPData, AVPSize);
	
	AVPData = _Data;
	AVPSize = 0;
	while ((AVPData = GetAVPData(AVPData + AVPSize, _Size - static_cast<unsigned int>(AVPData + AVPSize - _Data), 1201, AVPSize)) != NULL)
		m_vecRecipientAddress.push_back(new CRecipientAddressImpl(AVPData, AVPSize));
	
	AVPData = GetAVPData(_Data, _Size, 1210, AVPSize);
	if (AVPData)
		m_MessageId.assign((const char*)AVPData, AVPSize);

	AVPData = GetAVPData(_Data, _Size, 1211, AVPSize);
	if (AVPData)
		m_MessageType = (MessageType)ntohl(*(int*)AVPData);
	
	AVPData = GetAVPData(_Data, _Size, 1212, AVPSize);
	if (AVPData)
		m_MessageSize = ntohl(*(int*)AVPData);
	
	AVPData = GetAVPData(_Data, _Size, 1213, AVPSize);
	if (AVPData)
		m_pMessageClass = new CMessageClassImpl(AVPData, AVPSize);
}

CMMSInformationImpl::~CMMSInformationImpl()
{
	if (m_pOriginatorAddress)
		delete m_pOriginatorAddress;

	VEC_RECIPIENTADDR::iterator begin = m_vecRecipientAddress.begin(), end = m_vecRecipientAddress.end();
	for (; begin != end; begin++)
		delete *begin;

	if (m_pMessageClass)
		delete m_pMessageClass;
}

COriginatorAddress* CMMSInformationImpl::GetOriginatorAddress()
{
	return m_pOriginatorAddress;
}

CRecipientAddress* CMMSInformationImpl::GetRecipientAddress(int _Index)
{
	if (_Index < (int)m_vecRecipientAddress.size())
		return m_vecRecipientAddress[_Index];
	return NULL;
}

const char* CMMSInformationImpl::GetMessageId()
{
	return m_MessageId.c_str();
}

MessageType CMMSInformationImpl::GetMessageType()
{
	return m_MessageType;
}

int CMMSInformationImpl::GetMessageSize()
{
	return m_MessageSize;
}

CMessageClass* CMMSInformationImpl::GetMessageClass()
{
	return m_pMessageClass;
}

COriginatorAddress* CMMSInformationImpl::CreateOriginatorAddress()
{
	if (m_pOriginatorAddress)
		delete m_pOriginatorAddress;
	IAVP* pAVP = new CDiameterAVP(IAVP::GROUPED, NULL, 0, 886, 0xC0, 10415);
	m_pAVP->SetAVP(pAVP);
	m_pOriginatorAddress = new COriginatorAddressImpl(pAVP);
	return m_pOriginatorAddress;
}

CRecipientAddress* CMMSInformationImpl::CreateRecipientAddress(int _Index)
{
	if (static_cast<VEC_RECIPIENTADDR::size_type>(_Index) == m_vecRecipientAddress.size())
	{
		IAVP* RecipientAddrAVP = new CDiameterAVP(IAVP::GROUPED, NULL, 0, 1201, 0xC0, 10415);
		m_vecRecipientAddress.push_back(new CRecipientAddressImpl(RecipientAddrAVP));
		m_pRecipientAddrAVP->SetAVP(RecipientAddrAVP, _Index);
		return m_vecRecipientAddress[_Index];
	}
	return NULL;
}

bool CMMSInformationImpl::SetMessageId(const char* _MessageId)
{
	m_MessageId.assign(_MessageId);
	m_pAVP->SetAVP(new CDiameterAVP(IAVP::BASIC, (const unsigned char*)m_MessageId.c_str(), static_cast<unsigned int>(m_MessageId.size()), 1210, 0xC0, 10415));
	return true;
}

bool CMMSInformationImpl::SetMessageType(MessageType _MessageType)
{
	m_MessageType = _MessageType;
	m_pAVP->SetAVP(new CDiameterAVP(IAVP::BASIC, m_MessageType, 1211, 0xC0, 10415));
	return true;
}

bool CMMSInformationImpl::SetMessageSize(int _MessageSize)
{
	m_MessageSize = _MessageSize;
	m_pAVP->SetAVP(new CDiameterAVP(IAVP::BASIC, m_MessageSize, 1212, 0xC0, 10415));
	return true;
}

CMessageClass* CMMSInformationImpl::CreateMessageClass()
{
	if (m_pMessageClass)
		delete m_pMessageClass;
	IAVP* pAVP = new CDiameterAVP(IAVP::GROUPED, NULL, 0, 1213, 0xC0, 10415);
	m_pAVP->SetAVP(pAVP);
	m_pMessageClass = new CMessageClassImpl(pAVP);
	return m_pMessageClass;
}

}
