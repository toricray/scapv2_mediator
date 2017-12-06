#ifndef __SETTINGS_PARSER_H__
#define __SETTINGS_PARSER_H__

#include "tinyxml2.h"
#include "SettingManager.h"

//<Tags>
#define XML_TAG_PARAMS			"Params"
#define XML_TAG_PARAM			"Param"
//</Tags>

//<Attributes>
#define XML_ATTR_KEY			"Key"
#define XML_ATTR_VALUE			"Value"
//</Attributes>


class SettingsParser
{
public:
	static int parseSettings( const char* fileName, SettingManager* settingManager );
};

#endif //__SETTINGS_PARSER_H__
