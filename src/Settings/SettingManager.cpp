#include "SettingManager.h"

#include <algorithm>
#include "SettingsParser.h"

SettingParam::SettingParam( const char* key ):
	paramKey( NULL )
	, paramValue( NULL )
	, doubleValue( 0.0 )
	, longValue( 0 )
	, isDoubleValueParsed( false )
	, isLongValueParsed( false )
	, isNULL( true )
{
	setKey( key );
}

SettingParam::SettingParam(  const char* key, const char* value)
	: paramKey( NULL ), paramValue( NULL ), doubleValue( 0.0 ), longValue( 0 ), isDoubleValueParsed( false ), isLongValueParsed( false ), isNULL(true)
{
	if( !setKey( key ) )
	{
	}
	if( !setValue( value ) )
	{
	}
}

SettingParam::~SettingParam( void )
{
	delete[] paramKey;
	delete[] paramValue;
}

bool SettingParam::setKey( const char* key )
{
	if( key )
	{
		if( strlen( key ) > 0 )
		{
			if( paramKey ) delete[] paramKey;
			paramKey = new char[ strlen( key ) + 1 ];
			strcpy( paramKey, key );
			return true;
		}
	}
	return false;
}

bool SettingParam::setValue( const char* value )
{
	isLongValueParsed = false;
	isDoubleValueParsed = false;
	if( value )
	{
		if( strlen( value ) > 0 )
		{
			if( paramValue ) delete[] paramValue;
			paramValue = new char[ strlen( value ) + 1 ];
			strcpy( paramValue, value );
			isNULL = false;
			return true;
		}
	}
	return false;
}


long SettingParam::getLong( void )
{
	if( !isNULL && !isLongValueParsed )
	{
		longValue = atol( paramValue );
		isLongValueParsed = true;
	}
	return longValue;
};

double SettingParam::getDouble( void )
{
	if( !isNULL && !isDoubleValueParsed )
	{
		doubleValue = atof( paramValue );
		isDoubleValueParsed = true;
	}
	return doubleValue;
};

SettingManager::SettingManager( void )
{
}


SettingManager::~SettingManager( void )
{
	_params.clear();
}

void SettingManager::setParameter( const char* key, const char* value )
{
	if( !key )	{return;	}
	if( !value ){return;	}

	_mutex.lock();

	auto it = _params.find( key );
	if( it != _params.end() )
	{
		if( it->second )
		{
			it->second->setValue( value );
		}
	}
	else
	{
		if( strlen( key ) > 0 )
		{
			char* tkey = new char[ strlen( key ) + 1 ];
			strcpy( tkey, key );
			_params[ tkey ] = new SettingParam(key, value );
		}
		else
		{

		}
	}
	_mutex.unlock();
}


const char* SettingManager::getStrValue(const char* key)
{
	if(!key) return NULL;

	_mutex.lock();

	auto it = _params.find(key);
	if( it != _params.end() )
	{
		_mutex.unlock();
		return it->second->getValue();
	}
	_mutex.unlock();
	return NULL;
}

long SettingManager::getLongValue(const char* key)
{
    if( !key ) return false;

    _mutex.lock();

    auto it = _params.find(key);
    if( it != _params.end() )
    {
        _mutex.unlock();
        return it->second->getLong();
    }
    _mutex.unlock();
    return 0;
}



double SettingManager::getDoubleValue(const char* key)
{
	if( !key ) return false;

    _mutex.lock();

	auto it = _params.find(key);
	if( it != _params.end() )
	{
        _mutex.unlock();
		return it->second->getDouble();
	}
    _mutex.unlock();
	return 0;
}

int SettingManager::loadSettingsFromFile( const char* fileName )
{
	return SettingsParser::parseSettings( fileName, this );
}
