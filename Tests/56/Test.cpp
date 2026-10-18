/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
//  TEST    Frameworks::WebService
#include "Stroika/Foundation/StroikaPreComp.h"

#include <iostream>

#include "Stroika/Foundation/Common/GUID.h"
#include "Stroika/Foundation/Common/Property.h"
#include "Stroika/Foundation/Containers/KeyedCollection.h"
#include "Stroika/Foundation/DataExchange/BadFormatException.h"
#include "Stroika/Foundation/DataExchange/Compression/Deflate.h"
#include "Stroika/Foundation/DataExchange/InternetMediaTypeRegistry.h"
#include "Stroika/Foundation/DataExchange/JSON/Patch.h"
#include "Stroika/Foundation/DataExchange/ObjectVariantMapper.h"
#include "Stroika/Foundation/DataExchange/TypedBLOB.h"
#include "Stroika/Foundation/DataExchange/Variant/JSON/Reader.h"
#include "Stroika/Foundation/DataExchange/Variant/JSON/Writer.h"
#include "Stroika/Foundation/DataExchange/XML/Common.h"
#if qStroika_HasComponent_libxml2
#include "Stroika/Foundation/DataExchange/XML/Providers/LibXML2.h"
#endif
#if qStroika_HasComponent_xerces
#include "Stroika/Foundation/DataExchange/XML/Providers/Xerces.h"
#endif
#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/Debug/Visualizations.h"
#include "Stroika/Foundation/Execution/Module.h"
#include "Stroika/Foundation/Execution/RequiredComponentMissingException.h"
#include "Stroika/Foundation/Execution/Synchronized.h"
#include "Stroika/Foundation/IO/Network/HTTP/ClientErrorException.h"
#include "Stroika/Foundation/IO/Network/HTTP/Methods.h"
#include "Stroika/Foundation/IO/Network/HTTP/Status.h"
#include "Stroika/Foundation/IO/Network/InternetAddress.h"
#include "Stroika/Foundation/IO/Network/SocketAddress.h"
#include "Stroika/Foundation/IO/Network/Transfer/Connection.h"

#include "Stroika/Frameworks/Test/ArchtypeClasses.h"
#include "Stroika/Frameworks/Test/TestHarness.h"
#include "Stroika/Frameworks/WebServer/ConnectionManager.h"
#include "Stroika/Frameworks/WebServer/FileSystemRequestHandler.h"
#include "Stroika/Frameworks/WebServer/Router.h"
#include "Stroika/Frameworks/WebService/SOAP.h"
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

                  Route{"api/objs/?"_RegEx, ObjectRequestHandler::Factory{{kMapper},
                                                                          [] () -> Sequence<GUID> {
                                                                              return sData_.cget ().cref ().Map<Sequence<GUID>> (
                                                                                  [] (const ObjMapperableObj_& r) { return r.id; });
                                                                          }}}

                  ,
                  Route{"api/objs-context/?"_RegEx,
                        ObjectRequestHandler::Factory{{kMapper},
                                                      [] ([[maybe_unused]] const ObjectRequestHandler::Context& c) -> Sequence<GUID> {
                                                          return sData_.cget ().cref ().Map<Sequence<GUID>> (
                                                              [] (const ObjMapperableObj_& r) { return r.id; });
                                                      }}}

                  // PATCH could be implemented using ObjectRequestHandler::Factory, but it adds little value, and good to show
                  // mixing direct RequestHandlers with ObjectRequestHandler based ones
                  ,
                  Route{IO::Network::HTTP::MethodsRegEx::kPatch, "api/objs/(.+)"_RegEx,
                        [] (Message& m, const String& id) {
                            using DataExchange::VariantValue;
                            using JSON::Patch::OperationItemsType;
                            OperationItemsType patch = ClientErrorException::TreatExceptionsAsClientError ([&] () {
                                return OperationItemsType::kMapper.ToObject<OperationItemsType> (m.rwRequest ().GetBodyVariantValue ());
                            });
                            // automatic / generic patch implemented using the VariantValue representation - if that's good enuf for your purposes, easy to use
                            ObjMapperableObj_ obj2Patch =
                                sData_.cget ().cref ().LookupChecked (id, ClientErrorException{"obj with that ID not found"sv});
                            VariantValue obj2PatchVV = patch.Apply (kMapper.FromObject (obj2Patch));
                            obj2Patch                = kMapper.ToObject<ObjMapperableObj_> (obj2PatchVV);
                            sData_.rwget ().rwref ().Add (obj2Patch);
                            m.rwResponse ().status = IO::Network::HTTP::StatusCodes::kNoContent;
                        }}

                  ,
                  Route{IO::Network::HTTP::MethodsRegEx::kPost, "api/objs/?"_RegEx,
                        // redo so can POST raw data and arguments as query-args!
                        // break ObjectRequestHandler into parts/phases so can be used directly from regular message handler
                        ObjectRequestHandler::Factory{{kMapper},
                                                      [] (const ObjMapperableObj_& r) -> GUID {
                                                          ObjMapperableObj_ rr = r;
                                                          rr.id                = GUID::GenerateNew ();
                                                          sData_.rwget ().rwref ().Add (rr);
                                                          return rr.id;
                                                      }}}

                  ,
                  Route{IO::Network::HTTP::MethodsRegEx::kPost, "api/objs-context/?"_RegEx,
                        ObjectRequestHandler::Factory{{kMapper},
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
        if constexpr (Compression::Deflate::kSupported) { // without zlib the server just sends the body uncompressed
            EXPECT_EQ (Compression::Deflate::Compress::New ().Transform (TestDeflateEnc1_::kDecoded), TestDeflateEnc1_::kEncoded);
        }
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

#if qStroika_Foundation_DataExchange_XML_SupportDOM
namespace {
    /*
     *  SOAP's messages: a call, its answer, and a fault (with and without detail) - each read back as written, with values XML
     *  must quote, one not ASCII, and an empty one - and each read as what it is not, refused.
     */
    /*
     *  What Serialize writes holds against SOAP 1.1's own envelope schema - which is the only thing that can say so, our
     *  own reader being as lenient as it must be. The schema declares just what was argued about by eye: a Fault's
     *  children unqualified, encodingStyle qualified, and what a Body may hold. As published it also forbids
     *  encodingStyle on the Envelope, where SOAP 1.1's prose and examples - and so much of the world - put it, so
     *  EnvelopeSchema is by default corrected to allow it; both are checked here.
     */
    GTEST_TEST (Frameworks_WebService, SOAP_ValidatesAgainstItsSchema_)
    {
        Debug::TraceContextBumper ctx{"SOAP_ValidatesAgainstItsSchema_"};
        const SOAP::Options       kValidating_{.fValidateReadAgainst = SOAP::EnvelopeSchema ()};
        const SOAP::Options       kValidatingAsPublished_{.fValidateReadAgainst = SOAP::EnvelopeSchema ({}, false)};
        // a call, its answer, a fault, and a fault carrying XML of someone else's - each, as written by default, holding to both
        const SOAP::Request kRequest_{
            .fNamespace = "urn:example:stock"sv, .fMethod = "GetPrice"sv, .fArguments = {{"Symbol"sv, "IBM & <co>"sv}, {"Qty"sv, "100"sv}}};
        const SOAP::Response kResponse_{.fNamespace = "urn:example:stock"sv, .fMethod = "GetPrice"sv, .fArguments = {{"Price"sv, "1.23"sv}}};
        const SOAP::Fault kFault_{.fFaultCode = SOAP::Fault::kClient, .fFaultString = "bad"sv};
        const SOAP::Fault kFaultWithDetail_{
            .fFaultCode = SOAP::Fault::kServer, .fFaultString = "later"sv, .fDetail = "<x:Why xmlns:x=\"urn:example:why\">because</x:Why>"sv};
        for (const SOAP::Options& validating : {kValidating_, kValidatingAsPublished_}) {
            {
                SOAP::Request back;
                EXPECT_NO_THROW (SOAP::DeSerialize (SOAP::Serialize (kRequest_), &back, validating));
                EXPECT_EQ (back, kRequest_);
            }
            {
                SOAP::Response back;
                EXPECT_NO_THROW (SOAP::DeSerialize (SOAP::Serialize (kResponse_), &back, validating));
                EXPECT_EQ (back, kResponse_);
            }
            for (const SOAP::Fault& f : {kFault_, kFaultWithDetail_}) {
                SOAP::Fault back;
                EXPECT_NO_THROW (SOAP::DeSerialize (SOAP::Serialize (f), &back, validating)) << f.ToString ().AsNarrowSDKString ();
                EXPECT_EQ (back, f);
            }
        }
        // ... by default with no encodingStyle at all, as the WS-I Basic Profile requires
        EXPECT_EQ (SOAP::Serialize (kRequest_).As<string> ().find ("encodingStyle"), string::npos);
        EXPECT_EQ (SOAP::Serialize (kResponse_).As<string> ().find ("encodingStyle"), string::npos);
        EXPECT_EQ (SOAP::Serialize (kFault_).As<string> ().find ("encodingStyle"), string::npos);
        // where encodingStyle goes is the writer's choice (Options::fEncodingStyleAttribute): each is written where asked, holds
        // against the corrected schema, and - all but on the Envelope - against the published one too
        for (SOAP::EncodingStyleAttribute where :
             {SOAP::EncodingStyleAttribute::eOnBody, SOAP::EncodingStyleAttribute::eOnEnvelope, SOAP::EncodingStyleAttribute::eOmitted}) {
            const Memory::BLOB written       = SOAP::Serialize (kRequest_, SOAP::Options{.fEncodingStyleAttribute = where});
            const string       text          = written.As<string> ();
            const size_t       encodingStyle = text.find ("encodingStyle");
            const size_t       body          = text.find ("<s:Body");
            switch (where) {
                case SOAP::EncodingStyleAttribute::eOnBody:
                    EXPECT_TRUE (encodingStyle != string::npos and encodingStyle > body) << text;
                    break;
                case SOAP::EncodingStyleAttribute::eOnEnvelope:
                    EXPECT_TRUE (encodingStyle < body) << text;
                    break;
                case SOAP::EncodingStyleAttribute::eOmitted:
                    EXPECT_EQ (encodingStyle, string::npos) << text;
                    break;
            }
            SOAP::Request back;
            EXPECT_NO_THROW (SOAP::DeSerialize (written, &back, kValidating_)) << text;
            EXPECT_EQ (back, kRequest_);
            if (where == SOAP::EncodingStyleAttribute::eOnEnvelope) {
                EXPECT_THROW (SOAP::DeSerialize (written, &back, kValidatingAsPublished_), BadFormatException) << text;
            }
            else {
                EXPECT_NO_THROW (SOAP::DeSerialize (written, &back, kValidatingAsPublished_)) << text;
            }
        }
        // an envelope as SOAP 1.1's examples write one - s:encodingStyle on the Envelope, whose anyAttribute is ##other as
        // published, so that an attribute of SOAP's own namespace is not allowed there: the published schema refuses it, the
        // corrected one does not, and it is READ by default - as a message from any stack that does the same must be
        constexpr string_view kEncodingStyleOnEnvelope_ =
            "<?xml version=\"1.0\"?>\r\n"
            "<s:Envelope xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\" "
            "s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\">"
            "<s:Body><s:Fault><faultcode>s:Client</faultcode><faultstring>bad</faultstring></s:Fault></s:Body></s:Envelope>\r\n"sv;
        SOAP::Fault onEnvelope;
        EXPECT_THROW (SOAP::DeSerialize (Memory::BLOB{as_bytes (span{kEncodingStyleOnEnvelope_})}, &onEnvelope, kValidatingAsPublished_), BadFormatException);
        EXPECT_NO_THROW (SOAP::DeSerialize (Memory::BLOB{as_bytes (span{kEncodingStyleOnEnvelope_})}, &onEnvelope, kValidating_));
        EXPECT_NO_THROW (SOAP::DeSerialize (Memory::BLOB{as_bytes (span{kEncodingStyleOnEnvelope_})}, &onEnvelope, SOAP::Options{}));
        EXPECT_EQ (onEnvelope, kFault_) << "and means the same as what is written by default";
        // and something that is NOT a SOAP envelope is caught by either schema, not merely tolerated
        constexpr string_view kNotSOAP_ = "<Envelope xmlns=\"http://schemas.xmlsoap.org/soap/envelope/\"><Nonsense/></Envelope>"sv;
        SOAP::Request         ignored;
        EXPECT_ANY_THROW (SOAP::DeSerialize (Memory::BLOB{as_bytes (span{kNotSOAP_})}, &ignored, kValidating_));
        EXPECT_ANY_THROW (SOAP::DeSerialize (Memory::BLOB{as_bytes (span{kNotSOAP_})}, &ignored, kValidatingAsPublished_));
    }

    /*
     *  A fault's code is written as a QName, so it must be an XML name (Fault::fFaultCode). SOAP's own codes, and dotted ones,
     *  are written and hold to the schema. One that is not a name, from another stack, is read leniently - as its longest
     *  leading part that is a name, or Server - so it can be written back; checked, it is refused.
     */
    GTEST_TEST (Frameworks_WebService, SOAP_FaultCodes_)
    {
        Debug::TraceContextBumper ctx{"SOAP_FaultCodes_"};
        const SOAP::Options       kValidating_{.fValidateReadAgainst = SOAP::EnvelopeSchema ()};
        for (const String& code : {SOAP::Fault::kVersionMismatch, SOAP::Fault::kMustUnderstand, SOAP::Fault::kClient, SOAP::Fault::kServer,
                                   String{"Client.Authentication"sv}}) {
            const SOAP::Fault f{.fFaultCode = code, .fFaultString = "bad"sv};
            SOAP::Fault       back;
            EXPECT_NO_THROW (SOAP::DeSerialize (SOAP::Serialize (f), &back, kValidating_)) << code.AsNarrowSDKString ();
            EXPECT_EQ (back, f);
        }
        auto faultWithCode = [] (string_view code) {
            const string message = string{"<?xml version=\"1.0\"?><s:Envelope xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\">"
                                          "<s:Body><s:Fault><faultcode>"} +
                                   string{code} + "</faultcode><faultstring>bad</faultstring></s:Fault></s:Body></s:Envelope>";
            return Memory::BLOB{as_bytes (span{message})};
        };
        for (const auto& [sent, readAs] : std::initializer_list<std::pair<string_view, string_view>>{
                 {"s:Server Error"sv, "Server"sv}, {"s:Client.Auth Failure"sv, "Client.Auth"sv}, {"s:1Bad"sv, "Server"sv}, {"Client"sv, "Client"sv}}) {
            SOAP::Fault back;
            SOAP::DeSerialize (faultWithCode (sent), &back);
            EXPECT_EQ (back.fFaultCode, String{readAs}) << sent;
            SOAP::Fault again;
            EXPECT_NO_THROW (SOAP::DeSerialize (SOAP::Serialize (back), &again, kValidating_)) << sent; // written back, it holds
            EXPECT_EQ (again, back);
        }
        SOAP::Fault ignored;
        EXPECT_THROW (SOAP::DeSerialize (faultWithCode ("s:Server Error"sv), &ignored, kValidating_), BadFormatException);
    }

    /*
     *  Every message, through each XML provider, and across them - what one writes another reads. Nothing exercised
     *  SOAP through Xerces until this, which is how it went unnoticed that Xerces could not read back the envelope it
     *  had just written: its GetName () gave the QUALIFIED name, so "s:Envelope" never matched the "Envelope" this
     *  code looks for, and every prefixed message - which is to say every message - was "not a SOAP message".
     *  @see XML::NameWithNamespace, and Tests/34's DOM_GetNameIsTheLocalName_, which holds the providers to it.
     */
    GTEST_TEST (Frameworks_WebService, SOAP_EachXMLProvider_)
    {
        Debug::TraceContextBumper                                               ctx{"SOAP_EachXMLProvider_"};
        Containers::Sequence<const DataExchange::XML::Providers::IXMLProvider*> providers{nullptr}; // nullptr: whichever is default
#if qStroika_HasComponent_libxml2
        providers += &DataExchange::XML::Providers::LibXML2::kDefaultProvider;
#endif
#if qStroika_HasComponent_xerces
        providers += &DataExchange::XML::Providers::Xerces::kDefaultProvider;
#endif
        EXPECT_GE (providers.size (), 2u) << "this build configured no XML provider at all?";
        const String        kAwkward_{u"R&D <\"caf\u00e9\"> 'x'"sv};
        const SOAP::Request kRequest_{
            .fNamespace = "urn:example:stock"sv, .fMethod = "GetPrice"sv, .fArguments = {{"Symbol"sv, kAwkward_}, {"Empty"sv, String{}}}};
        const SOAP::Response kResponse_{.fNamespace = "urn:example:stock"sv, .fMethod = "GetPrice"sv, .fArguments = {{"Price"sv, "1.23"sv}}};
        const SOAP::Fault kFault_{.fFaultCode = SOAP::Fault::kServer, .fFaultString = "later"sv, .fDetail = "<x:Why xmlns:x=\"urn:example:why\">because</x:Why>"sv};
        for (const DataExchange::XML::Providers::IXMLProvider* writeWith : providers) {
            for (const DataExchange::XML::Providers::IXMLProvider* readWith : providers) {
                const SOAP::Options kW_{.fXMLOptions = {.fProvider = writeWith}};
                const SOAP::Options kR_{.fXMLOptions = {.fProvider = readWith}};
                {
                    SOAP::Request back;
                    EXPECT_NO_THROW (SOAP::DeSerialize (SOAP::Serialize (kRequest_, kW_), &back, kR_));
                    EXPECT_EQ (back, kRequest_) << back.ToString ().AsNarrowSDKString ();
                }
                {
                    SOAP::Response back;
                    EXPECT_NO_THROW (SOAP::DeSerialize (SOAP::Serialize (kResponse_, kW_), &back, kR_));
                    EXPECT_EQ (back, kResponse_) << back.ToString ().AsNarrowSDKString ();
                }
                {
                    SOAP::Fault back;
                    EXPECT_NO_THROW (SOAP::DeSerialize (SOAP::Serialize (kFault_, kW_), &back, kR_));
                    EXPECT_EQ (back, kFault_) << back.ToString ().AsNarrowSDKString ();
                }
            }
        }
    }

    GTEST_TEST (Frameworks_WebService, SOAP_RoundTrip_)
    {
        Debug::TraceContextBumper ctx{"SOAP_RoundTrip_"};
        const String              kAwkward_{u"R&D <\"caf\u00e9\"> 'x'"sv};
        const SOAP::Request       request{
            .fNamespace = "urn:example:stock"sv, .fMethod = "GetPrice"sv, .fArguments = {{"Symbol"sv, kAwkward_}, {"Empty"sv, String{}}}};
        {
            SOAP::Request back;
            SOAP::DeSerialize (SOAP::Serialize (request), &back);
            EXPECT_EQ (back, request) << back.ToString ().AsNarrowSDKString ();
        }
        for (const SOAP::Response& response :
             {SOAP::Response{.fNamespace = "urn:example:stock"sv, .fMethod = "GetPrice"sv, .fArguments = {{"Price"sv, kAwkward_}}},
              SOAP::Response{.fNamespace = "urn:example:stock"sv, .fMethod = "Buy"sv}}) {
            SOAP::Response back;
            SOAP::DeSerialize (SOAP::Serialize (response), &back);
            EXPECT_EQ (back, response) << back.ToString ().AsNarrowSDKString ();
        }
        for (const SOAP::Fault& fault :
             {SOAP::Fault{.fFaultCode = SOAP::Fault::kClient, .fFaultString = kAwkward_},
              SOAP::Fault{.fFaultCode = "Server.Busy"sv, .fFaultString = "try later"sv, .fDetail = "<x:Why xmlns:x=\"urn:example:why\">because</x:Why>"sv}}) {
            SOAP::Fault back;
            SOAP::DeSerialize (SOAP::Serialize (fault), &back);
            EXPECT_EQ (back, fault) << back.ToString ().AsNarrowSDKString ();
        }
        SOAP::Response notAResponse;
        EXPECT_THROW (SOAP::DeSerialize (SOAP::Serialize (SOAP::Fault{}), &notAResponse), DataExchange::BadFormatException);
        EXPECT_THROW (SOAP::DeSerialize (SOAP::Serialize (request), &notAResponse), DataExchange::BadFormatException);
        SOAP::Request notARequest;
        EXPECT_THROW (SOAP::DeSerialize (SOAP::Serialize (SOAP::Fault{}), &notARequest), DataExchange::BadFormatException);
        EXPECT_THROW (SOAP::DeSerialize (BLOB{as_bytes (span{"not XML"sv})}, &notARequest), DataExchange::BadFormatException);
    }

    /*
     *  SOAP end to end, over loopback: a client's Invoke, and a server's HandleRequest answering it - with its handler's Response,
     *  or the Fault it throws; and a Client fault for a body that is not a SOAP call.
     */
    GTEST_TEST (Frameworks_WebService, SOAP_InvokeAndHandleRequest_)
    {
        Debug::TraceContextBumper ctx{"SOAP_InvokeAndHandleRequest_"};
#if not qStroika_HasComponent_libcurl and not qStroika_HasComponent_WinHTTP
        GTEST_SKIP () << "no HTTP client built in (neither libcurl nor WinHTTP) - Invoke sends through one";
#endif
        constexpr IO::Network::PortType kPort_ = 8084; // on loopback only
        const IO::Network::URI          url{"http://127.0.0.1:{}/stock"_f(kPort_)};
        const SOAP::Fault kBusy_{.fFaultCode = "Server.Busy"sv, .fFaultString = "try later"sv, .fDetail = "<x:Why xmlns:x=\"urn:example:why\">because</x:Why>"sv};
        optional<ConnectionManager> server;
        try {
            server.emplace (IO::Network::SocketAddress{IO::Network::V4::kLocalhost, kPort_},
                            Sequence<Route>{Route{IO::Network::HTTP::MethodsRegEx::kPost, "stock"_RegEx, [&] (Message& m) {
                                                      SOAP::HandleRequest (m, [&] (const SOAP::Request& r) {
                                                          if (r.fMethod == "GetPrice"sv) {
                                                              return SOAP::Response{.fNamespace = r.fNamespace,
                                                                                    .fMethod    = r.fMethod,
                                                                                    .fArguments = {{"Price"sv, "34.5"sv}}};
                                                          }
                                                          Execution::Throw (SOAP::FaultException{kBusy_});
                                                      });
                                                  }}});
        }
        catch (...) {
            Stroika::Frameworks::Test::WarnTestIssue (
                "SOAP_InvokeAndHandleRequest_ skipped - could not run its web server here: {}"_f(current_exception ()).AsNarrowSDKString ().c_str ());
            return;
        }
        // answered
        EXPECT_EQ (SOAP::Invoke (url, "\"urn:example:stock#GetPrice\""sv,
                                 SOAP::Request{.fNamespace = "urn:example:stock"sv, .fMethod = "GetPrice"sv, .fArguments = {{"Symbol"sv, "IBM"sv}}}),
                   (SOAP::Response{.fNamespace = "urn:example:stock"sv, .fMethod = "GetPrice"sv, .fArguments = {{"Price"sv, "34.5"sv}}}));
        // faulted
        try {
            SOAP::Invoke (url, "\"urn:example:stock#Buy\""sv, SOAP::Request{.fNamespace = "urn:example:stock"sv, .fMethod = "Buy"sv});
            ADD_FAILURE () << "Buy was not refused";
        }
        catch (const SOAP::FaultException& e) {
            EXPECT_EQ (e.GetFault (), kBusy_) << e.GetFault ().ToString ().AsNarrowSDKString ();
        }
        // not a SOAP call
        IO::Network::Transfer::Connection::Ptr c = IO::Network::Transfer::Connection::New ();
        c.SetSchemeAndAuthority (url.GetSchemeAndAuthority ());
        IO::Network::Transfer::Request hello;
        hello.fMethod               = IO::Network::HTTP::Methods::kPost;
        hello.fAuthorityRelativeURL = IO::Network::URI{"/stock"sv};
        hello.SetTypedBLOB (TypedBLOB{.fData = BLOB{as_bytes (span{"<hello/>"sv})}, .fType = SOAP::kContentType});
        IO::Network::Transfer::Response answer = c.Send (hello);
        EXPECT_EQ (answer.GetStatus (), IO::Network::HTTP::StatusCodes::kInternalError);
        SOAP::Fault fault;
        SOAP::DeSerialize (answer.GetData (), &fault);
        EXPECT_EQ (fault.fFaultCode, SOAP::Fault::kClient);
    }
}
#endif
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
