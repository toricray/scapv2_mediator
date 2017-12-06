#ifndef __SETTING_MANAGER_H__
#define __SETTING_MANAGER_H__

#include <string>
#include <vector>
#include <map>
#include <cstring>
#include <mutex>

class ISettingParam
{
	friend class SettingParamContainer;
	friend class SettingManager;
private:
	virtual bool setValue( const char* ) = 0;

public:
	virtual ~ISettingParam() {};

	virtual const char*  getKey( void ) const = 0;
	virtual const char*  getValue( void ) const = 0;
	virtual long   getLong( void ) = 0;
	virtual double getDouble( void ) = 0;


	operator const char * () {return this->getValue();}
};

struct CStringComparer
{
    bool operator() (const char* lhs, const char* rhs)
    {
        return strcmp(lhs, rhs) < 0;
    }
};

class SettingManager
{
protected:

	std::map<const char *, ISettingParam*, CStringComparer> _params;
	std::mutex              			  _mutex;

public:
	SettingManager( void );
	virtual ~SettingManager( void );

	void 		setParameter( const char* key, const char* value );
	int 		loadSettingsFromFile( const char* fileName );

    const char* getStrValue(const char* key);
	long 		getLongValue(const char* key);
	double 		getDoubleValue(const char* key);


};

class SettingParam: public ISettingParam
{
	friend class SettingManager;

private:
	char*           paramKey;
	char*           paramValue;

	double          doubleValue;
	long            longValue;
	bool            isDoubleValueParsed;
	bool            isLongValueParsed;
	bool            isNULL;


	virtual bool setKey( const char* );

	virtual bool setValue( const char* );

public:
	SettingParam( const char* key );

	SettingParam( const char* key, const char* value);

	virtual ~SettingParam( void );

	virtual const char* getKey( void ) const
	{
		return paramKey;
	};

	virtual const char* getValue( void ) const
	{
		return paramValue;
	};
	virtual long getLong( void );

	virtual double getDouble( void );

	bool isDefined( void ) const
	{
		return !isNULL;
	};

};


#endif //__SETTING_MANAGER_H__
