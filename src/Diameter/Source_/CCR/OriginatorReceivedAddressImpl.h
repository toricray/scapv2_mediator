#ifndef __ORIGINATOR_RECEIVED_ADDRESS_IMPL_H__
#define __ORIGINATOR_RECEIVED_ADDRESS_IMPL_H__

#include "CCRCCAMessage.h"

#include "../RawMessage/DiameterAVP.h"
#include "../RawMessage/DiameterAVPGroup.h"

#include <vector>

namespace CoreDiam
{
	
typedef std::vector<unsigned char> VEC_DATA;

class DIAMCORE_API COriginatorReceivedAddressImpl : public COriginatorReceivedAddress
{
protected:
	AddressType m_AddressType;
	std::string m_AddressData;
protected:
	IAVP* m_pAVP;
public:
	COriginatorReceivedAddressImpl(IAVP* _pAVP);
	COriginatorReceivedAddressImpl(const unsigned char* _Data, unsigned int _Size);
	virtual ~COriginatorReceivedAddressImpl();
public:
	virtual AddressType GetAddressType();
	virtual const char* GetAddressData();
public:
	virtual bool SetAddressType(AddressType _AddressType);
	virtual bool SetAddressData(const char* _AddressData);
};
	
}

#endif //__ORIGINATOR_RECEIVED_ADDRESS_IMPL_H__
