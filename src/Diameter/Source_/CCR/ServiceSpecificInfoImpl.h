#ifndef __SERVICE_SPECIFIC_INFO_IMPL_H__
#define __SERVICE_SPECIFIC_INFO_IMPL_H__

#include "CCRCCAMessage.h"

#include "../RawMessage/DiameterAVP.h"

#include "../RawMessage/DiameterAVP.h"
#include "../RawMessage/DiameterAVPGroup.h"

#include <vector>

namespace CoreDiam
{

typedef std::vector<unsigned char> VEC_DATA;
	
class DIAMCORE_API CServiceSpecificInfoImpl : public CServiceSpecificInfo
{
protected:
	int m_ServiceSpecificInfoType;
	VEC_DATA m_vecServiceSpecificInfoData;
protected:
	IAVP* m_pAVP;
public:
	CServiceSpecificInfoImpl(IAVP* _pAVP);
	CServiceSpecificInfoImpl(const unsigned char* _Data, unsigned int _Size);
	virtual ~CServiceSpecificInfoImpl();
public:
	virtual int GetServiceSpecificInfoType();
	virtual const unsigned char* GetServiceSpecificInfoData(unsigned int& _Len);
public:
	virtual bool SetServiceSpecificInfoType(int _ServiceSpecificInfoType);
	virtual bool SetServiceSpecificInfoData(const unsigned char* _ServiceSpecificInfoData, unsigned int _Len);
};

}

#endif //__SERVICE_SPECIFIC_INFO_IMPL_H__