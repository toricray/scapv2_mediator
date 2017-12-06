#ifndef __RECIPIENT_INFO_IMPL_H__
#define __RECIPIENT_INFO_IMPL_H__

#include "CCRCCAMessage.h"

#include "RecipientAddressImpl.h"

#include "../RawMessage/DiameterAVP.h"
#include "../RawMessage/DiameterAVPGroup.h"


#include <vector>

namespace CoreDiam
{
	
typedef std::vector<unsigned char> VEC_DATA;
typedef std::vector<CRecipientAddress*> VEC_RECIPIENTADDRESS;

class DIAMCORE_API CRecipientInfoImpl : public CRecipientInfo
{
protected:
	VEC_DATA m_vecRecipientSCCPAddress;	
	VEC_RECIPIENTADDRESS m_vecRecipientAddress;
protected:
	IAVP* m_pAVP;
	IAVP* m_pRecipientAddressAVP;
public:
	CRecipientInfoImpl(IAVP* _pAVP);
	CRecipientInfoImpl(const unsigned char* _Data, unsigned int _Size);
	virtual ~CRecipientInfoImpl();
public:
	virtual CRecipientAddress* GetRecipientAddress(int _Index);
	virtual const unsigned char* GetRecipientSCCPAddress(unsigned int& _Len);
public:
	virtual CRecipientAddress* CreateRecipientAddress(int _Index);
	virtual bool SetRecipientSCCPAddress(const unsigned char* _RecipientSCCPAddress, unsigned int _Len);
};
	
}

#endif //__RECIPIENT_INFO_IMPL_H__
