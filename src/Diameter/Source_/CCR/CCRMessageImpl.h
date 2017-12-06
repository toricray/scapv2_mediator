#ifndef __CCR_MESSAGE_IMPL_H__
#define __CCR_MESSAGE_IMPL_H__

#include "CCRCCAMessage.h"

#include <vector>
#include <string>


#include "SubscriptionIdImpl.h"
#include "UserEquipmentInfoImpl.h"
#include "MSCCImpl.h"
#include "ServiceInformationImpl.h"
#include "ServiceParameterInfoImpl.h"

namespace CoreDiam
{

class DIAMCORE_API CCCRMessageImpl : public CCCRMessage
{
	typedef std::vector<CSubscriptionId *>       VEC_SUBSCRIPTIONID;
	typedef std::vector<CMSCC *>                 VEC_MSCC;
	typedef std::vector<CServiceParameterInfo *> VEC_SPI;

protected:
	RequestType          m_RequestType;
	TrafficCase			 m_TrafficCase;
	unsigned int         m_EventTimestamp;
	int                  m_TerminationCause;
	int          		 m_Timezone;
	std::string          m_SessionId;
    std::string          m_SubscriptionIdLocation;
	std::string          m_OriginHost;
	std::string          m_OriginRealm;
	std::string          m_DestinationHost;
	std::string          m_DestinationRealm;
	int                  m_AuthApplicationId;
	std::string          m_ServiceContextId;
	int                  m_RequestNumber;
	int          		 m_ServiceIdentifier;
	int                  m_RequestedAction;
	VEC_SUBSCRIPTIONID   m_vecSubscriptionID;
	CUserEquipmentInfo  *m_pUserEquipmentInfo;
	VEC_MSCC             m_vecMSCC;
    VEC_SPI              m_vecSPI;
	CServiceInformation *m_pServiceInformation;

	CUnits              *m_pRequestedServiceUnits;
	COtherPartyId*       m_OtherPartyId;
    CUnits              *m_pUsedServiceUnits;

	IDiameterMessage    *m_pRawMessage;
	IAVP                *m_pAVP;
	IAVP                *m_pMSCCAVP;
    IAVP                *m_pSPIAVP;
	IAVP                *m_pSubscriptionIdAVP;
	IAVP                *m_pServiceInformationAVP;

public:
	CCCRMessageImpl();
	CCCRMessageImpl(const unsigned char* _Data, unsigned int _Size, IDiameterMessage* _pRawMessage);
	virtual ~CCCRMessageImpl();

	// CCRMessage
	virtual const char          * GetSessionId();
	virtual const char          * GetOriginHost();
	virtual const char          * GetOriginRealm();
	virtual const char          * GetDestinationRealm();
	virtual int                   GetAuthApplicationId();
	virtual const char          * GetServiceContextId();
	virtual int                   GetRequestNumber();
	virtual RequestType           GetRequestType();
	virtual TrafficCase           GetTrafficCase();
	virtual unsigned int          GetEventTimestamp();
	virtual CSubscriptionId     * GetSubscriptionId(int _Index);
	virtual int                   GetTerminationCause();            // [295]
	virtual int                   GetTimezone(){ return  m_Timezone;};            // [295]
	virtual int                   GetRequestedAction();             // [436]
	virtual CMSCC               * GetMSCC(int _Index = 0);
    virtual CServiceParameterInfo               * GetSPI(int _Index = 0);
	virtual CUserEquipmentInfo  * GetUserEquipmentInfo();
	virtual CServiceInformation * GetServiceInformation();
	virtual int                   GetServiceIdentifier();
	virtual const char *          GetDestinationHost();
	virtual CUnits              * CreateRequestedServiceUnits();
	virtual CUnits              * GetRequestedServiceUnits();
    virtual CUnits              * CreateUsedServiceUnits();
    virtual CUnits              * GetUsedServiceUnits();
	virtual COtherPartyId          * GetOtherPartyId();
	virtual const char          * GetSubscriptionIdLocation();

	virtual bool                  SetSessionId(const char* _SessionId);
	virtual bool                  SetOriginHost(const char* _OriginHost);
	virtual bool                  SetOriginRealm(const char* _OriginRealm);
	virtual bool                  SetDestinationRealm(const char* _DestinationRealm);
	virtual bool                  SetAuthApplicationId(int _AuthApplicationId);
	virtual bool                  SetServiceContextId(const char* _ServiceContextId);
	virtual bool                  SetRequestNumber(int _RequestNumber);
	virtual bool                  SetRequestType(RequestType _RequestType);
    virtual bool                  SetTrafficCase(TrafficCase _TrafficCase);
	virtual bool                  SetEventTimestamp(unsigned int _EventTimestamp);
	virtual bool                  SetTerminationCause(int _terminationCause);            // [295]
	virtual bool                  SetTimezone(int _timezone);            // [295]
	virtual bool                  SetRequestedAction(int _requestedAction);              // [436]
	virtual bool                  SetServiceIdentifier(int _ServiceIdentifier);
	virtual bool                  SetDestinationHost(const char *_DestinationHost);
    virtual bool                  SetSubscriptionIdLocation(const char *_SubscriptionIdLocation);

	virtual CSubscriptionId     * CreateSubscriptionId(int _Index);
	virtual CMSCC               * CreateMSCC(int _Index);
    virtual CServiceParameterInfo* CreateSPI(int _Index);
	virtual CUserEquipmentInfo  * CreateUserEquipmentInfo();
	virtual CServiceInformation * CreateServiceInformation();
	virtual COtherPartyId       * CreateOtherPartyId();

	virtual bool                  DeleteMSCC(int _Index);

	//DiameterMessage
	virtual IDiameterHeader     * GetHeader();
	virtual IAVP                * GetAVPs();
	virtual void                  Dump(std::ostream &_OutStream);

};

}

#endif// __CCR_MESSAGE_IMPL_H__
