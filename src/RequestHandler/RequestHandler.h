//
// Created by dev on 1/19/17.
//

#ifndef SCAPV2_MEDIATOR_REQUESTHANDLER_H
#define SCAPV2_MEDIATOR_REQUESTHANDLER_H

#include <MQAdapter/mqAdapter.h>
#include "Settings/SettingManager.h"

#include "Diameter/Interface/CCRCCAMessage.h"
#include "DiameterAdapter/DiameterAdapter.h"

class RequestHandler: public IMessageReceiver, public IOCSResponseHandler
{
    DiameterAdapter*    diamAdapter;
    IMQAdapter*         mqAdapter;
public:
    ~RequestHandler(){}

    RequestHandler(DiameterAdapter* diamAdapter, IMQAdapter* activemqAdapter): diamAdapter(diamAdapter), mqAdapter(activemqAdapter)
    {
        if(diamAdapter)
            diamAdapter->registerOCSResponseHandler(this);
    }

    bool handleMQMessage(const std::string& text)
    {
        LOG_INFO("RequestHandler: New MQ message:" << text.c_str());

        OCSRequest* request = NULL;
        try
        {
            request = parseJsonMessage(text);
        }
        catch(std::domain_error e)
        {
            LOG_ERROR("Exception occurred in parseJsonMessage. Message: "<<text.c_str() << ". Exception: "<< e.what());
            return false;
        }
        catch(std::exception e)
        {
            LOG_ERROR("Exception occurred in parseJsonMessage. Message: "<<text.c_str() << ". Exception: "<< e.what());
            return false;
        };

        if (!request)
        {
            LOG_ERROR("handleMQMessage error: request is NULL");
            return false;
        }
        std::string sessionID = request->sessionIdStr;
        RequestType rt = request->requestType;

        if (diamAdapter->sendCCR(request)) // с этого момента доступ к request не безопасен
        {
            LOG_INFO("Send CCR: " << castOCSRequestTypeAsStr(rt) << " CCR; Session ID = " << sessionID.c_str());
        }
        else
        {
            LOG_ERROR("diamAdapter->sendCCR ERROR; Session ID = " << sessionID.c_str());
            return false;
        }

        return true;
    }

    virtual void handleOCSResponse(const OCSResponse* rsp)
    {
        if (!rsp)
        {
            LOG_INFO("handleOCSResponse error: rsp is NULL");
            return;
        }
        LOG_INFO("RequestHandler: OCS Response received: " << rsp->sessionIdStr.c_str());

        std::string responseStr;

        if(!createJsonResponseMessage(rsp, responseStr))
        {
            delete rsp;
            LOG_INFO("createXMLResponseMessage return error");
            return;
        }
        LOG_INFO("RequestHandler: Send response " << responseStr.c_str());
        mqAdapter->send(responseStr);
        delete rsp;
    }

    virtual void handleOCSResponseTimeout(const char* sessionID)
    {
        LOG_ERROR("RequestHandler: Response Timeout: " << sessionID);
    }
    virtual void handleSessionTimeout(const char* sessionID)
    {
        LOG_ERROR("RequestHandler: Session Timeout: " << sessionID);
    }

};

#endif //SCAPV2_MEDIATOR_REQUESTHANDLER_H
