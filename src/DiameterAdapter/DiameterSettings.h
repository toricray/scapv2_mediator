//
// Created by dev on 2/2/17.
//

#ifndef SCAPV2_MEDIATOR_DIAMETERSETTINGS_H
#define SCAPV2_MEDIATOR_DIAMETERSETTINGS_H

class DiameterSettings: public CoreDiam::ISettings
{
private:
    int             queueSize_;
    int             rcvBufferSize_;
    int             sndBufferSize_;
    int             isBlocking_;
    int             sessionTimeout_;
    int             responseTimeout_;

public:
            DiameterSettings()
            : queueSize_(100000)
            , rcvBufferSize_(8*1024*1024)
            , sndBufferSize_(8*1024*1024)
            , isBlocking_(1)
            , sessionTimeout_(120000)
            , responseTimeout_(60000)
    {}
    virtual ~DiameterSettings() {}

    virtual bool TakeSetting(const char *_SettingName, const unsigned char *&_Value, unsigned int &_Size)
    {
        _Value = (const unsigned char *)0;
        _Size  = 0;
        bool result = true;
        if(!strcmp(_SettingName, "QueueSize"))
        {
            _Value = (const unsigned char *)&queueSize_;
            _Size  = sizeof(queueSize_);
        }
        else if(!strcmp(_SettingName, "RcvBufferSize"))
        {
            _Value = (const unsigned char *)&rcvBufferSize_;
            _Size  = sizeof(rcvBufferSize_);
        }
        else if(!strcmp(_SettingName, "SndBufferSize"))
        {
            _Value = (const unsigned char *)&sndBufferSize_;
            _Size  = sizeof(sndBufferSize_);
        }
        else if(!strcmp(_SettingName, "IsBlocking"))
        {
            _Value = (const unsigned char *)&isBlocking_;
            _Size  = sizeof(isBlocking_);
        }
        else if(!strcmp(_SettingName, "SessionTimeout"))
        {
            _Value = (const unsigned char *)&sessionTimeout_;
            _Size  = sizeof(sessionTimeout_);
        }
        else if(!strcmp(_SettingName, "ResponseTimeout"))
        {
            _Value = (const unsigned char *)&responseTimeout_;
            _Size  = sizeof(responseTimeout_);
        }
        else
        {
            result = false;
        }
        return result;
    }
};

#endif //SCAPV2_MEDIATOR_DIAMETERSETTINGS_H
