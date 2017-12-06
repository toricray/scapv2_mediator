#ifndef __DIAMETER_HEADER_H__
#define __DIAMETER_HEADER_H__

#include "DiameterMessage.h"


namespace CoreDiam {

class CDiameterHeader : public IDiameterHeader
{
	unsigned char m_CommandFlags;
	int m_CommandCode;
	int m_ApplicationID;
	int m_HbHID;
	int m_EtEID;
public:
	CDiameterHeader();
	CDiameterHeader(const CDiameterHeader &_RefHeader);
	CDiameterHeader(const IDiameterHeader &_RefHeader);
	virtual ~CDiameterHeader();
public:
	virtual unsigned char GetCommandFlags() const;
	virtual int           GetCommandCode() const;
	virtual int           GetApplicationID() const;
	virtual int           GetHopByHopIdentifier() const;
	virtual int           GetEndToEndIdentifier() const;
public:
	virtual bool SetCommandFlags(unsigned char _Flags);
	virtual bool SetCommandCode(unsigned int _CommandCode);
	virtual bool SetApplicationID(int _ApplicationID);
	virtual bool SetHopByHopIdentifier(int _HopByHopIdentifier);
	virtual bool SetEndToEndIdentifier(int _EndToEndIdentifier);
public:
	virtual void Dump(std::ostream &_OutStream);
};

}

#endif //__DIAMETER_DECODER_H__
