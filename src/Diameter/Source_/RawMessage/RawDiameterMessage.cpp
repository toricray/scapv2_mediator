#include "RawDiameterMessage.h"

namespace CoreDiam {

CRawDiameterMessage::CRawDiameterMessage()
{
	m_pDiameterHeader = new CDiameterHeader();
	m_pAVP = new CDiameterAVP(IAVP::BASE, NULL, 0, 0, 0x00, 0);
}

CRawDiameterMessage::CRawDiameterMessage(const IDiameterHeader &_pDiameterHeader)
{
	m_pDiameterHeader = new CDiameterHeader(_pDiameterHeader);
	m_pAVP            = new CDiameterAVP(IAVP::BASE, NULL, 0, 0, 0x00, 0);
}

CRawDiameterMessage::CRawDiameterMessage(IAVP *_pAVP, IDiameterHeader *_pDiameterHeader) :
	m_pAVP(_pAVP),
	m_pDiameterHeader(_pDiameterHeader)
{
}

CRawDiameterMessage::~CRawDiameterMessage()
{
	if (m_pAVP)
		delete m_pAVP;
	if (m_pDiameterHeader)
		delete m_pDiameterHeader;
}

IDiameterHeader* CRawDiameterMessage::GetHeader()
{
	return m_pDiameterHeader;
}

IAVP* CRawDiameterMessage::GetAVPs()
{
	return m_pAVP;
}

void CRawDiameterMessage::Dump(std::ostream &_OutStream)
{
	m_pDiameterHeader->Dump(_OutStream);
	m_pAVP->Dump(_OutStream);
}

}
