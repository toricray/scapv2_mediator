//
// Created by dev on 3/22/17.
//

#ifndef SCAPV2_MEDIATOR_MQADAPTER_H
#define SCAPV2_MEDIATOR_MQADAPTER_H

class IMessageReceiver
{
public:
    virtual bool handleMQMessage(const std::string& text) = 0;
};

class IMessageSender
{
public:
    virtual bool send(std::string& text) = 0;
};

class IMQAdapter: public IMessageSender
{
public:
    virtual ~IMQAdapter(){};
    virtual void registerHandler(IMessageReceiver* handler) = 0;
    virtual bool start() = 0;
};

#endif //SCAPV2_MEDIATOR_MQADAPTER_H
