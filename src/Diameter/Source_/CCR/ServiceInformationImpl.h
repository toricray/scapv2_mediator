#ifndef __SERVICE_INFORMATION_IMPL_H__
#define __SERVICE_INFORMATION_IMPL_H__

#include "CCRCCAMessage.h"

#include "IMSInformationImpl.h"
#include "SMSInformationImpl.h"
#include "PSInformationImpl.h"
#include "MMSInformationImpl.h"

#include "../RawMessage/DiameterAVP.h"
#include "../RawMessage/DiameterAVPGroup.h"

namespace CoreDiam
{

class DIAMCORE_API CServiceInformationImpl : public CServiceInformation
{
protected:
	CIMSInformationImpl* m_pIMSInformation;
	CSMSInformationImpl* m_pSMSInformation;
	CPSInformationImpl* m_pPSInformation;
	CMMSInformationImpl* m_pMMSInformation;
protected:
	IAVP* m_pAVP;

public:
	CServiceInformationImpl(IAVP* _pAVP);
	CServiceInformationImpl(const unsigned char* _Data, unsigned int _Size);
	virtual ~CServiceInformationImpl();
public:
	virtual CIMSInformation* GetIMSInformation();
	virtual CSMSInformation* GetSMSInformation();
	virtual CPSInformation* GetPSInformation();
	virtual CMMSInformation* GetMMSInformation();
public:
	virtual CIMSInformation* CreateIMSInformation();
	virtual CSMSInformation* CreateSMSInformation();
	virtual CPSInformation* CreatePSInformation();
	virtual CMMSInformation* CreateMMSInformation();
};
	
}

#endif //__SERVICE_INFORMATION_IMPL_H__