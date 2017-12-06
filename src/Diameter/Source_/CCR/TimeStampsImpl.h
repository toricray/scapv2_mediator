#ifndef __TIMESTAMPS_IMPL_H__
#define __TIMESTAMPS_IMPL_H__

#include "CCRCCAMessage.h"

#include "../RawMessage/DiameterAVP.h"
#include "../RawMessage/DiameterAVPGroup.h"


namespace CoreDiam
{

class DIAMCORE_API CTimeStampsImpl : public CTimeStamps
{
protected:
	unsigned int m_SIPResponseTimeStamp;
protected:
	IAVP* m_pAVP;
public:
	CTimeStampsImpl(IAVP* _pAVP);
	CTimeStampsImpl(const unsigned char* _Data, unsigned int _Size);
	virtual ~CTimeStampsImpl();
public:
	virtual unsigned int GetSIPResponseTimeStamp();
public:
	virtual bool SetSIPResponseTimeStamp(unsigned int _SIPResponseTimeStamp);
};

}

#endif // __TIMESTAMPS_IMPL_H__
