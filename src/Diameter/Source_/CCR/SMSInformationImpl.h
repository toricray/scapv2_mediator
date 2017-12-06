#ifndef __SMS_INFORMATION_IMPL_H__
#define __SMS_INFORMATION_IMPL_H__

#include "CCRCCAMessage.h"

#include "RecipientInfoImpl.h"
#include "OriginatorReceivedAddressImpl.h"

#include "../RawMessage/DiameterAVP.h"
#include "../RawMessage/DiameterAVPGroup.h"

#include <vector>

namespace CoreDiam
{

typedef std::vector<unsigned char> VEC_DATA;
typedef std::vector<CRecipientInfo*> VEC_RECIPIENTINFO;

class DIAMCORE_API CSMSInformationImpl : public CSMSInformation
{
protected:
	SMSNode m_SMSNode;
	VEC_DATA m_vecSMSCAddress;
	unsigned short m_SMSCAddressFamily;
	VEC_DATA m_vecOriginatorSCCPAddress;
	VEC_RECIPIENTINFO m_vecRecipientInfo;
	COriginatorReceivedAddress* m_pOriginatorReceivedAddress;
	
protected:
	IAVP* m_pAVP;
	IAVP* m_pRecipientInfoAVP;
public:
	CSMSInformationImpl(IAVP* _pAVP);
	CSMSInformationImpl(const unsigned char* _Data, unsigned int _Size);
	virtual ~CSMSInformationImpl();
public:
	virtual SMSNode GetSMSNode();
	virtual const unsigned char* GetSMSCAddress(unsigned short& _AddressFamily, unsigned int& _Len);
	virtual const unsigned char* GetOriginatorSCCPAddress(unsigned int& _Len);
	virtual CRecipientInfo* GetRecipientInfo(int _Index = 0);
	virtual COriginatorReceivedAddress* GetOriginatorReceivedAddress();
public:
	virtual bool SetSMSNode(SMSNode _SMSNode);
	virtual bool SetSMSCAddress(unsigned short _AddressFamily, const unsigned char* _SMSCAddress, unsigned int _Len);
	virtual bool SetOriginatorSCCPAddress(const unsigned char* _OriginatorSCCPAddress, unsigned int _Len);
	virtual CRecipientInfo* CreateRecipientInfo(int _Index);
	virtual COriginatorReceivedAddress* CreateOriginatorReceivedAddress();
};

}

#endif //__SMS_INFORMATION_IMPL_H__
