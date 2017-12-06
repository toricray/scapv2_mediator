#ifndef __DIAMETER_ADAPTER_H_
#define __DIAMETER_ADAPTER_H_

#include <sstream>

#include "Diameter/Source_/CCR/CCRMessageImpl.h"
#include "Diameter/Interface/DiamcoreInterfaces.h"
#include "Diameter/Source_/DiameterClient.h"

#include "DiameterSettings.h"
#include "DiameterAdapter/DiameterTypes.h"

#include "RequestHandler/OCSRequestResponse.h"

class ICCAHandler
{
public:
    void handleCCA(CoreDiam::CCCAMessage* message);
};


enum DiameterClient
{
    DC_UNDEFINED,
    DC_MAIN,
    DC_RESERVE
};

struct CommonAVPs
{
    std::string originHost;
    std::string originRealm;
    std::string destinationHost;
    std::string destinationRealm;
    long serviceIdentifier;
};

class DiameterAdapter : public CoreDiam::IDiameterEvents, CoreDiam::ICCAEvents
{
public:
    enum DAState
    {
        DAS_UNDEFINED,
        DAS_ACTIVE,
        DAS_ACTIVE_RESERVE,
        DAS_INACTIVE,
        DAS_CREATION_FAILED
    };
private:
    CommonAVPs commonAVPs;
    CoreDiam::IDiameterClient*  _diamClient;
    IOCSResponseHandler*        ocsResponseHandler;
    DiameterSettings*           _diamSettings;
    std::string                 _connectionString;
    std::string                 _connectionStringReserve;

    std::string                 _destinationHostMain;
    std::string                 _destinationRealmMain;
    std::string                 _destinationHostReserve;
    std::string                 _destinationRealmReserve;

    DAState                     state;
    bool                        _tooBusy;
    uint64_t                    _successCounter;
    uint64_t                    _errorCounter;
    time_t                      reserveConnectionTime;
    Event                       connectReadyEvent;
    int                         watchdogPeriod;
    time_t                      lastWatchdogTime;
    std::map<std::string ,OCSRequest*> requestHoldingMap;
    std::mutex                          rhmMutex;
public:
    DiameterAdapter(const char* connectStringMain, const char* connectStringReserve, const char* destinationHostMain, const char* destinationRealmMain, const char* destinationHostReserve, const char* destinationRealmReserve, const char* originHost, const char* originRealm, long serviceIdentifier, bool isProxy):
            _diamClient(NULL), state(DAS_UNDEFINED),
        _tooBusy(false), _successCounter(0), _errorCounter(0), reserveConnectionTime(0), watchdogPeriod(10), lastWatchdogTime(0)
    {
        if(!connectStringMain ||!connectStringReserve ||!destinationHostMain ||!destinationRealmMain || !destinationHostReserve ||!destinationRealmReserve || !originHost ||!originRealm)
        {
            LOG_FATAL("DiameterAdapter not created. One of params is absent");
            state = DAS_CREATION_FAILED;
            return ;
        }
        _connectionString = connectStringMain;
        _connectionStringReserve = connectStringReserve;

        _destinationHostMain = destinationHostMain;
        _destinationRealmMain = destinationRealmMain;
        _destinationHostReserve = destinationHostReserve;
        _destinationRealmReserve = destinationRealmReserve;

		_diamSettings = new DiameterSettings();
		_diamClient = new CoreDiam::CDiameterClient(this, this, _diamSettings, 10000, isProxy);

        commonAVPs.originHost = originHost;
        commonAVPs.originRealm = originRealm;
        commonAVPs.serviceIdentifier = serviceIdentifier;
    }

    ~DiameterAdapter()
    {
        _diamClient->Stop();
        _diamClient->Release();
		delete _diamSettings;

        LOG_INFO("DiameterAdapter destroyed");
    }

    DAState const getState()
    {
        return state;
    }

    void checkConnection()
    {
        if (state == DAS_ACTIVE) //Всё хорошо, приложение подключено к основному Diameter-серверу
        {
            if(time(0) > lastWatchdogTime + watchdogPeriod)
            {
                _diamClient->SendDWR();
                lastWatchdogTime = time(0);
            }
            return;
        }

        if (state == DAS_ACTIVE_RESERVE)//Приложение подключено к резервному серверу
        {
            if(time(0) < reserveConnectionTime + 600) //, но пытаться переподключиться к основному ещё не пора
            {
                if(time(0) > lastWatchdogTime + watchdogPeriod)
                {
                    _diamClient->SendDWR();
                    lastWatchdogTime = time(0);
                }
                return;
            }
            else
            {
                LOG_INFO("Diameter client trying to reconnect to main server...");
                Reconnect();
            }
        }

        if (state != DAS_ACTIVE && state != DAS_ACTIVE_RESERVE)
        {
            Reconnect();
            sleep(5);
        }
    }

    bool Reconnect()
    {
        _diamClient->Stop();
        sleep(1);
        return Connect();
    }

    bool Connect()
    {
        if ( _diamClient->Connect(_connectionString.c_str(), commonAVPs.originHost.c_str(), commonAVPs.originRealm.c_str(), _destinationRealmMain.c_str()))
        {
            commonAVPs.destinationHost = _destinationHostMain;
            commonAVPs.destinationRealm = _destinationRealmMain;
            LOG_INFO("Diameter client connected to " << _connectionString.c_str());
            state = DAS_ACTIVE;
        }
        else
        {
            LOG_WARN("Diameter client connection warning. Unable to connect to "<<_connectionString.c_str() <<" Trying to connect to reserve server "<< _connectionStringReserve.c_str());
            if ( _diamClient->Connect(_connectionStringReserve.c_str(), commonAVPs.originHost.c_str(), commonAVPs.originRealm.c_str(), _destinationRealmReserve.c_str()))
            {
                commonAVPs.destinationHost = _destinationHostReserve;
                commonAVPs.destinationRealm = _destinationRealmReserve;

                reserveConnectionTime = time(0);
                state = DAS_ACTIVE_RESERVE;
                LOG_INFO("Diameter client connected to " << _connectionStringReserve.c_str());
            }
            else
            {
                LOG_ERROR("Both Diameter servers are unavailable");
                state = DAS_INACTIVE;
                return  false;
            }
        }
        connectReadyEvent.wait();
        return true;
    }

    void OnReadyConnection(int, int clientId )
    {
        LOG_INFO("Diameter client connection ready");
        connectReadyEvent.signal();
    }

    void OnBreakConnection(int , int clientId,  CoreDiam::DiamDisconnectionReason reason)
    {
        state = DAS_INACTIVE;
        LOG_INFO("Diameter Adapter connection closed. Reason: " << reason);
    }

	void OnReceive(CoreDiam::IDiameterMessage* diameterMessage, int _clientId)
    {
        CoreDiam::IDiameterHeader* header = diameterMessage->GetHeader();
        if ( header->GetCommandCode() == diam::PACKET_CMD_CODE_CREDIT_CONTROL
             && header->GetApplicationID() == diam::AUTH_APP_ID_CREDIT_CONTROL
             && !(header->GetCommandFlags() & diam::PACKET_CMD_FLAG_REQUEST) )
        {
			OnReceiveCCA(static_cast<CoreDiam::CCCAMessage*>(diameterMessage), _clientId);
        }
    }

	void OnResponseTimeout(const char* sessionID, int /*_clientId*/)
    {
        rhmMutex.lock();
        auto iter = requestHoldingMap.find(sessionID);
        if (iter == requestHoldingMap.end())
        {
            rhmMutex.unlock();
            //LOG_WARN("OnResponseTimeout warning. Can not find session = "<<sessionID);
            return;
        }
        requestHoldingMap.erase(iter);
        rhmMutex.unlock();
        ocsResponseHandler->handleOCSResponseTimeout(sessionID);
        return;
    }

	void OnSessionTimeout(const char* sessionID, int /*_clientId*/)
    {
        rhmMutex.lock();
        auto iter = requestHoldingMap.find(sessionID);
        if (iter == requestHoldingMap.end())
        {
            rhmMutex.unlock();
            LOG_WARN("OnResponseTimeout warning. Can not find session = "<<sessionID);
            return;
        }
        requestHoldingMap.erase(iter);
        rhmMutex.unlock();
        ocsResponseHandler->handleSessionTimeout( sessionID);
        return;
    }

	void OnReceiveCCA(CoreDiam::CCCAMessage* ccaMessage, int)
    {
        OCSResponse* response = new OCSResponse();
        response->sessionIdStr = std::string(ccaMessage->GetSessionId());
        response->resultCode = (OCSResponse::ResultCode)ccaMessage->GetResultCode();
        response->resultCodeExt = ccaMessage->GetResultCodeExt();
        response->errorMessage = std::string(ccaMessage->GetErrorMessage());
        rhmMutex.lock();
        auto iter = requestHoldingMap.find(response->sessionIdStr);
        if (iter == requestHoldingMap.end())
        {
            rhmMutex.unlock();
            LOG_ERROR("OnReceiveCCA Error. Can not find session = "<<response->sessionIdStr.c_str());
            delete response;
            return;
        }

        OCSRequest* request = iter->second;
        response->responseTube = request->responseTube;
        response->serviceType = request->serviceType;
        response->sessionId = request->sessionId;
        response->id = request->id;
        response->msgTypeId = request->msgTypeId;
        response->operation = request->operation;
        requestHoldingMap.erase(iter);
        rhmMutex.unlock();
        delete request;
        ocsResponseHandler->handleOCSResponse(response);
    }

    bool IsReady() const { return state == DAS_ACTIVE || state == DAS_ACTIVE_RESERVE; }

    bool IsBusy() const { return _tooBusy; }

    bool sendCCR(OCSRequest* request)
    {
        if (state != DAS_ACTIVE && state != DAS_ACTIVE_RESERVE)
        {
            LOG_ERROR("Connection not ready, session rejected. SessionId: " << request->sessionIdStr.c_str());
            return  false;
        }

        CoreDiam::CCCRMessage* ccrMsg = new CoreDiam::CCCRMessageImpl();

        ccrMsg->SetSessionId(request->sessionIdStr.c_str());
        ccrMsg->SetOriginHost(commonAVPs.originHost.c_str());
        ccrMsg->SetOriginRealm(commonAVPs.originRealm.c_str());
        //ccrMsg->SetDestinationHost(commonAVPs.destinationHost.c_str());
        ccrMsg->SetDestinationRealm(commonAVPs.destinationRealm.c_str());

        ccrMsg->SetServiceIdentifier(request->serviceType);

        ccrMsg->SetAuthApplicationId(4);
        ccrMsg->SetServiceContextId("SCAP_V.2.0@ericsson.com");

        ccrMsg->SetTimezone(5);

        ccrMsg->SetEventTimestamp(time(0) + 2208988800UL);
        CoreDiam::CSubscriptionId* subscriptionId = ccrMsg->CreateSubscriptionId(0);
        if(request->serviceType == SRVT_WIFI)
            subscriptionId->SetSubscriptionIdType(CoreDiam::END_USER_PRIVATE); //нетлогин типа
        else
            subscriptionId->SetSubscriptionIdType(CoreDiam::END_USER_E164);
            subscriptionId->SetSubscriptionIdData((unsigned char*)request->msisdn.c_str(), request->msisdn.length());

        int spiCount = 0;
        for (auto it = request->integerSPIMap.begin(); it!=request->integerSPIMap.end(); ++it)
        {
            CoreDiam:: CServiceParameterInfo* spi = ccrMsg->CreateSPI(spiCount++);
            spi->SetServiceParameterInfoValue(it->first, it->second.c_str());
        }
        for (auto it = request->utf8StringSPIMap.begin(); it!=request->utf8StringSPIMap.end(); ++it)
        {
            CoreDiam:: CServiceParameterInfo* spi = ccrMsg->CreateSPI(spiCount++);
            spi->SetServiceParameterInfoValue(200 + it->first, it->second.c_str());
        }
        for (auto it = request->octetStringSPIMap.begin(); it!=request->octetStringSPIMap.end(); ++it)
        {
            CoreDiam:: CServiceParameterInfo* spi = ccrMsg->CreateSPI(spiCount++);
            spi->SetServiceParameterInfoValue(400 + it->first, it->second.c_str());
        }

        CoreDiam::CUnits* units = request->requestType == RT_SESSION_CHARGE?ccrMsg->CreateUsedServiceUnits():ccrMsg->CreateRequestedServiceUnits();

        if (request->units > 0 || request->requestType == RT_SESSION_CHARGE)
        {
            if(request->serviceType == SRVT_MMS || request->serviceType == SRVT_WIFI || request->serviceType == SRVT_DATA )
                units->SetTotalOctets(request->units);
            else if(request->serviceType == SRVT_VOICE || request->serviceType == SRVT_FAX || request->serviceType == SRVT_VOICE_CHANNEL )
                units->SetTime(request->units);
            else
                units->SetServiceSpecificUnits(request->units);
        }

        if(request->serviceType == SRVT_VOICE || request->serviceType == SRVT_FAX || request->serviceType == SRVT_VOICE_CHANNEL )
            ccrMsg->SetTrafficCase(request->isIncoming?CoreDiam::TC_TERMINATING:CoreDiam::TC_ORIGINATING);

        if (request->requestType == RT_DIRECT_DEBIT_CHARGE)
        {
            ccrMsg->SetRequestedAction(0);
            ccrMsg->SetRequestType(CoreDiam::EVENT);
            ccrMsg->SetRequestNumber(0);
        }
        else if (request->requestType == RT_REFUND)
        {
            ccrMsg->SetRequestedAction(1);
            ccrMsg->SetRequestType(CoreDiam::EVENT);
            ccrMsg->SetRequestNumber(0);
        }
        else if (request->requestType == RT_SESSION_QUOTE)
        {
            ccrMsg->SetRequestType(CoreDiam::INITIAL);
            ccrMsg->SetRequestNumber(0);
        }
        else if (request->requestType == RT_SESSION_CHARGE)
        {
            ccrMsg->SetRequestType(CoreDiam::TERMINATION);
            ccrMsg->SetRequestNumber(1);
        }

        if(!request->location.empty())
            ccrMsg->SetSubscriptionIdLocation(request->location.c_str());

        if(!request->otherNum.empty())
        {
            CoreDiam::COtherPartyId* otherPartyId = ccrMsg->CreateOtherPartyId();
            if (otherPartyId)
            {
                otherPartyId->SetOtherPartyIdType(0);
                otherPartyId->SetOtherPartyIdNature(1);
                otherPartyId->SetOtherPartyIdData((unsigned char*)request->otherNum.c_str(),request->otherNum.size());
            }
            else
            {
                LOG_ERROR("otherPartyId creation error");
            }

        }
        rhmMutex.lock();
        requestHoldingMap[request->sessionIdStr.c_str()] = request;
        rhmMutex.unlock();
        CoreDiam::SendResult result = _diamClient->Send(ccrMsg);

        ccrMsg = NULL;

        switch ( result )
        {
        case CoreDiam::UNKNOWN_CONNECTION:
            LOG_ERROR("Diameter: unknown connection! SessionId: " << request->sessionIdStr.c_str());
            break;

        case CoreDiam::CODER_ERROR:
            LOG_ERROR("Diameter: coder error! SessionId: " << request->sessionIdStr.c_str());
            break;

        case CoreDiam::SEND_ERROR:
            LOG_ERROR("Diameter: sending failed!! SessionId: " << request->sessionIdStr.c_str());
            break;

        case CoreDiam::TRANS_ERROR:
            LOG_ERROR("Diameter: transport error! SessionId: " << request->sessionIdStr.c_str());
            break;

        case CoreDiam::QUEUE_OVERFLOW:
            LOG_ERROR("Diameter: send queue overflow! SessionId: " << request->sessionIdStr.c_str());
            break;

        default:
            break;
        }

        if ( result != CoreDiam::OK && result != CoreDiam::PUT_TO_QUEUE )
        {
            LOG_ERROR("Diameter client: error sending CCR for session " << request->sessionIdStr.c_str());
            return false;
        }
        return true;
    }

    void registerOCSResponseHandler(IOCSResponseHandler* handler)
    {
        ocsResponseHandler = handler;
    }

	void OnOverload(int )
    {
        LOG_DEBUG("Diameter Queue overloaded!");
    }

	void OnEmptyQueue(int )
    {
        LOG_DEBUG("Diameter Queue is empty!");
    }

};

#endif // __DIAMETER_ADAPTER_H_
