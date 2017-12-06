//
// Created by dev on 3/22/17.
//

#ifndef SCAPV2_MEDIATOR_RABBITMQADAPTER_H
#define SCAPV2_MEDIATOR_RABBITMQADAPTER_H

#include <Logger/logger.h>
#include "mqAdapter.h"

#include <SimpleAmqpClient.h>
#include <thread>
#include <AmqpLibraryException.h>

using namespace AmqpClient;

class RabbitMQAdapter: public IMQAdapter
{
public:
    enum RMQAState
    {
        RMQAS_UNDEFINED,
        RMQAS_ACTIVE,
        RMQAS_RECEIVING_STOPPED,
        RMQAS_CREATION_FAILED
    };
private:
    std::string serverIP;
    std::string virtHost;
    std::string sendQueue;
    std::string receiveQueue;
    std::thread* receiveThread;
    IMessageReceiver* handler;
    Channel::ptr_t channel;

    std::string user;
    std::string password;
    RMQAState state;
public:
    RabbitMQAdapter(const char* server, const char* sendQueueName, const char* rcvQueueName, const char* userName, const char* pass, const char* vHost):
            state(RMQAS_UNDEFINED)
    {
        if(!server ||!sendQueueName ||!rcvQueueName ||!userName ||!pass)
        {
            LOG_FATAL("RabbitMQAdapter not created. One of params is absent");
            state = RMQAS_CREATION_FAILED;
            return ;
        }
        sendQueue = sendQueueName;
        receiveQueue = rcvQueueName;
        serverIP = server;
        user = userName;
        password = pass;
        virtHost = vHost;
    }
    virtual ~RabbitMQAdapter()
    {

    }
    virtual void registerHandler(IMessageReceiver* handler)
    {
        this->handler = handler;
    }

    virtual bool start()
    {
        try
        {
            receiveThread = new std::thread(&RabbitMQAdapter::svc, this);
            channel = Channel::Create(serverIP.c_str(), 5672, user.c_str(), password.c_str(), virtHost.c_str());
            channel->DeclareQueue(sendQueue.c_str(), true);
        }
        catch (AmqpClient::AccessRefusedException e)
        {
            LOG_ERROR("RabbitMQAdapter start exception: "<<e.what());
            return false;
        }
        catch (AmqpClient::AmqpLibraryException e)
        {
            LOG_ERROR("RabbitMQAdapter start exception: "<<e.what());
            return false;
        }
        catch (AmqpClient::NotFoundException e)
        {
            LOG_ERROR("RabbitMQAdapter start exception: "<<e.what());
            return false;
        }

        state = RMQAS_ACTIVE;
        LOG_INFO("RabbitMQ Adapter started. Server = " << serverIP.c_str());
        return true;
    }

    void stop()
    {
        LOG_INFO("RabbitMQ Adapter stopped.");
        state = RMQAS_RECEIVING_STOPPED;
    }

    virtual bool send(std::string& text)
    {
        if (state != RMQAS_ACTIVE && state != RMQAS_RECEIVING_STOPPED)
            return false;

        try
        {

            // Create a message with a body larger than a single frame
            BasicMessage::ptr_t message = BasicMessage::Create(text);
            channel->BasicPublish("", sendQueue, message);
            return true;
        }
        catch (AmqpClient::AccessRefusedException e)
        {
            LOG_ERROR("RabbitMQAdapter start exception: "<<e.what());
            return false;
        }
        catch (AmqpClient::AmqpLibraryException e)
        {
            LOG_ERROR("RabbitMQAdapter start exception: "<<e.what());
            return false;
        }
        catch (AmqpClient::NotFoundException e)
        {
            LOG_ERROR("RabbitMQAdapter start exception: "<<e.what());
            return false;
        }
    };

    RMQAState const getState()
    {
        return state;
    }

    void svc()
    {
        Channel::ptr_t channel;
        std::string consumer_tag;
        try
        {
            channel = Channel::Create(serverIP.c_str(), 5672, user.c_str(), password.c_str(), virtHost.c_str(), 4096);
            consumer_tag = channel->BasicConsume(receiveQueue.c_str(), "", true, true, false);
        }
        catch (AmqpClient::AccessRefusedException e)
        {
            LOG_ERROR("RabbitMQAdapter consume start exception: "<<e.what());
            state = RMQAS_CREATION_FAILED;
            return;
        }
        catch (AmqpClient::NotFoundException e)
        {
            LOG_ERROR("RabbitMQAdapter start exception: "<<e.what());
            return;
        }
        catch (AmqpClient::AmqpLibraryException e)
        {
            LOG_ERROR("RabbitMQAdapter start exception: "<<e.what());
            return ;
        };

        while (state != RMQAS_RECEIVING_STOPPED)
        {
            Envelope::ptr_t envelope;
            try
            {
                channel->BasicConsumeMessage(consumer_tag, envelope);
            }
            catch (AmqpClient::AccessRefusedException e)
            {
                LOG_ERROR("RabbitMQAdapter consume exception: "<<e.what());
                return;
            }
            catch (AmqpClient::NotFoundException e)
            {
                LOG_ERROR("RabbitMQAdapter start exception: "<<e.what());
                return;
            }
            catch (AmqpClient::AmqpLibraryException e)
            {
                LOG_ERROR("RabbitMQAdapter start exception: "<<e.what());
                return ;
            } ;
            std::string message = envelope->Message()->Body();
            if (handler)
                handler->handleMQMessage(message);
        }
        LOG_INFO("RabbitMQ Adapter svc thread finished");
    }
};

#endif //SCAPV2_MEDIATOR_RABBITMQADAPTER_H
