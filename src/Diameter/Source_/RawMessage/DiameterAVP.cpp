#include <cstring>
#include <netinet/in.h>
#include "DiameterAVP.h"

namespace CoreDiam {

CDiameterAVP::CDiameterAVP(IAVP::AVPType _AVPType, const unsigned char* _Data, unsigned int _Size, int _Code, unsigned char _Flags, int _VendorID) :
	m_AVPType(_AVPType),
	m_Code(_Code),
	m_Flags(_Flags),
	m_VendorID(_VendorID),
	m_IntData(-1)
{

	if (_Size == sizeof(int)) //!!!
	{
		memcpy(&m_IntData, _Data, _Size);
		m_IntData = ntohl(m_IntData);
	}

	if (IsUnsigned64(_Code))
	{
		m_vecData.resize(8);
		m_vecData.assign(8, 0x00);
		unsigned char* Dst = &m_vecData[0];
		if (_Size == sizeof(int))
			Dst = &m_vecData[4];
		memcpy(Dst, _Data, _Size);
	}
	else
	{
		m_vecData.resize(_Size);
		if (_Size)
			m_vecData.assign(_Data, _Data + _Size);
	}

	m_CurrentIt = m_mapAVPs.begin();
}
//*/
CDiameterAVP::CDiameterAVP(IAVP::AVPType _AVPType, int _IntData, int _Code, unsigned char _Flags, int _VendorID) :
	m_AVPType(_AVPType),
	m_Code(_Code),
	m_Flags(_Flags),
	m_VendorID(_VendorID),
	m_IntData(_IntData)
{

	int NetworkData = htonl(_IntData);

	m_vecData.resize(sizeof(int));
	const unsigned char* Begin = (const unsigned char*)(&NetworkData);
	const unsigned char* End = Begin + sizeof(int);

	m_vecData.assign(Begin, End);

	m_CurrentIt = m_mapAVPs.begin();
}
//*/
CDiameterAVP::~CDiameterAVP()
{

	MAP_AVPS::iterator begin = m_mapAVPs.begin(), end = m_mapAVPs.end();
	for (; begin != end; begin++)
	{
		if (begin->second != NULL)
			delete begin->second;
	}
	m_mapAVPs.clear();
}

const char* CDiameterAVP::GetName()
{
	return NULL;
}

IAVP::AVPType CDiameterAVP::GetType()
{
	return m_AVPType;
}

int CDiameterAVP::GetVendorID()
{
	return m_VendorID;
}

int CDiameterAVP::GetCode()
{
	return m_Code;
}

unsigned char CDiameterAVP::GetFlags()
{
	return m_Flags;
}

int CDiameterAVP::GetCount()
{
	if (m_AVPType != IAVP::BASE)
		return static_cast<int>(m_mapAVPs.size());
	return -1;
}

IAVP* CDiameterAVP::GetAVPByName(const char* _Name, unsigned int _Index)
{
	return NULL;
}

IAVP * CDiameterAVP::GetAVPByCode(int _Code, unsigned int _Index)
{
	MAP_AVPS::iterator it = m_mapAVPs.find(_Code);
	if(it != m_mapAVPs.end())
		return it->second;
	return NULL;
}

const unsigned char * CDiameterAVP::GetData(unsigned int& _Size)
{
	_Size = static_cast<unsigned int>(m_vecData.size());
	return m_vecData.data();
}

bool CDiameterAVP::SetData(const unsigned char *_Data, unsigned int _Len)
{
	return false;
}

IAVP* CDiameterAVP::CreateAVPByName(const char *_Name, unsigned int _Index)
{
	return NULL;
}

IAVP* CDiameterAVP::CreateAVPByCode(int _Code, unsigned int _Index)
{
	return NULL;
}

bool CDiameterAVP::SetAVP(IAVP *_pAVP, unsigned int _Index)
{

	int Code = _pAVP->GetCode();
	MAP_AVPS::iterator it = m_mapAVPs.find(Code);
	if (it != m_mapAVPs.end())
	{
		delete it->second;
		if (!_pAVP)
		{
			m_mapAVPs.erase(it);
			return true;
		}
		it->second = _pAVP;
	}
	else
	{
		if (!_pAVP)
			return true;
		m_mapAVPs.insert(m_mapAVPs.lower_bound(Code), MAP_AVPS::value_type(Code, _pAVP));
		m_CurrentIt = m_mapAVPs.begin();
	}
	return true;
}

void CDiameterAVP::ResetList()
{

	m_bOnFixed = false;
	if (m_AVPType == IAVP::BASE)
	{
		m_CurrentIt = m_mapAVPs.find(263);
		if (m_CurrentIt != m_mapAVPs.end())
		{
			m_bOnFixed = true;
			return;
		}
	}
	m_CurrentIt = m_mapAVPs.begin();
}

IAVP* CDiameterAVP::GetNextAVP()
{

	if (m_CurrentIt == m_mapAVPs.end())
		return NULL;

	if (m_CurrentIt->first == 263)
	{
		if (m_bOnFixed)
		{
			IAVP* RetVal = m_CurrentIt->second;
			m_bOnFixed = false;
			m_CurrentIt = m_mapAVPs.begin();
			return RetVal;
		}
		else
			m_CurrentIt++;
	}

	if (m_CurrentIt == m_mapAVPs.end())
		return NULL;

	return (m_CurrentIt++)->second;
}

void CDiameterAVP::Dump(std::ostream &_OutStream, int _Level)
{

	if (m_AVPType == IAVP::BASE)
	{
		int TabCount = _Level;
		while (TabCount-- > 0)
			_OutStream << '\t';

		_OutStream << "----- Diameter AVPs -----" << std::endl;

		_Level++;
		ResetList();
		IAVP* pAVP;
		while((pAVP = GetNextAVP()) != NULL)
			pAVP->Dump(_OutStream, _Level);
		_Level--;

		TabCount = _Level;
		while (TabCount-- > 0)
			_OutStream << '\t';

		_OutStream << "-------------------------" << std::endl;
	}
	else if (m_AVPType == IAVP::BASIC)
	{
		int TabCount = _Level;
		while (TabCount-- > 0)
			_OutStream << '\t';

		char Buf[5];
		sprintf(Buf, "0x%.2x", m_Flags);
		_OutStream << "[AVP Code: " << m_Code << ", Flags: " << Buf << ", Vendor: " << m_VendorID << "]" << std::endl;
		TabCount = _Level;
		while (TabCount-- > 0)
			_OutStream << '\t';
		if (m_IntData == -1)
		{
			_OutStream << "RAW:";
			VEC_DATA::iterator begin = m_vecData.begin(), end = m_vecData.end();
			for (; begin != end; begin++)
				;//_OutStream << (const char)(*begin);
			_OutStream << std::endl << std::endl;
		}
		else
			_OutStream << "INT:" << m_IntData << std::endl << std::endl;
	}
	else if (m_AVPType == IAVP::GROUPED)
	{
		int TabCount = _Level;
		while (TabCount-- > 0)
			_OutStream << '\t';

		char Buf[5];
		sprintf(Buf, "0x%.2x", m_Flags);
		_OutStream << "[AVP GROUPED Code: " << m_Code << ", Flags: " << Buf << ", Vendor: " << m_VendorID << "]" << std::endl;

		_Level++;
		ResetList();
		IAVP* pAVP;
		while((pAVP = GetNextAVP()) != NULL)
			pAVP->Dump(_OutStream, _Level);
		_Level--;

		TabCount = _Level;
		while (TabCount-- > 0)
			_OutStream << '\t';

		_OutStream << "[AVP GROUPED END]" << std::endl;
	}
}

inline bool CDiameterAVP::IsUnsigned64(int _Code)
{
	return ((_Code == 421) || (_Code == 412) || (_Code == 414) || (_Code == 417));
}

}
