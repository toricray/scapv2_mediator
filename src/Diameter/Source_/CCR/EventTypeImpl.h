#ifndef __EVENT_TYPE_IMPL_H__
#define __EVENT_TYPE_IMPL_H__

#include <string>

#include "CCRCCAMessage.h"

#include "../RawMessage/DiameterAVP.h"
#include "../RawMessage/DiameterAVPGroup.h"

namespace CoreDiam
{

class DIAMCORE_API CEventTypeImpl : public CEventType
{
protected:
	IAVP* m_pAVP;
public:
	CEventTypeImpl(IAVP* _pAVP);
	CEventTypeImpl(const unsigned char* _Data, unsigned int _Size);
	virtual ~CEventTypeImpl();
protected:
	std::string m_SIPMethod;
	std::string m_Event;
	unsigned int m_Expires;
public:
	virtual const char* GetSIPMethod();
	virtual const char* GetEvent();
	virtual unsigned int GetExpires();
public:
	virtual bool SetSIPMethod(const char* _SIPMethod);
	virtual bool SetEvent(const char* _Event);
	virtual bool SetExpires(unsigned int _Expires);
};

}

#endif // __EVENT_TYPE_IMPL_H__
