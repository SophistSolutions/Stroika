/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
//  TEST    Frameworks::WebServer && Frameworks::WebService
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
#include "Stroika/Foundation/Streams/TextReader.h"

#include "Stroika/Frameworks/Test/ArchtypeClasses.h"
#include "Stroika/Frameworks/Test/TestHarness.h"
#include "Stroika/Frameworks/WebServer/ConnectionManager.h"
#include "Stroika/Frameworks/WebServer/FileSystemRequestHandler.h"
#include "Stroika/Frameworks/WebServer/Router.h"
#include "Stroika/Frameworks/WebService/Server/ObjectRequestHandler.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::DataExchange;
using namespace Stroika::Foundation::Execution;
using namespace Stroika::Foundation::Memory;

using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::WebServer;
using namespace Stroika::Frameworks::WebService;
using namespace Stroika::Frameworks::WebService::Server;

using Common::ConstantProperty;
using Common::GUID;
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

#if 0
using CALLBACK_FUNCTION = decltype([] (int) -> String { return String{}; });

using RR = invoke_result_t<CALLBACK_FUNCTION,int>;
static_assert (same_as<RR, String>);

ObjectVariantMapper kMapper;
auto                iiii = ObjectRequestHandler::Factory{ObjectRequestHandler::Options{kMapper},
                                        [] () -> String { return String{}; }};
auto iiii1 = ObjectRequestHandler::Factory<String, int>{ObjectRequestHandler::Options{kMapper}, [] (int) -> String { return String{}; }};
auto iiii1a = ObjectRequestHandler::Factory{ObjectRequestHandler::Options{kMapper}, [] (int) -> String { return String{}; }};
#endif

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
        // Can declare arguments as Message* message
        void SetAppState_ (Message& message)
        {
            if (fUseTransferEncoding_) {
                message.rwResponse ().rwHeaders ().transferEncoding = *fUseTransferEncoding_;
            }
            message.rwResponse ().contentType = DataExchange::InternetMediaTypes::kHTML;
            String argsAsString               = Streams::TextReader::New (message.rwRequest ().GetBody ()).ReadAll ();
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

namespace {
    // simple object to create keyed collection of, to test
    struct ObjMapperableObj_ {
        GUID id;

        static const inline ObjectVariantMapper kMapper = [] () {
            ObjectVariantMapper mapper;
            mapper.AddCommonType<GUID> ();
            mapper.AddClass<ObjMapperableObj_> ({
                {"id", &ObjMapperableObj_::id},
            });
            return mapper;
        }();
    };

    /*
     */
    struct MyObjectWebServiceWebServer_ {

        using MyKeyedCollection_ = KeyedCollection<
            ObjMapperableObj_, GUID,
            KeyedCollection_DefaultTraits<ObjMapperableObj_, GUID, decltype ([] (const ObjMapperableObj_& t) -> GUID { return t.id; })>>;

        static const inline ObjectVariantMapper kMapper = [] () {
            ObjectVariantMapper mapper;
            mapper += ObjMapperableObj_::kMapper;
            mapper.AddCommonType<MyKeyedCollection_> ();
            mapper.AddCommonType<Sequence<ObjMapperableObj_>> ();
            mapper.AddCommonType<GUID> ();
            mapper.AddCommonType<Sequence<GUID>> ();
            return mapper;
        }();

        static inline Synchronized<MyKeyedCollection_> sData_;

        const Sequence<Route> kRoutes_;
        ConnectionManager     fConnectionMgr_;

        MyObjectWebServiceWebServer_ (uint16_t portNumber)
            : kRoutes_{

                Route{"api/objs/?"_RegEx,
                             ObjectRequestHandler::Factory{
                                 {kMapper},
                                 [] () -> Sequence<GUID> {
                                     return sData_.cget ().cref ().Map<Sequence<GUID>> ([] (const ObjMapperableObj_& r) { return r.id; });
                                 }}}

                , Route{"api/objs-context/?"_RegEx, ObjectRequestHandler::Factory{{kMapper},
                                                                        [] ([[maybe_unused]]const ObjectRequestHandler::Context& c) -> Sequence<GUID> {
                                                                            return sData_.cget ().cref ().Map<Sequence<GUID>> (
                                                                                [] (const ObjMapperableObj_& r) { return r.id; });
                                                                        }}}

                // PATCH could be implemented using ObjectRequestHandler::Factory, but it adds little value, and good to show
                // mixing direct RequestHandlers with ObjectRequestHandler based ones
                 ,  Route{IO::Network::HTTP::MethodsRegEx::kPatch, "api/objs/(.+)"_RegEx,
                        [] (Message& m, const String& id) {
                    using DataExchange::VariantValue;
                            using JSON::Patch::OperationItemsType;
                            OperationItemsType patch = ClientErrorException::TreatExceptionsAsClientError ([&] () {return OperationItemsType::kMapper.ToObject<OperationItemsType> (m.rwRequest ().GetBodyVariantValue()); });
                            // automatic / generic patch implemented using the VariantValue representation - if that's good enuf for your purposes, easy to use
                            ObjMapperableObj_ obj2Patch = sData_.cget ().cref ().LookupChecked (id, ClientErrorException{"obj with that ID not found"sv});
                            VariantValue obj2PatchVV = patch.Apply (kMapper.FromObject (obj2Patch));
                            obj2Patch                = kMapper.ToObject<ObjMapperableObj_> (obj2PatchVV);
                            sData_.rwget ().rwref ().Add (obj2Patch);
                            m.rwResponse ().status = IO::Network::HTTP::StatusCodes::kNoContent;
                        }}

                , Route{IO::Network::HTTP::MethodsRegEx::kPost, "api/objs/?"_RegEx,
                      // redo so can POST raw data and arguments as query-args!
                      // break ObjectRequestHandler into parts/phases so can be used directly from regular message handler
                      ObjectRequestHandler::Factory{{kMapper},
                                                    [] (const ObjMapperableObj_& r) -> GUID {
                                                        ObjMapperableObj_ rr = r;
                                                        rr.id                = GUID::GenerateNew ();
                                                        sData_.rwget ().rwref ().Add (rr);
                                                        return rr.id;
                                                    }}}

                , Route{ IO::Network::HTTP::MethodsRegEx::kPost, "api/objs-context/?"_RegEx,
                    ObjectRequestHandler::Factory { {kMapper},
                        [] (const ObjMapperableObj_& r, [[maybe_unused]] const ObjectRequestHandler::Context& c) -> GUID {
                        ObjMapperableObj_ rr = r;
                        rr.id                = GUID::GenerateNew ();
                        sData_.rwget ().rwref ().Add (rr);
                        return rr.id;
                    }}}

        }

        , fConnectionMgr_{SocketAddresses (InternetAddresses_Any (), portNumber), kRoutes_}
        {
        }
    };
}

#if qStroika_HasComponent_googletest
namespace {
    GTEST_TEST (Frameworks_WebServer, SimpleStartStopServerTest)
    {
        const auto   portNumber = 8082;
        const auto   quitAfter  = 1s;
        MyWebServer_ myWebServer{portNumber, nullopt}; // listen and dispatch while this object exists
        WaitableEvent{}.WaitQuietly (quitAfter);       // leave it running for a bit
    }
}

namespace {
    GTEST_TEST (Frameworks_WebServer, SimpleCurlTestTalk2Server)
    {
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
        const IO::Network::PortType portNumber = 8082;
        MyWebServer_                myWebServer{portNumber, nullopt}; // listen and dispatch while this object exists
        try {
            auto c = IO::Network::Transfer::Connection::New ();
            using namespace DataExchange;
            using DataExchange::VariantValue;
            auto                            arg    = VariantValue{Mapping<String, VariantValue>{{"AppState", "Start"}}};
            auto                            toJson = [] (const VariantValue& v) { return Variant::JSON::Writer{}.WriteAsBLOB (v); };
            IO::Network::Transfer::Response r = c.POST (URI{"http", URI::Authority{URI::Host{"localhost"}, portNumber}, "/SetAppState2"sv},
                                                        toJson (arg), DataExchange::InternetMediaTypes::kJSON);
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
        // @todo add tests with different flags about allowed compression - and add asserts about returned content-encoding headers.

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

/// @todo - add tests with different Accept-Encoding headers

namespace {
    GTEST_TEST (Frameworks_WebServervice, TestWebServiceObjectRequestHandler1)
    {
        EXPECT_EQ (Compression::Deflate::Compress::New ().Transform (TestDeflateEnc1_::kDecoded), TestDeflateEnc1_::kEncoded);
        const IO::Network::PortType  portNumber = 8083;
        MyObjectWebServiceWebServer_ myWebServer{portNumber}; // listen and dispatch while this object exists
        try {
            auto c = IO::Network::Transfer::Connection::New ();
        }
        catch (const RequiredComponentMissingException&) {
            DbgTrace ("ignore RequiredComponentMissingException cuz no IO::Network::Transfer::Connection factory"_f);
        }

        // @todo do some calls to test api...
        //
        //

        //IO::Network::Transfer::Response r = c.GET (URI{"http", URI::Authority{URI::Host{"localhost"}, portNumber}, "/TEST"sv});
        //EXPECT_TRUE (r.GetSucceeded ());
        //EXPECT_GT (r.GetData ().size (), 1u);
        //String response = r.GetDataTextInputStream ().ReadAll ();
        ////DbgTrace (L"response={}"_f, response);
        //EXPECT_EQ (response, "TEST");
        //// @todo enhance this test so we force accept-encoding none, and force accept-endcing : deflate, and check raw
        //// result???
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
