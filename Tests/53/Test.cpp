/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
//  TEST    Frameworks::WebServe
#include "Stroika/Foundation/StroikaPreComp.h"

#include <iostream>

#include "Stroika/Foundation/Common/GUID.h"
#include "Stroika/Foundation/Common/Property.h"
#include "Stroika/Foundation/Containers/KeyedCollection.h"
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
                                      .LookupChecked ("AppState", RuntimeErrorException{"oops"})
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
#if 0
    // NOT SURE it was this test - but maybe one just before (which is why added TraceContextBumbers for next time).
    // But got this tracelog along with crash on Ubunutu 22
     (NEW THREAD, index=0000 Real Thread ID=0x7fc701d9aac0)  (pthread_self=0x7fc701d9aac0)   ***Starting TraceLog***
[0000][367451.249]      Starting at Sun Jan 4 1:51:42 2026
[0000][367451.249]      TraceFileName: /tmp/TraceLog_Test53_PID#1874999-2026-01-04T01-51-42-05-00.txt
[0000][367451.249]      EXEPath=/home/lewis/Sandbox/Stroika-Build-Dir-Ubuntu2204_x86_64/Builds/g++-12-release++2b/Tests/Test53
[0000][367451.249]      <debug-state>
[0000][367451.249]        Debug::kBuiltWithAddressSanitizer = false
[0000][367451.249]        Debug::kBuiltWithThreadSanitizer = false
[0000][367451.249]        Debug::kBuiltWithUndefinedBehaviorSanitizer = false(?)
[0000][367451.249]        Debug::IsRunningUnderValgrind () = false
[0000][367451.249]      </debug-state>
....
       } </Thread::Ptr::Rep_::ThreadMain_>
[MAIN][0001.021]                } </Thread::WaitForDoneUntil>
[MAIN][0001.021]        } </Thread::AbortAndWaitForDoneUntil>
[MAIN][0001.021]        <Thread::AbortAndWaitForDoneUntil (*this={id: 0x7fc6f27fc640, index: 7, name: WebServer-ConnectionMgr-Wait4IOReady,)> {
[MAIN][0001.021]                <Thread::Abort (*this={id: 0x7fc6f27fc640, index: 7, name: WebServer-ConnectionMgr-Wait4IOReady,)> {
[MAIN][0001.021]                        Transitioned state to aborting, so calling fThread_.get_stop_source ().request_stop ();
[MAIN][0001.021]                        Something triggered stop_token request stop, so doing abort to make sure we are in an aborting (flag) state.
[MAIN][0001.021]                        <Stroika::Foundation::Execution::Signals::Execution::SendSignal (target = 7fc6f27fc640 signal = SIGUSR2)/>
[MAIN][0001.021]                } </Thread::Abort>
[MAIN][0001.021]                <Thread::WaitForDoneUntil (*this={id: 0x7fc6f27fc640, index: 7, name: WebServer-ConnectionMgr-Wait4IOReady,)> {
[0007][0001.022]                Throwing exception: Thread Abort from ...  4# ;  5# ;  6# ;  7# ;  8# ;  9# ; 10# ; 11# ;
[0007][0001.022]                FAILED: SIGNAL= SIGSEGV
#endif
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
#endif

int main (int argc, const char* argv[])
{
    Test::Setup (argc, argv);
#if qStroika_HasComponent_googletest
    return RUN_ALL_TESTS ();
#else
    cerr << "Stroika regression tests require building with google test feature [  PASSED  ]" << endl;
#endif
}
