/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2025.  All rights reserved
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
#include "Stroika/Foundation/Execution/Module.h"
#include "Stroika/Foundation/IO/Network/HTTP/Exception.h"
#include "Stroika/Foundation/IO/Network/HTTP/Headers.h"
#include "Stroika/Foundation/IO/Network/HTTP/Methods.h"
#include "Stroika/Foundation/Memory/Optional.h"

#include "Stroika/Frameworks/Auth/Interceptor.h"
#include "Stroika/Frameworks/WebServer/ConnectionManager.h"
#include "Stroika/Frameworks/WebServer/DefaultFaultInterceptor.h"
#include "Stroika/Frameworks/WebServer/FileSystemRequestHandler.h"
#include "Stroika/Frameworks/WebServer/Router.h"
#include "Stroika/Frameworks/WebService/Server/Basic.h"
#include "Stroika/Frameworks/WebService/Server/ObjectRequestHandler.h"
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
using namespace Stroika::Foundation::Memory;

using namespace Stroika::Frameworks::WebServer;
using namespace Stroika::Frameworks::WebService;
using namespace Stroika::Frameworks::WebService::Server;
using namespace Stroika::Frameworks::WebService::Server::VariantValue;

using Memory::BLOB;
using Stroika::Frameworks::Auth::CurrentIdentityAuthInterceptor;
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

namespace {
    const ConstantProperty<FileSystemRequestHandler::Options> kStaticSiteHandlerOptions_{[] () {
        Sequence<pair<RegularExpression, CacheControl>> kFSCacheControlSettings_{
            /*
             *  Values with hashes in names never change value - you get new hashes when the files do, so these files are immutable, and
             *  can be cached forever by the browser.
             * 
             *  Empirically, vite/quasar appears to put hash/immutable files into dist/spa/assets/AboutPage-BBsu2LrN.css, AboutPage-D6hvYwgY.js etc...
             */
            {RegularExpression{".*\\bassets\\/.+"sv, eCaseInsensitive}, CacheControl::kImmutable},
            /*
             *  Top level (other) files, like "Home.html", its less clear how long to tell the browser they will be valid.
             *  If you pick to large a time, when you update your site it can lead to grave confusion. Pick a time that's too slow, and you
             *  needlessly generate web traffic (though still should do conditional gets and not actually transfer much data). 5 minutes
             *  is a compromise I've found suggested someplace on the web.
             */
            {RegularExpression::kAny, CacheControl{.fCacheability = CacheControl::ePublic, .fMaxAge = Duration{5min}.As<int32_t> ()}},
        };
        return FileSystemRequestHandler::Options{.fDefaultIndexFileNames = Sequence<filesystem::path>{"index.html"sv},
                                                 .fCacheControlSettings  = kFSCacheControlSettings_,
                                                 //fallback file to support createWebHistory () in vuejs, in turn to support oauth2 redirect
                                                 .fFallbackFile = "index.html"};
    }};
}

// Configuration object passed to GUI as startup parameters/configuration
namespace {
    struct Config_ {
        optional<String>       API_ROOT;         // if specified takes precedence over DEFAULT_API_PORT
        optional<unsigned int> DEFAULT_API_PORT; // added to remote host used in web browser for accessing API

        static inline const ObjectVariantMapper kMapper = [] () {
            ObjectVariantMapper mapper;
            mapper.AddCommonType<optional<String>> ();
            mapper.AddCommonType<optional<unsigned int>> ();
            mapper.AddClass<Config_> ({
                {"API_ROOT"sv, &Config_::API_ROOT},
                {"DEFAULT_API_PORT"sv, &Config_::DEFAULT_API_PORT},
            });
            return mapper;
        }();
    };
    const WebServiceMethodDescription kGUIConfig_{
        "config"sv,         Set<String>{HTTP::Methods::kGet},  DataExchange::InternetMediaTypes::kJSON, "GUI config"sv,
        Sequence<String>{}, Sequence<String>{"GUI config."sv},
    };
    Config_ GetConfig_ ()
    {
        return Config_{.API_ROOT = OptionallyCopy<String> (gAppConfiguration->ShowAsExternalURL, [] (URI u) { return u.As<String> (); }),
                       .DEFAULT_API_PORT = gAppConfiguration->WebServerPort};
    }
}

/*
 *  It's often helpful to structure together, routes, special interceptors, with your connection manager, to package up
 *  all the logic /options for HTTP interface.
 *
 *  This particular organization also makes it easy to save instance variables with the webserver (like a pointer to a handler)
 *  and access them from the Route handler functions.
 */
class WebServer::Rep_ {
public:
    const Sequence<Route> kRoutes_;        // rules saying how to map URLs to code
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
    IntervalTimer::Adder fStatsIntervalTimerAdder_;

    static const WebServiceMethodDescription kAbout_;
    static const WebServiceMethodDescription kAuth_;
    static const WebServiceMethodDescription kConnections_;
    static const WebServiceMethodDescription kHeathCheck_;

    Rep_ (optional<uint16_t> portNumber)
        : kRoutes_{
            
            Route{"api/?"_RegEx, DefaultPage_}

           /**
            * /auth
            */
            , Route{"api/(v1/)?auth/oauth/configurations/?"_RegEx, ObjectRequestHandler::Factory{
                        {Auth::kMapper},
                        [this] () {
                            ActiveCallCounter_ acc{*this};
                            return fWSImpl_->auth_oauth_configuration_GET ();
                        }}}
            , Route{IO::Network::HTTP::MethodsRegEx::kPost, "api/(v1/)?auth/oauth/tokens/?"_RegEx, ObjectRequestHandler::Factory{
                        {Auth::kMapper},
                        [this] (const Auth::TokenRequest& r) {
                            ActiveCallCounter_ acc{*this};
                            return fWSImpl_->auth_oauth_tokens_POST (r);
                        }}}
            , Route{IO::Network::HTTP::MethodsRegEx::kPost, "api/(v1/)?auth/oauth/tokens/revoke/?"_RegEx, ObjectRequestHandler::Factory{
                        {Auth::kMapper},
                        [this] (const Auth::TokenRevocationRequest& r) {
                            ActiveCallCounter_ acc{*this};
                            return fWSImpl_->auth_oauth_tokens_revoke_POST (r);
                        }}}
            , Route{"api/(v1/)?auth/oauth/user_info/?"_RegEx, ObjectRequestHandler::Factory{
                        {Auth::kMapper},
                        [this] () {
                            ActiveCallCounter_ acc{*this};
                            return fWSImpl_->auth_oauth_user_info_GET ();
                        }}}

            /**
             * /about - health check etc
             */
            , Route{"api/about/?"_RegEx, ObjectRequestHandler::Factory{{About::kMapper}, [this] () {
                                            ActiveCallCounter_ acc{*this};
                                            return fWSImpl_->about_GET ();
                                        }}}

            /**
             * /healthcheck - health check etc
             */
            , Route{"api/healthcheck/?"_RegEx, ObjectRequestHandler::Factory{{HealthStatus::kMapper}, [this] () {
                                                    ActiveCallCounter_ acc{*this};
                                                    return fWSImpl_->healthcheck_GET ();
                                            }}}

            /**
             * /connections - just for debugging - maybe useful - probably wouldn't leave i a real product
             */
            , Route{"api/connections/?"_RegEx, [this] (Message& m) {
                        ActiveCallCounter_ acc{*this};
                        m.rwResponse ().contentType = InternetMediaTypes::kText_PLAIN;
                        m.rwResponse ().writeln ("{"sv);
                        m.rwResponse ().writeln ("  \"tickCount\": {},"_f (Time::GetTickCount()));
                        m.rwResponse ().writeln ("  \"connections\": ["sv);
                        for (auto i : this->fConnectionMgr_.connections ()) {
                            m.rwResponse ().writeln ("    {},"_f(i));
                        }
                        m.rwResponse ().writeln ("  ]"sv);
                        m.rwResponse ().writeln ("}"sv);
                    }}

            /**
             * /resource
             * 
             *      \note saying HTTP::MethodsRegEx::kGet here is not needed, and just as an example - its the default and can
             *            be omitted.
             */
            , Route{HTTP::MethodsRegEx::kGet, "api/resource/(.+)"_RegEx,
                    [this] (Message& m, const String& resID) {
                        ActiveCallCounter_ acc{*this};
                        auto               r        = fWSImpl_->resource_GET (resID);
                        m.rwResponse ().contentType = r.fType;
                        m.rwResponse ().write (r.fData);
                    }}

            /*
             * configuration data for web-gui - private - just so can communicate with /api
             */
           , Route{"config.json"_RegEx, ObjectRequestHandler::Factory{{Config_::kMapper}, [=] () {
                return GetConfig_ (); }}}

            /*
             * Serve up contents of html folder as static site
             * 
             *  Note - since this matches any URL, and is the last in the router, the above patterns match first, and anything
             *  else is assumed to come from the html folder (else if fall-through, the router will issue 404).
             */
           , Route{RegularExpression::kAny, FileSystemRequestHandler{Execution::GetEXEDir () / "html"sv, kStaticSiteHandlerOptions_}}
          }
        , fWSImpl_{ make_shared<WSImpl>(   [this](const WSImpl::WithWebServerCallbackType& f) { f (fConnectionMgr_);}  )}
        , fConnectionMgr_{SocketAddresses (InternetAddresses_Any (), portNumber.value_or (gAppConfiguration->WebServerPort.value_or (AppConfigurationType::kWebServerPort_Default)))
                         , kRoutes_
                         , ConnectionManager::Options{.fMaxConcurrentlyHandledConnections = 10,
                                                     .fDefaultResponseHeaders            = kDefaultResponseHeaders_,
                                                     .fCollectStatistics                 = true}}
        , fStatsIntervalTimerAdder_{[this] () {
                                   // capture stats at regular time intervals
                                   Debug::TraceContextBumper ctx{"webserver status gather TIMER HANDLER"}; // to debug https://github.com/SophistSolutions/WhyTheFuckIsMyNetworkSoSlow/issues/78
                                   OperationalStatisticsMgr::sThe.RecordActiveRunningTasksCount (fActiveCallCnt_);
                                   OperationalStatisticsMgr::sThe.RecordOpenConnectionCount (fConnectionMgr_.statistics ().fConnections.fNumberOfOpenConnections);
                                   OperationalStatisticsMgr::sThe.RecordActiveRunningTasksCount (fConnectionMgr_.statistics ().fConnections.fNumberOfActiveConnections);
                               },
                               15s, IntervalTimer::Adder::eRunImmediately}
    {
        using Stroika::Frameworks::WebServer::DefaultFaultInterceptor;
        DefaultFaultInterceptor defaultHandler;
        fConnectionMgr_.defaultErrorHandler = DefaultFaultInterceptor{[defaultHandler] (Message& m, const exception_ptr& e) {
            // Unsure if we should bother recording 404s
            DbgTrace ("faulting on request {}"_f, Characters::ToString (m.request ()));
            OperationalStatisticsMgr::ProcessAPICmd::NoteError ();
            defaultHandler.HandleFault (m, e);
        }};
        auto convertAuthHeaderToIDObject    = [] (Request& request) -> optional<WebServiceIdentity> {
            if (auto authHeader = request.headers ().authorization (); authHeader and authHeader->StartsWith ("Bearer "sv)) {
                return WebServiceIdentity{.fBearerToken = authHeader->SubString (7).Trim ()};
            }
            return nullopt;
        };
        fConnectionMgr_.AddInterceptor (CurrentIdentityAuthInterceptor{convertAuthHeaderToIDObject}, ConnectionManager::ePrependsToEarly);
        Logger::sThe.Log (Logger::eInfo, "Started WebServices on {}"_f, fConnectionMgr_.bindings ());
    }
    // Can declare arguments as Request*,Response*
    static void DefaultPage_ (Request&, Response& response)
    {
        WriteDocsPage (
            response, Sequence<WebServiceMethodDescription>{kAbout_, kAuth_, kConnections_, kHeathCheck_},
            DocsOptions{.fH1Text = "Stroika-Sample-HTMLUI"_k,
                        .fIntroductoryText = "Just a sample set of webservices to show how to hook C++ code into html via ajax callbacks..."_k,
                        .fVariables2Substitute =
                            Mapping<String, String>{
                                {"ShowAsExternalURI"sv,
                                 gAppConfiguration->ShowAsExternalURL
                                     .value_or (Characters::Format ("http://[::1]:{}"_f, gAppConfiguration->WebServerPort.value_or (AppConfigurationType::kWebServerPort_Default)))
                                     .As<String> ()
                                     .AssureEndsWith ('/')}},
                        //.fOpenAPISpecification    = kOpenAPISpecification,
                        .fOpenAPISpecificationURI = URI{"api/resource/api.json"sv}});
    }
};

/*
 *  Documentation on WSAPIs
 */
const WebServiceMethodDescription WebServer::Rep_::kAbout_{
    "api/about"sv,
    Set<String>{HTTP::Methods::kGet},
    DataExchange::InternetMediaTypes::kJSON,
    "Data about the Sample HTMLUI server status, version etc"sv,
    Sequence<String>{
        "curl {{ShowAsExternalURI}}/api/about"sv,
    },
    Sequence<String>{"Fetch the component versions, web server connections, thread pool etc, etc."sv},
};
const WebServiceMethodDescription WebServer::Rep_::kAuth_{
    "api/v1/auth/oauth/configurations"sv,
    Set<String>{IO::Network::HTTP::Methods::kGet, IO::Network::HTTP::Methods::kPost},
    DataExchange::InternetMediaTypes::kJSON,
    "Authentication/OAuth2 related API support"sv,
    Sequence<String>{
        "curl -v {{ShowAsExternalURI}}api/v1/auth/oauth/configurations"sv,
        "curl -v -X POST -H \"Content-Type: application/json\" {{ShowAsExternalURI}}api/v1/auth/oauth/tokens -d \"{\\\"authorizationCode\\\": \\\"123\\\", \\\"provider\\\": \\\"google\\\", \\\"applicationId\\\": \\\"xxx\\\", \\\"redirectURL\\\": \\\"http://localhost:9000/oauth/google\\\" }\""sv,
        "curl -v -X POST -H \"Content-Type: application/json\" {{ShowAsExternalURI}}api/v1/auth/oauth/tokens -d \"{\\\"authorizationCode\\\": \\\"456\\\", \\\"provider\\\": \\\"google\\\", \\\"applicationId\\\": \\\"xxx\\\", \\\"redirectURL\\\": \\\"http://localhost:9000/oauth/google\\\", \\\"codeVerifier\\\": \\\"OPTIONAL-PASS-IF-USING-PKCE\\\" }\""sv,
        "curl -v -X POST -H \"Content-Type: application/json\" {{ShowAsExternalURI}}api/v1/auth/oauth/tokens/revoke -d \"{\\\"provider\\\": \\\"google\\\", \\\"access_token\\\": \\\"xxx\\\", \\\"refresh_token\\\": \\\"xxxx\\\" }\""sv,
        "curl -v {{ShowAsExternalURI}}api/v1/auth/oauth/user_info -H \"Authorization: Bearer XXX\" ; echo where XXX is authentication_token from above"sv,
    },
    Sequence<String>{
        "<em>Auth/Login</em>",
        "<li>in web browser: "
        "https://accounts.google.com/o/oauth2/v2/"
        "auth?scope=openid+profile+email&access_type=offline&include_granted_scopes=true&response_type=code&state={}&redirect_uri=http%3A//"
        "localhost:9000/oauth/google&client_id=291846620235-b7737mjsce5k6trrik7oi9b4dgum0sgg.apps.googleusercontent.com</li>"
        "<li>see https://developers.google.com/identity/protocols/oauth2/web-server#httprest for info on getting authenication code</li>",
        "<li>GET auth/oauth/configurations - returns available oauth configurations (from list of redirect urls you can pick any). </li>",
        "<li>For 'tokens' API - just uses (hidden) client-secret and calls auth server token endpoint and returns its results;</li>",
        "<li>For revoke API, refresh_token is optional. Silently does nothing if token revocation not supported by the provider API</li>",
        "<li>user_info endpoint expects 'BEARER TOKEN' of auth token to be provided, and returns 401 otherwise, but if OK, returns info from user_info auth endpoint (which you might have gotten from JWT id_token).</li>"sv},
};
const WebServiceMethodDescription WebServer::Rep_::kConnections_{
    "api/connections"sv,
    Set<String>{HTTP::Methods::kGet},
    DataExchange::InternetMediaTypes::kText_PLAIN,
    "debugging dump of connections internals"sv,
    Sequence<String>{
        "curl {{ShowAsExternalURI}}/api/connections"sv,
    },
    Sequence<String>{"Fetch the webservers connections list."sv},
};
const WebServiceMethodDescription WebServer::Rep_::kHeathCheck_{
    "api/healthcheck"sv,
    Set<String>{HTTP::Methods::kGet},
    DataExchange::InternetMediaTypes::kJSON,
    "Data about the Sample HTMLUI server health"sv,
    Sequence<String>{
        "curl {{ShowAsExternalURI}}/api/healthcheck"sv,
    },
    Sequence<String>{"Fetch the app health status."sv},
};

WebServer::WebServer (optional<uint16_t> portNumber)
    : fRep_{make_shared<Rep_> (portNumber)}
{
}