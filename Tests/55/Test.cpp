/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
//  TEST    Frameworks::WebServer
#include "Stroika/Foundation/StroikaPreComp.h"

#include <iostream>

#include "Stroika/Foundation/Common/GUID.h"
#include "Stroika/Foundation/Common/Property.h"
#include "Stroika/Foundation/Containers/KeyedCollection.h"
#include "Stroika/Foundation/Containers/Set.h"
#include "Stroika/Foundation/DataExchange/Compression/Deflate.h"
#include "Stroika/Foundation/DataExchange/InternetMediaTypeRegistry.h"
#include "Stroika/Foundation/DataExchange/JSON/Patch.h"
#include "Stroika/Foundation/DataExchange/ObjectVariantMapper.h"
#include "Stroika/Foundation/DataExchange/Variant/JSON/Reader.h"
#include "Stroika/Foundation/DataExchange/Variant/JSON/Writer.h"
#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/Debug/Visualizations.h"
#include "Stroika/Foundation/Execution/Module.h"
#include "Stroika/Foundation/Execution/RequiredComponentMissingException.h"
#include "Stroika/Foundation/Execution/Synchronized.h"
#include "Stroika/Foundation/IO/Network/HTTP/ClientErrorException.h"
#include "Stroika/Foundation/IO/Network/Transfer/Connection.h"
#include "Stroika/Foundation/Streams/BinaryToText.h"

#include "Stroika/Frameworks/Test/ArchtypeClasses.h"
#include "Stroika/Frameworks/Test/TestHarness.h"
#include "Stroika/Frameworks/WebServer/ConnectionManager.h"
#include "Stroika/Frameworks/WebServer/FileSystemRequestHandler.h"
#include "Stroika/Frameworks/WebServer/Router.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::DataExchange;
using namespace Stroika::Foundation::Execution;
using namespace Stroika::Foundation::Memory;

using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::WebServer;

using Common::GUID;
using Debug::TraceContextBumper;
using IO::Network::HTTP::ClientErrorException;
using Memory::BLOB;
using Time::Duration;

namespace {
    namespace TestDeflateEnc1_ {
        const BLOB kDecoded = "TEST"_blob;
        // Produced with echo -n TEST | openssl zlib -e | od -t x1
        const BLOB kEncoded = "\x78\x9c\x0b\x71\x0d\x0e\x01\x00\x03\x1d\x01\x41"_blob;
    }
}

namespace {
    /*
     *  It's often helpful to structure together, routes, special interceptors, with your connection manager, to package up
     *  all the logic /options for HTTP interface.
     *
     *  This particular organization also makes it easy to save instance variables with the webserver (like a pointer to a handler)
     *  and access them from the Route handler functions.
     */
    struct MyWebServer_ {

        /**
         *  Routes specify the 'handlers' for the various web urls your webserver will support.
         */
        const Sequence<Route> kRoutes_;

        /**
         *  The connectionMgr specifies parameters that govern the behavior of your webserver.
         *  For example, caching settings go here, thread pool settings, network bindings, and so on.
         */
        ConnectionManager fConnectionMgr_;

        optional<HTTP ::TransferEncoding> fUseTransferEncoding_;

        MyWebServer_ (uint16_t portNumber, optional<HTTP::TransferEncoding> transferEncoding)
            : kRoutes_{Route{""_RegEx, [this] (Request& req, Response& res) { DefaultPage_ (req, res); }},
                       Route{"test-chunked-transfer.*"_RegEx, [] (Request& req, Response& res) { TestOptionalTransferChunking_ (req, res); }},
                       Route{HTTP::MethodsRegEx::kPost, "SetAppState"_RegEx, [this] (Message& message) { SetAppState_ (message); }},
                       Route{HTTP::MethodsRegEx::kPost, "SetAppState2"_RegEx, [this] (Message& message) { SetAppState2_ (message); }},
                       Route{"FRED"_RegEx,
                             [] (Request&, Response& response) {
                                 response.contentType = DataExchange::InternetMediaTypes::kText_PLAIN;
                                 response.write ("FRED");
                             }},
                       Route{"TEST"_RegEx,
                             [] (Request&, Response& response) {
                                 response.contentType = DataExchange::InternetMediaTypes::kText_PLAIN;
                                 response.write (TestDeflateEnc1_::kDecoded);
                             }}}
            , fConnectionMgr_{SocketAddresses (InternetAddresses_Any (), portNumber), kRoutes_}
            , fUseTransferEncoding_{transferEncoding}
        {
        }
        // Can declare arguments as Request*,Response*
        void DefaultPage_ (Request&, Response& response)
        {
            //constexpr bool kUseTransferCoding_ = true;
            //            constexpr bool kUseTransferCoding_ = false;
            //          if (kUseTransferCoding_) {
            //            response.rwHeaders ().transferEncoding = HTTP::TransferEncoding::kChunked;
            //      }
            if (fUseTransferEncoding_) {
                response.rwHeaders ().transferEncoding = *fUseTransferEncoding_;
            }
            response.contentType = DataExchange::InternetMediaTypes::kHTML;
            response.writeln ("<html><body>"sv);
            response.writeln ("<p>Hi Mom</p>"sv);
            response.writeln ("<ul>"sv);
            response.writeln ("Run the service (under the debugger if you wish)"sv);
            response.writeln ("<li>curl http://localhost:8080/ OR</li>"sv);
            response.writeln ("<li>curl http://localhost:8080/FRED OR      (to see error handling)</li>"sv);
            response.writeln ("<li>curl -H \"Content-Type: application/json\" -X POST -d '{\"AppState\":\"Start\"}' http://localhost:8080/SetAppState</li>"sv);
            response.writeln ("<li>curl http://localhost:8080/Files/index.html -v</li>"sv);
            response.writeln ("</ul>"sv);
            response.writeln ("</body></html>"sv);
        }
        static void TestOptionalTransferChunking_ (Request& request, Response& response)
        {
            if (request.url ().LookupQueryArg ("useChunked"sv) == "true"sv) {
                response.automaticTransferChunkSize = 25;
            }
            else if (request.url ().LookupQueryArg ("useChunked"sv) == "false"sv) {
                response.automaticTransferChunkSize = Response::kNoChunkedTransfer;
            }
            else {
                // default behavior...
            }
            DbgTrace ("response.automaticTransferChunkSize={}"_f, response.automaticTransferChunkSize ());
            response.contentType = DataExchange::InternetMediaTypes::kHTML;
            response.writeln ("<html><body>"sv);
            response.writeln ("<p>Hi Mom</p>"sv);
            response.writeln ("<ul>"sv);
            response.writeln ("Run the service (under the debugger if you wish)"sv);
            response.writeln ("<li>curl http://localhost:8080/ OR</li>"sv);
            response.writeln ("<li>curl http://localhost:8080/FRED OR      (to see error handling)</li>"sv);
            response.writeln ("<li>curl -H \"Content-Type: application/json\" -X POST -d '{\"AppState\":\"Start\"}' http://localhost:8080/SetAppState</li>"sv);
            response.writeln ("<li>curl http://localhost:8080/Files/index.html -v</li>"sv);
            response.writeln ("</ul>"sv);
            response.writeln ("</body></html>"sv);
        }
        void SetAppState_ (Message& message)
        {
            if (fUseTransferEncoding_) {
                message.rwResponse ().rwHeaders ().transferEncoding = *fUseTransferEncoding_;
            }
            message.rwResponse ().contentType = DataExchange::InternetMediaTypes::kHTML;
            String argsAsString               = Streams::BinaryToText::Reader::New (message.rwRequest ().GetBody ()).ReadAll ();
            message.rwResponse ().writeln ("<html><body><p>Hi SetAppState ("sv + argsAsString + ")</p></body></html>");
        }
        void SetAppState2_ (Message& message)
        {
            if (fUseTransferEncoding_) {
                message.rwResponse ().rwHeaders ().transferEncoding = *fUseTransferEncoding_;
            }
            message.rwResponse ().contentType = DataExchange::InternetMediaTypes::kText_PLAIN;
            String argsAsString               = DataExchange::Variant::JSON::Reader{}
                                                    .Read (message.rwRequest ().GetBody ())
                                                    .As<Mapping<String, DataExchange::VariantValue>> ()
                                                    .LookupChecked ("AppState", Execution::Exception<runtime_error>{"oops"})
                                                    .As<String> ();
            message.rwResponse ().write (argsAsString);
        }
    };
}

#if qStroika_HasComponent_googletest
namespace {
    GTEST_TEST (Frameworks_WebServer, SimpleStartStopServerTest)
    {
        TraceContextBumper ctx{"SimpleStartStopServerTest"};
        const auto         portNumber = 8082;
        const auto         quitAfter  = 1s;
        MyWebServer_       myWebServer{portNumber, nullopt}; // listen and dispatch while this object exists
        WaitableEvent{}.WaitQuietly (quitAfter);             // leave it running for a bit
    }
}

namespace {
    GTEST_TEST (Frameworks_WebServer, SimpleCurlTestTalk2Server)
    {
        TraceContextBumper          ctx{"SimpleCurlTestTalk2Server"};
        const IO::Network::PortType portNumber = 8082;
        MyWebServer_                myWebServer{portNumber, nullopt}; // listen and dispatch while this object exists
        try {
            auto                            c = IO::Network::Transfer::Connection::New ();
            IO::Network::Transfer::Response r = c.GET (URI{"http", URI::Authority{URI::Host{"localhost"}, portNumber}});
            EXPECT_TRUE (r.GetSucceeded ());
            EXPECT_GT (r.GetData ().size (), 1u);
            String response = r.GetDataTextInputStream ().ReadAll ();
            //DbgTrace (L"response={}"_f, response);
            EXPECT_TRUE (response.StartsWith ("<html>"));
            EXPECT_TRUE (response.EndsWith ("</html>\r\n"));
        }
        catch (const RequiredComponentMissingException&) {
            DbgTrace ("ignore RequiredComponentMissingException cuz no IO::Network::Transfer::Connection factory"_f);
        }
    }
}

namespace {
    GTEST_TEST (Frameworks_WebServer, SimpleCurlTestWithChunkedEncodingResponse)
    {
        TraceContextBumper          ctx{"SimpleCurlTestWithChunkedEncodingResponse"};
        const IO::Network::PortType portNumber = 8082;
        MyWebServer_ myWebServer{portNumber, HTTP::TransferEncoding::kChunked}; // listen and dispatch while this object exists
        try {
            auto                            c = IO::Network::Transfer::Connection::New ();
            IO::Network::Transfer::Response r = c.GET (URI{"http", URI::Authority{URI::Host{"localhost"}, portNumber}});
            EXPECT_TRUE (r.GetSucceeded ());
            EXPECT_GT (r.GetData ().size (), 1u);
            //DbgTrace ("headers={}"_f, r.GetHeaders ());
            //DbgTrace ("data=byte[{}]{}"_f, r.GetData ().size (), r.GetData ());
            String response = r.GetDataTextInputStream ().ReadAll ();
            //DbgTrace (L"response={}"_f, response);
            EXPECT_TRUE (response.StartsWith ("<html>"));
            EXPECT_TRUE (response.EndsWith ("</html>\r\n"));
        }
        catch (const RequiredComponentMissingException&) {
            DbgTrace ("ignore RequiredComponentMissingException cuz no IO::Network::Transfer::Connection factory"_f);
        }
    }
}

namespace {
    GTEST_TEST (Frameworks_WebServer, TestPOST)
    {
        TraceContextBumper          ctx{"TestPOST"};
        const IO::Network::PortType portNumber = 8082;
        MyWebServer_                myWebServer{portNumber, nullopt}; // listen and dispatch while this object exists
        try {
            auto c = IO::Network::Transfer::Connection::New ();
            using namespace DataExchange;
            using DataExchange::VariantValue;
            auto                            arg    = VariantValue{Mapping<String, VariantValue>{{"AppState", "Start"}}};
            auto                            toJson = [] (const VariantValue& v) { return Variant::JSON::Writer{}.WriteAsBLOB (v); };
            IO::Network::Transfer::Response r = c.POST (URI{"http", URI::Authority{URI::Host{"localhost"}, portNumber}, "/SetAppState2"sv},
                                                        TypedBLOB{toJson (arg), DataExchange::InternetMediaTypes::kJSON});
            EXPECT_TRUE (r.GetSucceeded ());
            EXPECT_GT (r.GetData ().size (), 1u);
            String response = r.GetDataTextInputStream ().ReadAll ();
            //DbgTrace (L"response={}"_f, response);
            EXPECT_EQ (response, "Start");
        }
        catch (const RequiredComponentMissingException&) {
            DbgTrace ("ignore RequiredComponentMissingException cuz no IO::Network::Transfer::Connection factory"_f);
        }
    }
}

namespace {
    GTEST_TEST (Frameworks_WebServer, TestEncContent)
    {
        TraceContextBumper ctx{"TestEncContent"};
        // @todo add tests with different flags about allowed compression - and add asserts about returned content-encoding headers.
        /// @todo - add tests with different Accept-Encoding headers

        EXPECT_EQ (Compression::Deflate::Compress::New ().Transform (TestDeflateEnc1_::kDecoded), TestDeflateEnc1_::kEncoded);
        const IO::Network::PortType portNumber = 8082;
        MyWebServer_                myWebServer{portNumber, nullopt}; // listen and dispatch while this object exists
        try {
            auto                            c = IO::Network::Transfer::Connection::New ();
            IO::Network::Transfer::Response r = c.GET (URI{"http", URI::Authority{URI::Host{"localhost"}, portNumber}, "/TEST"sv});
            EXPECT_TRUE (r.GetSucceeded ());
            EXPECT_GT (r.GetData ().size (), 1u);
            String response = r.GetDataTextInputStream ().ReadAll ();
            //DbgTrace (L"response={}"_f, response);
            EXPECT_EQ (response, "TEST");
            // @todo enhance this test so we force accept-encoding none, and force accept-endcing : deflate, and check raw
            // result???
        }
        catch (const RequiredComponentMissingException&) {
            DbgTrace ("ignore RequiredComponentMissingException cuz no IO::Network::Transfer::Connection factory"_f);
        }
    }
}

namespace {
    GTEST_TEST (Frameworks_WebServer, TestChunkedTransfer_)
    {
        TraceContextBumper ctx{"TestChunkedTransfer_"};

        // @todo add tests with different flags about allowed compression - and add asserts about returned content-encoding headers.
        const IO::Network::PortType portNumber = 8082;
        MyWebServer_                myWebServer{portNumber, nullopt}; // listen and dispatch while this object exists
        try {
            auto c = IO::Network::Transfer::Connection::New ();
            {
                IO::Network::Transfer::Response r =
                    c.GET (URI{"http", URI::Authority{URI::Host{"localhost"}, portNumber}, "/test-chunked-transfer"sv});
                EXPECT_TRUE (r.GetSucceeded ());
                EXPECT_GT (r.GetData ().size (), 100u);
                //DbgTrace ("respheaders={}"_f, r.GetHeaders ());
            }
            {
                c = IO::Network::Transfer::Connection::New ();
                IO::Network::Transfer::Response r =
                    c.GET (URI{"http", URI::Authority{URI::Host{"localhost"}, portNumber}, "/test-chunked-transfer", "useChunked=true"sv});
                EXPECT_TRUE (r.GetSucceeded ());
                //DbgTrace ("respheaders={}"_f, r.GetHeaders ());
                EXPECT_TRUE (r.GetHeaders ().LookupValue (IO ::Network::HTTP::HeaderName::kTransferEncoding).Contains ("chunked"));
                EXPECT_GT (r.GetData ().size (), 100u);
            }
            {
                IO::Network::Transfer::Response r =
                    c.GET (URI{"http", URI::Authority{URI::Host{"localhost"}, portNumber}, "/test-chunked-transfer", "useChunked=false"sv});
                //DbgTrace ("respheaders={}"_f, r.GetHeaders ());
                EXPECT_TRUE (r.GetSucceeded ());
                EXPECT_EQ (r.GetHeaders ().Lookup (IO ::Network::HTTP::HeaderName::kTransferEncoding), nullopt);
                EXPECT_GT (r.GetData ().size (), 100u);
            }
        }
        catch (const RequiredComponentMissingException&) {
            DbgTrace ("ignore RequiredComponentMissingException cuz no IO::Network::Transfer::Connection factory"_f);
        }
    }
}

namespace {
    // a server on loopback with routes, and a request to it with headers - its response
    constexpr IO::Network::PortType kRouterTestPort_ = 8082;
    IO::Network::Transfer::Response Ask_ (const String& method, const String& path, const Mapping<String, String>& headers)
    {
        auto c = IO::Network::Transfer::Connection::New ();
        c.SetSchemeAndAuthority (URI{"http", URI::Authority{URI::Host{"localhost"}, kRouterTestPort_}});
        IO::Network::Transfer::Request r;
        r.fMethod               = method;
        r.fAuthorityRelativeURL = URI{path};
        r.fOverrideHeaders      = headers;
        return c.Send (r);
    }
    const Route kFRED_{"FRED"_RegEx, [] (Request&, Response& response) {
                           response.contentType = DataExchange::InternetMediaTypes::kText_PLAIN;
                           response.write ("FRED");
                       }};

    /*
     *  A handler given the handled flag - and the request and response, and the URL's matches - leaves it to the handler: one that
     *  declines (handled = false) has the next route matching the request handle it. Before Stroika v3.0d25 that overload did
     *  not compile.
     */
    GTEST_TEST (Frameworks_WebServer, Router_HandledFlag_)
    {
        TraceContextBumper ctx{"Router_HandledFlag_"};
        auto               writeText = [] (Response& response, const String& text) {
            response.contentType = DataExchange::InternetMediaTypes::kText_PLAIN;
            response.write (text);
        };
        ConnectionManager server{
            SocketAddresses (InternetAddresses_Any (), kRouterTestPort_),
            Sequence<Route>{Route{"decline"_RegEx, [] (Request&, Response&, const Sequence<String>&, bool& handled) { handled = false; }},
                            Route{"decline"_RegEx, [&] (Request&, Response& response) { writeText (response, "the next route"sv); }},
                            Route{"take/(.+)"_RegEx, [&] (Request&, Response& response, const Sequence<String>& matches, bool& handled) {
                                      writeText (response, matches[0]);
                                      handled = true;
                                  }}}};
        try {
            EXPECT_EQ (Ask_ (IO::Network::HTTP::Methods::kGet, "/decline"sv, {}).GetDataTextInputStream ().ReadAll (), "the next route"sv);
            EXPECT_EQ (Ask_ (IO::Network::HTTP::Methods::kGet, "/take/abc"sv, {}).GetDataTextInputStream ().ReadAll (), "abc"sv);
        }
        catch (const RequiredComponentMissingException&) {
            DbgTrace ("ignore RequiredComponentMissingException cuz no IO::Network::Transfer::Connection factory"_f);
        }
    }

    /*
     *  A CORS preflight asking for headers, with CORSOptions::fAllowedHeaders a list: Access-Control-Allow-Headers gives those of
     *  them allowed - and is not sent if none is. Before Stroika v3.0d25 that test was inverted: none sent when some were allowed.
     */
    GTEST_TEST (Frameworks_WebServer, CORS_AllowedHeaders_)
    {
        TraceContextBumper ctx{"CORS_AllowedHeaders_"};
        using namespace IO::Network::HTTP;
        ConnectionManager server{SocketAddresses (InternetAddresses_Any (), kRouterTestPort_), Sequence<Route>{kFRED_},
                                 ConnectionManager::Options{.fCORS = CORSOptions{.fAllowedHeaders = Set<String>{"X-Custom"sv}}}};
        try {
            auto preflight = [] (const String& requestHeaders) {
                return Ask_ (Methods::kOptions, "/FRED"sv,
                             {{String{HeaderName::kOrigin}, "http://example.com"sv},
                              {"Access-Control-Request-Method"sv, String{Methods::kGet}},
                              {String{HeaderName::kAccessControlRequestHeaders}, requestHeaders}})
                    .GetHeaders ();
            };
            EXPECT_EQ (preflight ("X-Custom"sv).Lookup (String{HeaderName::kAccessControlAllowHeaders}), "X-Custom"sv);
            EXPECT_EQ (preflight ("X-Other"sv).Lookup (String{HeaderName::kAccessControlAllowHeaders}), nullopt);
        }
        catch (const RequiredComponentMissingException&) {
            DbgTrace ("ignore RequiredComponentMissingException cuz no IO::Network::Transfer::Connection factory"_f);
        }
    }

    /*
     *  CORS with credentials (Fetch, "CORS protocol and credentials"): an origin allowed by name is answered with itself and
     *  Access-Control-Allow-Credentials: true - on an ordinary response as on a preflight; any origin allowed (*) gets *, and never
     *  Allow-Credentials, which browsers refuse beside *; with credentials not allowed, Allow-Credentials is never sent (its only
     *  value is true). Before Stroika v3.0d25 an ordinary response never sent it, a preflight sent true beside *, and false.
     */
    GTEST_TEST (Frameworks_WebServer, CORS_Credentials_)
    {
        TraceContextBumper ctx{"CORS_Credentials_"};
        using namespace IO::Network::HTTP;
        const String kOrigin_{"http://example.com"sv};
        auto         ask = [&] (const CORSOptions& cors, const String& method) {
            ConnectionManager       server{SocketAddresses (InternetAddresses_Any (), kRouterTestPort_), Sequence<Route>{kFRED_},
                                           ConnectionManager::Options{.fCORS = cors}};
            Mapping<String, String> headers{{String{HeaderName::kOrigin}, kOrigin_}};
            if (method == Methods::kOptions) {
                headers.Add ("Access-Control-Request-Method"sv, String{Methods::kGet});
            }
            Mapping<String, String> r = Ask_ (method, "/FRED"sv, headers).GetHeaders ();
            return make_pair (r.Lookup (String{HeaderName::kAccessControlAllowOrigin}), r.Lookup (String{HeaderName::kAccessControlAllowCredentials}));
        };
        try {
            const CORSOptions kByName_{.fAllowCredentials = true, .fAllowedOrigins = Set<String>{kOrigin_}};
            const CORSOptions kAny_{.fAllowCredentials = true, .fAllowedOrigins = Set<String>{CORSOptions::kAccessControlWildcard}};
            const CORSOptions kByNameNoCredentials_{.fAllowCredentials = false, .fAllowedOrigins = Set<String>{kOrigin_}};
            for (const String& method : {String{Methods::kGet}, String{Methods::kOptions}}) {
                EXPECT_EQ (ask (kByName_, method), make_pair (optional<String>{kOrigin_}, optional<String>{"true"sv})) << method.AsNarrowSDKString ();
                EXPECT_EQ (ask (kAny_, method), make_pair (optional<String>{String{CORSOptions::kAccessControlWildcard}}, optional<String>{}))
                    << method.AsNarrowSDKString ();
                EXPECT_EQ (ask (kByNameNoCredentials_, method), make_pair (optional<String>{kOrigin_}, optional<String>{}))
                    << method.AsNarrowSDKString ();
            }
        }
        catch (const RequiredComponentMissingException&) {
            DbgTrace ("ignore RequiredComponentMissingException cuz no IO::Network::Transfer::Connection factory"_f);
        }
    }
}
#endif

int main (int argc, const char* argv[])
{
    Test::Setup (argc, argv);
#if qStroika_HasComponent_googletest
    return RUN_ALL_TESTS ();
#else
    cerr << "[  SKIPPED ] every test - Stroika regression tests require building with google test feature" << endl;
#endif
}
