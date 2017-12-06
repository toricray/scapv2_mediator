#ifndef __CCA_MESSAGE_IMPL_H__
#define __CCA_MESSAGE_IMPL_H__

#include "CCRCCAMessage.h"
#include "../RawMessage/RawDiameterMessage.h"

#include "../RawMessage/DiameterAVP.h"
#include "../RawMessage/DiameterAVPGroup.h"

#include "MSCCImpl.h"

#include <vector>
#include <string>


namespace CoreDiam {

class DIAMCORE_API CCCAMessageImpl : public CCCAMessage
{
typedef std::vector<CMSCC*> VEC_MSCC;

protected:
	std::string m_SessionId;
	std::string m_ErrorMessage;
	int         m_ResultCode;
	int         m_ResultCodeExt;
	std::string m_OriginHost;
	std::string m_OriginRealm;
	int         m_AuthApplicationId;
	std::string m_ServiceContextId;
	int         m_RequestNumber;
	RequestType m_RequestType;
	VEC_MSCC    m_vecMSCC;
protected:
	IDiameterMessage* m_pRawMessage;
	IAVP* m_pAVP;
	IAVP* m_pMSCCAVP;

public:
	CCCAMessageImpl();
	CCCAMessageImpl(CCCRMessage *_pCCRMessage, const char *originHost = 0);
	CCCAMessageImpl(const unsigned char* _Data, unsigned int _Size, IDiameterMessage* _pRawMessage);
	virtual ~CCCAMessageImpl();

public:
	virtual const char  * GetSessionId()         const;
	virtual const char  * GetErrorMessage()         const;

	virtual int           GetResultCode()        const;
	virtual int           GetResultCodeExt()        const;

	virtual const char  * GetOriginHost()        const;
	virtual const char  * GetOriginRealm()       const;

	virtual int           GetAuthApplicationId() const;

	virtual const char  * GetServiceContextId()  const;

	virtual int           GetRequestNumber()     const;
	virtual RequestType   GetRequestType()       const;

	virtual CMSCC       * GetMSCC(int _Index)    const;

	virtual bool SetSessionId(const char* _SessionId);
	virtual bool SetErrorMessage(const char* _SessionId);

	virtual bool SetResultCode(int _ResultCode);
	virtual bool SetResultCodeExt(int _ResultCodeExt);

	virtual bool SetOriginHost(const char* _OriginHost);
	virtual bool SetOriginRealm(const char* _OriginRealm);

	virtual bool SetAuthApplicationId(int _AuthApplicationId);

	virtual bool SetServiceContextId(const char* _ServiceContextId);

	virtual bool SetRequestNumber(int _RequestNumber);
	virtual bool SetRequestType(RequestType _RequestType);

	virtual CMSCC* CreateMSCC(int _Index);

	virtual bool DeleteMSCC(int _Index);

//DiameterMessage
public:
	virtual IDiameterHeader* GetHeader();
	virtual IAVP* GetAVPs();
public:
	virtual void Dump(std::ostream &_OutStream);

};

}

#endif //__CCA_MESSAGE_IMPL_H__
