#ifndef __SETTINGS_H__
#define __SETTINGS_H__

#include "Exports.h"

namespace CoreDiam
{
class ISettings
{
public:
	virtual ~ISettings() {}
	virtual bool TakeSetting(const char* _SettingName, const unsigned char*& _Value, unsigned int& _Size) = 0;
};

extern "C" DIAMCORE_API ISettings* CreateSettings(const char* _SettingsFile);

}

#endif //__SETTINGS_H__
