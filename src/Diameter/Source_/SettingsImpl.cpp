#include <cstring>

#include "SettingsImpl.h"
#include "Settings.h"

namespace CoreDiam
{

CSettingsImpl::CSettingsImpl(const char* _SettingsFile)
{
	m_SessionTimeout = 120000;//default
	m_OwnRealm = "OwnRealm";//default
	m_ResponseTimeout = 10000;//default
	m_OwnHost = "";//default
	m_ServedRealmList = "";//default
}

CSettingsImpl::~CSettingsImpl()
{

}

bool CSettingsImpl::TakeSetting(const char* _SettingName, const unsigned char*& _Value, unsigned int& _Size)
{
	_Value = NULL;
	_Size = 0;
	if (!strcmp(_SettingName, "SessionTimeout"))
	{
		_Value = (const unsigned char*)&m_SessionTimeout;
		_Size = sizeof(m_SessionTimeout);
		return true;
	}
	if (!strcmp(_SettingName, "OwnRealm"))
	{
		_Value = (const unsigned char*)m_OwnRealm.c_str();
		_Size = m_OwnRealm.size();
		return true;
	}
	if (!strcmp(_SettingName, "ResponseTimeout"))
	{
		_Value = (const unsigned char*)&m_ResponseTimeout;
		_Size = sizeof(m_ResponseTimeout);
		return true;
	}
	if (!strcmp(_SettingName, "OwnHost"))
	{
		_Value = (const unsigned char*)m_OwnHost.c_str();
		_Size = m_OwnHost.size();
		return true;
	}
	if (!strcmp(_SettingName, "ServedRealmList"))
	{
		_Value = (const unsigned char*)m_ServedRealmList.c_str();
		_Size = m_ServedRealmList.size();
		return true;
	}
	return false;
}



ISettings* CreateSettings(const char* _SettingsFile)
{
	return new CSettingsImpl(_SettingsFile);
}

}
