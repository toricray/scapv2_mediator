#ifndef __CCRCCAMESSAGE_H__
#define __CCRCCAMESSAGE_H__

#include <stdint.h>

#include "DiameterMessage.h"
#include "RefObject.h"

#include "Exports.h"

namespace CoreDiam
{

typedef enum
{
	THRESHOLD               = 0,
	QHT                     = 1,
	FINAL                   = 2,
	QUOTA_EXHAUSTED         = 3,
	VALIDITY_TIME           = 4,
	OTHER_QUOTA_TYPE        = 5,
	RATING_CONDITION_CHANGE = 6,
	FORCED_REAUTHORISATION  = 7,
	POOL_EXHAUSTED          = 8
} ReportingReason;

typedef enum
{
	INITIAL     = 1,
	UPDATE      = 2,
	TERMINATION = 3,
	EVENT       = 4
} RequestType;

    enum TrafficCase
    {
        TC_UNDEFINED = 0,
        TC_ORIGINATING = 20,
        TC_TERMINATING = 21

    };

typedef enum
{
	DIAMETER_LOGOUT                 = 1,
	DIAMETER_SERVICE_NOT_PROVIDED   = 2,
	DIAMETER_BAD_ANSWER             = 3,
	DIAMETER_ADMINISTRATIVE         = 4,
	DIAMETER_LINK_BROKEN            = 5,
	DIAMETER_AUTH_EXPIRED           = 6,
	DIAMETER_USER_MOVED             = 7,
	DIAMETER_SESSION_TIMEOUT        = 8
} TerminationCause;

typedef enum
{
	END_USER_E164       = 0,
	END_USER_IMSI       = 1,
	END_USER_SIP_URI    = 2,
	END_USER_NAI        = 3,
	END_USER_PRIVATE    = 4
} SubscriptionIdType;

typedef enum
{
	TERMINATE       = 0,
	REDIRECT        = 1,
	RESTRICT_ACCESS = 2
} FinalUnitAction;

typedef enum
{
	IMEISV          = 0,
	MAC             = 1,
	EUI64           = 2,
	MODIFIED_EUI64  = 3
} UserEquipmentInfoType;

typedef enum
{
	PDPTYPE_IPV4        = 0,
	PDPTYPE_PPP         = 1,
	PDPTYPE_IPV6        = 2
} PDPType;

typedef enum
{
	STATIC          = 0,
	DYNAMIC         = 1
} DynamicAddressFlag;

typedef enum
{
	APPEND          = 0,
	OVERWRITE       = 1
} PSAppendFreeFormatData;

typedef enum
{
	SMSROUTER               = 0,
	IPSMGW                  = 1,
	SMSROUTER_AND_IPSMGW    = 2,
	SMSSC                   = 3
} SMSNode;

typedef enum
{
	IPV4    = 0,
	IPV6    = 1,
	URL     = 2,
	SIP     = 3
} RedirectAddressType;

typedef enum
{
	S_CSCF  =  0,
	P_CSCF  =  1,
	I_CSCF  =  2,
	MRFC    =  3,
	MGCF    =  4,
	BGCF    =  5,
	AS      =  6,
	IBCF    =  7,
	S_GW    =  8,
	P_GW    =  9,
	HSGW    = 10,
	E_CSCF  = 11
} NodeFunctionality;

typedef enum
{
	ADDRTYPE_EMAIL_ADDRESS          = 0,
	ADDRTYPE_MSISDN                 = 1,
	ADDRTYPE_IPV4                   = 2,
	ADDRTYPE_IPV6                   = 3,
	ADDRTYPE_NUMERIC_SHORTCODE      = 4,
	ADDRTYPE_ALPHANUMERIC_SHORTCODE = 5,
	ADDRTYPE_OTHER                  = 6,
	ADDRTYPE_IMSI                   = 7
} AddressType;

typedef enum
{
	M_SEND_REQ          =  0,
	M_SEND_CONF         =  1,
	M_NOTIFICATION_IND  =  2,
	M_NOTIFYRESP_IND    =  3,
	M_RETRIEVE_CONF     =  4,
	M_ACKNOWLEDGE_IND   =  5,
	M_DELIVERY_IND      =  6,
	M_READ_REC_IND      =  7,
	M_READ_ORIG_IND     =  8,
	M_FORWARD_REQ       =  9,
	M_FORWARD_CONF      = 10,
	M_MBOX_STORE_CONF   = 11,
	M_MBOX_VIEW_CONF    = 12,
	M_MBOX_UPLOAD_CONF  = 13,
	M_MBOX_DELETE_CONF  = 14
} MessageType;

typedef enum
{
	PERSONAL            = 1,
	ADVERTISEMENT       = 2,
	INFORMATIONAL       = 3,
	AUTO                = 4
} ClassIdentifier;

typedef enum
{
	REBOOTING					= 0,
	BUSY						= 1,
	NO_NOT_WANT_TO_TALK_TO_YOU	= 2
} DisconnectCause;

typedef enum {
	CHANGE_NULL                                        =  0,
	CHANGE_IN_SGSN_IP_ADDRESS                          =  1,
	CHANGE_IN_QOS                                      =  2,  // NOTE 1: This should not be used in conjunction with enumerated values 10 to 23.
	CHANGE_IN_LOCATION                                 =  3,  // NOTE 2: This should not be used in conjunction with enumerated values 30 to 34.
	CHANGE_IN_RAT                                      =  4,
	CHANGE_IN_UE_TIMEZONE                              =  5,
	CHANGEINQOS_TRAFFIC_CLASS                          = 10,
	CHANGEINQOS_RELIABILITY_CLASS                      = 11,
	CHANGEINQOS_DELAY_CLASS                            = 12,
	CHANGEINQOS_PEAK_THROUGHPUT                        = 13,
	CHANGEINQOS_PRECEDENCE_CLASS                       = 14,
	CHANGEINQOS_MEAN_THROUGHPUT                        = 15,
	CHANGEINQOS_MAXIMUM_BIT_RATE_FOR_UPLINK            = 16,
	CHANGEINQOS_MAXIMUM_BIT_RATE_FOR_DOWNLINK          = 17,
	CHANGEINQOS_RESIDUAL_BER                           = 18,
	CHANGEINQOS_SDU_ERROR_RATIO                        = 19,
	CHANGEINQOS_TRANSFER_DELAY                         = 20,
	CHANGEINQOS_TRAFFIC_HANDLING_PRIORITY              = 21,
	CHANGEINQOS_GUARANTEED_BIT_RATE_FOR_UPLINK         = 22,
	CHANGEINQOS_GUARANTEED_BIT_RATE_FOR_DOWNLINK       = 23,
	CHANGEINLOCATION_MCC                               = 30,
	CHANGEINLOCATION_MNC                               = 31,
	CHANGEINLOCATION_RAC                               = 32,
	CHANGEINLOCATION_LAC                               = 33,
	CHANGEINLOCATION_CellId                            = 34,
	CHANGEINLOCATION_TAC                               = 35,
	CHANGEINLOCATION_ECGI                              = 36,
	CHANGE_IN_MEDIA_COMPOSITION                        = 40,
	CHANGE_IN_PARTICIPANTS_NMB                         = 50,
	CHANGE_IN_THRSHLD_OF_PARTICIPANTS_NMB              = 51,  // NOTE 3: The threshold and the granularity of threshold are operator configurable. This should not be used in conjunction with value 50.
	CHANGE_IN_USER_PARTICIPATING_TYPE                  = 52,
	CHANGE_IN_SERVICE_CONDITION                        = 60,
	CHANGE_IN_SERVING_NODE                             = 61,
	CHANGE_IN_USER_CSG_INFORMATION                     = 70,
	CHANGE_IN_HYBRID_SUBSCRIBED_USER_CSG_INFORMATION   = 71,
	CHANGE_IN_HYBRID_UNSUBSCRIBED_USER_CSG_INFORMATION = 72
} TriggerType;



class CServiceSpecificInfo
{
public:
	virtual ~CServiceSpecificInfo() {}

	virtual int                   GetServiceSpecificInfoType() = 0;
	virtual const unsigned char * GetServiceSpecificInfoData(unsigned int& _Len) = 0;

	virtual bool SetServiceSpecificInfoType(int _ServiceSpecificInfoType) = 0;
	virtual bool SetServiceSpecificInfoData(const unsigned char* _ServiceSpecificInfoData, unsigned int _Len) = 0;
};

class CPSFurnishChargingInformation
{
public:
	virtual ~CPSFurnishChargingInformation() {}

	virtual const unsigned char* GetGPRSChargingId(unsigned int& _Len) = 0;
	virtual const unsigned char* GetPSFreeFormatData(unsigned int& _Len) = 0;
	virtual PSAppendFreeFormatData GetPSAppendFreeFormatData() = 0;

	virtual bool SetGPRSChargingId(const unsigned char* _GPRSChargingId, unsigned int _Len) = 0;
	virtual bool SetPSFreeFormatData(const unsigned char* _PSFreeFormatData, unsigned int _Len) = 0;
	virtual bool SetPSAppendFreeFormatData(PSAppendFreeFormatData _PSAppendFreeFormatData) = 0;
};

class CEventType
{
public:
	virtual ~CEventType() {}

	virtual const char* GetSIPMethod() = 0;
	virtual const char* GetEvent() = 0;
	virtual unsigned int GetExpires() = 0;

	virtual bool SetSIPMethod(const char* _SIPMethod) = 0;
	virtual bool SetEvent(const char* _Event) = 0;
	virtual bool SetExpires(unsigned int _Expires) = 0;
};

class CApplicationServerInformation
{
public:
	virtual ~CApplicationServerInformation() {}

	virtual const char* GetApplicationServer() = 0;
	virtual const char* GetApplicationProvidedCalledPartyAddress() = 0;

	virtual bool SetApplicationServer(const char* _ApplicationServer) = 0;
	virtual bool SetApplicationProvidedCalledPartyAddress(const char* _ApplicationProvidedCalledPartyAddress) = 0;
};

class CMoney
{
public:
	virtual ~CMoney() {}

	virtual double GetUnitValue() = 0;
	virtual int GetCurrencyCode() = 0;

	virtual bool SetUnitValue(double _UnitValue) = 0;
	virtual bool SetCurrencyCode(int _CurrencyCode) = 0;
};

/*
class CSPI
{
    enum ValueType
    {
        SPI_INTEGER32,
        SPI_OCTET_STRING,
        SPI_UTF_STRING
    };

public:
	virtual ~CSPI() {}
	virtual int GetServiceParameterType() = 0;
    virtual int GetServiceParameterValue() = 0;
	virtual bool SetServiceParameterType(int type) = 0;
	virtual bool SetServiceParameterValue(int value) = 0;
};
*/

class CUnits
{
public:
	virtual ~CUnits() {}

	virtual int GetTime() = 0;
	virtual CMoney* GetMoney() = 0;
	virtual uint64_t GetTotalOctets() = 0;
	virtual uint64_t GetInputOctets() = 0;
	virtual uint64_t GetOutputOctets() = 0;
	virtual uint64_t GetServiceSpecificUnits() = 0;
	virtual unsigned int GetEventChargingTimeStamp() = 0;

	virtual bool SetTime(int _Time) = 0;
	virtual CMoney* CreateMoney() = 0;
	virtual bool SetTotalOctets(uint64_t _TotalOctets) = 0;
	virtual bool SetInputOctets(uint64_t _InputOctets) = 0;
	virtual bool SetOutputOctets(uint64_t _OutputOctets) = 0;
	virtual bool SetServiceSpecificUnits(uint64_t _ServiceSpecificUnits) = 0;
	virtual bool SetEventChargingTimeStamp(unsigned int _EventChargingTimeStamp) = 0;
};

class CRedirectServer
{
public:
	virtual ~CRedirectServer() {}

	virtual RedirectAddressType GetRedirectAddressType() = 0;
	virtual const unsigned char* GetRedirectServerAddress(unsigned int& _Len) = 0;

	virtual bool SetRedirectAddressType(RedirectAddressType _RedirectAddressType) = 0;
	virtual bool SetRedirectServerAddress(const unsigned char* _RedirectServerAddress, unsigned int _Len) = 0;
};

class CFinalUnitIndication
{
public:
	virtual ~CFinalUnitIndication() {}

	virtual FinalUnitAction GetFinalUnitAction() = 0;
	virtual CRedirectServer* GetRedirectServer() = 0;

	virtual bool SetFinalUnitAction(FinalUnitAction _FUI) = 0;
	virtual CRedirectServer* CreateRedirectServer() = 0;
};

class CMSCC
{
public:
	virtual ~CMSCC() {}

	virtual CUnits                        * GetGrantedServiceUnits(bool _bAutoCreate = false) = 0;
	virtual CUnits                        * GetRequestedServiceUnits() = 0;
	virtual CUnits                        * GetUsedServiceUnits() = 0;

	virtual int                             GetServiceIdentifier(int _Index) = 0;
	virtual int                             GetRatingGroup() = 0;
	virtual int                             GetValidityTime() = 0;
	virtual int                             GetResultCode() = 0;
	virtual CFinalUnitIndication          * GetFinalUnitIndication()           = 0;
	virtual TriggerType                     GetTriggerType(int index) = 0;
	virtual size_t                          GetTriggerTypeCount() = 0;
	virtual ReportingReason                 GetReportingReason()               = 0;

	virtual CServiceSpecificInfo          * GetServiceSpecificInfo(int _Index) = 0;
	virtual size_t                          GetServiceSpecificInfoCount()      = 0;

	virtual CPSFurnishChargingInformation * GetPSFurnishChargingInformation()  = 0;

	virtual CUnits                        * CreateGrantedServiceUnits() = 0;
	virtual CUnits                        * CreateRequestedServiceUnits() = 0;
	virtual CUnits                        * CreateUsedServiceUnits() = 0;
	virtual CFinalUnitIndication          * CreateFinalUnitIndication() = 0;
	virtual CPSFurnishChargingInformation * CreatePSFurnishChargingInformation() = 0;
	virtual CServiceSpecificInfo          * CreateServiceSpecificInfo(int _Index) = 0;

	virtual bool                            SetServiceIdentifier(int _ServiceIdentifier, int _Index) = 0;
	virtual bool                            SetRatingGroup(int _RatingGroup) = 0;
	virtual bool                            SetValidityTime(int _ValidityTimeSeconds) = 0;
	virtual bool                            SetResultCode(int _ResultCode) = 0;
	virtual bool                            SetReportingReason(ReportingReason _RR) = 0;

	virtual int                             AddTriggerType(TriggerType triggerType) = 0;
	virtual bool                            SetTriggerType(unsigned int index, TriggerType triggerType) = 0;
	virtual bool                            DelTriggerType(TriggerType triggerType) = 0;
	virtual bool                            DelTriggerType(unsigned int index) = 0;

	virtual int                             DelTrigger() = 0;
};

class CSubscriptionId
{
public:
	virtual ~CSubscriptionId() {}

	virtual SubscriptionIdType    GetSubscriptionIdType() = 0;
	virtual const unsigned char * GetSubscriptionIdData(unsigned int& _Len) = 0;
public:
	virtual bool SetSubscriptionIdType(SubscriptionIdType _SubscriptionIdType) = 0;
	virtual bool SetSubscriptionIdData(const unsigned char* _SubscriptionIdValue, unsigned int _Len) = 0;
};

class COtherPartyId
{
public:
	virtual ~COtherPartyId() {}
	virtual int    GetOtherPartyIdType() = 0;
	virtual int    GetOtherPartyIdNature() = 0;
	virtual const char * GetOtherPartyIdData(unsigned int& _Len) = 0;
public:
	virtual bool SetOtherPartyIdType(int _SubscriptionIdType) = 0;
	virtual bool SetOtherPartyIdData(const unsigned char* _OtherPartyIdValue, unsigned int _Len) = 0;
	virtual bool SetOtherPartyIdNature(int _SubscriptionIdNature) = 0;
};

class CUserEquipmentInfo
{
public:
	virtual ~CUserEquipmentInfo() {}

	virtual UserEquipmentInfoType GetUserEquipmentInfoType() = 0;
	virtual const unsigned char*  GetUserEquipmentInfoValue(unsigned int& _Len) = 0;

	virtual bool SetUserEquipmentInfoType(UserEquipmentInfoType _UserEquipmentInfoType) = 0;
	virtual bool SetUserEquipmentInfoValue(const unsigned char* _UserEquipmentInfoValue, unsigned int _Len) = 0;
};

class CServiceParameterInfo
{
public:
	virtual ~CServiceParameterInfo() {}

	virtual int                   GetServiceParameterInfoType() = 0;
	virtual const char * GetServiceParameterInfoValue(unsigned int& _Len) = 0;

	//virtual bool SetServiceParameterInfoType(int _ServiceParameterInfoType) = 0;
	virtual bool SetServiceParameterInfoValue(int _ServiceParameterInfoType, const  char* _ServiceParameterInfoValue) = 0;
};

class CTimeStamps
{
public:
	virtual ~CTimeStamps() {}

	virtual unsigned int  GetSIPResponseTimeStamp() = 0;
	virtual bool SetSIPResponseTimeStamp(unsigned int _SIPResponseTimeStamp) = 0;
};

class COriginatorAddress
{
public:
	virtual ~COriginatorAddress() {}

	virtual AddressType GetAddressType() = 0;
	virtual const char* GetAddressData() = 0;

	virtual bool SetAddressType(AddressType _AddressType) = 0;
	virtual bool SetAddressData(const char* _AddressData) = 0;
};

class CRecipientAddress
{
public:
	virtual ~CRecipientAddress() {}

	virtual AddressType GetAddressType() = 0;
	virtual const char* GetAddressData() = 0;

	virtual bool SetAddressType(AddressType _AddressType) = 0;
	virtual bool SetAddressData(const char* _AddressData) = 0;
};

class CRecipientInfo
{
public:
	virtual ~CRecipientInfo() {}

	virtual CRecipientAddress* GetRecipientAddress(int _Index) = 0;
	virtual const unsigned char * GetRecipientSCCPAddress(unsigned int& _Len) = 0;

	virtual CRecipientAddress* CreateRecipientAddress(int _Index) = 0;
	virtual bool SetRecipientSCCPAddress(const unsigned char* _RecipientSCCPAddress, unsigned int _Len) = 0;
};

class COriginatorReceivedAddress
{
public:
	virtual ~COriginatorReceivedAddress() {}

	virtual AddressType GetAddressType() = 0;
	virtual const char* GetAddressData() = 0;

	virtual bool SetAddressType(AddressType _AddressType) = 0;
	virtual bool SetAddressData(const char* _AddressData) = 0;
};

class CSMSInformation
{
public:
	virtual ~CSMSInformation() {}

	virtual SMSNode GetSMSNode() = 0;
	virtual const unsigned char* GetSMSCAddress(unsigned short& _AddressFamily, unsigned int& _Len) = 0;
	virtual const unsigned char* GetOriginatorSCCPAddress(unsigned int& _Len) = 0;
	virtual CRecipientInfo* GetRecipientInfo(int _Index = 0) = 0;
	virtual COriginatorReceivedAddress* GetOriginatorReceivedAddress() = 0;

	virtual bool SetSMSNode(SMSNode _SMSNode) = 0;
	virtual bool SetSMSCAddress(unsigned short _AddressFamily, const unsigned char* _SMSCAddress, unsigned int _Len) = 0;
	virtual bool SetOriginatorSCCPAddress(const unsigned char* _OriginatorSCCPAddress, unsigned int _Len) = 0;
	virtual CRecipientInfo* CreateRecipientInfo(int _Index) = 0;
	virtual COriginatorReceivedAddress* CreateOriginatorReceivedAddress() = 0;
};

class CPSInformation
{
public:
	virtual ~CPSInformation() {}

	virtual unsigned int         Get3GPPChargingId() = 0;
	// PDN-Connection-Charging-Id
	virtual const unsigned char* GetNodeId(unsigned int& _Len) = 0;
	virtual PDPType              Get3GPPPDPType() = 0;
	virtual const unsigned char* GetPDPAddress(unsigned short& _AddressFamily, unsigned int& _Len, std::string& _StrAddress) = 0;
	// PDN-Address-Prefix-Length
	virtual DynamicAddressFlag   GetDynamicAddressFlag() = 0;
	// Dynamic-Address-Flag-Extension
	// QoS-Information
	virtual const unsigned char* GetSGSNAddress(unsigned short& _AddressFamily, unsigned int& _Len, std::string& _StrAddress) = 0;
	virtual const unsigned char* GetGGSNAddress(unsigned short& _AddressFamily, unsigned int& _Len, std::string& _StrAddress) = 0;
	// SGW-Address
	// CG-Address
	// Serving-Node-Type
	// SGW-Change
	virtual const char* Get3GPP_IMSI_MCCMNC(unsigned int &_Len) = 0;  // -> 3GPP-IMSI-MCC-MNC
	// IMSI-Unauthenticated-Flag
	virtual const char* Get3GPP_GGSN_MCCMNC(unsigned int &_Len) = 0;  // -> 3GPP-GGSN-MCC-MNC
	// 3GPP-NSAPI
	virtual const char*          GetCalledStationId() = 0;
	// 3GPP-Session-Stop-Indicator
	// 3GPP-Selection-Mode
	// 3GPP-Charging-Characteristics
	// Charging-Characteristics-Selection-Mode
	virtual const char* Get3GPP_SGSN_MCCMNC(unsigned int &_Len) = 0;  // -> 3GPP-SGSN-MCC-MNC
	// 3GPP-MS-TimeZone
	// Charging-Rule-Base-Name
	virtual const unsigned char* Get3GPPUserLocationInfo(unsigned char& _GeographicLocationType, unsigned int& _Len) = 0;
	// User-Location-Info-Time
	// User-CSG-Information
	// 3GPP2-BSID
	// TWAN-User-Location-Info
	virtual unsigned char        Get3GPPRATType() = 0;
	// PS-Furnish-Charging-Information
	// PDP-Context-Type
	// Offline-Charging
	// Traffic-Data-Volumes
	// Service-Data-Container
	// User-Equipment-Info
	// Terminal-Information
	virtual unsigned int         GetStartTime() = 0;
	virtual unsigned int         GetStopTime() = 0;
	// Change-Condition
	// Diagnostics
	// Low-Priority-Indicator
	// MME-Number-for-MT-SMS
	// MME-Name
	// MME-Realm

	virtual bool Set3GPPChargingId(unsigned int _3GPPChargingId) = 0;
	virtual bool SetNodeId(const unsigned char* _NodeId, unsigned int _Len) = 0;
	virtual bool Set3GPPPDPType(PDPType _PDPType) = 0;
	virtual bool SetPDPAddress(unsigned short _AddressFamily, const unsigned char* _PDPAddress, unsigned int _Len) = 0;
	virtual bool SetDynamicAddressFlag(DynamicAddressFlag _DynamicAddressFlag) = 0;
	virtual bool SetSGSNAddress(unsigned short _AddressFamily, const unsigned char* _SGSNAddress, unsigned int _Len) = 0;
	virtual bool SetGGSNAddress(unsigned short _AddressFamily, const unsigned char* _GGSNAddress, unsigned int _Len) = 0;
	virtual bool Set3GPP_IMSI_MCCMNC(const char *_mccMNC, unsigned int &_Len) = 0;  // -> 3GPP-IMSI-MCC-MNC
	virtual bool Set3GPP_GGSN_MCCMNC(const char *_mccMNC, unsigned int &_Len) = 0;  // -> 3GPP-GGSN-MCC-MNC
	virtual bool SetCalledStationId(const char* _CalledStationId) = 0;
	virtual bool Set3GPP_SGSN_MCCMNC(const char *_mccMNC, unsigned int &_Len) = 0;  // -> 3GPP-SGSN-MCC-MNC
	virtual bool Set3GPPUserLocationInfo(unsigned char _GeographicLocationType, const unsigned char* _3GPPUserLocationInfo, unsigned int _Len) = 0;
	virtual bool Set3GPPRATType(unsigned char _ratType) = 0;
	virtual bool SetStartTime(unsigned int _StartTime) = 0;
	virtual bool SetStopTime(unsigned int _StopTime) = 0;
};

class CIMSInformation
{
public:
	virtual ~CIMSInformation() {}

	virtual CEventType        * GetEventType() = 0;
	virtual NodeFunctionality   GetNodeFunctionality() = 0;
	virtual const char* GetCallingPartyAddress() = 0;
	virtual const char* GetCalledPartyAddress() = 0;
	virtual const char* GetRequestedPartyAddress() = 0;
	virtual CTimeStamps* GetTimeStamps() = 0;
	virtual CApplicationServerInformation* GetApplicationServerInformation() = 0;
	virtual const unsigned char* GetBearerService(unsigned int& _Len) = 0;
	virtual const char* GetServiceId() = 0;

	virtual CServiceSpecificInfo * GetServiceSpecificInfo(int _Index) = 0;
	virtual size_t                 GetServiceSpecificInfoCount() = 0;

	virtual CEventType* CreateEventType() = 0;

	virtual bool SetNodeFunctionality(NodeFunctionality _NodeFunctionality) = 0;
	virtual bool SetCallingPartyAddress(const char* _CallingPartyAddress) = 0;
	virtual bool SetCalledPartyAddress(const char* _CalledPartyAddress) = 0;
	virtual bool SetRequestedPartyAddress(const char* _RequestedPartyAddress) = 0;

	virtual CTimeStamps* CreateTimeStamps() = 0;

	virtual CApplicationServerInformation* CreateApplicationServerInformation() = 0;

	virtual bool SetBearerService(const unsigned char* _BearerService, unsigned int _Len) = 0;
	virtual bool SetServiceId(const char* _ServiceId) = 0;

	virtual CServiceSpecificInfo* CreateServiceSpecificInfo(int _Index) = 0;
};

class CMessageClass
{
public:
	virtual ~CMessageClass() {}

	virtual ClassIdentifier GetClassIdentifier() = 0;

	virtual bool SetClassIdentifier(ClassIdentifier _ClassIdentifier) = 0;
};

class CMMSInformation
{
public:
	virtual ~CMMSInformation() {}

	virtual COriginatorAddress* GetOriginatorAddress() = 0;
	virtual CRecipientAddress* GetRecipientAddress(int _Index) = 0;
	virtual const char* GetMessageId() = 0;
	virtual MessageType GetMessageType() = 0;
	virtual int GetMessageSize() = 0;
	virtual CMessageClass* GetMessageClass() = 0;

	virtual COriginatorAddress* CreateOriginatorAddress() = 0;
	virtual CRecipientAddress* CreateRecipientAddress(int _Index) = 0;
	virtual bool SetMessageId(const char* _MessageId) = 0;
	virtual bool SetMessageType(MessageType _MessageType) = 0;
	virtual bool SetMessageSize(int _MessageSize) = 0;
	virtual CMessageClass* CreateMessageClass() = 0;
};

class CServiceInformation
{
public:
	virtual ~CServiceInformation() {}

	virtual CIMSInformation * GetIMSInformation() = 0;
	virtual CSMSInformation * GetSMSInformation() = 0;
	virtual CPSInformation  * GetPSInformation() = 0;
	virtual CMMSInformation * GetMMSInformation() = 0;

	virtual CIMSInformation * CreateIMSInformation() = 0;
	virtual CSMSInformation * CreateSMSInformation() = 0;
	virtual CPSInformation  * CreatePSInformation() = 0;
	virtual CMMSInformation * CreateMMSInformation() = 0;
};

class CCCRMessage : public IDiameterMessage//, public IRefObject
{
public:
	virtual const char          * GetSessionId() = 0;
	virtual const char          * GetOriginHost() = 0;
	virtual const char          * GetOriginRealm() = 0;
	virtual const char          * GetDestinationRealm() = 0;
	virtual int                   GetAuthApplicationId() = 0;
	virtual const char          * GetServiceContextId() = 0;
	virtual RequestType           GetRequestType() = 0;
    virtual TrafficCase           GetTrafficCase() = 0;
	virtual int                   GetRequestNumber() = 0;
	virtual unsigned int          GetEventTimestamp() = 0;
	virtual CSubscriptionId     * GetSubscriptionId(int _Index) = 0;
	virtual int                   GetTerminationCause() = 0;            // [295]
	virtual int                   GetTimezone() = 0;            // [295]
	virtual int                   GetRequestedAction() = 0;             //
	virtual CMSCC               * GetMSCC(int _Index = 0) = 0;
	virtual CUserEquipmentInfo  * GetUserEquipmentInfo() = 0;
	virtual CServiceInformation * GetServiceInformation() = 0;
	virtual int                   GetServiceIdentifier() = 0;
	virtual const char *          GetDestinationHost() = 0;
	virtual CUnits              * GetRequestedServiceUnits() = 0;
    virtual CUnits              * GetUsedServiceUnits() = 0;
	virtual CServiceParameterInfo * GetSPI(int _Index) = 0;

    virtual COtherPartyId          * GetOtherPartyId() = 0;
    virtual const char          * GetSubscriptionIdLocation() = 0;


	virtual bool                  SetSessionId(const char *_SessionId) = 0;
	virtual bool                  SetOriginHost(const char *_OriginHost) = 0;
	virtual bool                  SetOriginRealm(const char *_OriginRealm) = 0;
	virtual bool                  SetDestinationHost(const char *_DestinationHost) = 0;
	virtual bool                  SetDestinationRealm(const char *_DestinationRealm) = 0;
	virtual bool                  SetAuthApplicationId(int _AuthApplicationId) = 0;
	virtual bool                  SetServiceContextId(const char *_ServiceContextId) = 0;
	virtual bool                  SetServiceIdentifier(int _ServiceIdentifier) = 0;
	virtual bool                  SetRequestNumber(int _RequestNumber) = 0;
	virtual bool                  SetRequestType(RequestType _RequestType) = 0;
    virtual bool                  SetTrafficCase(TrafficCase _TrafficCase) = 0;
	virtual bool                  SetTimezone(int _timezone) = 0;
	virtual bool                  SetEventTimestamp(unsigned int _EventTimestamp) = 0;
	virtual bool                  SetTerminationCause(int _terminationCause) = 0;
	virtual bool                  SetRequestedAction(int _requestedAction) = 0;

	virtual bool                  SetSubscriptionIdLocation(const char *_SubscriptionIdLocation) = 0;

	virtual CSubscriptionId     * CreateSubscriptionId(int _Index) = 0;
	virtual COtherPartyId       * CreateOtherPartyId() = 0;
	virtual CMSCC               * CreateMSCC(int _Index) = 0;
	virtual CUserEquipmentInfo  * CreateUserEquipmentInfo() = 0;
	virtual CServiceInformation * CreateServiceInformation() = 0;
	virtual CUnits              * CreateRequestedServiceUnits() = 0;
    virtual CUnits              * CreateUsedServiceUnits() = 0;
	virtual CServiceParameterInfo                * CreateSPI(int _Index) = 0;

public:
	virtual bool                  DeleteMSCC(int _Index) = 0;

};

class CCCAMessage : public IDiameterMessage//, public IRefObject
{
public:
	virtual const char  * GetSessionId()         const = 0;
	virtual int           GetResultCode()        const = 0;
    virtual int           GetResultCodeExt()        const = 0;
    virtual const char  * GetErrorMessage()      const = 0;

	virtual const char  * GetOriginHost()        const = 0;
	virtual const char  * GetOriginRealm()       const = 0;

	virtual int           GetAuthApplicationId() const = 0;
	virtual const char  * GetServiceContextId()  const = 0;

	virtual int           GetRequestNumber()     const = 0;
	virtual RequestType   GetRequestType()       const = 0;

	virtual CMSCC       * GetMSCC(int _Index)    const = 0;

	virtual bool          SetSessionId(const char* _SessionId) = 0;
	virtual bool          SetResultCode(int _ResultCode) = 0;
    virtual bool          SetResultCodeExt(int _ResultCodeExt) = 0;

	virtual bool          SetOriginHost(const char* _OriginHost) = 0;
	virtual bool          SetOriginRealm(const char* _OriginRealm) = 0;

	virtual bool          SetAuthApplicationId(int _AuthApplicationId) = 0;
	virtual bool          SetServiceContextId(const char* _ServiceContextId) = 0;

	virtual bool          SetRequestNumber(int _RequestNumber) = 0;
	virtual bool          SetRequestType(RequestType _RequestType) = 0;

	virtual CMSCC       * CreateMSCC(int _Index) = 0;

	virtual bool          DeleteMSCC(int _Index) = 0;

};

class CDPRMessage : public IDiameterMessage
{
public:
	virtual const char* GetSessionId() = 0;
	virtual const char* GetOriginHost() = 0;
	virtual const char* GetOriginRealm() = 0;
	virtual const char* GetDestinationRealm() = 0;
	virtual int GetAuthApplicationId() = 0;
	virtual const char* GetServiceContextId() = 0;
	virtual int GetRequestNumber() = 0;
	virtual RequestType GetRequestType() = 0;
	virtual unsigned int GetEventTimestamp() = 0;
	virtual CSubscriptionId* GetSubscriptionId(int _Index) = 0;
	virtual CUserEquipmentInfo* GetUserEquipmentInfo() = 0;
	virtual CMSCC* GetMSCC(int _Index = 0) = 0;
	virtual CServiceInformation* GetServiceInformation() = 0;
	virtual bool SetOriginHost(const char* _OriginHost) = 0;
	virtual bool SetOriginRealm(const char* _OriginRealm) = 0;
	virtual bool SetDisconnectCause(DisconnectCause _DisconnectCause) = 0;
};

class CDPAMessage : public IDiameterMessage
{
public:
	virtual int           GetResultCode()        const = 0;

	virtual const char  * GetOriginHost()        const = 0;
	virtual const char  * GetOriginRealm()       const = 0;
	virtual DisconnectCause  GetDisconnectCause() const = 0;
	virtual bool          SetResultCode(int _ResultCode) = 0;

	virtual bool          SetOriginHost(const char* _OriginHost) = 0;
	virtual bool          SetOriginRealm(const char* _OriginRealm) = 0;

};

}

#endif //__CCACCAMESSAGE_H__
