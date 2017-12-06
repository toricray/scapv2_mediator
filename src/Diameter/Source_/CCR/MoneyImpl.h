#ifndef __MONEY_IMPL_H__
#define __MONEY_IMPL_H__

#include <math.h>

#include "CCRCCAMessage.h"

#include "../RawMessage/DiameterAVP.h"
#include "../RawMessage/DiameterAVPGroup.h"


namespace CoreDiam
{
	
class DIAMCORE_API CMoneyImpl : public CMoney
{
protected:
	double m_UnitValue;
	int    m_CurrencyCode;
protected:
	IAVP* m_pAVP;
public:
	CMoneyImpl(IAVP* _pAVP);
	CMoneyImpl(const unsigned char* _Data, unsigned int _Size);
	virtual ~CMoneyImpl();
public:
	virtual double GetUnitValue();
	virtual int    GetCurrencyCode();
public:
	virtual bool SetUnitValue(double _UnitValue);
	virtual bool SetCurrencyCode(int _CurrencyCode);
};

}

#endif //__MONEY_IMPL_H__