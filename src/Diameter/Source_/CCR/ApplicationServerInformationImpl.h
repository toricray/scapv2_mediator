#ifndef __APPLICATION_SERVER_INFORMATION_H__
#define __APPLICATION_SERVER_INFORMATION_H__

#include <string>

#include "CCRCCAMessage.h"

#include "../RawMessage/DiameterAVP.h"
#include "../RawMessage/DiameterAVPGroup.h"

namespace CoreDiam
{

class DIAMCORE_API CApplicationServerInformationImpl : public CApplicationServerInformation
{
protected:
	IAVP* m_pAVP;
protected:
	std::string m_ApplicationServer;
	std::string m_ApplicationProvidedCalledPartyAddress;
public:
	CApplicationServerInformationImpl(IAVP* _pAVP);
	CApplicationServerInformationImpl(const unsigned char* _Data, unsigned int _Size);
	virtual ~CApplicationServerInformationImpl();
public:
	virtual const char* GetApplicationServer();
	virtual const char* GetApplicationProvidedCalledPartyAddress();
public:
	virtual bool SetApplicationServer(const char* _ApplicationServer);
	virtual bool SetApplicationProvidedCalledPartyAddress(const char* _ApplicationProvidedCalledPartyAddress);
};

}

#endif // __APPLICATION_SERVER_INFORMATION_H__
