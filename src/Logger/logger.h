#ifndef __LOGGER_H__
#define __LOGGER_H__

#include <stdlib.h>
#include <iostream>
#include <cstring>

#include <Utills/utills.h>
#include <Settings/SettingManager.h>

enum LogLevel
{
    LOGLVL_DEBUG    =   0,
    LOGLVL_TRACE    =   1,
    LOGLVL_INFO     =   2,
    LOGLVL_WARNING  =   3,
    LOGLVL_ERROR    =   4,
    LOGLVL_CRITICAL =   5,
    MAX_LOGLVL      =   6
};

#define LOG_MESSAGE_LENGTH      2048
#define MAX_FILENAME_LEN        128

#define LOG_POOL_MESSAGE_COUNT  100

#if defined NO_LOG_TO_CONSOLE
#  define _LOG_CON_D(ARG) do {} while(false)
#  define _LOG_CON_T(ARG) do {} while(false)
#  define _LOG_CON_I(ARG) do {} while(false)
#  define _LOG_CON_W(ARG) do {} while(false)
#  define _LOG_CON_E(ARG) do {} while(false)
#  define _LOG_CON_F(ARG) do {} while(false)
#else

#  define _LOG_CON_D(ARG) do {} while(false)
#  define _LOG_CON_T(ARG) do {std::cout << ARG << std::endl;} while(false)
#  define _LOG_CON_I(ARG) do {std::cout << ARG << std::endl;} while(false)
#  define _LOG_CON_W(ARG) do {std::cout << ARG << std::endl;} while(false)
#  define _LOG_CON_E(ARG) do {std::cout << ARG << std::endl;} while(false)
#  define _LOG_CON_F(ARG) do {std::cout << ARG << std::endl;} while(false)
#endif

#define _LOG_TO(SUFFIX__) _LOG_##SUFFIX__
#define DATE_TIME_NOW "[" << currentDateTimeStr() << "]"

#define LOG_FOR_LEVEL(LEVEL_ID, LOG_TO_SUFFIX, LEVEL_STR, ARG) \
	do {LogMessage &logMsg__ = Logger::getLogMsg(LEVEL_ID); \
		if(&logMsg__) { \
			logMsg__ << DATE_TIME_NOW << LEVEL_STR << ARG; \
            _LOG_TO(LOG_TO_SUFFIX)(logMsg__.get());\
			Logger::putLogMsg(logMsg__); \
		} \
	} while(false)


#define LOG_DEBUG(ARG)  LOG_FOR_LEVEL(LOGLVL_DEBUG,    CON_D, "[D]",   ARG)
#define LOG_INFO(ARG)   LOG_FOR_LEVEL(LOGLVL_INFO,     CON_I, "[I]",   ARG)
#define LOG_WARN(ARG)   LOG_FOR_LEVEL(LOGLVL_WARNING,  CON_W, "[W]",   ARG)
#define LOG_ERROR(ARG)  LOG_FOR_LEVEL(LOGLVL_ERROR,    CON_E, "[E]",   ARG)
#define LOG_FATAL(ARG)  LOG_FOR_LEVEL(LOGLVL_CRITICAL, CON_F, "[F]",   ARG)


class LogMessage
{
private:
    std::string        msgText;
    LogLevel    level;

    friend class Logger;

public:
    LogMessage()
    {
        msgText[LOG_MESSAGE_LENGTH] = '\0';
    }

    LogMessage& operator << (char* str)
    {
        if (this&&str)
        {
            msgText += str;
        }
        return *this;
    }

    LogMessage & operator << (const char *str)
    {
        if(this && str)
        {
            msgText += str;
        }
        return *this;
    }

    LogMessage & operator << (int c)
    {
        msgText += std::to_string(c);
        return *this;
    }

    LogMessage & operator << (unsigned int c)
    {
        msgText += std::to_string(c);
        return *this;
    }

    LogMessage & operator << (long c)
    {
        msgText += std::to_string(c);
        return *this;
    }

    LogMessage & operator << (unsigned long n)
    {
        msgText += std::to_string(n);
        return *this;
    }

    LogMessage& operator << (double c)
    {
        if (this)
        {
            msgText += std::to_string(c);
        }
        return *this;
    }

    const char * get()
    {
        return msgText.c_str();
    }
    unsigned long getLength()
    {
        return msgText.length();
    }
    void clear()
    {
        return msgText.clear();
    }
};


class Logger: public Task
{
private:
    static Logger*          instance;

    std::string		        hostName;
    bool                    isLogInCommonFile;
    bool                    logInSeparateFiles;
    bool                    logToSyslog;
    char                    fileNames[MAX_LOGLVL + 1][MAX_FILENAME_LEN];
    ObjectPool<LogMessage>* messagePool;
    SafeQueue  <LogMessage*>*queue_;
    LogLevel                commonLogLevel;
    LogLevel                separateLogLevel;
    LogLevel                syslogLogLevel;
    FILE*                   files[MAX_LOGLVL + 1];
    int                     fileCounter[MAX_LOGLVL + 1];
    int                     recordCounter[MAX_LOGLVL + 1];
    long                    recordCount;
    std::string             logPath;
    std::string             syslogIdent;
    int						syslogFacility;
    Semaphore               newMsgSemaphore;

    static LogLevel          castStrAsLoglvl(const char *strLevel)
    {
        if (!strLevel)
            return MAX_LOGLVL;

        if(0 == strcmp(strLevel, "DEBUG"))
            return LOGLVL_DEBUG;
        if(0 == strcmp(strLevel, "TRACE"))
            return LOGLVL_TRACE;
        if(0 == strcmp(strLevel, "INFO"))
            return LOGLVL_INFO;
        if(0 == strcmp(strLevel, "WARNING"))
            return LOGLVL_WARNING;
        if(0 == strcmp(strLevel, "ERROR"))
            return LOGLVL_ERROR;
        if(0 == strcmp(strLevel, "CRITICAL"))
            return LOGLVL_CRITICAL;

        return MAX_LOGLVL;
    }

    int openFS(LogLevel lvl);

    int checkCounters(LogLevel lvl)
    {
        recordCounter[lvl]++;
        if (recordCounter[lvl] > recordCount)
        {
            recordCounter[lvl] = 0;
            fileCounter[lvl]++;
            if (fileCounter[lvl] > 999)
            {
                fileCounter[lvl] = 0;
            }

            if(openFS(lvl) == -1)
                return -1;
        }
        return 0;
    }

public:
    Logger();
    virtual ~Logger();

    static int                      init(SettingManager* sManager);
    static void                     close();
    static void                     release();
    static LogMessage&              getLogMsg(LogLevel lvl);
    static int                      putLogMsg(LogMessage& msg);

    virtual void svc(void);
};

#endif // __LOGGER_H__
