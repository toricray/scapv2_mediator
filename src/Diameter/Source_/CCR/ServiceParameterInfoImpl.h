#ifndef __SERVICE_PARAMETER_INFO_IMPL_H__
#define __SERVICE_PARAMETER_INFO_IMPL_H__

#include "CCRCCAMessage.h"

#include "../RawMessage/DiameterAVP.h"
#include "../RawMessage/DiameterAVPGroup.h"

#include <vector>

namespace CoreDiam
{

typedef std::vector<unsigned char> VEC_DATA;
	
class DIAMCORE_API CServiceParameterInfoImpl : public CServiceParameterInfo
{
protected:
	int m_ServiceParameterInfoType;
	std::string m_ServiceParameterInfoValue;
protected:
	IAVP* m_pAVP;
public:
	CServiceParameterInfoImpl(IAVP* _pAVP);
	CServiceParameterInfoImpl(const unsigned char* _Data, unsigned int _Size);
	virtual ~CServiceParameterInfoImpl();
public:
	virtual int GetServiceParameterInfoType();
	virtual const char* GetServiceParameterInfoValue(unsigned int& _Len);
public:
	//virtual bool SetServiceParameterInfoType(int _ServiceParameterInfoType);
	virtual bool SetServiceParameterInfoValue(int _ServiceParameterInfoType, const char* _ServiceParameterInfoValue);
};

}

#endif //__SERVICE_PARAMETER_INFO_IMPL_H__