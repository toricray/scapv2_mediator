#ifndef __REDIRECT_SERVER_IMPL_H__
#define __REDIRECT_SERVER_IMPL_H__

#include "CCRCCAMessage.h"

#include "../RawMessage/RawDiameterMessage.h"

#include "../RawMessage/DiameterAVP.h"
#include "../RawMessage/DiameterAVPGroup.h"


#include <vector>

namespace CoreDiam
{

typedef std::vector<unsigned char> VEC_DATA;

class DIAMCORE_API CRedirectServerImpl : public CRedirectServer
{
protected:
	IAVP* m_pAVP;
protected:
	RedirectAddressType m_RedirectAddressType;
	VEC_DATA m_vecRedirectServerAddress;
public:
	CRedirectServerImpl(IAVP* _pAVP);
	CRedirectServerImpl(const unsigned char* _Data, unsigned int _Size);
	virtual ~CRedirectServerImpl();
public:
	virtual RedirectAddressType GetRedirectAddressType();
	virtual const unsigned char* GetRedirectServerAddress(unsigned int& _Len);
public:
	virtual bool SetRedirectAddressType(RedirectAddressType _RedirectAddressType);
	virtual bool SetRedirectServerAddress(const unsigned char* _RedirectServerAddress, unsigned int _Len);
};

}

#endif //__REDIRECT_SERVER_IMPL_H__
