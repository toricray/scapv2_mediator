#ifndef __SE_TYPE_H_
#define __SE_TYPE_H_


#include <cstddef>
#include <string>

//////////////////////////////////////////////////////////////////////////////////////////////
/*
 * Diameter Definitions and Types
 */
namespace diam {

// Service Type
enum ServiceType {
    ST_UNKNOWN  = 0,
    ST_GPRS     = 1,
    ST_VOICE    = 2,
    ST_SMS      = 3,
    ST_MMS      = 4,

};

// CC-Request-Type
enum RequestType {
    RT_INIT         = 1,
    RT_UPDATE       = 2,
    RT_TERMINATE    = 3,
    RT_EVENT        = 4
};

// Diameter packet command flags
const uint8_t PACKET_CMD_FLAG_REQUEST     = 0x80;  // The "R" (Request) bit. If set, the message is a request. If cleared, the message is an answer.
const uint8_t PACKET_CMD_FLAG_PROXIABLE   = 0x40;  // The "P" (Proxiable) bit. If set, the message MAY be proxied, relayed or redirected. If cleared, the message MUST be locally processed.
const uint8_t PACKET_CMD_FLAG_ERROR       = 0x20;       // The "E" (Error) bit. If set, the message contains a protocol error, and the message will not conform to the ABNF described for this command. Messages with the "E" bit set are commonly referred to as error messages. This bit MUST NOT be set in request messages.
const uint8_t PACKET_CMD_FLAG_TRANSM      = 0x10;  // The "T" (Potentially re-transmitted message) bit. This flag is set after a link fail over procedure, to aid the removal of duplicate requests. It is set when resending requests not yet acknowledged as an indication of a possible duplicate due to a link failure.

// Diameter packet command codes
const int PACKET_CMD_CODE_CREDIT_CONTROL = 272;

const int AUTH_APP_ID_CREDIT_CONTROL     =   4;

const int DIAM_AVP_CODE_MSCC                   =  456;
const int DIAM_AVP_CODE_REQUESTED_SERVICE_UNIT =  437;
const int DIAM_AVP_CODE_USED_SERVICE_UNIT      =  446;
const int DIAM_AVP_CODE_CC_TIME                =  420;
const int DIAM_AVP_CODE_SERVICE_SPECIFIC_INFO  = 1249;
const int DIAM_AVP_CODE_SERVICE_SPECIFIC_TYPE  = 1257;
const int DIAM_AVP_CODE_SERVICE_SPECIFIC_DATA  =  863;

// Diameter Result Codes
enum ResultCode {
    DIAMETER_SUCCESS                = 2001,
    DIAMETER_TOO_BUSY               = 3004,
    DIAMETER_CREDIT_LIMIT_REACHED   = 4012,
    DIAMETER_MISSING_AVP            = 5005,
    DIAMETER_RATING_FAILED          = 5031,
};


namespace ServiceContextId
{
const char * DIAM_CTX_VOICE = "32260@3gpp.org";
const char * DIAM_CTX_SMS   = "32274@3gpp.org";
const char * DIAM_CTX_GPRS  = "32251@3gpp.org";
const char * DIAM_CTX_MMS   = "32270@3gpp.org";
} // namespace ServiceContextId


static const char* getServiceContextId(unsigned serviceType)
{
    if( serviceType == ST_VOICE )
        return ServiceContextId::DIAM_CTX_VOICE;
    else if ( serviceType == ST_SMS )
        return ServiceContextId::DIAM_CTX_SMS;
    else if ( serviceType == ST_GPRS )
        return ServiceContextId::DIAM_CTX_GPRS;
    else if ( serviceType == ST_MMS )
        return ServiceContextId::DIAM_CTX_MMS;
    else
        return "";
}

static int getServiceType(const char* serviceContextId)
{
    if ( strcmp(serviceContextId, ServiceContextId::DIAM_CTX_VOICE) == 0 )
        return ST_VOICE;
    else if ( strcmp(serviceContextId, ServiceContextId::DIAM_CTX_SMS) == 0 )
        return ST_SMS;
    else if ( strcmp(serviceContextId, ServiceContextId::DIAM_CTX_GPRS) == 0 )
        return ST_GPRS;
    else
        return -1;
}

static std::string resultCodeToString(int resultCode)
{
    std::string result;
    switch ( resultCode )
    {
    case DIAMETER_SUCCESS:
        result = "SUCCESS";
        break;

    case DIAMETER_RATING_FAILED:
        result = "RATING FAILED";
        break;

    case DIAMETER_CREDIT_LIMIT_REACHED:
        result = "CREDIT LIMIT REACHED";
        break;

    case DIAMETER_TOO_BUSY:
        result = "DIAMETER TOO BUSY";
        break;

    case DIAMETER_MISSING_AVP:
        result = "MISSING AVP";
        break;
    }

    return result;
}

} // namespace diam
//////////////////////////////////////////////////////////////////////////////////////////////

#endif // __SE_TYPE_H_
