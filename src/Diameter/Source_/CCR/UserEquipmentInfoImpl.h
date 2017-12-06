#ifndef __USER_EQUIPMENT_INFO_IMPL_H__
#define __USER_EQUIPMENT_INFO_IMPL_H__

#include "CCRCCAMessage.h"
#include <vector>

#include "../RawMessage/DiameterAVP.h"

#include "../RawMessage/DiameterAVP.h"
#include "../RawMessage/DiameterAVPGroup.h"

namespace CoreDiam
{

typedef std::vector<unsigned char> VEC_DATA;

class DIAMCORE_API CUserEquipmentInfoImpl : public CUserEquipmentInfo
{
protected:
	UserEquipmentInfoType m_UserEquipmentInfoType;
	VEC_DATA m_vecUserEquipmentInfoValue;
protected:
	IAVP* m_pAVP;
public:
	CUserEquipmentInfoImpl(IAVP* _pAVP);
	CUserEquipmentInfoImpl(const unsigned char* _Data, unsigned int _Size);
	virtual ~CUserEquipmentInfoImpl();
public:
	virtual UserEquipmentInfoType GetUserEquipmentInfoType();
	virtual const unsigned char* GetUserEquipmentInfoValue(unsigned int& _Len);
public:
	virtual bool SetUserEquipmentInfoType(UserEquipmentInfoType _UserEquipmentInfoType);
	virtual bool SetUserEquipmentInfoValue(const unsigned char* _UserEquipmentInfoValue, unsigned int _Len);
};

}

#endif //__USER_EQUIPMENT_INFO_IMPL_H__
