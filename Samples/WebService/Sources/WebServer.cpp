/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include <iostream>

#include "Stroika/Foundation/Characters/FloatConversion.h"
#include "Stroika/Foundation/Characters/String2Int.h"
#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/Common/Property.h"
#include "Stroika/Foundation/DataExchange/InternetMediaTypeRegistry.h"
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
using namespace Stroika::Foundation::DataExchange;
using namespace Stroika::Foundation::IO::Network;

using namespace Stroika::Frameworks::WebServer;
using namespace Stroika::Frameworks::WebService;
using namespace Stroika::Frameworks::WebService::Server;
using namespace Stroika::Frameworks::WebService::Server::VariantValue;

using Memory::BLOB;
using Stroika::Frameworks::WebServer::Request;
using Stroika::Frameworks::WebServer::Response;

using namespace StroikaSample::WebServices;

namespace {
    const Common::ConstantProperty<Headers> kDefaultResponseHeaders_{[] () {
        Headers h;
        h.server = L"Stroika-Sample-WebServices/"_k + AppVersion::kVersion.AsMajorMinorString ();
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

    static const WebServiceMethodDescription kVariables_;

    static const WebServiceMethodDescription kPlus_;
    static const WebServiceMethodDescription kMinus;
    static const WebServiceMethodDescription kTimes;
    static const WebServiceMethodDescription kDivide;

    Rep_ (uint16_t portNumber, const shared_ptr<IWSAPI>& wsImpl)
        : kRoutes_{
              Route{L""_RegEx, DefaultPage_},

              Route{HTTP::MethodsRegEx::kPost, L"SetAppState"_RegEx, SetAppState_},

              Route{L"FRED"_RegEx, [] (Request*, Response* response) {
                        response->write (L"FRED");
                        response->contentType = InternetMediaTypes::kText_PLAIN;
                    }},

              /*
               * the 'variable' API demonstrates a typical REST style CRUD usage - where the 'arguments' mainly come from 
               * the URL itself.
               */
              Route{L"variables(/?)"_RegEx, [this] (Message* m) {
                        WriteResponse (&m->rwResponse (), kVariables_, kMapper.FromObject (fWSImpl_->Variables_GET ()));
                    }},
              Route{L"variables/(.+)"_RegEx, [this] (Message* m, const String& varName) {
                        WriteResponse (&m->rwResponse (), kVariables_, kMapper.FromObject (fWSImpl_->Variables_GET (varName)));
                    }},
              Route{HTTP::MethodsRegEx::kPostOrPut, L"variables/(.+)"_RegEx, [this] (Message* m, const String& varName) {
                        optional<Number> number;
                        // demo getting argument from the body
                        if (not number) {
                            // read if content-type is text (not json)
                            if (m->request ().contentType () and InternetMediaTypeRegistry::Get ().IsA (InternetMediaTypes::kText_PLAIN, *m->request ().contentType ())) {
                                String argsAsString = Streams::TextReader::New (m->rwRequest ().GetBody ()).ReadAll ();
                                number              = kMapper.ToObject<Number> (DataExchange::VariantValue{argsAsString});
                            }
                        }
                        // demo getting argument from the query argument
                        if (not number) {
                            static const String                         kValueParamName_ = L"value"sv;
                            Mapping<String, DataExchange::VariantValue> args             = PickoutParamValuesFromURL (&m->request ());
                            number                                                       = Model::kMapper.ToObject<Number> (args.LookupValue (kValueParamName_));
                        }
                        // demo getting either query arg, or url encoded arg
                        if (not number) {
                            static const String kValueParamName_ = "value"sv;
                            // NOTE - PickoutParamValues combines PickoutParamValuesFromURL, and PickoutParamValuesFromBody. You can use
                            // Either one of those instead. PickoutParamValuesFromURL assumes you know the name of the parameter, and its
                            // encoded in the query string. PickoutParamValuesFromBody assumes you have something equivilent you can parse ouf
                            // of the body, either json encoded or form-encoded (as of 2.1d23, only json encoded supported)
                            Mapping<String, DataExchange::VariantValue> args = PickoutParamValues (&m->rwRequest ());
                            number                                           = Model::kMapper.ToObject<Number> (args.LookupValue (kValueParamName_));
                        }
                        if (not number) {
                            Execution::Throw (HTTP::ClientErrorException{"Expected argument to PUT/POST variable"sv});
                        }
                        fWSImpl_->Variables_SET (varName, *number);
                        WriteResponse (&m->rwResponse (), kVariables_);
                    }},
              Route{HTTP::MethodsRegEx::kDelete, L"variables/(.+)"_RegEx, [this] (Message* m, const String& varName) {
                        fWSImpl_->Variables_DELETE (varName);
                        WriteResponse (&m->rwResponse (), kVariables_);
                    }},

              /*
               *    plus, minus, times, and divide, test-void-return all all demonstrate passing in variables through either the POST body, or query-arguments.
               */
              Route{HTTP::MethodsRegEx::kPost, L"plus"_RegEx, mkRequestHandler (kPlus_, Model::kMapper, Traversal::Iterable<String>{L"arg1", L"arg2"}, function<Number (Number, Number)>{[this] (Number arg1, Number arg2) { return fWSImpl_->plus (arg1, arg2); }})},
              Route{HTTP::MethodsRegEx::kPost, L"minus"_RegEx, mkRequestHandler (kMinus, Model::kMapper, Traversal::Iterable<String>{L"arg1", L"arg2"}, function<Number (Number, Number)>{[this] (Number arg1, Number arg2) { return fWSImpl_->minus (arg1, arg2); }})},
              Route{HTTP::MethodsRegEx::kPost, L"times"_RegEx, mkRequestHandler (kTimes, Model::kMapper, Traversal::Iterable<String>{L"arg1", L"arg2"}, function<Number (Number, Number)>{[this] (Number arg1, Number arg2) { return fWSImpl_->times (arg1, arg2); }})},
              Route{HTTP::MethodsRegEx::kPost, L"divide"_RegEx, mkRequestHandler (kDivide, Model::kMapper, Traversal::Iterable<String>{L"arg1", L"arg2"}, function<Number (Number, Number)>{[this] (Number arg1, Number arg2) { return fWSImpl_->divide (arg1, arg2); }})},
              Route{L"test-void-return"_RegEx, mkRequestHandler (WebServiceMethodDescription{}, Model::kMapper, Traversal::Iterable<String>{L"err-if-more-than-10"}, function<void (double)>{[] (double check) {
                                    if (check > 10) {
                                        Execution::Throw (Execution::Exception{L"more than 10"sv});
                                    } }})},

          }
        , fWSImpl_{wsImpl}
        , fConnectionMgr_{SocketAddresses (InternetAddresses_Any (), portNumber), kRoutes_, ConnectionManager::Options{.fDefaultResponseHeaders = kDefaultResponseHeaders_}}
    {
        // @todo - move this to some framework-specific regtests...
        using VariantValue         = DataExchange::VariantValue;
        Sequence<VariantValue> tmp = OrderParamValues (Iterable<String>{"page", "xxx"}, PickoutParamValuesFromURL (URI{"http://www.sophist.com?page=5"}));
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
                kVariables_,
                kPlus_,
                kMinus,
                kTimes,
                kDivide,
            },
            DocsOptions{"Stroika Sample WebService - Web Methods"_k, "Note - curl lines all in bash quoting syntax"_k});
    }
    static void SetAppState_ (Message* message)
    {
        String argsAsString = Streams::TextReader::New (message->rwRequest ().GetBody ()).ReadAll ();
        message->rwResponse ().writeln (L"<html><body><p>Hi SetAppState (" + argsAsString.As<wstring> () + L")</p></body></html>");
        message->rwResponse ().contentType = InternetMediaTypes::kHTML;
    }
};

/*
 *  Documentation on WSAPIs
 */
const WebServiceMethodDescription WebServer::Rep_::kVariables_{
    L"variables"_k,
    Set<String>{HTTP::Methods::kGet, HTTP::Methods::kPost, HTTP::Methods::kDelete},
    InternetMediaTypes::kJSON,
    {},
    Sequence<String>{
        L"curl http://localhost:8080/variables -v --output -",
        L"curl http://localhost:8080/variables/x -v --output -",
        L"curl  -X POST http://localhost:8080/variables/x -v --output -",
        L"curl -H \"Content-Type: application/json\" -X POST -d '{\"value\": 3}' http://localhost:8080/variables/x --output -",
        L"curl -H \"Content-Type: text/plain\" -X POST -d 3 http://localhost:8080/variables/x --output -"},
    Sequence<String>{L"@todo - this is a rough draft (but functional). It could use alot of cleanup and review to see WHICH way I recommend using, and just provide the recommended ways in samples"},
};

const WebServiceMethodDescription WebServer::Rep_::kPlus_{
    L"plus"_k,
    Set<String>{HTTP::Methods::kPost},
    InternetMediaTypes::kJSON,
    {},
    Sequence<String>{
        L"curl -H \"Content-Type: application/json\" -X POST -d '{\"arg1\": 3, \"arg2\": 5 }' http://localhost:8080/plus --output -",
    },
    Sequence<String>{L"add the two argument numbers"},
};
const WebServiceMethodDescription WebServer::Rep_::kMinus{
    L"minus"_k,
    Set<String>{HTTP::Methods::kPost},
    InternetMediaTypes::kJSON,
    {},
    Sequence<String>{
        L"curl -H \"Content-Type: application/json\" -X POST -d '{\"arg1\": 4.5, \"arg2\": -3.23 }' http://localhost:8080/minus --output -",
    },
    Sequence<String>{L"subtract the two argument numbers"},
};
const WebServiceMethodDescription WebServer::Rep_::kTimes{
    L"times"_k,
    Set<String>{HTTP::Methods::kPost},
    InternetMediaTypes::kJSON,
    {},
    Sequence<String>{
        L"curl -H \"Content-Type: application/json\" -X POST -d '{\"arg1\":\"2 + 4i\", \"arg2\": 3.2 }' http://localhost:8080/times --output -",
        L"curl -H \"Content-Type: application/json\" -X POST -d '{\"arg1\":\"2 + i\", \"arg2\": \"2 - i\" }' http://localhost:8080/times --output -",
    },
    Sequence<String>{L"multiply the two argument numbers"},
};
const WebServiceMethodDescription WebServer::Rep_::kDivide{
    L"divide"_k,
    Set<String>{HTTP::Methods::kPost},
    InternetMediaTypes::kJSON,
    {},
    Sequence<String>{
        L"curl -H \"Content-Type: application/json\" -X POST -d '{\"arg1\":\"2 + i\", \"arg2\": 0 }' http://localhost:8080/divide --output -",
    },
    Sequence<String>{L"divide the two argument numbers"},
};

WebServer::WebServer (uint16_t portNumber, const shared_ptr<IWSAPI>& wsImpl)
    : fRep_{make_shared<Rep_> (portNumber, wsImpl)}
{
}