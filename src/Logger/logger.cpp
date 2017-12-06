#include "logger.h"
#include <syslog.h>

static unsigned int LogLevelToSyslogLevel (LogLevel lvl)
{
    switch (lvl)
    {
        case	LOGLVL_DEBUG:		return LOG_DEBUG;
        case	LOGLVL_TRACE:		return LOG_INFO;
        case	LOGLVL_INFO:		return LOG_NOTICE;
        case	LOGLVL_WARNING:		return LOG_WARNING;
        case	LOGLVL_ERROR:		return LOG_ERR;
        case	LOGLVL_CRITICAL:	return LOG_CRIT;
        default:
            return LOG_WARNING;
    }
    return LOG_DEBUG;
}

static int castStrAsFacility(const char *facilityLevel)
{
    if(!facilityLevel) return -1;
    if(0 == strcmp(facilityLevel, "auth"))      return LOG_AUTH;
    if(0 == strcmp(facilityLevel, "authpriv"))  return LOG_AUTHPRIV;
    if(0 == strcmp(facilityLevel, "cron"))      return LOG_CRON;
    if(0 == strcmp(facilityLevel, "daemon"))    return LOG_DAEMON;
    if(0 == strcmp(facilityLevel, "ftp"))       return LOG_FTP;
    if(0 == strcmp(facilityLevel, "kern"))      return LOG_KERN;
    if(0 == strcmp(facilityLevel, "lpr"))       return LOG_LPR;
    if(0 == strcmp(facilityLevel, "mail"))      return LOG_MAIL;
    if(0 == strcmp(facilityLevel, "news"))      return LOG_NEWS;
    if(0 == strcmp(facilityLevel, "security"))  return LOG_AUTH;
    if(0 == strcmp(facilityLevel, "syslog"))    return LOG_SYSLOG;
    if(0 == strcmp(facilityLevel, "user"))      return LOG_USER;
    if(0 == strcmp(facilityLevel, "uucp"))      return LOG_UUCP;
    if(0 == strcmp(facilityLevel, "local0"))    return LOG_LOCAL0;
    if(0 == strcmp(facilityLevel, "local1"))    return LOG_LOCAL1;
    if(0 == strcmp(facilityLevel, "local2"))    return LOG_LOCAL2;
    if(0 == strcmp(facilityLevel, "local3"))    return LOG_LOCAL3;
    if(0 == strcmp(facilityLevel, "local4"))    return LOG_LOCAL4;
    if(0 == strcmp(facilityLevel, "local5"))    return LOG_LOCAL5;
    if(0 == strcmp(facilityLevel, "local6"))    return LOG_LOCAL6;
    if(0 == strcmp(facilityLevel, "local7"))    return LOG_LOCAL7;

    return -1;
}

char logLvlChrs[MAX_LOGLVL + 1] = {'D', 'T', 'I', 'W', 'E', 'C', 'A'};

Logger *Logger::instance = 0;

Logger::Logger(): Task(), newMsgSemaphore(0)
{
    messagePool = new ObjectPool<LogMessage>;
    queue_ = new SafeQueue<LogMessage*>();

    for (int i = 0; i <= MAX_LOGLVL; i++)
    {
        files[i] = NULL;
    }
}

Logger::~Logger()
{
    delete messagePool;
    delete queue_;
}

int Logger::openFS(LogLevel lvl)
{
    char totalFileName[100];
    char timeBuffer[40];
    time_t now = time(NULL);
    strftime(timeBuffer, 40, "%Y%m%d%H%M%S", localtime(&now));

    sprintf(
            totalFileName, "%s%s_%c_%s.%03d", logPath.c_str(), hostName.c_str(), logLvlChrs[lvl], timeBuffer,
            instance->fileCounter[lvl]
    );

    strncpy(instance->fileNames[lvl], totalFileName, MAX_FILENAME_LEN);
    instance->files[lvl] = fopen(totalFileName, "wb");
    if (instance->files[lvl])
    {
        instance->fileCounter[lvl]++;
        return 0;
    }
    return -1;
}

void Logger::close()
{
    instance->queue_->stop();
    for (int i = 0; i <= MAX_LOGLVL; i++)
    {
        if (instance->files[i] != NULL)
        {
            fclose(instance->files[i]);
        }
    }
    if(instance->logToSyslog)
        closelog();
}

void Logger::release()
{
    Logger::close();
    delete instance;
}

int Logger::init(SettingManager *settingManager)
{
    if(!instance)
    {
        instance = new Logger();
        for (int i = 0; i < LOG_POOL_MESSAGE_COUNT; i++)
        {
            instance->messagePool->push(new LogMessage);
        }
    }
    if (!settingManager->getStrValue("LOG_PATH"))
    {
        printf("Param LOG_PATH not found in config.xml");
        return -1;
    }

    instance->commonLogLevel		= castStrAsLoglvl(settingManager->getStrValue("COMMON_LOG_LEVEL"));
    instance->syslogLogLevel		= castStrAsLoglvl(settingManager->getStrValue("SYSLOG_LOG_LEVEL"));
    instance->separateLogLevel		= castStrAsLoglvl(settingManager->getStrValue("SEPARATE_LOG_LEVEL"));
    instance->logInSeparateFiles	= (settingManager->getLongValue("LOG_IN_SEPARATE_FILES") == 1);
    instance->isLogInCommonFile		= (settingManager->getLongValue("LOG_IN_COMMON_FILE") == 1);
    instance->logToSyslog			= (settingManager->getLongValue("LOG_TO_SYSLOG") == 1);
    instance->logPath               = settingManager->getStrValue("LOG_PATH");
    instance->recordCount           = (settingManager->getLongValue("RECORDS_COUNT"))?settingManager->getLongValue("RECORDS_COUNT"):1000000;
    instance->syslogIdent           = (settingManager->getStrValue("SYSLOG_IDENT"))? settingManager->getStrValue("SYSLOG_IDENT"):"SCAPv2MD";
    instance->hostName              =(settingManager->getStrValue("HOST_PREFIX"))? settingManager->getStrValue("HOST_PREFIX"):"SCAPv2MD_";

    if(instance->logInSeparateFiles)
    {
        for(int i = 0; i < MAX_LOGLVL; i++)
        {
            if(i < instance->separateLogLevel)
            {
                instance->files[i] = NULL;
            }
            else
            {
                instance->fileCounter[i] = 0;
                instance->recordCounter[i] = 0;

                if(instance->openFS((LogLevel)i) == -1)
                    return -1;
            }
        }
    }
    else
    {
        for(int i = 0; i < MAX_LOGLVL; i++)
        {
            instance->files[i] = NULL;
        }
    }

    if(instance->isLogInCommonFile)
    {
        instance->fileCounter[MAX_LOGLVL] = 0;
        instance->recordCounter[MAX_LOGLVL] = 0;
        if(instance->openFS(MAX_LOGLVL) == -1)
           return -1;
    }
    else
    {
       instance->files[(int)MAX_LOGLVL] = NULL;
    }

	if(instance->logToSyslog)
	{
		instance->syslogFacility = castStrAsFacility((settingManager->getStrValue("SYSLOG_FACILITY"))? settingManager->getStrValue("SYSLOG_FACILITY"):"local4");
		openlog(instance->syslogIdent.c_str(), LOG_NDELAY, instance->syslogFacility);
	}

    instance->queue_->start();
    instance->activate();
    return 0;
}
//----------------------------------------------------------------------------
LogMessage& Logger::getLogMsg(LogLevel lvl)
{
    LogMessage* msg = NULL;
    if(instance)
    {
        if(instance->messagePool->size() > 0 )
        {
            msg = instance->messagePool->get();
            msg->level = lvl;
        }
        else
        {
            //svc не успевает записывать данные на диск/в сислог. текущее сообщение в итоге не будет записано
        }
    }
    return *msg;
}
//----------------------------------------------------------------------------
int Logger::putLogMsg(LogMessage &msg)
{
    if(instance)
    {
        instance->queue_->enqueue(&msg);
        instance->newMsgSemaphore.signal();
        return 0;
    }
    return -1;
}

//----------------------------------------------------------------------------
void Logger::svc(void)
{
    LogMessage *logMsg = NULL;
    while(queue_->isActive())
    {
        newMsgSemaphore.wait();
        logMsg = queue_->dequeue();
        if(logMsg)
        {
            *logMsg << "\n";
            if(isLogInCommonFile && logMsg->level >= commonLogLevel)
            {
                if(checkCounters(MAX_LOGLVL) != -1)
                {
                    fwrite(logMsg->get(), logMsg->getLength(), 1, files[MAX_LOGLVL]);
                    fflush(files[MAX_LOGLVL]);
                }
            }

            if(logInSeparateFiles && logMsg->level >= separateLogLevel)
            {
                if(checkCounters(logMsg->level) != -1)
                {
                    fwrite(logMsg->get(), logMsg->getLength(), 1, files[logMsg->level]);
                    fflush(files[logMsg->level]);
                }
            }

            if(logToSyslog && logMsg->level >= syslogLogLevel)
            {
                syslog(LogLevelToSyslogLevel(logMsg->level), "%s", logMsg->get());
            }

            logMsg->clear();
            messagePool->push(logMsg);
        }
    }
    return ;
}
