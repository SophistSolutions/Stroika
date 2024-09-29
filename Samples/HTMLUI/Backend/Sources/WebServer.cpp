/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include <iostream>

#include "Stroika/Foundation/Characters/FloatConversion.h"
#include "Stroika/Foundation/Characters/String2Int.h"
#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/Common/Property.h"
#include "Stroika/Foundation/DataExchange/InternetMediaTypeRegistry.h"
#include "Stroika/Foundation/Execution/IntervalTimer.h"
#include "Stroika/Foundation/Execution/Logger.h"
#include "Stroika/Foundation/IO/Network/HTTP/Exception.h"
#include "Stroika/Foundation/IO/Network/HTTP/Headers.h"
#include "Stroika/Foundation/IO/Network/HTTP/Methods.h"
#include "Stroika/Foundation/Streams/TextReader.h"

#include "Stroika/Frameworks/WebServer/ConnectionManager.h"
#include "Stroika/Frameworks/WebServer/DefaultFaultInterceptor.h"
#include "Stroika/Frameworks/WebServer/Router.h"
#include "Stroika/Frameworks/WebService/Server/Basic.h"
#include "Stroika/Frameworks/WebService/Server/VariantValue.h"

#include "AppConfiguration.h"
#include "AppVersion.h"
#include "OperationalStatistics.h"
#include "WSImpl.h"

#include "WebServer.h"

using namespace std;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::DataExchange;
using namespace Stroika::Foundation::IO::Network;
using namespace Stroika::Foundation::Execution;

using namespace Stroika::Frameworks::WebServer;
using namespace Stroika::Frameworks::WebService;
using namespace Stroika::Frameworks::WebService::Server;
using namespace Stroika::Frameworks::WebService::Server::VariantValue;

using Memory::BLOB;
using Stroika::Frameworks::WebServer::Request;
using Stroika::Frameworks::WebServer::Response;

using namespace Stroika::Samples::HTMLUI;

namespace {
    const Common::ConstantProperty<Headers> kDefaultResponseHeaders_{[] () {
        Headers h;
        h.server = "Stroika-Sample-HTMLUI/"_k + AppVersion::kVersion.AsMajorMinorString ();
        return h;
    }};
}

/*
 *  It's often helpful to structure together, routes, special interceptors, with your connection manager, to package up
 *  all the logic /options for HTTP interface.
 *
 *  This particular organization also makes it easy to save instance variables with the webserver (like a pointer to a handler)
 *  and accesss them from the Route handler functions.
 */
class WebServer::Rep_ {
public:
    const Sequence<Route> kRoutes_;        // rules saying how to map urls to code
    shared_ptr<IWSAPI>    fWSImpl_;        // application logic actually handling webservices
    ConnectionManager     fConnectionMgr_; // manage http connection objects, thread pool, etc

    /*
     * data to track web method call counts, just to report api usage stats
     */
    atomic<unsigned int> fActiveCallCnt_{0};
    struct ActiveCallCounter_ {
        ActiveCallCounter_ (Rep_& r)
            : fRep_{r}
        {
            ++r.fActiveCallCnt_;
        }
        ~ActiveCallCounter_ ()
        {
            --fRep_.fActiveCallCnt_;
        }
        Rep_& fRep_;
    };

    // for usage stats
    IntervalTimer::Adder fIntervalTimerAdder_;

    static const WebServiceMethodDescription kAbout_;

    Rep_ (optional<uint16_t> portNumber)
        : kRoutes_{
              Route{"/api"_RegEx, DefaultPage_},

            /**
             * /about - health check etc
             */
            Route{
                "/api/about"_RegEx,
                mkRequestHandler (kAbout_, About::kMapper, function<About (void)>{[this] () { ActiveCallCounter_ acc{*this}; return fWSImpl_->about_GET (); }})},

            /**
             * /resource
             */
            Route{HTTP::MethodsRegEx::kGet, "/api/resource/(.+)"_RegEx, [this] (Message* m, const String& resID) {
                ActiveCallCounter_ acc{*this};
                auto r = fWSImpl_->resource_GET (resID);
                m->rwResponse().contentType  = get<InternetMediaType> (r);
                m->rwResponse().write (get<BLOB> (r));
            }}

          }
        , fWSImpl_{make_shared<WSImpl> ([this] () -> About::APIServerInfo::WebServer {
                About::APIServerInfo::WebServer r;
                auto rr = this->fConnectionMgr_.statistics();
                r.fThreadPool.fThreads             = static_cast<unsigned int> ( rr.fThreadPoolSize); // todo begingings of data to report
                r.fThreadPool.fTasksStillQueued = rr.fThreadPoolStatistics.fNumberOfTasksAdded - rr.fThreadPoolStatistics.fNumberOfTasksCompleted;
                r.fThreadPool.fAverageTaskRunTime = rr.fThreadPoolStatistics.GetMeanTimeConsumed ();
                return r;
            })}
        , fConnectionMgr_{SocketAddresses (InternetAddresses_Any (), portNumber.value_or (gAppConfiguration->WebServerPort.value_or(80))),
         kRoutes_, 
         ConnectionManager::Options{
             .fMaxConcurrentlyHandledConnections = 20
             ,.fDefaultResponseHeaders = kDefaultResponseHeaders_     
             ,.fCollectStatistics = true
             }
         }
 , fIntervalTimerAdder_{[this] () {
                           Debug::TraceContextBumper ctx{"webserver status gather TIMER HANDLER"}; // to debug https://github.com/SophistSolutions/WhyTheFuckIsMyNetworkSoSlow/issues/78
                           OperationalStatisticsMgr::sThe.RecordActiveRunningTasksCount (fActiveCallCnt_);
                           OperationalStatisticsMgr::sThe.RecordOpenConnectionCount (fConnectionMgr_.connections ().length ());
                           OperationalStatisticsMgr::sThe.RecordActiveRunningTasksCount (fConnectionMgr_.activeConnections ().length ());
                       },
                       15s, IntervalTimer::Adder::eRunImmediately}
    {
        using Stroika::Frameworks::WebServer::DefaultFaultInterceptor;
        DefaultFaultInterceptor defaultHandler;
        fConnectionMgr_.defaultErrorHandler = DefaultFaultInterceptor{[defaultHandler] (Message* m, const exception_ptr& e) {
            // Unsure if we should bother recording 404s
            DbgTrace ("faulting on request {}"_f, Characters::ToString (m->request ()));
            OperationalStatisticsMgr::ProcessAPICmd::NoteError ();
            defaultHandler.HandleFault (m, e);
        }};

        Logger::sThe.Log (Logger::eInfo, "Started WebServices on {}"_f, fConnectionMgr_.bindings ());
    }
    // Can declare arguments as Request*,Response*
    static void DefaultPage_ (Request*, Response* response)
    {
         WriteDocsPage (
            response,
            Sequence<WebServiceMethodDescription>{
                kAbout_,
            },
            DocsOptions{.fH1Text = "Stroika-Sample-HTMLUI"_k,
                        .fIntroductoryText = "EARLY DRAFT. ."_k,
                        .fVariables2Substitute =
                            Mapping<String, String>{
                                {"ShowAsExternalURI"sv,
                                 gAppConfiguration->ShowAsExternalURL
                                     .value_or (Characters::Format ("http://[::1]:{}"_f, gAppConfiguration->WebServerPort.value_or (AppConfigurationType::kWebServerPort_Default)))
                                     .As<String> ()
                                     .AssureEndsWith ('/')}},
                        //.fOpenAPISpecification    = kOpenAPISpecification,
                        .fOpenAPISpecificationURI = URI{"resource/api.json"sv}});
    }
};

/*
 *  Documentation on WSAPIs
 */
const WebServiceMethodDescription WebServer::Rep_::kAbout_{
    "/api/about"sv,
    Set<String>{IO::Network::HTTP::Methods::kGet},
    DataExchange::InternetMediaTypes::kJSON,
    "Data about the AskHealthFrame server status, version etc"sv,
    Sequence<String>{
        "curl {{ShowAsExternalURI}}/api/about"sv,
    },
    Sequence<String>{"Fetch the component versions, web server connections, thread pool etc, etc."sv},
};

WebServer::WebServer (optional<uint16_t> portNumber)
    : fRep_{make_shared<Rep_> (portNumber)}
{
}