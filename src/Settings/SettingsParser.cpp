#include "SettingsParser.h"

int SettingsParser::parseSettings(const char* fileName, SettingManager* settingManager)
{
	if( !fileName )
	{
		return -1;
	}
	if( !settingManager )
	{
		return -1;
	}

	tinyxml2::XMLDocument doc;

	if( !doc.LoadFile( fileName ) )
	{
		int parsedParamCount = 0;
		tinyxml2::XMLElement* paramsElement = doc.FirstChildElement( XML_TAG_PARAMS );
		if( paramsElement )
		{
			tinyxml2::XMLElement* param = paramsElement->FirstChildElement( XML_TAG_PARAM );
			const char *key, *val;
			while( param )
			{
				key = param->Attribute( XML_ATTR_KEY );
				val = param->Attribute( XML_ATTR_VALUE );
				if( key && val )
				{
					settingManager->setParameter( key, val );
					parsedParamCount++;
				}
				param = param->NextSiblingElement( XML_TAG_PARAM );
			}
			return parsedParamCount;
		}
		else
		{

		}
        }
        else{
	}
	return -1;
}
