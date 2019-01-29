/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include <iostream>

#include "Stroika/Foundation/Characters/String2Int.h"
#include "Stroika/Foundation/Characters/String_Constant.h"
#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/IO/Network/HTTP/Exception.h"
#include "Stroika/Foundation/IO/Network/HTTP/Headers.h"
#include "Stroika/Foundation/IO/Network/HTTP/Methods.h"
#include "Stroika/Foundation/Streams/TextReader.h"

#include "Stroika/Frameworks/WebServer/ConnectionManager.h"
#include "Stroika/Frameworks/WebServer/Router.h"
#include "Stroika/Frameworks/WebService/Server/Basic.h"
#include "Stroika/Frameworks/WebService/Server/VariantValue.h"

#include "WebServer.h"

#include "AppVersion.h"

using namespace std;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::IO::Network;
using namespace Stroika::Frameworks::WebServer;
using namespace Stroika::Frameworks::WebService;
using namespace Stroika::Frameworks::WebService::Server;
using namespace Stroika::Frameworks::WebService::Server::VariantValue;

using Memory::BLOB;

using namespace StroikaSample::WebServices;

/*
 *  It's often helpful to structure together, routes, special interceptors, with your connection manager, to package up
 *  all the logic /options for HTTP interface.
 *
 *  This particular organization also makes it easy to save instance variables with the webserver (like a pointer to a handler)
 *  and accesss them from the Route handler functions.
 */
class WebServer::Rep_ {
public:
    const Router       kRouter_;            // rules saying how to map urls to code
    shared_ptr<IWSAPI> fWSImpl_;            // application logic actually handling webservices
    ConnectionManager  fConnectionMgr_;     // manage http connection objects, thread pool, etc

    static const WebServiceMethodDescription kPlus_;
    static const WebServiceMethodDescription kMinus;
    static const WebServiceMethodDescription kTimes;
    static const WebServiceMethodDescription kDivide;

    Rep_ (uint16_t portNumber, const shared_ptr<IWSAPI>& wsImpl)
        : kRouter_{
              Sequence<Route>{
                  Route{
                      RegularExpression{IO::Network::HTTP::Methods::kOptions},
                      RegularExpression::kAny,
                      []([[maybe_unused]] Message* m) { Lambda_Arg_Unused_BWA (m); }},

                  Route{L""_RegEx, DefaultPage_},

                  Route{L"POST"_RegEx, L"SetAppState"_RegEx, SetAppState_},

                  Route{L"GET"_RegEx, L"FRED"_RegEx, [](Request*, Response* response) {
                            response->write (L"FRED");
                            response->SetContentType (DataExchange::PredefinedInternetMediaType::kText);
                        }},

                  Route{L"plus"_RegEx, mkRequestHandler (kPlus_, Model::kMapper, Traversal::Iterable<String>{L"arg1", L"arg2"}, function<Number (Number, Number)>{[=](Number arg1, Number arg2) { return fWSImpl_->plus (arg1, arg2); }})},
                 
                  Route{L"minus"_RegEx, mkRequestHandler (kMinus, Model::kMapper, Traversal::Iterable<String>{L"arg1", L"arg2"}, function<Number (Number, Number)>{[=](Number arg1, Number arg2) { return fWSImpl_->minus (arg1, arg2); }})},
                  
                  Route{L"times"_RegEx, mkRequestHandler (kTimes, Model::kMapper, Traversal::Iterable<String>{L"arg1", L"arg2"}, function<Number (Number, Number)>{[=](Number arg1, Number arg2) { return fWSImpl_->times (arg1, arg2); }})},
                  
                  Route{L"divide"_RegEx, mkRequestHandler (kDivide, Model::kMapper, Traversal::Iterable<String>{L"arg1", L"arg2"}, function<Number (Number, Number)>{[=](Number arg1, Number arg2) { return fWSImpl_->divide (arg1, arg2); }})},

                  Route{L"test-void-return"_RegEx, mkRequestHandler (WebServiceMethodDescription{}, Model::kMapper, Traversal::Iterable<String>{L"err-if-more-than-10"}, function<void(double)>{[=](double check) {
                                    if (check > 10) {
                                        Execution::Throw (Execution::StringException (L"more than 10"));
                                    }}}
                                    )},

              }}
        , fWSImpl_{wsImpl}
        , fConnectionMgr_{SocketAddresses (InternetAddresses_Any (), portNumber), kRouter_, ConnectionManager::Options{{}, Socket::BindFlags{}, String{L"Stroika-Sample-WebServices/"} + AppVersion::kVersion.AsMajorMinorString ()}}
    {
        // @todo - move this to some framework-specific regtests...
        using VariantValue         = DataExchange::VariantValue;
        Sequence<VariantValue> tmp = OrderParamValues (Iterable<String>{L"page", L"xxx"}, PickoutParamValuesFromURL (URL (L"http://www.sophist.com?page=5", URL::eFlexiblyAsUI)));
        Assert (tmp.size () == 2);
        Assert (tmp[0] == 5);
        Assert (tmp[1] == nullptr);
    }
    // Can declare arguments as Request*,Response*
    static void DefaultPage_ (Request*, Response* response)
    {
        WriteDocsPage (
            response,
            Sequence<WebServiceMethodDescription>{
                kPlus_,
                kMinus,
                kTimes,
                kDivide,
            },
            DocsOptions{L"Stroika Sample WebService - Web Methods"_k});
    }
    // Can declare arguments as Message* message
    static void SetAppState_ (Message* message)
    {
        String argsAsString = Streams::TextReader::New (message->PeekRequest ()->GetBody ()).ReadAll ();
        message->PeekResponse ()->writeln (L"<html><body><p>Hi SetAppState (" + argsAsString.As<wstring> () + L")</p></body></html>");
        message->PeekResponse ()->SetContentType (DataExchange::PredefinedInternetMediaType::kText_HTML);
    }
};

/*
 *  Documentation on WSAPIs
 */
const WebServiceMethodDescription WebServer::Rep_::kPlus_{
    L"plus"_k,
    Set<String>{String_Constant{IO::Network::HTTP::Methods::kPost}},
    DataExchange::PredefinedInternetMediaType::kJSON,
    {},
    Sequence<String>{
        L"curl -H \"Content-Type: application/json\" -X POST -d '{\"arg1\":\"2 + i\", \"arg2\": 3.2 }' http://localhost:8080/plus --output -",
    },
    Sequence<String>{L"add the two argument numbers"},
};
const WebServiceMethodDescription WebServer::Rep_::kMinus{
    L"minus"_k,
    Set<String>{String_Constant{IO::Network::HTTP::Methods::kPost}},
    DataExchange::PredefinedInternetMediaType::kJSON,
    {},
    Sequence<String>{
        L"curl -H \"Content-Type: application/json\" -X POST -d '{\"arg1\":\"2 + i\", \"arg2\": 3.2 }' http://localhost:8080/minus --output -",
    },
    Sequence<String>{L"subtract the two argument numbers"},
};
const WebServiceMethodDescription WebServer::Rep_::kTimes{
    L"times"_k,
    Set<String>{String_Constant{IO::Network::HTTP::Methods::kPost}},
    DataExchange::PredefinedInternetMediaType::kJSON,
    {},
    Sequence<String>{
        L"curl -H \"Content-Type: application/json\" -X POST -d '{\"arg1\":\"2 + i\", \"arg2\": 3.2 }' http://localhost:8080/times --output -",
    },
    Sequence<String>{L"multiply the two argument numbers"},
};
const WebServiceMethodDescription WebServer::Rep_::kDivide{
    L"divide"_k,
    Set<String>{String_Constant{IO::Network::HTTP::Methods::kPost}},
    DataExchange::PredefinedInternetMediaType::kJSON,
    {},
    Sequence<String>{
        L"curl -H \"Content-Type: application/json\" -X POST -d '{\"arg1\":\"2 + i\", \"arg2\": 3.2 }' http://localhost:8080/divide --output -",
    },
    Sequence<String>{L"divide the two argument numbers"},
};

WebServer::WebServer (uint16_t portNumber, const shared_ptr<IWSAPI>& wsImpl)
    : fRep_ (make_shared<Rep_> (portNumber, wsImpl))
{
}
