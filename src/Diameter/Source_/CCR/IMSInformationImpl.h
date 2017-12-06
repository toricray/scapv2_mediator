#ifndef __IMS_INFORMATION_IMPL_H__
#define __IMS_INFORMATION_IMPL_H__

#include "CCRCCAMessage.h"

#include "EventTypeImpl.h"
#include "TimeStampsImpl.h"
#include "ApplicationServerInformationImpl.h"
#include "ServiceSpecificInfoImpl.h"

#include "../RawMessage/DiameterAVP.h"
#include "../RawMessage/DiameterAVPGroup.h"

#include <vector>

namespace CoreDiam
{

typedef std::vector<CServiceSpecificInfo*> VEC_SERVICESPECIFICINFO;
typedef std::vector<unsigned char> VEC_DATA;

class DIAMCORE_API CIMSInformationImpl : public CIMSInformation
{
protected:
	CEventType* m_pEventType;
	NodeFunctionality m_NodeFunctionality;
	std::string m_CallingPartyAddress;
	std::string m_CalledPartyAddress;
	std::string m_RequestedPartyAddress;
	CTimeStamps* m_pTimeStamps;
	CApplicationServerInformation* m_pApplicationServerInformation;
	VEC_DATA m_vecBearerService;
	std::string m_ServiceId;
	VEC_SERVICESPECIFICINFO m_vecServiceSpecificInfo;

	IAVP* m_pAVP;
	IAVP* m_pSSInfoAVP;
public:
	CIMSInformationImpl(IAVP* _pAVP);
	CIMSInformationImpl(const unsigned char* _Data, unsigned int _Size);

	virtual ~CIMSInformationImpl();

	virtual CEventType* GetEventType();

	virtual NodeFunctionality GetNodeFunctionality();
	virtual const char* GetCallingPartyAddress();
	virtual const char* GetCalledPartyAddress();
	virtual const char* GetRequestedPartyAddress();
	virtual CTimeStamps* GetTimeStamps();
	virtual CApplicationServerInformation* GetApplicationServerInformation();
	virtual const unsigned char* GetBearerService(unsigned int& _Len);
	virtual const char* GetServiceId();

	virtual CServiceSpecificInfo * GetServiceSpecificInfo(int _Index);
	virtual size_t                 GetServiceSpecificInfoCount();

	virtual CEventType* CreateEventType();

	virtual bool SetNodeFunctionality(NodeFunctionality _NodeFunctionality);
	virtual bool SetCallingPartyAddress(const char* _CallingPartyAddress);
	virtual bool SetCalledPartyAddress(const char* _CalledPartyAddress);
	virtual bool SetRequestedPartyAddress(const char* _RequestedPartyAddress);
	virtual CTimeStamps* CreateTimeStamps();
	virtual CApplicationServerInformation* CreateApplicationServerInformation();
	virtual bool SetBearerService(const unsigned char* _BearerService, unsigned int _Len);
	virtual bool SetServiceId(const char* _ServiceId);
	virtual CServiceSpecificInfo* CreateServiceSpecificInfo(int _Index);
};

}

#endif //__IMS_INFORMATION_IMPL_H__
