#ifndef __FINAL_UNITS_INDICATION_IMPL_H__
#define __FINAL_UNITS_INDICATION_IMPL_H__

#include "CCRCCAMessage.h"

#include "RedirectServerImpl.h"

#include "../RawMessage/RawDiameterMessage.h"

#include "../RawMessage/DiameterAVP.h"
#include "../RawMessage/DiameterAVPGroup.h"


namespace CoreDiam
{

class DIAMCORE_API CFinalUnitIndicationImpl : public CFinalUnitIndication
{
protected:
	IAVP* m_pAVP;
protected:
	FinalUnitAction m_FinalUnitAction;
	CRedirectServerImpl* m_pRedirectServer;
public:
	CFinalUnitIndicationImpl(IAVP* _pAVP);
	CFinalUnitIndicationImpl(const unsigned char* _Data, unsigned int _Size);
	virtual ~CFinalUnitIndicationImpl();
public:
	virtual FinalUnitAction GetFinalUnitAction();
	virtual CRedirectServer* GetRedirectServer();
public:
	virtual bool SetFinalUnitAction(FinalUnitAction _FUI);
	virtual CRedirectServer* CreateRedirectServer();
};

}

#endif //__FINAL_UNITS_INDICATION_IMPL_H__
