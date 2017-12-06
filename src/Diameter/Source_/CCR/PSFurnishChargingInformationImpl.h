#ifndef __PS_FURNISH_CHARGING_INFORMATION_IMPL_H__
#define __PS_FURNISH_CHARGING_INFORMATION_IMPL_H__

#include "CCRCCAMessage.h"

#include "../RawMessage/RawDiameterMessage.h"

#include "../RawMessage/DiameterAVP.h"
#include "../RawMessage/DiameterAVPGroup.h"

#include <vector>

namespace CoreDiam
{

typedef std::vector<unsigned char> VEC_DATA;

class DIAMCORE_API CPSFurnishChargingInformationImpl : public CPSFurnishChargingInformation
{
protected:
	IAVP * m_pAVP;

	VEC_DATA               m_vec3GPPChargingId;
	VEC_DATA               m_vecPSFreeFormatData;
	PSAppendFreeFormatData m_PSAppendFreeFormatData;

public:
	CPSFurnishChargingInformationImpl(IAVP* _pAVP);
	CPSFurnishChargingInformationImpl(const unsigned char* _Data, unsigned int _Size);
	virtual ~CPSFurnishChargingInformationImpl();

	virtual const unsigned char* GetGPRSChargingId(unsigned int& _Len);
	virtual const unsigned char* GetPSFreeFormatData(unsigned int& _Len);
	virtual PSAppendFreeFormatData GetPSAppendFreeFormatData();

	virtual bool SetGPRSChargingId(const unsigned char* _GPRSChargingId, unsigned int _Len);
	virtual bool SetPSFreeFormatData(const unsigned char* _PSFreeFormatData, unsigned int _Len);
	virtual bool SetPSAppendFreeFormatData(PSAppendFreeFormatData _PSAppendFreeFormatData);
};

}

#endif //__PS_FURNISH_CHARGING_INFORMATION_IMPL_H__
