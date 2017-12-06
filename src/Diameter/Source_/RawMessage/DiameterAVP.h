#ifndef __DIAMETER_AVP_H__
#define __DIAMETER_AVP_H__

#pragma warning (disable:4786)

#include "DiameterMessage.h"
#include <cstring>
#include <netinet/in.h>
#include <map>
#include <vector>


namespace CoreDiam {

typedef std::map<int, IAVP *>       MAP_AVPS;
typedef std::vector<unsigned char>  VEC_DATA;

class CDiameterAVP: public IAVP
{
protected:
	AVPType            m_AVPType;
	int                m_Code;
	unsigned char      m_Flags;
	int                m_VendorID;

	VEC_DATA           m_vecData;
	MAP_AVPS           m_mapAVPs;

	int                m_IntData;   // used only for Dump()

	MAP_AVPS::iterator m_CurrentIt;

	bool               m_bOnFixed;

	inline bool             IsUnsigned64(int _Code);

public:
	CDiameterAVP(IAVP::AVPType _AVPType, const unsigned char *_Data, unsigned int _Size, int _Code, unsigned char _Flags, int _VendorID);
	CDiameterAVP(IAVP::AVPType _AVPType, int _IntData, int _Code, unsigned char _Flags, int _VendorID);
	virtual ~CDiameterAVP();

	virtual const char    * GetName();
	virtual AVPType         GetType();
	virtual int             GetVendorID();
	virtual int             GetCode();
	virtual unsigned char   GetFlags();
	virtual int             GetCount();

	virtual IAVP          * GetAVPByName(const char* _Name, unsigned int _Index = 0);
	virtual IAVP          * GetAVPByCode(int _Code, unsigned int _Index = 0);

	virtual const unsigned char * GetData(unsigned int &_Size);
	virtual bool                  SetData(const unsigned char *_Data, unsigned int _Len);

	virtual IAVP          * CreateAVPByName(const char* _Name, unsigned int _Index = 0);
	virtual IAVP          * CreateAVPByCode(int _Code, unsigned int _Index = 0);

	virtual bool            SetAVP(IAVP* _pAVP, unsigned int _Index = 0);

	virtual void            ResetList();
	virtual IAVP          * GetNextAVP();

	virtual void            Dump(std::ostream &_OutStream, int _Level = 0);
};


	inline const unsigned char* GetAVPData(const unsigned char* _Data, unsigned int _Size, unsigned int _Code, unsigned int &_AVPDataSize, bool _SkipHeader = false)
	{
		const unsigned char *End = _Data + _Size;
		if (_SkipHeader)
			_Data += 20;

		while (_Data < End)
		{
			unsigned int AvpCode;
			memcpy((void*)&AvpCode, _Data, sizeof(unsigned int));
			AvpCode = ntohl(AvpCode);

			unsigned int AvpLength;
			memcpy((void*)&AvpLength, _Data + 4, sizeof(unsigned int));
			AvpLength = ntohl(AvpLength) & 0x00ffffff;

			if ((AvpLength == 0) || (AvpLength > (unsigned int)(End - _Data)))
			{
				_AVPDataSize = 0;
				return NULL;
			}

			if (AvpCode == _Code)
			{
				bool VendorIdPresent = !((*(_Data + 4) & 0x80) == 0);
				_Data += 8;
				_AVPDataSize = AvpLength - 8;
				if (VendorIdPresent)
				{
					_AVPDataSize -= 4;
					_Data += 4;
				}
				return _Data;
			}

			_Data += 4 * ((AvpLength / 4) + (AvpLength % 4 == 0 ? 0 : 1));
		}
		_AVPDataSize = 0;
		return NULL;
	}

}

#endif //__DIAMETER_AVP_H__
