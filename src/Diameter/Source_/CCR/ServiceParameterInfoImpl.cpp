#include <arpa/inet.h>
#include "ServiceParameterInfoImpl.h"

namespace CoreDiam {

    CServiceParameterInfoImpl::CServiceParameterInfoImpl(IAVP *_pAVP) :
            m_pAVP(_pAVP) {
    }

    CServiceParameterInfoImpl::CServiceParameterInfoImpl(const unsigned char *_Data, unsigned int _Size) {
        const unsigned char *AVPData;
        unsigned int AVPSize;

        AVPData = GetAVPData(_Data, _Size, 441, AVPSize);
        if (AVPData)
            m_ServiceParameterInfoType = ntohl(*(int *) AVPData);

        AVPData = GetAVPData(_Data, _Size, 442, AVPSize);
        if (AVPData)
        {
            if (m_ServiceParameterInfoType<200)
            {
                int value = ntohl(*(int *) AVPData);
                m_ServiceParameterInfoValue = std::to_string(value);
            }
            else
            {
                m_ServiceParameterInfoValue = (const char*)AVPData;
            }
        }
    }

    CServiceParameterInfoImpl::~CServiceParameterInfoImpl() {
    }

    int CServiceParameterInfoImpl::GetServiceParameterInfoType() {
        return m_ServiceParameterInfoType;
    }

    const char *CServiceParameterInfoImpl::GetServiceParameterInfoValue(unsigned int &_Len)
    {
        _Len = static_cast<unsigned int>(m_ServiceParameterInfoValue.size());
        return m_ServiceParameterInfoValue.c_str();
    }

 /*   bool CServiceParameterInfoImpl::SetServiceParameterInfoType(int _ServiceParameterInfoType) {
        m_ServiceParameterInfoType = _ServiceParameterInfoType;
        m_pAVP->SetAVP(new CDiameterAVP(IAVP::BASIC, m_ServiceParameterInfoType, 441, 0x40, 10415));
        return true;
    }*/

    bool CServiceParameterInfoImpl::SetServiceParameterInfoValue(int _ServiceParameterInfoType, const char *_ServiceParameterInfoValue)
    {
        m_ServiceParameterInfoType = _ServiceParameterInfoType;
        m_pAVP->SetAVP(new CDiameterAVP(IAVP::BASIC, m_ServiceParameterInfoType, 441, 0x40, 10415));

        m_ServiceParameterInfoValue.assign(_ServiceParameterInfoValue);

        if(0 <= _ServiceParameterInfoType && _ServiceParameterInfoType <= 199) //Integer32
        {
            m_pAVP->SetAVP(new CDiameterAVP(IAVP::BASIC, std::stoi(m_ServiceParameterInfoValue), 442, 0x40, 10415));
        }

        if(200 <= _ServiceParameterInfoType && _ServiceParameterInfoType <= 399) //UTF8String
        {
            m_pAVP->SetAVP(new CDiameterAVP(IAVP::BASIC, (const unsigned char *) m_ServiceParameterInfoValue.c_str(), static_cast<unsigned int>(m_ServiceParameterInfoValue.size()), 442, 0x40, 10415));
        }

        if(400 <= _ServiceParameterInfoType && _ServiceParameterInfoType <= 599) //OctetString
        {
            size_t len = 4;
            in_addr maskAddr;
            inet_aton(_ServiceParameterInfoValue, &maskAddr);
            unsigned char* Buf = new unsigned char[len];
            memcpy(Buf, &maskAddr.s_addr, len);
            m_pAVP->SetAVP(new CDiameterAVP(IAVP::BASIC, Buf, len, 442, 0x40, 10415));
        }

        return true;
    }
}