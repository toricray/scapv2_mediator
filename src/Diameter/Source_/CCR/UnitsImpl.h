#ifndef __UNITS_IMPL_H__
#define __UNITS_IMPL_H__

#include "CCRCCAMessage.h"

#include "MoneyImpl.h"

#include "../RawMessage/DiameterAVP.h"
#include "../RawMessage/DiameterAVPGroup.h"


namespace CoreDiam
{
	
class DIAMCORE_API CUnitsImpl : public CUnits
{
protected:
	int m_Time;
	CMoney* m_pMoney;
	uint64_t m_TotalOctets;
	uint64_t m_InputOctets;
	uint64_t m_OutputOctets;
	uint64_t m_ServiceSpecificUnits;
	unsigned int m_EventChargingTimeStamp;
protected:
	IAVP* m_pAVP;
public:
	CUnitsImpl(IAVP* _pAVP);
	CUnitsImpl(const unsigned char* _Data, unsigned int _Size);
	virtual ~CUnitsImpl();
public:
	virtual int          GetTime();
	virtual CMoney     * GetMoney();
	virtual uint64_t     GetTotalOctets();
	virtual uint64_t     GetInputOctets();
	virtual uint64_t     GetOutputOctets();
	virtual uint64_t     GetServiceSpecificUnits();
	virtual unsigned int GetEventChargingTimeStamp();
public:
	virtual bool         SetTime(int _Time);
	virtual CMoney     * CreateMoney();
	virtual bool         SetTotalOctets(uint64_t _TotalOctets);
	virtual bool         SetInputOctets(uint64_t _InputOctets);
	virtual bool         SetOutputOctets(uint64_t _OutputOctets);
	virtual bool         SetServiceSpecificUnits(uint64_t _ServiceSpecificUnits);
	virtual bool         SetEventChargingTimeStamp(unsigned int _EventChargingTimeStamp);
};

}

#endif //__UNITS_IMPL_H__