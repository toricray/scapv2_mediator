//
// Created by dev on 1/20/17.
//

#ifndef SCAPV2_MEDIATOR_OCSREQUESTRESPONSE_H
#define SCAPV2_MEDIATOR_OCSREQUESTRESPONSE_H

#include "json.hpp"
#include "Logger/logger.h"

using json = nlohmann::json;

enum MsgType
{
    SCAPv2VoiceDD               = 100,
    SCAPv2FaxDD                 = 101,
    SCAPv2VoiceChannelDD        = 102,
    SCAPv2SingleQuote           = 103,
    SCAPv2SingleCharge          = 104,
    SCAPv2SmsDD                 = 105,
    SCAPv2PeriodicQuote         = 106,
    SCAPv2PeriodicCharge        = 107,
    SCAPv2MmsDD                 = 108,
    SCAPv2WiFiDD                = 109,
    SCAPv2DataDD                = 110,
    SCAPv2RefundDD              = 111,
    SCAPv2RefundQuote           = 112,
    SCAPv2RefundCharge          = 113,
    SCAPv2BonusDD               = 114,
    SCAPv2SingleDD              = 115,
    SCAPv2ContentRefundDD       = 116,
    SCAPv2DeferredPaymentDD     = 117,
    SCAPv2ReleaseSingleQuota    = 118,
    SCAPv2ReleasePeriodicQuota  = 119,
    SCAPv2ReleaseRefundQuota    = 120
};

enum RequestType
{
    RT_DIRECT_DEBIT_CHARGE  = 1,
    RT_SESSION_QUOTE        = 2,
    RT_SESSION_CHARGE       = 3,
    RT_REFUND               = 4,
    RT_UNDEFINED            = -1
};

enum ServiceType
{
    SRVT_VOICE                      = 0,
    SRVT_FAX                        = 1,
    SRVT_VOICE_CHANNEL              = 2,
    SRVT_SINGLE_CHARGE              = 3,
    SRVT_SMS                        = 4,
    SRVT_PERIODIC_CHARGE            = 5,
    SRVT_MMS                        = 8,
    SRVT_WIFI                       = 9,
    SRVT_DATA                       = 13,
    SRVT_MONEY_REFUND               = 14,
    SRVT_BONUS                      = 15,
    SRVT_CONTENT_REFUND             = 16,
    SRVT_DEFFERED_PAYMENT_CHARGE    = 17,
    SRVT_CPA                        = 102,
    SRVT_UNDEFINED                  = -1
};

static RequestType  msgTypetoRequestType(MsgType value)
{
    switch (value)
    {
        case SCAPv2VoiceDD:
        case SCAPv2FaxDD:
        case SCAPv2VoiceChannelDD:
        case SCAPv2SmsDD:
        case SCAPv2MmsDD:
        case SCAPv2WiFiDD:
        case SCAPv2DataDD:
        case SCAPv2RefundDD:
        case SCAPv2BonusDD:
        case SCAPv2SingleDD:
        case SCAPv2ContentRefundDD:
        case SCAPv2DeferredPaymentDD:
            return RT_DIRECT_DEBIT_CHARGE;
        case SCAPv2SingleQuote:
        case SCAPv2PeriodicQuote:
        case SCAPv2RefundQuote:
            return RT_SESSION_QUOTE;
        case SCAPv2SingleCharge:
        case SCAPv2PeriodicCharge:
        case SCAPv2RefundCharge:
        case SCAPv2ReleaseSingleQuota:
        case SCAPv2ReleasePeriodicQuota:
        case SCAPv2ReleaseRefundQuota:
            return RT_SESSION_CHARGE;
        default:
            return RT_UNDEFINED;
    }
}

static const char * castOCSRequestTypeAsStr(RequestType requestType)
{
    static const char * STR_DIRECT_DEBIT_CHARGE     = "DIRECT_DEBIT";
    static const char * STR_SESSION_QUOTE           = "SESSION_QUOTE";
    static const char * STR_SESSION_CHARGE          = "SESSION_CHARGE";
    static const char * STR_REFUND                  = "REFUND";
    static const char * STR_UNDEFINED               = "UNDEFINED";

    switch(requestType)
    {
        case RT_DIRECT_DEBIT_CHARGE:    return STR_DIRECT_DEBIT_CHARGE;
        case RT_SESSION_QUOTE:          return STR_SESSION_QUOTE;
        case RT_SESSION_CHARGE:         return STR_SESSION_CHARGE;
        case RT_REFUND:                 return STR_REFUND;
        default:
            return STR_UNDEFINED;
    }
}

struct OCSRequest
{
    long long                   id;
    long long                   sessionId;
    std::string                 msisdn;
    RequestType                 requestType;
    ServiceType                 serviceType;
    int                         units;
    bool                        isIncoming;
    std::string                 sessionIdStr;
    std::map<int, std::string>  integerSPIMap;
    std::map<int, std::string>  utf8StringSPIMap;
    std::map<int, std::string>  octetStringSPIMap;
    std::string                 responseTube;
    std::string                 otherNum;
    std::string                 location;
    std::string                 operation;
    MsgType                     msgTypeId;
};

struct OCSResponse
{
    enum ResultCode
    {
        DRC_SUCCESS                          = 2001,
        DRC_TOO_BUSY                         = 3004,
        DRC_END_USER_SERVICE_DENIED          = 4010,
        DRC_CREDIT_CONTROL_NOT_APPLICABLE    = 4011,
        DRC_CREDIT_LIMIT_REACHED             = 4012,
        DRC_USER_UNKNOWN                     = 5030,
        DRC_RATING_FAILED                    = 5031
    };

    std::string     responseTube;
    std::string     sessionIdStr;
    std::string     operation;
    std::string     errorMessage;
    ServiceType     serviceType;
    long long       sessionId;
    long long       id;
    ResultCode      resultCode;
    int             resultCodeExt;
    MsgType         msgTypeId;
};

class IOCSResponseHandler
{
public:
    virtual void handleOCSResponse(const OCSResponse* rsp)          = 0;
    virtual void handleOCSResponseTimeout(const char* sessionID)    = 0;
    virtual void handleSessionTimeout(const char* sessionID)        = 0;
};

static OCSRequest* parseJsonMessage(const std::string& msg)
{
    json jsonRequest;
    try
    {
        jsonRequest = json::parse(msg);
    }
    catch(std::exception e)
    {
        LOG_ERROR("Exception occurred while parsing JSON. Message: "<<msg.c_str() << ". Exception: "<< e.what());
        return NULL;
    };

    OCSRequest* request = new OCSRequest;

    if (jsonRequest.count("msg_type_id"))
    {
        request->msgTypeId = (MsgType)jsonRequest["msg_type_id"].get<int>();
        request->requestType = msgTypetoRequestType((MsgType)jsonRequest["msg_type_id"].get<int>());
        if (request->requestType == RT_UNDEFINED)
        {
            LOG_ERROR("parseJsonMessage(): msg_type_id is undefined");
            delete request;
            return NULL;
        }
    }
    else
    {
        LOG_ERROR("parseJsonMessage(): Can`t find type!");
        delete request;
        return NULL;
    }

    if (jsonRequest.count("ocs_si"))
    {
        request->serviceType = (ServiceType)jsonRequest["ocs_si"].get<int>();
        if (request->serviceType == SRVT_UNDEFINED)
        {
            LOG_ERROR("parseJsonMessage(): ocs_si is undefined");
            delete request;
            return NULL;
        }
    }
    else
    {
        LOG_ERROR("parseJsonMessage(): ocs_si not found");
        delete request;
        return NULL;
    }

    if (jsonRequest.count("id"))
    {
        request->id = jsonRequest["id"].get<long>();
    }
    else
    {
        LOG_ERROR("parseJsonMessage(): id not found");
        delete request;
        return NULL;
    }

    if (jsonRequest.count("session_id"))
    {
        request->sessionId = jsonRequest["session_id"].get<long>();
    }
    else
    {
        request->sessionId = 0;
    }

    if (jsonRequest.count("operation"))
    {
        request->operation = jsonRequest["operation"].get<std::string>();
    }
    else
    {
        LOG_ERROR("parseJsonMessage(): operation not found");
        delete request;
        return NULL;
    }

    if (jsonRequest.count("tube_resp"))
    {
        request->responseTube = jsonRequest["tube_resp"].get<std::string>();
    }
    else
    {
        LOG_ERROR("parseJsonMessage(): tube_resp not found");
        delete request;
        return NULL;
    }

    request->sessionIdStr = std::to_string(request->serviceType)  + ";" + ((request->sessionId != 0 )?std::to_string(request->sessionId):std::to_string(request->id)) + ";SCAPv2";

    if (jsonRequest.count("msisdn"))
    {
        request->msisdn = jsonRequest["msisdn"].get<std::string>();
    }
    else
    {
        if(request->requestType != RT_SESSION_CHARGE)
        {
            LOG_ERROR("parseJsonMessage(): msisdn not found");
            delete request;
            return NULL;
        }
    }

    if (jsonRequest.count("units"))
        request->units = jsonRequest["units"].get<int>();
    else
    {
        request->units = 0;
    }

    if (jsonRequest.count("units") && request->serviceType == SRVT_MONEY_REFUND && request->requestType == RT_SESSION_QUOTE)
    {
        request->units = 1;
        request->integerSPIMap[2] = std::to_string(jsonRequest["units"].get<int>());
    }

    if (request->serviceType == SRVT_SINGLE_CHARGE || request->serviceType == SRVT_PERIODIC_CHARGE)
    {
        if(request->msgTypeId == SCAPv2ReleaseSingleQuota || request->msgTypeId == SCAPv2ReleasePeriodicQuota || request->msgTypeId == SCAPv2ReleaseRefundQuota)
            request->units = 0;
        else
            request->units = 1;
    }

    if (jsonRequest.count("other_num"))
    {
        request->otherNum = jsonRequest["other_num"].get<std::string>();
    }
    if (jsonRequest.count("traffic_case"))
    {
        request->isIncoming = jsonRequest["traffic_case"].get<std::string>() == "MT";
    }
    if (jsonRequest.count("location"))
    {
        request->location = jsonRequest["location"].get<std::string>();
    }
    if (jsonRequest.count("event_id") && request->serviceType == SRVT_DEFFERED_PAYMENT_CHARGE)
        request->integerSPIMap[0] = std::to_string(jsonRequest["event_id"].get<int>());

    if (jsonRequest.count("service_id") && (request->serviceType == SRVT_SINGLE_CHARGE || request->serviceType == SRVT_PERIODIC_CHARGE || request->serviceType == SRVT_BONUS))
        request->integerSPIMap[0] = std::to_string(jsonRequest["service_id"].get<int>());

    if (jsonRequest.count("tariff_id") && request->serviceType == SRVT_WIFI)
        request->integerSPIMap[0] = std::to_string(jsonRequest["tariff_id"].get<int>());

    if (jsonRequest.count("apn") && request->serviceType == SRVT_DATA)
        request->utf8StringSPIMap[0] = jsonRequest["apn"].get<std::string>();

    if (jsonRequest.count("mccmnc") && (request->serviceType == SRVT_VOICE || request->serviceType == SRVT_FAX
                                                                              || request->serviceType == SRVT_VOICE_CHANNEL
                                        || request->serviceType == SRVT_SMS || request->serviceType == SRVT_DATA))
    {
        request->integerSPIMap[0] = std::to_string(jsonRequest["mccmnc"].get<int>());
    }

    if (jsonRequest.count("sgsn") && request->serviceType == SRVT_DATA)
        request->octetStringSPIMap[0] = jsonRequest["sgsn"].get<std::string>();

    if (jsonRequest.count("dattim"))
        request->utf8StringSPIMap[1] = jsonRequest["dattim"].get<std::string>();

    if (jsonRequest.count("rg") && request->serviceType == SRVT_DATA)
        request->integerSPIMap[1] = std::to_string(jsonRequest["rg"].get<int>());

    if (jsonRequest.count("overdraft_flag") && (request->serviceType == SRVT_SINGLE_CHARGE || request->serviceType == SRVT_PERIODIC_CHARGE|| request->serviceType == SRVT_MONEY_REFUND))
        request->integerSPIMap[1] = std::to_string(jsonRequest["overdraft_flag"].get<int>());


    if (jsonRequest.count("rat_type") && request->serviceType == SRVT_DATA)
        request->integerSPIMap[2] = std::to_string(jsonRequest["rat_type"].get<int>());

    if(request->serviceType == SRVT_PERIODIC_CHARGE)
    {
        if (jsonRequest.count("periodic_charge_part_size"))
            request->integerSPIMap[2] = std::to_string(jsonRequest["periodic_charge_part_size"].get<int>());
        else
            request->integerSPIMap[2] = std::to_string(100);
    }



    if (request->serviceType == SRVT_SINGLE_CHARGE || request->serviceType == SRVT_PERIODIC_CHARGE)
    {
        if (jsonRequest.count("discount_percent1"))
            request->integerSPIMap[3] = std::to_string(jsonRequest["discount_percent1"].get<int>());
        else
            request->integerSPIMap[3] = std::to_string(100);

        if (jsonRequest.count("discount_percent2"))
            request->integerSPIMap[4] = std::to_string(jsonRequest["discount_percent2"].get<int>());
        else
            request->integerSPIMap[4] = std::to_string(100);

        if (jsonRequest.count("discount_percent3"))
            request->integerSPIMap[5] = std::to_string(jsonRequest["discount_percent3"].get<int>());
        else
            request->integerSPIMap[5] = std::to_string(100);

        if (jsonRequest.count("discount_percent4"))
            request->integerSPIMap[6] = std::to_string(jsonRequest["discount_percent4"].get<int>());
        else
            request->integerSPIMap[6] = std::to_string(100);

        if (jsonRequest.count("discount_percent5"))
            request->integerSPIMap[7] = std::to_string(jsonRequest["discount_percent5"].get<int>());
        else
            request->integerSPIMap[7] = std::to_string(100);

        if (jsonRequest.count("discount_descr1"))
            request->utf8StringSPIMap[3] = jsonRequest["discount_descr1"].get<std::string>();
        if (jsonRequest.count("discount_descr2"))
            request->utf8StringSPIMap[4] = jsonRequest["discount_descr2"].get<std::string>();
        if (jsonRequest.count("discount_descr3"))
            request->utf8StringSPIMap[5] = jsonRequest["discount_descr3"].get<std::string>();
        if (jsonRequest.count("discount_descr4"))
            request->utf8StringSPIMap[6] = jsonRequest["discount_descr4"].get<std::string>();
        if (jsonRequest.count("discount_descr5"))
            request->utf8StringSPIMap[7] = jsonRequest["discount_descr5"].get<std::string>();
    }
    return request;
}

bool createJsonResponseMessage(const OCSResponse* response, std::string& result)
{
    json responseJson;
    responseJson["id"]= response->id;
    responseJson["session_id"] = response->sessionId;
    responseJson["tube_resp"] = response->responseTube;
    responseJson["result_code"] = response->resultCode;
    if (response->resultCodeExt > 0)
        responseJson["result_code_extended"] = response->resultCodeExt;
    responseJson["operation"] = response->operation;
    responseJson["msg_type_id"] = response->msgTypeId;

    if(response->errorMessage.size() > 0)
        responseJson["error_msg"] = response->errorMessage;

    if (response->resultCode == OCSResponse::DRC_SUCCESS)
        responseJson["result"] = 0;
    else
        responseJson["result"] = response->resultCode;

    result = responseJson.dump();
    return  true;
}
#endif //SCAPV2_MEDIATOR_OCSREQUESTRESPONSE_H
