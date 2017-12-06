#ifndef __SETTINGSIMPL_H__
#define __SETTINGSIMPL_H__

#include "../Interface/Settings.h"

#include <string>

namespace CoreDiam
{

class CSettingsImpl : public ISettings
{
protected:
	int m_SessionTimeout;
	std::string m_OwnRealm;
	int m_ResponseTimeout;
	std::string m_OwnHost;
	std::string m_ServedRealmList;
public:
	CSettingsImpl(const char* _SettingsFile);
	virtual ~CSettingsImpl();
public:
	virtual bool TakeSetting(const char* _SettingName, const unsigned char*& _Value, unsigned int& _Size);
};

}

#endif //__SETTINGSIMPL_H__
