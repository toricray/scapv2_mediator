
#include "MoneyImpl.h"

namespace CoreDiam
{

CMoneyImpl::CMoneyImpl(IAVP* _pAVP) :
	m_UnitValue(0),
	m_CurrencyCode(-1),
	m_pAVP(_pAVP)
{
}

CMoneyImpl::CMoneyImpl(const unsigned char* _Data, unsigned int _Size) :
	m_UnitValue(0),
	m_CurrencyCode(-1)
{
	const unsigned char *avpData;
	const unsigned char *avpUnitValue;

	unsigned int         avpSize;
	unsigned int         avpUnitValueSize;

	// Currency-Code (uint32)
	avpData = GetAVPData(_Data, _Size, 425, avpSize);
	if(avpData)
		m_CurrencyCode = ntohl(*(long *)avpData);

	avpUnitValue = GetAVPData(_Data, _Size, 445, avpUnitValueSize);
	if(avpUnitValue)
	{
		// Value-Digits (int64)
		avpData = GetAVPData(avpUnitValue, avpUnitValueSize, 447, avpSize);
		if(avpData && avpSize == 8)
		{
			//m_UnitValue = (double)reinterpret_cast<unsigned long>(ntohl(*(unsigned long *)(avpData + 4)));

			int64_t digits = ntohl(*(unsigned long *)(avpData));
			digits = (digits << 32) | (int64_t)ntohl(*(unsigned long *)(avpData + 4));
			m_UnitValue = (double)digits;

			// Exponent (int32)
			avpData = GetAVPData(avpUnitValue, avpUnitValueSize, 429, avpSize);
			if(avpData && avpSize == 4)
			{
				m_UnitValue *= pow(10.0, (int)((long)ntohl(*(unsigned long *)avpData)));
			}
		}
	}
}

CMoneyImpl::~CMoneyImpl()
{
}

double CMoneyImpl::GetUnitValue()
{
	return m_UnitValue;
}

int CMoneyImpl::GetCurrencyCode()
{
	return m_CurrencyCode;
}

bool CMoneyImpl::SetUnitValue(double _UnitValue)
{
	m_UnitValue = _UnitValue;

	int ResValue = 0, ResExponent = 0;
	char Buffer[255];
	
	int Values[2] = {0, 0};
	int Index = 0, PosLeft = 0, Sign = 1;
	int BufferLen = sprintf(Buffer, "%6f", m_UnitValue);
	
	for(int i = 0; i < BufferLen; ++i)
	{
		if(Buffer[i] == '.' || Buffer[i] == ',')
		{
			Index = 1;
			PosLeft = i + 1;
			continue;
		}
		if(Buffer[i] == '-')
		{
			Sign = -1;
			continue;
		}
		if(Buffer[i] == '+')
			continue;
		
		if(Buffer[i] >= '0' && Buffer[i] <= '9')
			Values[Index] = (Values[Index] * 10 + (Buffer[i] - '0'));
	}
	
	if(Values[1] == 0)
	{
		if(Values[0] != 0)
		{
			while(Values[0] % 10 == 0)
			{
				ResExponent += 1;
				Values[0] /= 10;
			}
			ResValue = Values[0];
		}
		
	}
	else
	{
		int Exclude = 6;
		while(Values[1] % 10 == 0)
		{
			--Exclude;
			Values[1] /= 10;
		}
		ResExponent = Exclude;
		
		ResValue = Values[0];
		for(int i = 0; i < ResExponent; ++i, ++PosLeft)
			ResValue = ResValue * 10 + (Buffer[PosLeft] - '0');
		ResExponent *= -1;
	}
	ResValue *= Sign;
	
	m_pAVP->SetAVP(new CDiameterAVP(IAVP::BASIC, ResValue, 447, 0x40, 10415));
	m_pAVP->SetAVP(new CDiameterAVP(IAVP::BASIC, ResExponent, 429, 0x40, 10415));

	return true;
}

bool CMoneyImpl::SetCurrencyCode(int _CurrencyCode)
{
	m_CurrencyCode = _CurrencyCode;
	m_pAVP->SetAVP(new CDiameterAVP(IAVP::BASIC, m_CurrencyCode, 425, 0x40, 10415));
	return true;
}

}
