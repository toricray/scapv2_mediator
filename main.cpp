#include <iostream>

#include "RequestHandler/RequestHandler.h"
#include <MQAdapter/rabbitmqAdapter.h>
#include <signal.h>

sig_atomic_t terminated = 0;

void signalHandler(int)
{
    terminated = 1;
    printf("SIGINT handling...\n");
}

int main(int argc, char* argv[])
{
    printf("%s",currentDateTimeStr());
    SettingManager* sm = new SettingManager();
    if (sm->loadSettingsFromFile("config.xml") < 0)
    {
        printf("Error! config.xml not found or invalid");
        return  -1;
    }

    if (Logger::init(sm) != 0)
    {
        printf("Error! Can`t initialize logger");
        return -1;
    }

    DiameterAdapter* diamAdapter = new DiameterAdapter(sm->getStrValue("DIAM_MAIN_CONNECTION_STRING"),
                                                      sm->getStrValue("DIAM_RESERVE_CONNECTION_STRING"),
                                                      sm->getStrValue("DIAM_DESTINATION_HOST_MAIN"),
                                                      sm->getStrValue("DIAM_DESTINATION_REALM_MAIN"),
                                                      sm->getStrValue("DIAM_DESTINATION_HOST_RESERVE"),
                                                      sm->getStrValue("DIAM_DESTINATION_REALM_RESERVE"),
                                                      sm->getStrValue("DIAM_ORIGIN_HOST"),
                                                      sm->getStrValue("DIAM_ORIGIN_REALM"),
                                                      sm->getLongValue("DIAM_SERVICE_IDENTIFIER"),
                                                      sm->getLongValue("DIAM_IS_PROXY") == 1);

    if(diamAdapter->getState() == DiameterAdapter::DAS_CREATION_FAILED)
    {
        LOG_ERROR("DiameterAdapter creation failed");
        return -1;
    }

    RabbitMQAdapter* mqAdapter = new RabbitMQAdapter(sm->getStrValue("MQ_SERVER"),
                                                     sm->getStrValue("SEND_QUEUE"),
                                                     sm->getStrValue("RECIVE_QUEUE"),
                                                     sm->getStrValue("MQ_USER"),
                                                     sm->getStrValue("MQ_PASSWORD"),
                                                     sm->getStrValue("MQ_VIRT_HOST"));

    if(mqAdapter->getState() == RabbitMQAdapter::RMQAS_CREATION_FAILED)
    {
        LOG_ERROR("RabbitMQAdapter creation failed");
        return -1;
    }

    RequestHandler* handler = new RequestHandler(diamAdapter, mqAdapter);

    mqAdapter->registerHandler(handler);
    if (!diamAdapter->Connect())
    {
        LOG_ERROR("Diameter Connection failed");
        return -1;

    }

    if (!mqAdapter->start())
    {
        LOG_ERROR("RabbitMQAdapter start failed");
        return -1;

    }

    LOG_INFO("Program started");

    signal(SIGINT, signalHandler);

    while(!terminated)
    {
        sleep(1);
        diamAdapter->checkConnection();
    }

    mqAdapter->stop();
    sleep(2);
    LOG_INFO("Program finished");

    delete sm;
    delete mqAdapter;
    delete diamAdapter;

    Logger::release();

    return 0;
}