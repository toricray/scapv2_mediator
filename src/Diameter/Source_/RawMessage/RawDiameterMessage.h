#ifndef __RAW_DIAMETER_MESSAGE_H__
#define __RAW_DIAMETER_MESSAGE_H__

#include "DiameterMessage.h"

#include "DiameterHeader.h"
#include "DiameterAVP.h"


namespace CoreDiam {

class CRawDiameterMessage : public IDiameterMessage
{
protected:
	IAVP* m_pAVP;
	IDiameterHeader* m_pDiameterHeader;
public:
	CRawDiameterMessage();
	CRawDiameterMessage(const IDiameterHeader &_pDiameterHeader);
	CRawDiameterMessage(IAVP* _pAVP, IDiameterHeader* _pDiameterHeader);
	virtual ~CRawDiameterMessage();
public:
	virtual IDiameterHeader * GetHeader();
	virtual IAVP            * GetAVPs();
public:
	virtual void Dump(std::ostream &_OutStream);
};

}

#endif //__RAW_DIAMETER_MESSAGE_H__
