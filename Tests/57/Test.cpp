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
#include "Stroika/Foundation/Execution/Finally.h"
#include "Stroika/Foundation/Execution/Module.h"
#include "Stroika/Foundation/Execution/RequiredComponentMissingException.h"
#include "Stroika/Foundation/Execution/SignalHandlers.h"
#include "Stroika/Foundation/Execution/Sleep.h"
#include "Stroika/Foundation/Execution/Synchronized.h"
#include "Stroika/Foundation/Execution/ThreadPool.h"
#include "Stroika/Foundation/Execution/WaitableEvent.h"
#include "Stroika/Foundation/IO/Network/HTTP/ClientErrorException.h"
#include "Stroika/Foundation/IO/Network/Transfer/Connection.h"
#include "Stroika/Foundation/Streams/BinaryToText.h"

#include "Stroika/Frameworks/Test/ArchtypeClasses.h"
#include "Stroika/Frameworks/Test/TestHarness.h"
#include "Stroika/Frameworks/WebServer/ConnectionManager.h"
#include "Stroika/Frameworks/WebServer/FileSystemRequestHandler.h"
#include "Stroika/Frameworks/WebServer/Router.h"

using std::byte;

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
            //DbgTrace ("response={}"_f, response);
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
            //DbgTrace ("response={}"_f, response);
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
            //DbgTrace ("response={}"_f, response);
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

        if constexpr (Compression::Deflate::kSupported) { // without zlib the server just sends the body uncompressed
            EXPECT_EQ (Compression::Deflate::Compress::New ().Transform (TestDeflateEnc1_::kDecoded), TestDeflateEnc1_::kEncoded);
        }
        const IO::Network::PortType portNumber = 8082;
        MyWebServer_                myWebServer{portNumber, nullopt}; // listen and dispatch while this object exists
        try {
            auto                            c = IO::Network::Transfer::Connection::New ();
            IO::Network::Transfer::Response r = c.GET (URI{"http", URI::Authority{URI::Host{"localhost"}, portNumber}, "/TEST"sv});
            EXPECT_TRUE (r.GetSucceeded ());
            EXPECT_GT (r.GetData ().size (), 1u);
            String response = r.GetDataTextInputStream ().ReadAll ();
            //DbgTrace ("response={}"_f, response);
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
                EXPECT_TRUE (r.GetHeaders ().LookupOne (String{IO::Network::HTTP::HeaderName::kTransferEncoding}).value_or (String{}).Contains ("chunked"));
                EXPECT_GT (r.GetData ().size (), 100u);
            }
            {
                IO::Network::Transfer::Response r =
                    c.GET (URI{"http", URI::Authority{URI::Host{"localhost"}, portNumber}, "/test-chunked-transfer", "useChunked=false"sv});
                //DbgTrace ("respheaders={}"_f, r.GetHeaders ());
                EXPECT_TRUE (r.GetSucceeded ());
                EXPECT_EQ (r.GetHeaders ().LookupOne (String{IO::Network::HTTP::HeaderName::kTransferEncoding}), nullopt);
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
        r.fOverrideHeaders      = IO::Network::HTTP::Headers{headers};
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
            EXPECT_EQ (preflight ("X-Custom"sv).LookupOne (String{HeaderName::kAccessControlAllowHeaders}), "X-Custom"sv);
            EXPECT_EQ (preflight ("X-Other"sv).LookupOne (String{HeaderName::kAccessControlAllowHeaders}), nullopt);
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
            Mapping<String, String> r = Ask_ (method, "/FRED"sv, headers).GetHeaders ().As<Mapping<String, String>> ();
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

    /*
     *  A server that has fallen behind - every handler thread busy, and the queue full - answers a further request 503 (Service
     *  Unavailable) at once, and still serves those it took. Before Stroika v3.0d26 that request was never answered, its
     *  connection left open.
     */
    GTEST_TEST (Frameworks_WebServer, BusyServerAnswers503_)
    {
        TraceContextBumper ctx{"BusyServerAnswers503_"};
        WaitableEvent      handlerEntered;
        WaitableEvent      releaseHandlers;
        // one handler thread, so a queue of one: the first request runs, the second waits, the third finds no room
        ConnectionManager server{SocketAddresses (InternetAddresses_Any (), kRouterTestPort_),
                                 Sequence<Route>{Route{"slow"_RegEx,
                                                       [&] (Request&, Response& response) {
                                                           handlerEntered.Set ();
                                                           releaseHandlers.Wait ();
                                                           response.contentType = DataExchange::InternetMediaTypes::kText_PLAIN;
                                                           response.write ("done"sv);
                                                       }}},
                                 ConnectionManager::Options{.fMaxConcurrentlyHandledConnections = 1}};
        auto              askStatus = [] () {
            auto c = IO::Network::Transfer::Connection::New ();
            c.SetTimeout (10s); // a request never answered fails the test, not hangs it
            c.SetSchemeAndAuthority (URI{"http", URI::Authority{URI::Host{"localhost"}, kRouterTestPort_}});
            IO::Network::Transfer::Request r;
            r.fMethod               = IO::Network::HTTP::Methods::kGet;
            r.fAuthorityRelativeURL = URI{"/slow"sv};
            return c.Send (r).GetStatus ();
        };
        try {
            (void)IO::Network::Transfer::Connection::New (); // here, not in a client task: so a build with no HTTP client says so at once
            ThreadPool              clients{ThreadPool::Options{.fThreadCount = 2}};
            [[maybe_unused]] auto&& cleanup = Finally ([&] () noexcept { releaseHandlers.Set (); }); // before ~clients waits for them
            auto                    first   = clients.AddTaskWithResult (askStatus);
            handlerEntered.Wait (10s);
            auto second = clients.AddTaskWithResult (askStatus);
            for (Time::TimePointSeconds giveUpAt = Time::GetTickCount () + 10s; server.statistics ().fConnections.fNumberOfActiveConnections < 2;) {
                ASSERT_LT (Time::GetTickCount (), giveUpAt) << "the second request never reached the server's queue";
                Execution::Sleep (10ms);
            }
            EXPECT_EQ (askStatus (), IO::Network::HTTP::StatusCodes::kServiceUnavailable);
            EXPECT_EQ (server.statistics ().fConnections.fRequestsRejected, 1u);
            releaseHandlers.Set ();
            EXPECT_EQ (first.GetResult (10s), IO::Network::HTTP::StatusCodes::kOK);
            EXPECT_EQ (second.GetResult (10s), IO::Network::HTTP::StatusCodes::kOK);
        }
        catch (const RequiredComponentMissingException&) {
            DbgTrace ("ignore RequiredComponentMissingException cuz no IO::Network::Transfer::Connection factory"_f);
        }
    }

    // a connection to the test server, on loopback
    IO::Network::ConnectionOrientedStreamSocket::Ptr ConnectToTestServer_ ()
    {
        auto s = IO::Network::ConnectionOrientedStreamSocket::New (IO::Network::SocketAddress::INET);
        s.Connect (IO::Network::SocketAddress{IO::Network::V4::kLocalhost, kRouterTestPort_});
        return s;
    }

    // what the server sent on s before closing it - or nullopt if s is still open after 10 seconds
    optional<string> ReadUntilClosed_ (const IO::Network::ConnectionOrientedStreamSocket::Ptr& s)
    {
        string received;
        for (Time::TimePointSeconds giveUpAt = Time::GetTickCount () + 10s; Time::GetTickCount () < giveUpAt;) {
            byte buf[1024];
            try {
                if (optional<span<byte>> r = s.ReadNonBlocking (span{buf})) {
                    if (r->empty ()) {
                        return received; // end of stream
                    }
                    received.append (reinterpret_cast<const char*> (r->data ()), r->size ());
                    continue;
                }
            }
            catch (...) {
                return received; // reset: closed too
            }
            Execution::Sleep (50ms);
        }
        return nullopt;
    }

    // what the server sent on s, once it ends with suffix - or nullopt if s closes first, or 10 seconds pass
    optional<string> ReadUntilEndsWith_ (const IO::Network::ConnectionOrientedStreamSocket::Ptr& s, string_view suffix)
    {
        string received;
        for (Time::TimePointSeconds giveUpAt = Time::GetTickCount () + 10s; Time::GetTickCount () < giveUpAt;) {
            byte buf[1024];
            try {
                if (optional<span<byte>> r = s.ReadNonBlocking (span{buf})) {
                    if (r->empty ()) {
                        return nullopt; // closed
                    }
                    received.append (reinterpret_cast<const char*> (r->data ()), r->size ());
                    if (received.ends_with (suffix)) {
                        return received;
                    }
                    continue;
                }
            }
            catch (...) {
                return nullopt; // reset: closed too
            }
            Execution::Sleep (10ms);
        }
        return nullopt;
    }

    /*
     *  A server restarts at once on its port, though it was stopped with a client connected: it closed that connection first,
     *  which leaves its end in TIME_WAIT for a minute or so - and meanwhile POSIX refuses to bind the port without SO_REUSEADDR.
     *  Before Stroika v3.0d26 the restart failed there: address already in use.
     */
    GTEST_TEST (Frameworks_WebServer, ServerRestartsAtOnce_)
    {
        TraceContextBumper ctx{"ServerRestartsAtOnce_"};
        auto               start = [] () {
            return make_unique<ConnectionManager> (SocketAddresses (InternetAddresses_Any (), kRouterTestPort_), Sequence<Route>{kFRED_});
        };
        auto server = start ();
        auto client = ConnectToTestServer_ ();
        for (Time::TimePointSeconds giveUpAt = Time::GetTickCount () + 10s; server->statistics ().fConnections.fNumberOfOpenConnections < 1;) {
            ASSERT_LT (Time::GetTickCount (), giveUpAt) << "the server never took the connection";
            Execution::Sleep (10ms);
        }
        server.reset (); // closes the client's connection - first
        client.Close ();
        EXPECT_NO_THROW (start ());
    }

    /*
     *  The server closes a connection idle past Options::fIdleConnectionTimeout - one that never sent a request, and one kept
     *  alive after one. Before Stroika v3.0d26 a connection stayed open until its client closed it, or the ConnectionManager went.
     */
    GTEST_TEST (Frameworks_WebServer, IdleConnectionsClosed_)
    {
        TraceContextBumper ctx{"IdleConnectionsClosed_"};
        namespace ConnectionOrientedStreamSocket = IO::Network::ConnectionOrientedStreamSocket;
        ConnectionManager                   server{SocketAddresses (InternetAddresses_Any (), kRouterTestPort_), Sequence<Route>{kFRED_},
                                                   ConnectionManager::Options{.fIdleConnectionTimeout = 1s}};
        ConnectionOrientedStreamSocket::Ptr silent    = ConnectToTestServer_ ();
        ConnectionOrientedStreamSocket::Ptr keptAlive = ConnectToTestServer_ ();
        keptAlive.Write (as_bytes (span{"GET /FRED HTTP/1.1\r\nHost: localhost\r\n\r\n"sv}));
        EXPECT_EQ (ReadUntilClosed_ (silent), string{});
        optional<string> response = ReadUntilClosed_ (keptAlive);
        ASSERT_TRUE (response) << "the kept-alive connection is still open";
        EXPECT_TRUE (String::FromLatin1 (*response).Contains ("keep-alive"sv, eCaseInsensitive)) << *response; // so closed idle, not after its response
        EXPECT_TRUE (response->ends_with ("FRED")) << *response;
        EXPECT_EQ (server.statistics ().fConnections.fNumberOfOpenConnections, 0u);
    }

    /*
     *  A request that does not ask to keep its connection - "Connection: close", or HTTP/1.0 without "Connection: keep-alive" -
     *  is answered, and its connection closed at once, not left for the idle timeout. Before Stroika v3.0d26 both were kept
     *  alive: the request line's CRLF stayed on its HTTP version ("1.1\r\n"), which then matched neither 1.0 nor 1.1.
     */
    GTEST_TEST (Frameworks_WebServer, ConnectionCloseHonored_)
    {
        TraceContextBumper ctx{"ConnectionCloseHonored_"};
        ConnectionManager  server{SocketAddresses (InternetAddresses_Any (), kRouterTestPort_), Sequence<Route>{kFRED_}};
        for (string_view request : {"GET /FRED HTTP/1.1\r\nHost: localhost\r\nConnection: close\r\n\r\n"sv, "GET /FRED HTTP/1.0\r\n\r\n"sv}) {
            IO::Network::ConnectionOrientedStreamSocket::Ptr client = ConnectToTestServer_ ();
#if qCompilerAndStdLib_span_requires_explicit_type_for_BLOBCVT_Buggy
            client.Write (as_bytes (span<const char>{request}));
#else
            client.Write (as_bytes (span{request}));
#endif
            optional<string> response = ReadUntilClosed_ (client);
            ASSERT_TRUE (response) << "the server left the connection open, after: " << request;
            EXPECT_TRUE (String::FromLatin1 (*response).Contains ("Connection: close"sv, eCaseInsensitive)) << *response;
            EXPECT_TRUE (response->ends_with ("FRED")) << *response;
        }
    }

    /*
     *  A request whose header arrives in pieces - over a slow link, or from a client that writes its request line and its
     *  header lines separately - is answered. Before Stroika v3.0d26 the pieces before the last were lost: the server read
     *  them, found the header incomplete, and on the next piece began a new request with it.
     */
    GTEST_TEST (Frameworks_WebServer, RequestHeaderInPieces_)
    {
        TraceContextBumper ctx{"RequestHeaderInPieces_"};
        ConnectionManager  server{SocketAddresses (InternetAddresses_Any (), kRouterTestPort_), Sequence<Route>{kFRED_}};
        IO::Network::ConnectionOrientedStreamSocket::Ptr client = ConnectToTestServer_ ();
        for (string_view piece : {"GET /FRED HT"sv, "TP/1.1\r\nHost: local"sv, "host\r\n"sv, "\r\n"sv}) {
#if qCompilerAndStdLib_span_requires_explicit_type_for_BLOBCVT_Buggy
            client.Write (as_bytes (span<const char>{piece}));
#else
            client.Write (as_bytes (span{piece}));
#endif
            Execution::Sleep (100ms); // so the server reads each piece on its own
        }
        optional<string> response = ReadUntilEndsWith_ (client, "FRED"sv);
        ASSERT_TRUE (response) << "no answer to a request whose header came in pieces";
        EXPECT_TRUE (response->starts_with ("HTTP/1.1 200")) << *response;
    }

    /*
     *  A request line whose URL has no path - only a query or fragment mark - is answered 400 (Bad Request): a request-target
     *  is "/...", "*" or an absolute URI.
     */
    GTEST_TEST (Frameworks_WebServer, RequestURLWithoutPath_)
    {
        TraceContextBumper ctx{"RequestURLWithoutPath_"};
        ConnectionManager  server{SocketAddresses (InternetAddresses_Any (), kRouterTestPort_), Sequence<Route>{kFRED_}};
        for (string_view request : {"GET ? HTTP/1.1\r\nHost: localhost\r\nConnection: close\r\n\r\n"sv,
                                    "GET # HTTP/1.1\r\nHost: localhost\r\nConnection: close\r\n\r\n"sv}) {
            IO::Network::ConnectionOrientedStreamSocket::Ptr client = ConnectToTestServer_ ();
#if qCompilerAndStdLib_span_requires_explicit_type_for_BLOBCVT_Buggy
            client.Write (as_bytes (span<const char>{request}));
#else
            client.Write (as_bytes (span{request}));
#endif
            optional<string> response = ReadUntilClosed_ (client);
            ASSERT_TRUE (response) << "the server left the connection open, after: " << request;
            EXPECT_TRUE (response->starts_with ("HTTP/1.1 400")) << *response;
        }
    }

    /*
     *  A malformed request - its request line, a header line, or its HTTP version - is answered 400 (Bad Request), and its
     *  connection closed.
     */
    GTEST_TEST (Frameworks_WebServer, MalformedRequest_)
    {
        TraceContextBumper ctx{"MalformedRequest_"};
        ConnectionManager  server{SocketAddresses (InternetAddresses_Any (), kRouterTestPort_), Sequence<Route>{kFRED_}};
        for (string_view request : {"GARBAGE\r\nHost: localhost\r\n\r\n"sv, "GET /FRED HTTP/1.1\r\nHost localhost\r\n\r\n"sv,
                                    "GET /FRED HTTP/\r\nHost: localhost\r\n\r\n"sv, "GET /FRED HTTP/one\r\nHost: localhost\r\n\r\n"sv}) {
            IO::Network::ConnectionOrientedStreamSocket::Ptr client = ConnectToTestServer_ ();
#if qCompilerAndStdLib_span_requires_explicit_type_for_BLOBCVT_Buggy
            client.Write (as_bytes (span<const char>{request}));
#else
            client.Write (as_bytes (span{request}));
#endif
            optional<string> response = ReadUntilClosed_ (client);
            ASSERT_TRUE (response) << "the server left the connection open, after: " << request;
            EXPECT_TRUE (response->starts_with ("HTTP/1.1 400")) << request << " -> " << *response;
            EXPECT_TRUE (String::FromLatin1 (*response).Contains ("Connection: close"sv, eCaseInsensitive)) << *response;
        }
    }

    /*
     *  A client that resets its connection with its request's body unread - the server reading the rest of it, so the
     *  connection's next request starts after it - has the connection closed, and the server goes on serving.
     */
    GTEST_TEST (Frameworks_WebServer, ClientResetMidBody_)
    {
        TraceContextBumper ctx{"ClientResetMidBody_"};
        WaitableEvent      handlerEntered;
        WaitableEvent      clientGone;
        Route              ignoreBody{IO::Network::HTTP::MethodsRegEx::kPost, "IGNORE"_RegEx, [&] (Request&, Response& response) {
                             handlerEntered.Set ();
                             clientGone.Wait (10s);
                             Execution::Sleep (100ms); // so its reset has come
                             response.write ("IGNORED");
                                      }};
        ConnectionManager  server{SocketAddresses (InternetAddresses_Any (), kRouterTestPort_), Sequence<Route>{kFRED_, ignoreBody}};
        {
            IO::Network::ConnectionOrientedStreamSocket::Ptr client = ConnectToTestServer_ ();
            client.SetLinger (0); // so closing it resets the connection (RST)
            client.Write (as_bytes (span{"POST /IGNORE HTTP/1.1\r\nHost: localhost\r\nContent-Length: 10\r\n\r\nabc"sv})); // and never the rest
            handlerEntered.Wait (10s);
            client.Close ();
            clientGone.Set ();
        }
        IO::Network::ConnectionOrientedStreamSocket::Ptr next = ConnectToTestServer_ ();
        next.Write (as_bytes (span{"GET /FRED HTTP/1.1\r\nHost: localhost\r\n\r\n"sv}));
        EXPECT_TRUE (ReadUntilEndsWith_ (next, "FRED"sv).has_value ()) << "the server stopped serving";
    }

    /*
     *  A request whose header has not all come Options::fRequestHeaderTimeout after it began has its connection closed, with
     *  no response.
     */
    GTEST_TEST (Frameworks_WebServer, RequestHeaderTimeout_)
    {
        TraceContextBumper ctx{"RequestHeaderTimeout_"};
        ConnectionManager  server{SocketAddresses (InternetAddresses_Any (), kRouterTestPort_), Sequence<Route>{kFRED_},
                                  ConnectionManager::Options{.fRequestHeaderTimeout = 1s}};
        IO::Network::ConnectionOrientedStreamSocket::Ptr client = ConnectToTestServer_ ();
        Time::TimePointSeconds                           start  = Time::GetTickCount ();
        client.Write (as_bytes (span{"GET /FRED HTTP/1.1\r\nHost: local"sv})); // and never the rest
        EXPECT_EQ (ReadUntilClosed_ (client), string{}) << "the connection was left open, its request's header incomplete";
        EXPECT_GE ((Time::GetTickCount () - start).count (), 0.9) << "closed before its header's timeout";
    }

    /*
     *  A request whose body stops coming has its connection closed Options::fClientStallTimeout after the last of it came, and
     *  the thread handling it freed.
     */
    GTEST_TEST (Frameworks_WebServer, ClientStall_RequestBody_)
    {
        TraceContextBumper ctx{"ClientStall_RequestBody_"};
        atomic<bool>       handlerDone{false};
        Route              readBody{IO::Network::HTTP::MethodsRegEx::kPost, "BODY"_RegEx, [&] (Request& request, Response&) {
                           [[maybe_unused]] auto&& done = Finally ([&] () noexcept { handlerDone = true; });
                           (void)request.GetBody ();
                                    }};
        ConnectionManager  server{SocketAddresses (InternetAddresses_Any (), kRouterTestPort_), Sequence<Route>{readBody},
                                  ConnectionManager::Options{.fClientStallTimeout = 1s}};
        IO::Network::ConnectionOrientedStreamSocket::Ptr client = ConnectToTestServer_ ();
        client.Write (as_bytes (span{"POST /BODY HTTP/1.1\r\nHost: localhost\r\nContent-Length: 10\r\n\r\nabc"sv})); // and never the rest
        EXPECT_TRUE (ReadUntilClosed_ (client).has_value ()) << "the connection was left open, its request's body stalled";
        EXPECT_TRUE (handlerDone) << "the handler is still reading the body";
    }

    /*
     *  A response its client stops taking has its connection closed Options::fClientStallTimeout after the client last took
     *  some, and the thread handling it freed.
     */
    GTEST_TEST (Frameworks_WebServer, ClientStall_Response_)
    {
        TraceContextBumper ctx{"ClientStall_Response_"};
        atomic<bool>       handlerDone{false};
        atomic<bool>       wroteAll{false};
        Route              big{"BIG"_RegEx, [&] (Request&, Response& response) {
                      [[maybe_unused]] auto&&   done = Finally ([&] () noexcept { handlerDone = true; });
                      static const vector<byte> kMegabyte_ (1024 * 1024);
                      for (int i = 0; i < 256; ++i) { // far more than the sockets' buffers hold
                          response.write (span{kMegabyte_});
                      }
                      wroteAll = true;
                               }};
        ConnectionManager  server{SocketAddresses (InternetAddresses_Any (), kRouterTestPort_), Sequence<Route>{big},
                                  ConnectionManager::Options{.fClientStallTimeout = 1s}};
        IO::Network::ConnectionOrientedStreamSocket::Ptr client = ConnectToTestServer_ ();
        client.Write (as_bytes (span{"GET /BIG HTTP/1.1\r\nHost: localhost\r\n\r\n"sv})); // and never read the response
        for (Time::TimePointSeconds giveUpAt = Time::GetTickCount () + 10s; not handlerDone;) {
            ASSERT_LT (Time::GetTickCount (), giveUpAt) << "the handler is still writing a response its client stopped taking";
            Execution::Sleep (10ms);
        }
        EXPECT_FALSE (wroteAll);
    }

    /*
     *  A request's Keep-Alive: max=N limits its connection to N requests: the Nth is answered "Connection: close", and the
     *  connection closed.
     */
    GTEST_TEST (Frameworks_WebServer, KeepAliveMax_)
    {
        TraceContextBumper ctx{"KeepAliveMax_"};
        ConnectionManager  server{SocketAddresses (InternetAddresses_Any (), kRouterTestPort_), Sequence<Route>{kFRED_}};
        IO::Network::ConnectionOrientedStreamSocket::Ptr client = ConnectToTestServer_ ();
        client.Write (as_bytes (span{"GET /FRED HTTP/1.1\r\nHost: localhost\r\nKeep-Alive: max=2\r\n\r\n"sv}));
        optional<string> first = ReadUntilEndsWith_ (client, "FRED"sv);
        ASSERT_TRUE (first);
        EXPECT_TRUE (String::FromLatin1 (*first).Contains ("Connection: keep-alive"sv, eCaseInsensitive)) << *first;
        client.Write (as_bytes (span{"GET /FRED HTTP/1.1\r\nHost: localhost\r\nKeep-Alive: max=2\r\n\r\n"sv}));
        optional<string> second = ReadUntilClosed_ (client);
        ASSERT_TRUE (second) << "the connection was left open after its last request";
        EXPECT_TRUE (String::FromLatin1 (*second).Contains ("Connection: close"sv, eCaseInsensitive)) << *second;
        EXPECT_TRUE (second->ends_with ("FRED")) << *second;
    }

    /*
     *  A request's Keep-Alive: timeout=T limits how long its connection may stay idle after it: closed sooner than
     *  Options::fIdleConnectionTimeout would.
     */
    GTEST_TEST (Frameworks_WebServer, KeepAliveTimeout_)
    {
        TraceContextBumper ctx{"KeepAliveTimeout_"};
        ConnectionManager  server{SocketAddresses (InternetAddresses_Any (), kRouterTestPort_), Sequence<Route>{kFRED_},
                                  ConnectionManager::Options{.fIdleConnectionTimeout = 6s}}; // swept every 3 seconds
        IO::Network::ConnectionOrientedStreamSocket::Ptr client = ConnectToTestServer_ ();
        client.Write (as_bytes (span{"GET /FRED HTTP/1.1\r\nHost: localhost\r\nKeep-Alive: timeout=1\r\n\r\n"sv}));
        ASSERT_TRUE (ReadUntilEndsWith_ (client, "FRED"sv).has_value ());
        Time::TimePointSeconds answered = Time::GetTickCount ();
        EXPECT_EQ (ReadUntilClosed_ (client), string{}) << "the connection was left open";
        EXPECT_LT ((Time::GetTickCount () - answered).count (), 5.0) << "closed only by the server's own idle timeout";
    }

    /*
     *  A kept-alive response says when its connection will be closed idle (Keep-Alive: timeout=, from
     *  Options::fIdleConnectionTimeout), and - a request's Keep-Alive having set max= - how many requests it may yet serve. A
     *  response closing its connection says neither.
     */
    GTEST_TEST (Frameworks_WebServer, KeepAliveAdvertised_)
    {
        TraceContextBumper ctx{"KeepAliveAdvertised_"};
        ConnectionManager  server{SocketAddresses (InternetAddresses_Any (), kRouterTestPort_), Sequence<Route>{kFRED_},
                                  ConnectionManager::Options{.fIdleConnectionTimeout = 30s}};
        IO::Network::ConnectionOrientedStreamSocket::Ptr client = ConnectToTestServer_ ();
        client.Write (as_bytes (span{"GET /FRED HTTP/1.1\r\nHost: localhost\r\n\r\n"sv}));
        optional<string> first = ReadUntilEndsWith_ (client, "FRED"sv);
        ASSERT_TRUE (first);
        EXPECT_TRUE (String::FromLatin1 (*first).Contains ("Keep-Alive: timeout=30\r\n"sv, eCaseInsensitive)) << *first;
        client.Write (as_bytes (span{"GET /FRED HTTP/1.1\r\nHost: localhost\r\nKeep-Alive: max=3\r\n\r\n"sv}));
        optional<string> second = ReadUntilEndsWith_ (client, "FRED"sv);
        ASSERT_TRUE (second);
        EXPECT_TRUE (String::FromLatin1 (*second).Contains ("Keep-Alive: timeout=30, max=2\r\n"sv, eCaseInsensitive)) << *second;
        client.Write (as_bytes (span{"GET /FRED HTTP/1.1\r\nHost: localhost\r\nConnection: close\r\n\r\n"sv}));
        optional<string> last = ReadUntilClosed_ (client);
        ASSERT_TRUE (last);
        EXPECT_FALSE (String::FromLatin1 (*last).Contains ("Keep-Alive:"sv, eCaseInsensitive)) << *last;
    }

    /*
     *  Interceptors added from two threads at once are all in the chain a new connection gets.
     */
    GTEST_TEST (Frameworks_WebServer, AddInterceptorConcurrently_)
    {
        TraceContextBumper     ctx{"AddInterceptorConcurrently_"};
        ConnectionManager      server{SocketAddresses (InternetAddresses_Any (), kRouterTestPort_), Sequence<Route>{kFRED_}};
        atomic<unsigned int>   called{0};
        constexpr unsigned int kPerThread_ = 100;
        WaitableEvent          go;
        auto                   addMany = [&] (ConnectionManager::InterceptorAddRelativeTo where) {
            go.Wait (); // so the two threads add at once
            for (unsigned int i = 0; i < kPerThread_; ++i) {
                server.AddInterceptor (Interceptor{[&] (Message&) { ++called; }}, where);
            }
        };
        Thread::Ptr appender  = Thread::New ([&] () { addMany (ConnectionManager::eAppend); }, Thread::eAutoStart);
        Thread::Ptr prepender = Thread::New ([&] () { addMany (ConnectionManager::ePrepend); }, Thread::eAutoStart);
        go.Set ();
        appender.Join ();
        prepender.Join ();
        IO::Network::ConnectionOrientedStreamSocket::Ptr client = ConnectToTestServer_ ();
        client.Write (as_bytes (span{"GET /FRED HTTP/1.1\r\nHost: localhost\r\n\r\n"sv}));
        EXPECT_TRUE (ReadUntilEndsWith_ (client, "FRED"sv).has_value ());
        EXPECT_EQ (called.load (), 2 * kPerThread_) << "interceptors added at once were lost from the chain";
    }

    /*
     *  An interceptor added among the early ones (ePrependsToEarly) can be removed - and is then not called.
     */
    GTEST_TEST (Frameworks_WebServer, RemoveEarlyInterceptor_)
    {
        TraceContextBumper   ctx{"RemoveEarlyInterceptor_"};
        ConnectionManager    server{SocketAddresses (InternetAddresses_Any (), kRouterTestPort_), Sequence<Route>{kFRED_}};
        atomic<unsigned int> called{0};
        Interceptor          counter{[&] (Message&) { ++called; }};
        server.AddInterceptor (counter, ConnectionManager::ePrependsToEarly);
        server.RemoveInterceptor (counter);
        IO::Network::ConnectionOrientedStreamSocket::Ptr client = ConnectToTestServer_ ();
        client.Write (as_bytes (span{"GET /FRED HTTP/1.1\r\nHost: localhost\r\n\r\n"sv}));
        EXPECT_TRUE (ReadUntilEndsWith_ (client, "FRED"sv).has_value ());
        EXPECT_EQ (called.load (), 0u);
    }

    /*
     *  At Options::fMaxConnections, a new connection makes room by closing the one idle longest. Before Stroika v3.0d26 the
     *  limit was not enforced.
     */
    GTEST_TEST (Frameworks_WebServer, MaxConnections_ClosesLongestIdle_)
    {
        TraceContextBumper ctx{"MaxConnections_ClosesLongestIdle_"};
        namespace ConnectionOrientedStreamSocket = IO::Network::ConnectionOrientedStreamSocket;
        ConnectionManager server{SocketAddresses (InternetAddresses_Any (), kRouterTestPort_), Sequence<Route>{kFRED_},
                                 ConnectionManager::Options{.fMaxConnections = 2}};
        auto              served = [] (const ConnectionOrientedStreamSocket::Ptr& s) {
            s.Write (as_bytes (span{"GET /FRED HTTP/1.1\r\nHost: localhost\r\n\r\n"sv}));
            return ReadUntilEndsWith_ (s, "FRED"sv).has_value ();
        };
        ConnectionOrientedStreamSocket::Ptr older = ConnectToTestServer_ ();
        ASSERT_TRUE (served (older));
        Execution::Sleep (100ms); // so older has been idle longer
        ConnectionOrientedStreamSocket::Ptr newer = ConnectToTestServer_ ();
        ASSERT_TRUE (served (newer));
        ConnectionOrientedStreamSocket::Ptr third = ConnectToTestServer_ ();
        EXPECT_TRUE (served (third)) << "the connection beyond the limit was not served";
        EXPECT_EQ (ReadUntilClosed_ (older), string{}) << "the connection idle longest was not closed";
        EXPECT_TRUE (served (newer)) << "the other idle connection was closed too";
    }

    /*
     *  At Options::fMaxConnections with every connection handling a request, a new one is answered 503 (Service Unavailable)
     *  and closed. Before Stroika v3.0d26 the limit was not enforced.
     */
    GTEST_TEST (Frameworks_WebServer, MaxConnections_AllBusyAnswers503_)
    {
        TraceContextBumper ctx{"MaxConnections_AllBusyAnswers503_"};
        namespace ConnectionOrientedStreamSocket = IO::Network::ConnectionOrientedStreamSocket;
        atomic<unsigned int> handlersEntered{0};
        WaitableEvent        releaseHandlers;
        // more handler threads than fMaxConnections: so the limit, not the handler queue, is what refuses the third
        ConnectionManager       server{SocketAddresses (InternetAddresses_Any (), kRouterTestPort_),
                                       Sequence<Route>{Route{"slow"_RegEx,
                                                             [&] (Request&, Response& response) {
                                                           ++handlersEntered;
                                                           releaseHandlers.Wait ();
                                                           response.contentType = DataExchange::InternetMediaTypes::kText_PLAIN;
                                                           response.write ("done"sv);
                                                             }}},
                                       ConnectionManager::Options{.fMaxConnections = 2, .fMaxConcurrentlyHandledConnections = 4}};
        [[maybe_unused]] auto&& cleanup = Finally ([&] () noexcept { releaseHandlers.Set (); }); // before ~server waits for them
        auto                    slow    = [] () {
            ConnectionOrientedStreamSocket::Ptr s = ConnectToTestServer_ ();
            s.Write (as_bytes (span{"GET /slow HTTP/1.1\r\nHost: localhost\r\n\r\n"sv}));
            return s;
        };
        ConnectionOrientedStreamSocket::Ptr a = slow ();
        ConnectionOrientedStreamSocket::Ptr b = slow ();
        for (Time::TimePointSeconds giveUpAt = Time::GetTickCount () + 10s; handlersEntered < 2;) {
            ASSERT_LT (Time::GetTickCount (), giveUpAt) << "the two requests never reached their handlers";
            Execution::Sleep (10ms);
        }
        optional<string> answer = ReadUntilClosed_ (ConnectToTestServer_ ());
        ASSERT_TRUE (answer) << "the connection beyond the limit was left open";
        EXPECT_TRUE (answer->starts_with ("HTTP/1.1 503")) << *answer;
        EXPECT_EQ (server.statistics ().fConnections.fRequestsRejected, 1u);
        releaseHandlers.Set ();
        EXPECT_TRUE (ReadUntilEndsWith_ (a, "done"sv).has_value ());
        EXPECT_TRUE (ReadUntilEndsWith_ (b, "done"sv).has_value ());
    }
}
#endif

int main (int argc, const char* argv[])
{
    Test::Setup (argc, argv);
#if qStroika_Platform_POSIX
    // a write to a connection its peer closed fails the test - rather than killing it with SIGPIPE
    Execution::SignalHandlerRegistry::sThe.SetSignalHandlers (SIGPIPE, Execution::SignalHandlerRegistry::kIGNORED);
#endif
#if qStroika_HasComponent_googletest
    return RUN_ALL_TESTS ();
#else
    cerr << "[  SKIPPED ] every test - Stroika regression tests require building with google test feature" << endl;
#endif
}
