#ifndef __MSCC_IMPL_H__
#define __MSCC_IMPL_H__

#include "CCRCCAMessage.h"

#include "UnitsImpl.h"
#include "FinalUnitIndicationImpl.h"
#include "ServiceSpecificInfoImpl.h"
#include "PSFurnishChargingInformationImpl.h"

#include "../RawMessage/RawDiameterMessage.h"

#include "../RawMessage/DiameterAVP.h"
#include "../RawMessage/DiameterAVPGroup.h"
#include <algorithm>


namespace CoreDiam {

class DIAMCORE_API CMSCCImpl : public CMSCC
{
	typedef std::vector<CServiceSpecificInfo *>  VEC_SERVICESPECIFICINFO;
	typedef std::vector<int>                     VEC_INT;

protected:
	CDiameterAVPGroup             *m_TriggerTypeAVPGroup;
	CUnits                        *m_pGrantedServiceUnits;
	CUnits                        *m_pRequestedServiceUnits;
	CUnits                        *m_pUsedServiceUnits;
	VEC_INT                        m_vecServiceIdentifier;
	int                            m_RatingGroup;
	int                            m_ValidityTime;
	CFinalUnitIndication          *m_pFinalUnitIndication;
	ReportingReason                m_ReportingReason;
	VEC_SERVICESPECIFICINFO        m_vecServiceSpecificInfo;
	CPSFurnishChargingInformation *m_pPSFurnishChargingInformation;
	VEC_INT                        m_vecTriggerTypes;

	IAVP* m_pAVP;
	IAVP* m_pSSInfoAVP;
	int                            m_ResultCode;


	int                            getTriggerTypeGroupIndex(TriggerType triggerType);

public:
	CMSCCImpl(IAVP* _pAVP);
	CMSCCImpl(const unsigned char* _Data, unsigned int _Size);
	virtual ~CMSCCImpl();

	virtual CUnits                        * GetGrantedServiceUnits(bool _bAutoCreate = false);
	virtual CUnits                        * GetRequestedServiceUnits();
	virtual CUnits                        * GetUsedServiceUnits();

	virtual int                             GetServiceIdentifier(int _Index);
	virtual int                             GetRatingGroup();
	virtual int                             GetValidityTime();
	virtual int                             GetResultCode();
	virtual CFinalUnitIndication          * GetFinalUnitIndication();
	virtual ReportingReason                 GetReportingReason();

	virtual CServiceSpecificInfo          * GetServiceSpecificInfo(int _Index);
	virtual size_t                          GetServiceSpecificInfoCount();

	virtual CPSFurnishChargingInformation * GetPSFurnishChargingInformation();

	virtual CUnits                        * CreateGrantedServiceUnits();
	virtual CUnits                        * CreateRequestedServiceUnits();
	virtual CUnits                        * CreateUsedServiceUnits();
	virtual CFinalUnitIndication          * CreateFinalUnitIndication();
	virtual CServiceSpecificInfo          * CreateServiceSpecificInfo(int _Index);

	virtual CPSFurnishChargingInformation * CreatePSFurnishChargingInformation();

	virtual bool                            SetServiceIdentifier(int _ServiceIdentifier, int _Index);
	virtual bool                            SetRatingGroup(int _RatingGroup);
	virtual bool                            SetValidityTime(int _ValidityTimeSeconds);
	virtual bool                            SetResultCode(int _ResultCode);
	virtual bool                            SetReportingReason(ReportingReason _RR);

	virtual TriggerType                     GetTriggerType(int index);
	virtual size_t                          GetTriggerTypeCount();
	virtual int                             AddTriggerType(TriggerType triggerType);
	virtual bool                            SetTriggerType(unsigned int index, TriggerType triggerType);
	virtual bool                            DelTriggerType(TriggerType triggerType);
	virtual bool                            DelTriggerType(unsigned int index);

	virtual int                             DelTrigger();
};

}

#endif //__MSCC_IMPL_H__
