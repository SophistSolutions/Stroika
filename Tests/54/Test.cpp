/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
//  TEST    Frameworks::WebService
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

#include "Stroika/Frameworks/Test/ArchtypeClasses.h"
#include "Stroika/Frameworks/Test/TestHarness.h"
#include "Stroika/Frameworks/WebServer/ConnectionManager.h"
#include "Stroika/Frameworks/WebServer/FileSystemRequestHandler.h"
#include "Stroika/Frameworks/WebServer/Router.h"
#include "Stroika/Frameworks/WebService/Server/ObjectRequestHandler.h"
#include "Stroika/Frameworks/WebService/Server/VariantValue.h"

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
    GTEST_TEST (Frameworks_WebService, TestVariantValueSupport)
    {
        using namespace WebService::Server::VariantValue;
        // @todo - move this to some framework-specific regtests...
        using VariantValue = DataExchange::VariantValue;
        Sequence<VariantValue> tmp =
            OrderParamValues (Iterable<String>{"page", "xxx"}, PickoutParamValuesFromURL (URI{"http://www.sophist.com?page=5"}));
        Assert (tmp.size () == 2);
        Assert (tmp[0].ConvertTo (VariantValue::eInteger) == 5);
        Assert (tmp[1] == nullptr);
    }
}

namespace {
    GTEST_TEST (Frameworks_WebService, TestWebServiceObjectRequestHandler1)
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
        //// @todo enhance this test so we force accept-encoding none, and force accept-encoding : deflate, and check raw
        //// result???
    }
}

namespace {
    GTEST_TEST (Frameworks_WebService, DurationPrecision)
    {
        Debug::TraceContextBumper ctx{"DurationPrecision"};
        using DataExchange::VariantValue;
        {
            ObjectVariantMapper m;
            m.AddCommonType<Duration> ();
            VariantValue vv = m.FromObject (Duration{numbers::pi});
            EXPECT_EQ (Variant::JSON::Writer{}.WriteAsString (vv), "\"PT3.14159S\"");
        }
        {
            ObjectVariantMapper m;
            m.AddCommonType<Duration> (FloatConversion::SignificantFigures{2});
            VariantValue vv = m.FromObject (Duration{numbers::pi});
            EXPECT_EQ (Variant::JSON::Writer{}.WriteAsString (vv), "\"PT3.1S\"");
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
