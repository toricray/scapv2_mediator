#ifndef __MESSAGE_CLASS_IMPL_H__
#define __MESSAGE_CLASS_IMPL_H__

#include "CCRCCAMessage.h"

#include "../RawMessage/DiameterAVP.h"
#include "../RawMessage/DiameterAVPGroup.h"

namespace CoreDiam
{
	
class DIAMCORE_API CMessageClassImpl : public CMessageClass
{
protected:
	ClassIdentifier m_ClassIdentifier;
protected:
	IAVP* m_pAVP;
public:
	CMessageClassImpl(IAVP* _pAVP);
	CMessageClassImpl(const unsigned char* _Data, unsigned int _Size);
	virtual ~CMessageClassImpl();
public:
	virtual ClassIdentifier GetClassIdentifier();
public:	
	virtual bool SetClassIdentifier(ClassIdentifier _ClassIdentifier);
};

}

#endif //__MESSAGE_CLASS_IMPL_H__