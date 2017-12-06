#include <stdio.h>

#include "DiameterHeader.h"

namespace CoreDiam {

CDiameterHeader::CDiameterHeader() :
	m_CommandFlags(0x20),
	m_CommandCode(-1),
	m_ApplicationID(-1),
	m_HbHID(-1),
	m_EtEID(-1)
{
}

CDiameterHeader::CDiameterHeader(const CDiameterHeader &_RefHeader):
	m_CommandFlags(_RefHeader.m_CommandFlags),
	m_CommandCode(_RefHeader.m_CommandCode),
	m_ApplicationID(_RefHeader.m_ApplicationID),
	m_HbHID(_RefHeader.m_HbHID),
	m_EtEID(_RefHeader.m_EtEID)
{
}

CDiameterHeader::CDiameterHeader(const IDiameterHeader &_RefHeader):
	m_CommandFlags(_RefHeader.GetCommandFlags()),
	m_CommandCode(_RefHeader.GetCommandCode()),
	m_ApplicationID(_RefHeader.GetApplicationID()),
	m_HbHID(_RefHeader.GetHopByHopIdentifier()),
	m_EtEID(_RefHeader.GetEndToEndIdentifier())
{
}

CDiameterHeader::~CDiameterHeader()
{
}

unsigned char CDiameterHeader::GetCommandFlags() const
{
	return m_CommandFlags;
}

int CDiameterHeader::GetCommandCode() const
{
	return m_CommandCode;
}

int CDiameterHeader::GetApplicationID() const
{
	return m_ApplicationID;
}

int CDiameterHeader::GetHopByHopIdentifier() const
{
	return m_HbHID;
}

int CDiameterHeader::GetEndToEndIdentifier() const
{
	return m_EtEID;
}

bool CDiameterHeader::SetCommandFlags(unsigned char _Flags)
{
	m_CommandFlags = _Flags;
	return true;
}

bool CDiameterHeader::SetCommandCode(unsigned int _CommandCode)
{
	m_CommandCode = _CommandCode;
	return true;
}

bool CDiameterHeader::SetApplicationID(int _ApplicationID)
{
	m_ApplicationID = _ApplicationID;
	return true;
}

bool CDiameterHeader::SetHopByHopIdentifier(int _HopByHopIdentifier)
{
	m_HbHID = _HopByHopIdentifier;
	return true;
}

bool CDiameterHeader::SetEndToEndIdentifier(int _EndToEndIdentifier)
{
	m_EtEID = _EndToEndIdentifier;
	return true;
}

void CDiameterHeader::Dump(std::ostream &_OutStream)
{
	_OutStream << "---- Diameter Header ----" << std::endl;
	char Buf[5];
	sprintf(Buf, "0x%.2x", m_CommandFlags);
	_OutStream << "CommandFlags: " << Buf << std::endl;
	_OutStream << "CommandCode: " << m_CommandCode << std::endl;
	_OutStream << "ApplicationID: " << m_ApplicationID << std::endl;
	_OutStream << "HopByHopID: " << m_HbHID << std::endl;
	_OutStream << "EndToEndID: " << m_EtEID << std::endl;
}

}
