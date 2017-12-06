#ifndef __DIAMETER_AVP_GROUP_H__
#define __DIAMETER_AVP_GROUP_H__

#pragma warning (disable:4786)

#include "DiameterMessage.h"

#include <vector>

namespace CoreDiam
{

typedef std::vector<IAVP*> VEC_AVPS;

class CDiameterAVPGroup : public IAVP
{
protected:
	AVPType            m_AVPType;
	int                m_Code;
	int                m_Flags;
	int                m_VendorID;

	VEC_AVPS           m_vecAVPs;
	VEC_AVPS::iterator m_CurrentIt;
public:
	CDiameterAVPGroup(IAVP::AVPType _AVPType, int _Code);
	virtual ~CDiameterAVPGroup();

	virtual const char    * GetName();
	virtual IAVP::AVPType   GetType();
	virtual int             GetVendorID();
	virtual int             GetCode();
	virtual unsigned char   GetFlags();
	virtual int             GetCount();

	virtual IAVP* GetAVPByName(const char* _Name, unsigned int _Index = 0);
	virtual IAVP* GetAVPByCode(int _Code, unsigned int _Index = 0);

	virtual const unsigned char* GetData(unsigned int& _Size);
	virtual bool SetData(const unsigned char* _Data, unsigned int _Len);

	virtual IAVP* CreateAVPByName(const char* _Name, unsigned int _Index = 0);
	virtual IAVP* CreateAVPByCode(int _Code, unsigned int _Index = 0);

	virtual bool   SetAVP(IAVP *_pAVP, unsigned int _Index = 0);

	virtual void   ResetList();
	virtual IAVP * GetNextAVP();

	virtual void   Dump(std::ostream &_OutStream, int _Level);
};

}

#endif //__DIAMETER_AVP_GROUP_H__
