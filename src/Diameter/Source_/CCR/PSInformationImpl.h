#ifndef __PS_INFORMATION_IMPL_H__
#define __PS_INFORMATION_IMPL_H__

#include "CCRCCAMessage.h"

#include "../RawMessage/DiameterAVP.h"
#include "../RawMessage/DiameterAVPGroup.h"

#include <vector>

namespace CoreDiam
{

typedef std::vector<unsigned char> VEC_DATA;

class DIAMCORE_API CPSInformationImpl : public CPSInformation
{
protected:
	unsigned int       m_3GPPChargingId;
	VEC_DATA           m_vecNodeId;
	PDPType            m_PDPType;
	unsigned short     m_PDPAddressFamily;
	VEC_DATA           m_vecPDPAddress;
	DynamicAddressFlag m_DynamicAddressFlag;
	unsigned short     m_SGSNAddressFamily;
	unsigned short     m_GGSNAddressFamily;
	VEC_DATA           m_vecSGSNAddress;
	VEC_DATA           m_vecGGSNAddress;
	std::string        m_3GPP_IMSI_MCCMNC;
	std::string        m_3GPP_GGSN_MCCMNC;
	std::string        m_CalledStationId;
	std::string        m_3GPP_SGSN_MCCMNC;
	VEC_DATA           m_vec3GPPUserLocationInfo;
	unsigned char      m_RATType;
	unsigned int       m_StartTime;
	unsigned int       m_StopTime;
	unsigned char      m_GeographicLocationType;
protected:
	IAVP *m_pAVP;
public:
	CPSInformationImpl(IAVP* _pAVP);
	CPSInformationImpl(const unsigned char* _Data, unsigned int _Size);
	virtual ~CPSInformationImpl();
public:
	virtual unsigned int         Get3GPPChargingId();
	virtual const unsigned char* GetNodeId(unsigned int& _Len);
	virtual PDPType              Get3GPPPDPType();
	virtual const unsigned char* GetPDPAddress(unsigned short& _AddressFamily, unsigned int& _Len, std::string& _StrAddress);
	virtual DynamicAddressFlag   GetDynamicAddressFlag();
	virtual const unsigned char* GetSGSNAddress(unsigned short& _AddressFamily, unsigned int& _Len, std::string& _StrAddress);
	virtual const unsigned char* GetGGSNAddress(unsigned short& _AddressFamily, unsigned int& _Len, std::string& _StrAddress);
	virtual const char*          Get3GPP_IMSI_MCCMNC(unsigned int &_Len);
	virtual const char*          Get3GPP_GGSN_MCCMNC(unsigned int &_Len);
	virtual const char*          GetCalledStationId();
	virtual const char*          Get3GPP_SGSN_MCCMNC(unsigned int &_Len);
	virtual const unsigned char* Get3GPPUserLocationInfo(unsigned char& _GeographicLocationType, unsigned int& _Len);
	virtual unsigned char        Get3GPPRATType();
	virtual unsigned int         GetStartTime();
	virtual unsigned int         GetStopTime();
public:
	virtual bool Set3GPPChargingId(unsigned int _3GPPChargingId);
	virtual bool SetNodeId(const unsigned char* _NodeId, unsigned int _Len);
	virtual bool Set3GPPPDPType(PDPType _PDPType);
	virtual bool SetPDPAddress(unsigned short _AddressFamily, const unsigned char* _PDPAddress, unsigned int _Len);
	virtual bool SetDynamicAddressFlag(DynamicAddressFlag _DynamicAddressFlag);
	virtual bool SetSGSNAddress(unsigned short _AddressFamily, const unsigned char* _SGSNAddress, unsigned int _Len);
	virtual bool SetGGSNAddress(unsigned short _AddressFamily, const unsigned char* _GGSNAddress, unsigned int _Len);
	virtual bool Set3GPP_IMSI_MCCMNC(const char *_mccMNC, unsigned int &_Len);
	virtual bool Set3GPP_GGSN_MCCMNC(const char *_mccMNC, unsigned int &_Len);
	virtual bool SetCalledStationId(const char* _CalledStationId);
	virtual bool Set3GPP_SGSN_MCCMNC(const char *_mccMNC, unsigned int &_Len);
	virtual bool Set3GPPUserLocationInfo(unsigned char _GeographicLocationType, const unsigned char* _3GPPUserLocationInfo, unsigned int _Len);
	virtual bool Set3GPPRATType(unsigned char _ratType);
	virtual bool SetStartTime(unsigned int _StartTime);
	virtual bool SetStopTime(unsigned int _StopTime);
};

}

#endif //__PS_INFORMATION_IMPL_H__
