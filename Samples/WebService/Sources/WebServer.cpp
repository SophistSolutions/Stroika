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

using Characters::String;
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
    const Router       kRouter_;
    shared_ptr<IWSAPI> fWSImpl_;
    ConnectionManager  fConnectionMgr_;

    static const WebServiceMethodDescription kPlus_;

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

                  Route{L"plus"_RegEx, mkRequestHandler (WebServiceMethodDescription{{}, {}, DataExchange::PredefinedInternetMediaType::JSON_CT ()}, Model::kMapper, Traversal::Iterable<String>{L"arg1", L"arg2"}, function<Number (Number, Number)>{[=](Number arg1, Number arg2) { return fWSImpl_->plus (arg1, arg2); }})},

                  Route{L"test-void-return"_RegEx, mkRequestHandler (WebServiceMethodDescription{}, Model::kMapper, Traversal::Iterable<String>{L"err-if-more-than-10"}, function<void(double)>{[=](double check) {
                                    if (check > 10) {
                                        Execution::Throw (Execution::StringException (L"more than 10"));
                                    } }})},

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
        response->writeln (L"<html><body>");
        response->writeln (L"<p>Stroika WebService Sample</p>");
        response->writeln (L"<ul>");
        response->writeln (L"Run the service (under the debugger if you wish)");
        response->writeln (L"<li>curl http://localhost:8080/ OR</li>");
        response->writeln (L"<li>curl http://localhost:8080/FRED OR      (to see error handling)</li>");
        response->writeln (L"<li>curl -H \"Content-Type: application/json\" -X POST -d '{\"AppState\":\"Start\"}' http://localhost:8080/SetAppState</li>");
        response->writeln (L"<li>curl -H \"Content-Type: application/json\" -X POST -d '{\"arg1\": 1, \"arg2\": 3 }' http://localhost:8080/plus</li>");
        response->writeln (L"</ul>");
        response->writeln (L"</body></html>");

        response->SetContentType (DataExchange::PredefinedInternetMediaType::kText_HTML);
    }
    // Can declare arguments as Message* message
    static void SetAppState_ (Message* message)
    {
        String argsAsString = Streams::TextReader::New (message->PeekRequest ()->GetBody ()).ReadAll ();
        message->PeekResponse ()->writeln (L"<html><body><p>Hi SetAppState (" + argsAsString.As<wstring> () + L")</p></body></html>");
        message->PeekResponse ()->SetContentType (DataExchange::PredefinedInternetMediaType::kText_HTML);
    }
};
const WebServiceMethodDescription WebServer::Rep_::kPlus_{
    L"plus"_k,
    Set<String>{String_Constant{IO::Network::HTTP::Methods::kGet}},
    DataExchange::PredefinedInternetMediaType::kJSON,
    {},
    Sequence<String>{
        L"curl http://localhost:8080/plus",
    },
    Sequence<String>{L"Fetch the xxxx.",
                     L"@todo - in the xxxx"},
};

WebServer::WebServer (uint16_t portNumber, const shared_ptr<IWSAPI>& wsImpl)
    : fRep_ (make_shared<Rep_> (portNumber, wsImpl))
{
}
