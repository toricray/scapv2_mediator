#include "DiameterAVPGroup.h"
namespace CoreDiam
{

CDiameterAVPGroup::CDiameterAVPGroup(IAVP::AVPType _AVPType, int _Code) :
	m_AVPType(_AVPType),
	m_Code(_Code),
	m_VendorID(10415)
{

	ResetList();
}

CDiameterAVPGroup::~CDiameterAVPGroup()
{

	VEC_AVPS::iterator begin = m_vecAVPs.begin(), end = m_vecAVPs.end();
	for (; begin != end; begin++)
		delete *begin;
	m_vecAVPs.clear();
}

const char* CDiameterAVPGroup::GetName()
{
	return NULL;
}

IAVP::AVPType CDiameterAVPGroup::GetType()
{
	return m_AVPType;
}

int CDiameterAVPGroup::GetVendorID()
{
	return m_VendorID;
}

int CDiameterAVPGroup::GetCode()
{
	return m_Code;
}

unsigned char CDiameterAVPGroup::GetFlags()
{
	return 0xFF;
}

int CDiameterAVPGroup::GetCount()
{
	return static_cast<int>(m_vecAVPs.size());
}

IAVP* CDiameterAVPGroup::GetAVPByName(const char* _Name, unsigned int _Index)
{
	return NULL;
}

IAVP * CDiameterAVPGroup::GetAVPByCode(int _Code, unsigned int _Index)
{
	if(_Code != m_Code)
		return NULL;

	if(_Index >= m_vecAVPs.size())
		return NULL;

	return m_vecAVPs[_Index];
}

const unsigned char* CDiameterAVPGroup::GetData(unsigned int& _Size)
{
	_Size = 0;
	return NULL;
}

bool CDiameterAVPGroup::SetData(const unsigned char* _Data, unsigned int _Len)
{
	return false;
}

IAVP* CDiameterAVPGroup::CreateAVPByName(const char* _Name, unsigned int _Index)
{
	return NULL;
}

IAVP* CDiameterAVPGroup::CreateAVPByCode(int _Code, unsigned int _Index)
{
	return NULL;
}

bool CDiameterAVPGroup::SetAVP(IAVP *_pAVP, unsigned int _Index)
{

	if(_Index == 0xffffffff || _Index == m_vecAVPs.size())
	{
		m_vecAVPs.push_back(_pAVP);
		ResetList();
		return true;
	}

	if(_Index > m_vecAVPs.size())
		return false;

	delete m_vecAVPs[_Index];
	m_vecAVPs[_Index] = _pAVP;
	ResetList();
	return true;
}

void CDiameterAVPGroup::ResetList()
{

	m_CurrentIt = m_vecAVPs.begin();
	return;
}

IAVP * CDiameterAVPGroup::GetNextAVP()
{

	if(m_CurrentIt == m_vecAVPs.end())
		return NULL;
	return *(m_CurrentIt++);
}

void CDiameterAVPGroup::Dump(std::ostream &_OutStream, int _Level)
{

	if (m_AVPType == IAVP::GROUPEDSET)
	{
		int TabCount = _Level;
		while (TabCount-- > 0)
			_OutStream << '\t';
		
		_OutStream << "[AVP GROUPEDSET Code: " << m_Code << ", Vendor: " << m_VendorID << "]" << std::endl;
		
		_Level++;
		ResetList();
		IAVP* pAVP;
		while((pAVP = GetNextAVP()) != NULL)
			pAVP->Dump(_OutStream, _Level);
		_Level--;
		
		TabCount = _Level;
		while (TabCount-- > 0)
			_OutStream << '\t';
		
		_OutStream << "[AVP GROUPEDSET END]" << std::endl << std::endl;
	}
}

}
