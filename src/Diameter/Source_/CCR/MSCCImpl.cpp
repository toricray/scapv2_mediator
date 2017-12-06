#include "MSCCImpl.h"

namespace CoreDiam {

static const int AVP_CODE_TRIGGER      = 1264;
static const int AVP_CODE_TRIGGER_TYPE =  870;


CMSCCImpl::CMSCCImpl(IAVP* _pAVP) :
	m_pGrantedServiceUnits(NULL),
	m_pRequestedServiceUnits(NULL),
	m_pUsedServiceUnits(NULL),
	m_RatingGroup(-1),
	m_ValidityTime(-1),
	m_pFinalUnitIndication(NULL),
	m_ReportingReason((ReportingReason)-1),
	m_pPSFurnishChargingInformation(NULL),
	m_pAVP(_pAVP),
	m_TriggerTypeAVPGroup(NULL)
{
	m_pSSInfoAVP = new CDiameterAVPGroup(IAVP::GROUPEDSET, 1249);
	m_pAVP->SetAVP(m_pSSInfoAVP);

	/*/
	if(m_TriggerTypeAVPGroup == NULL)
	{
		CDiameterAVP *triggerAVP = (CDiameterAVP *)(m_pAVP->GetAVPByCode(AVP_CODE_TRIGGER));
		if(!triggerAVP)
		{
			triggerAVP = new CDiameterAVP(IAVP::GROUPED, NULL, 0, AVP_CODE_TRIGGER, 0x40, 10415);
			m_pAVP->SetAVP(triggerAVP);
		}

		CDiameterAVPGroup *triggerTypeAVPGroup = (CDiameterAVPGroup *)(triggerAVP->GetAVPByCode(AVP_CODE_TRIGGER_TYPE));
		if(!triggerTypeAVPGroup)
		{
			triggerTypeAVPGroup = new CDiameterAVPGroup(IAVP::GROUPEDSET, AVP_CODE_TRIGGER_TYPE);
			triggerAVP->SetAVP(triggerTypeAVPGroup);
		}

		m_TriggerTypeAVPGroup = triggerTypeAVPGroup;
	}
	//*/
}
//----------------------------------------------------------------------------
CMSCCImpl::CMSCCImpl(const unsigned char* _Data, unsigned int _Size) :
	m_TriggerTypeAVPGroup(NULL),
	m_pGrantedServiceUnits(NULL),
	m_pRequestedServiceUnits(NULL),
	m_pUsedServiceUnits(NULL),
	m_RatingGroup(-1),
	m_ValidityTime(-1),
	m_pFinalUnitIndication(NULL),
	m_ReportingReason((ReportingReason)-1),
	m_pPSFurnishChargingInformation(NULL),
	m_pAVP(NULL),
    m_ResultCode(-1)
{
	const unsigned char* AVPData;
	unsigned int AVPSize;

	AVPData = GetAVPData(_Data, _Size, 431, AVPSize);
	if (AVPData)
		m_pGrantedServiceUnits = new CUnitsImpl(AVPData, AVPSize);

	AVPData = GetAVPData(_Data, _Size, 437, AVPSize);
	if (AVPData)
		m_pRequestedServiceUnits = new CUnitsImpl(AVPData, AVPSize);

	AVPData = GetAVPData(_Data, _Size, 446, AVPSize);
	if (AVPData)
		m_pUsedServiceUnits = new CUnitsImpl(AVPData, AVPSize);

        AVPData = GetAVPData(_Data, _Size, 268, AVPSize);
        if (AVPData)
                m_ResultCode = ntohl(*(int*)AVPData);

	AVPData = _Data;
	AVPSize = 0;
	while ((AVPData = GetAVPData(AVPData + AVPSize, _Size - static_cast<unsigned int>(AVPData + AVPSize - _Data), 439, AVPSize)) != NULL)
		m_vecServiceIdentifier.push_back(ntohl(*(int*)AVPData));

	AVPData = GetAVPData(_Data, _Size, 432, AVPSize);
	if (AVPData)
		m_RatingGroup = ntohl(*(int*)AVPData);

	AVPData = GetAVPData(_Data, _Size, 448, AVPSize);
	if (AVPData)
		m_ValidityTime = ntohl(*(int*)AVPData);

	AVPData = GetAVPData(_Data, _Size, 430, AVPSize);
	if (AVPData)
		m_pFinalUnitIndication = new CFinalUnitIndicationImpl(AVPData, AVPSize);

	AVPData = GetAVPData(_Data, _Size, 872, AVPSize);
	if (AVPData)
		m_ReportingReason = (ReportingReason)ntohl(*(int*)AVPData);

	AVPData = _Data;
	AVPSize = 0;
	while ((AVPData = GetAVPData(AVPData + AVPSize, _Size - static_cast<unsigned int>(AVPData + AVPSize - _Data), 1249, AVPSize)) != NULL)
		m_vecServiceSpecificInfo.push_back(new CServiceSpecificInfoImpl(AVPData, AVPSize));

	//AVPData = _Data;
	//AVPSize = 0;
	AVPData = GetAVPData(_Data, _Size, AVP_CODE_TRIGGER, AVPSize);
	if(AVPData)
	{
		const unsigned char *avpTT     = AVPData;
		unsigned int         avpTTSize = 0;
		while(NULL != (avpTT = GetAVPData(avpTT + avpTTSize, AVPSize - static_cast<unsigned int>(avpTT + avpTTSize - AVPData), AVP_CODE_TRIGGER_TYPE, avpTTSize)))
		{
			m_vecTriggerTypes.push_back(ntohl(*(int *)avpTT));
		}
	}
}

CMSCCImpl::~CMSCCImpl()
{
	if (m_pGrantedServiceUnits)
		delete m_pGrantedServiceUnits;

	if (m_pRequestedServiceUnits)
		delete m_pRequestedServiceUnits;

	if (m_pUsedServiceUnits)
		delete m_pUsedServiceUnits;

	if (m_pFinalUnitIndication)
		delete m_pFinalUnitIndication;

	VEC_SERVICESPECIFICINFO::iterator bssinfo = m_vecServiceSpecificInfo.begin(), essinfo = m_vecServiceSpecificInfo.end();
	for (; bssinfo != essinfo; bssinfo++)
		delete *bssinfo;
	m_vecServiceSpecificInfo.clear();

	if (m_pPSFurnishChargingInformation)
		delete m_pPSFurnishChargingInformation;
}

CUnits* CMSCCImpl::GetGrantedServiceUnits(bool _bAutoCreate)
{
	if (!m_pGrantedServiceUnits && _bAutoCreate)
		return CreateGrantedServiceUnits();
	return m_pGrantedServiceUnits;
}

CUnits* CMSCCImpl::GetRequestedServiceUnits()
{
	return m_pRequestedServiceUnits;
}

CUnits* CMSCCImpl::GetUsedServiceUnits()
{
	return m_pUsedServiceUnits;
}

int CMSCCImpl::GetServiceIdentifier(int _Index)
{
	if(static_cast<size_t>(_Index) < m_vecServiceIdentifier.size())
		return m_vecServiceIdentifier[_Index];
	return -1;
}

int CMSCCImpl::GetRatingGroup()
{
	return m_RatingGroup;
}

int CMSCCImpl::GetValidityTime()
{
	return m_ValidityTime;
}

CFinalUnitIndication* CMSCCImpl::GetFinalUnitIndication()
{
	return m_pFinalUnitIndication;
}

ReportingReason CMSCCImpl::GetReportingReason()
{
	return m_ReportingReason;
}

CServiceSpecificInfo* CMSCCImpl::GetServiceSpecificInfo(int _Index)
{
	if (_Index < (int)m_vecServiceSpecificInfo.size())
		return m_vecServiceSpecificInfo[_Index];
	return NULL;
}

CPSFurnishChargingInformation* CMSCCImpl::GetPSFurnishChargingInformation()
{
	return m_pPSFurnishChargingInformation;
}


CUnits * CMSCCImpl::CreateGrantedServiceUnits()
{
	if (m_pGrantedServiceUnits)
		delete m_pGrantedServiceUnits;
	IAVP* pAVP = new CDiameterAVP(IAVP::GROUPED, NULL, 0, 431, 0x40, 10415);
	m_pAVP->SetAVP(pAVP);
	m_pGrantedServiceUnits = new CUnitsImpl(pAVP);
	return m_pGrantedServiceUnits;
}

CUnits * CMSCCImpl::CreateRequestedServiceUnits()
{
	if (m_pRequestedServiceUnits)
		delete m_pRequestedServiceUnits;
	IAVP* pAVP = new CDiameterAVP(IAVP::GROUPED, NULL, 0, 437, 0x40, 10415);
	m_pAVP->SetAVP(pAVP);
	m_pRequestedServiceUnits = new CUnitsImpl(pAVP);
	return m_pRequestedServiceUnits;
}

CUnits * CMSCCImpl::CreateUsedServiceUnits()
{
	if(m_pUsedServiceUnits)
		delete m_pUsedServiceUnits;

	IAVP *pAVP = new CDiameterAVP(IAVP::GROUPED, NULL, 0, 446, 0x40, 10415);
	m_pAVP->SetAVP(pAVP);
	m_pUsedServiceUnits = new CUnitsImpl(pAVP);

	return m_pUsedServiceUnits;
}

bool CMSCCImpl::SetServiceIdentifier(int _ServiceIdentifier, int _Index)
{
	if(static_cast<size_t>(_Index) > m_vecServiceIdentifier.size())
		return false;

	CDiameterAVPGroup* AVPGroup = (CDiameterAVPGroup*)(m_pAVP->GetAVPByCode(439));
	if(!AVPGroup)
	{
		AVPGroup = new CDiameterAVPGroup(IAVP::GROUPEDSET, 439);
		m_pAVP->SetAVP(AVPGroup);
	}
	AVPGroup->SetAVP(new CDiameterAVP(IAVP::BASIC, _ServiceIdentifier, 439, 0x40, 10415), _Index);

	if(static_cast<size_t>(_Index) == m_vecServiceIdentifier.size())
		m_vecServiceIdentifier.push_back(_ServiceIdentifier);
	else
		m_vecServiceIdentifier[_Index] = _ServiceIdentifier;
	return true;
}

bool CMSCCImpl::SetRatingGroup(int _RatingGroup)
{
	m_RatingGroup = _RatingGroup;
	m_pAVP->SetAVP(new CDiameterAVP(IAVP::BASIC, m_RatingGroup, 432, 0x40, 10415));
	return true;
}

bool CMSCCImpl::SetValidityTime(int _ValidityTimeSeconds)
{
	m_ValidityTime = _ValidityTimeSeconds;
	m_pAVP->SetAVP(new CDiameterAVP(IAVP::BASIC, m_ValidityTime, 448, 0x40, 10415));
	return true;
}

CFinalUnitIndication* CMSCCImpl::CreateFinalUnitIndication()
{
	if (m_pFinalUnitIndication)
		delete m_pFinalUnitIndication;
	IAVP* pAVP = new CDiameterAVP(IAVP::GROUPED, NULL, 0, 430, 0x40, 10415);
	m_pAVP->SetAVP(pAVP);
	m_pFinalUnitIndication = new CFinalUnitIndicationImpl(pAVP);
	return m_pFinalUnitIndication;
}

bool CMSCCImpl::SetReportingReason(ReportingReason _RR)
{
	m_ReportingReason = _RR;
	m_pAVP->SetAVP(new CDiameterAVP(IAVP::BASIC, m_ReportingReason, 872, 0xC0, 10415));
	return true;
}

CServiceSpecificInfo* CMSCCImpl::CreateServiceSpecificInfo(int _Index)
{
	if (_Index == (int)m_vecServiceSpecificInfo.size())
	{
		IAVP* SSIAVP = new CDiameterAVP(IAVP::GROUPED, NULL, 0, 1249, 0xC0, 10415);
		m_vecServiceSpecificInfo.push_back(new CServiceSpecificInfoImpl(SSIAVP));
		m_pSSInfoAVP->SetAVP(SSIAVP, _Index);
		return m_vecServiceSpecificInfo[_Index];
	}
	return NULL;
}

CPSFurnishChargingInformation* CMSCCImpl::CreatePSFurnishChargingInformation()
{
	if(!m_pPSFurnishChargingInformation)
	{
		IAVP *FCIAVP = new CDiameterAVP(IAVP::GROUPED, NULL, 0, 865, 0xC0, 10415);
		m_pPSFurnishChargingInformation = new CPSFurnishChargingInformationImpl(FCIAVP);
		m_pAVP->SetAVP(FCIAVP);
	}

	return m_pPSFurnishChargingInformation;
}

size_t CMSCCImpl::GetServiceSpecificInfoCount()
{
	return m_vecServiceSpecificInfo.size();
}
//----------------------------------------------------------------------------
CoreDiam::TriggerType CMSCCImpl::GetTriggerType(int index)
{
	if(index < 0 || static_cast<size_t>(index) >= m_vecTriggerTypes.size())
		return CHANGE_NULL;

	return (CoreDiam::TriggerType)m_vecTriggerTypes[index];
}
//----------------------------------------------------------------------------
size_t CMSCCImpl::GetTriggerTypeCount()
{
	return m_vecTriggerTypes.size();
}
//----------------------------------------------------------------------------
int CMSCCImpl::AddTriggerType(TriggerType triggerType)
{
	//if(m_TriggerTypeAVPGroup == NULL)
	//	return -1;

	if(m_TriggerTypeAVPGroup == NULL)
	{
		CDiameterAVP *triggerAVP = (CDiameterAVP *)(m_pAVP->GetAVPByCode(AVP_CODE_TRIGGER));
		if(!triggerAVP)
		{
			triggerAVP = new CDiameterAVP(IAVP::GROUPED, NULL, 0, AVP_CODE_TRIGGER, 0xC0, 10415);
			m_pAVP->SetAVP(triggerAVP);
		}

		CDiameterAVPGroup *triggerTypeAVPGroup = (CDiameterAVPGroup *)(triggerAVP->GetAVPByCode(AVP_CODE_TRIGGER_TYPE));
		if(!triggerTypeAVPGroup)
		{
			triggerTypeAVPGroup = new CDiameterAVPGroup(IAVP::GROUPEDSET, AVP_CODE_TRIGGER_TYPE);
			triggerAVP->SetAVP(triggerTypeAVPGroup);
		}

		m_TriggerTypeAVPGroup = triggerTypeAVPGroup;
	}

	// Trigger (1264)
	// - *[Trigger-Type] (870)

	if(triggerType == CHANGE_NULL)
		return -1;

	int tt = (int)triggerType;

	int index = getTriggerTypeGroupIndex(triggerType);
	if(index < 0)
		m_TriggerTypeAVPGroup->SetAVP(new CDiameterAVP(IAVP::BASIC, tt, AVP_CODE_TRIGGER_TYPE, 0xC0, 10415), m_TriggerTypeAVPGroup->GetCount());

	if(!m_vecTriggerTypes.empty())
	{
		auto it = std::find(m_vecTriggerTypes.begin(), m_vecTriggerTypes.end(), tt);
		if(m_vecTriggerTypes.end() != it)
			return it - m_vecTriggerTypes.begin();
	}

	m_vecTriggerTypes.push_back(tt);
	return m_vecTriggerTypes.size() - 1;
}
//----------------------------------------------------------------------------
bool CMSCCImpl::SetTriggerType(int index, TriggerType triggerType)
{
	if(m_TriggerTypeAVPGroup == NULL)
		return false;

	if(m_vecTriggerTypes.empty())
		return false;

	if(index < 0 || index >= m_vecTriggerTypes.size())
		return false;

	VEC_INT::iterator it = std::find(m_vecTriggerTypes.begin(), m_vecTriggerTypes.end(), (int)triggerType);
	if(m_vecTriggerTypes.end() != it)
	{
		if(index == it - m_vecTriggerTypes.begin())
			return true;

		return false;
	}

	int oldTT = m_vecTriggerTypes[index];
	m_vecTriggerTypes[index] = (int)triggerType;

	int i = getTriggerTypeGroupIndex((TriggerType)oldTT);
	if(i >= 0)
		m_TriggerTypeAVPGroup->SetAVP(new CDiameterAVP(IAVP::BASIC, (int)triggerType, AVP_CODE_TRIGGER_TYPE, 0xC0, 10415), i);

	return true;
}
//----------------------------------------------------------------------------
bool CMSCCImpl::DelTriggerType(TriggerType triggerType)
{
	// ! doesn't work properly - we can not delete avps
	return false;
	/*/
	if(m_vecTriggerTypes.empty())
		return true;

	VEC_INT::iterator it = std::find(m_vecTriggerTypes.begin(), m_vecTriggerTypes.end(), (int)triggerType);
	if(m_vecTriggerTypes.end() == it)
		return true;

	m_vecTriggerTypes.erase(it);

	int i = getTriggerTypeGroupIndex(triggerType);
	if(i >= 0)
		m_TriggerTypeAVPGroup->SetAVP(new CDiameterAVP(IAVP::BASIC, (int)triggerType, AVP_CODE_TRIGGER_TYPE, 0x40, 10415), i);

	{
		if(index == it - m_vecTriggerTypes.begin())
			return true;

		return false;
	}
	/*/
}
//----------------------------------------------------------------------------
bool CMSCCImpl::DelTriggerType(int index)
{
	return false;
}
//----------------------------------------------------------------------------
int CMSCCImpl::getTriggerTypeGroupIndex(TriggerType triggerType)
{
	if(m_TriggerTypeAVPGroup == NULL)
	{
		CDiameterAVP *triggerAVP = (CDiameterAVP *)(m_pAVP->GetAVPByCode(AVP_CODE_TRIGGER));
		if(!triggerAVP)
		{
			triggerAVP = new CDiameterAVP(IAVP::GROUPED, NULL, 0, AVP_CODE_TRIGGER, 0x40, 10415);
			m_pAVP->SetAVP(triggerAVP);
		}

		CDiameterAVPGroup *triggerTypeAVPGroup = (CDiameterAVPGroup *)(triggerAVP->GetAVPByCode(AVP_CODE_TRIGGER_TYPE));
		if(!triggerTypeAVPGroup)
		{
			triggerTypeAVPGroup = new CDiameterAVPGroup(IAVP::GROUPEDSET, AVP_CODE_TRIGGER_TYPE);
			triggerAVP->SetAVP(triggerTypeAVPGroup);
		}

		m_TriggerTypeAVPGroup = triggerTypeAVPGroup;
	}

	if(!m_TriggerTypeAVPGroup->GetCount())
	{
		return -1; //triggerTypeAVPGroup->SetAVP(new CDiameterAVP(IAVP::BASIC, tt, AVP_CODE_TRIGGER_TYPE, 0x40, 10415), 0);
	}
	else
	{
		// find triggerType
		unsigned int dataSize;

		int i = 0;
		for(; i < m_TriggerTypeAVPGroup->GetCount(); i++)
		{
			IAVP *avp = m_TriggerTypeAVPGroup->GetAVPByCode(AVP_CODE_TRIGGER_TYPE, (unsigned int)i);
			if(avp)
			{
				if(ntohl(*(int *)avp->GetData(dataSize)) == (int)triggerType)
					return i; //break;
			}
		}

		if(i >= m_TriggerTypeAVPGroup->GetCount())
		{
			// not found
			return -1; //triggerTypeAVPGroup->GetCount(); //triggerTypeAVPGroup->SetAVP(new CDiameterAVP(IAVP::BASIC, tt, AVP_CODE_TRIGGER_TYPE, 0x40, 10415), triggerTypeAVPGroup->GetCount());
		}
	}
	return -1;
}
//----------------------------------------------------------------------------
int CMSCCImpl::DelTrigger()
{
	return 0;
}
//----------------------------------------------------------------------------
int CMSCCImpl::GetResultCode()
{
	return m_ResultCode;
}
//----------------------------------------------------------------------------
bool CMSCCImpl::SetResultCode(int _ResultCode)
{
	m_ResultCode = _ResultCode;
	m_pAVP->SetAVP(new CDiameterAVP(IAVP::BASIC, m_ResultCode, 268, 0x40, 10415));
	return true;
}

}
