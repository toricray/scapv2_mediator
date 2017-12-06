#ifndef __RECIPIENT_ADDRESS_IMPL_H__
#define __RECIPIENT_ADDRESS_IMPL_H__

#include "CCRCCAMessage.h"

#include "../RawMessage/DiameterAVP.h"
#include "../RawMessage/DiameterAVPGroup.h"

#include <vector>

namespace CoreDiam
{
	
class DIAMCORE_API CRecipientAddressImpl : public CRecipientAddress
{
protected:
	AddressType m_AddressType;
	std::string m_AddressData;
protected:
	IAVP* m_pAVP;
public:
	CRecipientAddressImpl(IAVP* _pAVP);
	CRecipientAddressImpl(const unsigned char* _Data, unsigned int _Size);
	virtual ~CRecipientAddressImpl();
public:
	virtual AddressType GetAddressType();
	virtual const char* GetAddressData();
public:
	virtual bool SetAddressType(AddressType _AddressType);
	virtual bool SetAddressData(const char* _AddressData);
};
	
}

#endif //__RECIPIENT_ADDRESS_IMPL_H__
