#ifndef __MMS_INFORMATION_IMPL_H__
#define __MMS_INFORMATION_IMPL_H__

#include "CCRCCAMessage.h"

#include "OriginatorAddressImpl.h"
#include "RecipientAddressImpl.h"
#include "MessageClassImpl.h"

#include "../RawMessage/DiameterAVP.h"
#include "../RawMessage/DiameterAVPGroup.h"

#include <vector>

namespace CoreDiam
{

typedef std::vector<CRecipientAddress*> VEC_RECIPIENTADDR;

class DIAMCORE_API CMMSInformationImpl : public CMMSInformation
{
protected:
	COriginatorAddress* m_pOriginatorAddress;
	VEC_RECIPIENTADDR m_vecRecipientAddress;
	std::string m_MessageId;
	MessageType m_MessageType;
	int m_MessageSize;
	CMessageClass* m_pMessageClass;
protected:
	IAVP* m_pAVP;
	IAVP* m_pRecipientAddrAVP;
public:
	CMMSInformationImpl(IAVP* _pAVP);
	CMMSInformationImpl(const unsigned char* _Data, unsigned int _Size);
	virtual ~CMMSInformationImpl();
public:
	virtual COriginatorAddress* GetOriginatorAddress();
	virtual CRecipientAddress* GetRecipientAddress(int _Index);
	virtual const char* GetMessageId();
	virtual MessageType GetMessageType();
	virtual int GetMessageSize();
	virtual CMessageClass* GetMessageClass();
public:
	virtual COriginatorAddress* CreateOriginatorAddress();
	virtual CRecipientAddress* CreateRecipientAddress(int _Index);
	virtual bool SetMessageId(const char* _MessageId);
	virtual bool SetMessageType(MessageType _MessageType);
	virtual bool SetMessageSize(int _MessageSize);
	virtual CMessageClass* CreateMessageClass();
};

}

#endif //__MMS_INFORMATION_IMPL_H__
