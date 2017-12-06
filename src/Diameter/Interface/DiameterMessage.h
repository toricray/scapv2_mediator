#ifndef __DIAMETERMESSAGE_H__
#define __DIAMETERMESSAGE_H__

#include <netinet/in.h>

#if defined(_MSC_VER) && (_MSC_VER < 1310)
	#include <ostream.h>
#else
	#include <ostream>
#endif

namespace CoreDiam
{

namespace avpcode {

enum DiameterAVPCode {
    SESSION_ID                       = 263,
    RESULT_CODE                      = 268,
    CC_REQUEST_TYPE                  = 416,
    FINAL_UNIT_INDICATION            = 430,
    MULTIPLE_SERVICES_CREDIT_CONTROL = 456
};

}  // namespace avpcode


/** @interface IAVP
*   @brief Èםעונפויס AVP
*/
class IAVP
{
public:
	typedef enum
	{
		BASE,
		BASIC,
		GROUPED,
		BASICSET,   // warning: not used
		GROUPEDSET
	} AVPType;

public:
	virtual ~IAVP() {}

	virtual const char*   GetName()     = 0;
	virtual AVPType       GetType()     = 0;
	virtual int           GetVendorID() = 0;
	virtual int           GetCode()     = 0;
	virtual unsigned char GetFlags()    = 0;
	virtual int           GetCount()    = 0;

	virtual IAVP* GetAVPByName(const char* _Name, unsigned int _Index = 0) = 0;
	virtual IAVP* GetAVPByCode(int _Code, unsigned int _Index = 0)         = 0;

	virtual const unsigned char * GetData(unsigned int &_Size)                           = 0;
	virtual bool                  SetData(const unsigned char *_Data, unsigned int _Len) = 0;

	virtual IAVP* CreateAVPByName(const char* _Name, unsigned int _Index = 0) = 0;
	virtual IAVP* CreateAVPByCode(int _Code, unsigned int _Index = 0)         = 0;

	virtual bool  SetAVP(IAVP* _pAVP, unsigned int _Index = 0) = 0;

	virtual void  ResetList()  = 0;
	virtual IAVP* GetNextAVP() = 0;

	virtual void Dump(std::ostream &_OutStream, int _Level = 0) = 0;
};


/** @interface IDiameterHeader
*   @brief Èםעונפויס Header
*/
class IDiameterHeader
{
public:
	virtual ~IDiameterHeader() {}

	virtual unsigned char GetCommandFlags()       const = 0;
	virtual int           GetCommandCode()        const = 0;
	virtual int           GetApplicationID()      const = 0;
	virtual int           GetHopByHopIdentifier() const = 0;
	virtual int           GetEndToEndIdentifier() const = 0;

	virtual bool SetCommandFlags(unsigned char _Flags) = 0;
	virtual bool SetCommandCode(unsigned int _CommandCode) = 0;
	virtual bool SetApplicationID(int _ApplicationID) = 0;
	virtual bool SetHopByHopIdentifier(int _HopByHopIdentifier) = 0;
	virtual bool SetEndToEndIdentifier(int _EndToEndIdentifier) = 0;

	virtual void Dump(std::ostream &_OutStream) = 0;
};


/** @interface IDiameterMessage
*   @brief Èםעונפויס DiameterMessage
*/
class IDiameterMessage
{
public:
	virtual ~IDiameterMessage() {}

	virtual IDiameterHeader* GetHeader() = 0;
	virtual IAVP*            GetAVPs()   = 0;

	virtual void Dump(std::ostream &_OutStream) = 0;
};

}

#endif //__DIAMETERMESSAGE_H__
