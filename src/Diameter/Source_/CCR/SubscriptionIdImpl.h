#ifndef __SUBSCRIPTION_ID_IMPL_H__
#define __SUBSCRIPTION_ID_IMPL_H__

#include "CCRCCAMessage.h"
#include <vector>

#include "../RawMessage/DiameterAVP.h"

#include "../RawMessage/DiameterAVP.h"
#include "../RawMessage/DiameterAVPGroup.h"

namespace CoreDiam
{

typedef std::vector<unsigned char> VEC_DATA;

class DIAMCORE_API CSubscriptionIdImpl : public CSubscriptionId
{
protected:
	SubscriptionIdType m_SubscriptionIdType;
	VEC_DATA m_vecSubscriptionIdData;
protected:
	IAVP* m_pAVP;
public:
	CSubscriptionIdImpl(IAVP* _pAVP);
	CSubscriptionIdImpl(const unsigned char* _Data, unsigned int _Size);
	virtual ~CSubscriptionIdImpl();
public:
	virtual SubscriptionIdType GetSubscriptionIdType();
	virtual const unsigned char* GetSubscriptionIdData(unsigned int& _Len);
public:
	virtual bool SetSubscriptionIdType(SubscriptionIdType _SubscriptionIdType);
	virtual bool SetSubscriptionIdData(const unsigned char* _SubscriptionIdValue, unsigned int _Len);
};

	class DIAMCORE_API COtherPartyIdImpl : public COtherPartyId
	{
	protected:
		int m_OtherPartyIdNature;
		int m_OtherPartyIdType;
		std::string m_OtherPartyIdData;
	protected:
		IAVP* m_pAVP;
	public:
		COtherPartyIdImpl(IAVP* _pAVP);
		COtherPartyIdImpl(const unsigned char* _Data, unsigned int _Size);
		virtual ~COtherPartyIdImpl();
	public:
		virtual int    GetOtherPartyIdType();
		virtual int    GetOtherPartyIdNature();
		virtual const char * GetOtherPartyIdData(unsigned int& _Len);
	public:
		virtual bool SetOtherPartyIdType(int _SubscriptionIdType);
		virtual bool SetOtherPartyIdData(const unsigned char* _OtherPartyIdValue, unsigned int _Len);
		virtual bool SetOtherPartyIdNature(int _SubscriptionIdNature);
	};

}

#endif //__SUBSCRIPTION_ID_IMPL_H__
