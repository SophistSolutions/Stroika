/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
//  TEST    Foundation::IO::Network::Transfer
#include "Stroika/Foundation/StroikaPreComp.h"

#include <iostream>
#include <random>

#if qHasFeature_LibCurl
// for error codes
#include <curl/curl.h>
#endif

#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/Cryptography/Encoding/Algorithm/Base64.h"
#include "Stroika/Foundation/DataExchange/Variant/JSON/Reader.h"
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/Execution/RequiredComponentMissingException.h"
#include "Stroika/Foundation/Execution/SignalHandlers.h"
#include "Stroika/Foundation/IO/Network/Transfer/Client.h"
#if qHasFeature_LibCurl
#include "Stroika/Foundation/IO/Network/Transfer/Client_libcurl.h"
#endif
#if qHasFeature_WinHTTP
#include "Stroika/Foundation/IO/Network/Transfer/Client_WinHTTP.h"
#endif

#include "../TestHarness/TestHarness.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::DataExchange;
using namespace Stroika::Foundation::IO;
using namespace Stroika::Foundation::IO::Network;
using namespace Stroika::Foundation::IO::Network::Transfer;

namespace {
    const Connection::Options kDefaultTestOptions_ = []() {
        Connection::Options o;
        o.fMaxAutomaticRedirects = 1;
        return o;
    }();
}

namespace {
    namespace Test_1_SimpleConnnectionTests_ {
        namespace Private_ {
            void Test_1_SimpleFetch_Google_C_ (Connection c)
            {
                c.SetURL (URL::Parse (L"http://www.google.com"));
                Response r = c.GET ();
                VerifyTestResult (r.GetSucceeded ());
                VerifyTestResult (r.GetData ().size () > 1);
            }
            void Test_2_SimpleFetch_SSL_Google_C_ (Connection c)
            {
                try {
                    c.SetURL (URL::Parse (L"https://www.google.com"));
                    Response r = c.GET ();
                    VerifyTestResult (r.GetSucceeded ());
                    VerifyTestResult (r.GetData ().size () > 1);
                }
#if qHasFeature_LibCurl
                catch (const LibCurlException& lce) {
#if !qHasFeature_OpenSSL
                    if (lce.GetCode () == CURLE_UNSUPPORTED_PROTOCOL) {
                        DbgTrace ("Warning - ignored exception doing LibCurl/ssl - for now probably just no SSL support with libcurl");
                        return;
                    }
#endif
                    throw;
                }
#else
                catch (...) {
                    throw;
                }
#endif
            }
            void DoRegressionTests_ForConnectionFactory_ (Connection (*factory) ())
            {
                Test_1_SimpleFetch_Google_C_ (factory ());
                Test_2_SimpleFetch_SSL_Google_C_ (factory ());
            }
        }
        void DoTests_ ()
        {
            Debug::TraceContextBumper ctx ("{}::Test_1_SimpleConnnectionTests_");
            using namespace Private_;
            try {
                DoRegressionTests_ForConnectionFactory_ ([]() -> Connection { return CreateConnection (kDefaultTestOptions_); });
            }
            catch (const Execution::RequiredComponentMissingException&) {
#if !qHasFeature_LibCurl && !qHasFeature_WinHTTP
// OK to ignore. We don't wnat to call this failing a test, because there is nothing to fix.
// This is more like the absence of a feature beacuse of the missing component.
#else
                Execution::ReThrow ();
#endif
            }

#if qHasFeature_LibCurl
            DoRegressionTests_ForConnectionFactory_ ([]() -> Connection { return Connection_LibCurl (kDefaultTestOptions_); });
#endif
#if qHasFeature_WinHTTP
            DoRegressionTests_ForConnectionFactory_ ([]() -> Connection { return Connection_WinHTTP (kDefaultTestOptions_); });
#endif
        }
    }
}

namespace {
    namespace Test_2_SimpleFetch_httpbin_ {
        namespace Private_ {
            void T1_httpbin_SimpleGET_ (Connection c)
            {
                Debug::TraceContextBumper ctx ("T1_httpbin_SimpleGET_");
                c.SetURL (URL::Parse (L"http://httpbin.org/get"));
                Response r = c.GET ();
                VerifyTestResult (r.GetSucceeded ());
                VerifyTestResult (r.GetData ().size () > 1);
                {
                    VariantValue                  v  = Variant::JSON::Reader ().Read (r.GetDataBinaryInputStream ());
                    Mapping<String, VariantValue> vv = v.As<Mapping<String, VariantValue>> ();
                    VerifyTestResult (vv.ContainsKey (L"args"));
                    VerifyTestResult (vv[L"url"] == L"http://httpbin.org/get");
                }
            }
            void T2_httpbin_SimplePOST_ (Connection c)
            {
                Debug::TraceContextBumper ctx ("T2_httpbin_SimplePOST_");
                using Memory::BLOB;

                static mt19937 sRNG_;

                c.SetURL (URL::Parse (L"http://httpbin.org/post"));
                BLOB roundTripTestData = []() {
                    Memory::SmallStackBuffer<Byte> buf (1024);
                    for (size_t i = 0; i < buf.GetSize (); ++i) {
                        buf[i] = static_cast<Byte> (uniform_int_distribution<unsigned short> () (sRNG_));
                    }
                    return BLOB (buf.begin (), buf.end ());
                }();
                Response r = c.POST (roundTripTestData, DataExchange::PredefinedInternetMediaType::kOctetStream);
                VerifyTestResult (r.GetSucceeded ());
                {
                    VariantValue                  v  = Variant::JSON::Reader ().Read (r.GetDataBinaryInputStream ());
                    Mapping<String, VariantValue> vv = v.As<Mapping<String, VariantValue>> ();
                    DbgTrace ("POST parsed response:");
                    for (auto i : vv) {
                        DbgTrace (L"%s : %s", i.fKey.c_str (), i.fValue.As<String> ().c_str ());
                    }
                    String dataValueString = vv.Lookup (L"data").Value ().As<String> ();
                    {
                        size_t i = dataValueString.Find (',').Value (String::npos);
                        if (i != -1) {
                            dataValueString = dataValueString.SubString (i + 1);
                        }
                    }
                    BLOB resultBLOB = Cryptography::Encoding::Algorithm::DecodeBase64 (dataValueString.AsUTF8 ());
                    VerifyTestResult (resultBLOB == roundTripTestData);
                }
            }
            void T3_httpbin_SimplePUT_ (Connection c)
            {
                Debug::TraceContextBumper ctx ("T3_httpbin_SimplePUT_");
                using Memory::BLOB;

                static mt19937 sRNG_;

                c.SetURL (URL::Parse (L"http://httpbin.org/put"));
                BLOB roundTripTestData = []() {
                    Memory::SmallStackBuffer<Byte> buf (1024);
                    for (size_t i = 0; i < buf.GetSize (); ++i) {
                        buf[i] = static_cast<Byte> (uniform_int_distribution<unsigned short> () (sRNG_));
                    }
                    return BLOB (buf.begin (), buf.end ());
                }();
                Response r = c.PUT (roundTripTestData, DataExchange::PredefinedInternetMediaType::kOctetStream);
                VerifyTestResult (r.GetSucceeded ()); // because throws on failure
                {
                    VariantValue                  v  = Variant::JSON::Reader ().Read (r.GetDataBinaryInputStream ());
                    Mapping<String, VariantValue> vv = v.As<Mapping<String, VariantValue>> ();
                    DbgTrace ("PUT parsed response:");
                    for (auto i : vv) {
                        DbgTrace (L"%s : %s", i.fKey.c_str (), i.fValue.As<String> ().c_str ());
                    }
                    String dataValueString = vv.Lookup (L"data").Value ().As<String> ();
                    {
                        size_t i = dataValueString.Find (',').Value (String::npos);
                        if (i != -1) {
                            dataValueString = dataValueString.SubString (i + 1);
                        }
                    }
                    BLOB resultBLOB = Cryptography::Encoding::Algorithm::DecodeBase64 (dataValueString.AsUTF8 ());
                    VerifyTestResult (resultBLOB == roundTripTestData);
                }
            }
            void DoRegressionTests_ForConnectionFactory_ (Connection (*factory) ())
            {
                // Ignore some server-side errors, because this service we use (http://httpbin.org/) sometimes fails
                try {
                    {
                        T1_httpbin_SimpleGET_ (factory ());
                        T2_httpbin_SimplePOST_ (factory ());
                    }
                    {
                        // Connection re-use
                        Connection conn = factory ();
                        T1_httpbin_SimpleGET_ (conn);
                        T2_httpbin_SimplePOST_ (conn);
                        T3_httpbin_SimplePUT_ (conn);
                    }
                }
                catch (const HTTP::Exception& e) {
                    if (e.GetStatus () == HTTP::StatusCodes::kServiceUnavailable or e.GetStatus () == HTTP::StatusCodes::kGatewayTimeout or e.GetStatus () == HTTP::StatusCodes::kRequestTimeout) {
                        // Ignore/Eat
                        DbgTrace (L"Ignored error HTTP status %d", e.GetStatus ());
                    }
                    else {
                        Execution::ReThrow ();
                    }
                }
            }
        }
        void DoTests_ ()
        {
            Debug::TraceContextBumper ctx ("{}::Test_2_SimpleFetch_httpbin_");
            using namespace Private_;
            try {
                DoRegressionTests_ForConnectionFactory_ ([]() -> Connection { return CreateConnection (kDefaultTestOptions_); });
            }
            catch (const Execution::RequiredComponentMissingException&) {
#if !qHasFeature_LibCurl && !qHasFeature_WinHTTP
// OK to ignore. We don't wnat to call this failing a test, because there is nothing to fix.
// This is more like the absence of a feature beacuse of the missing component.
#else
                Execution::ReThrow ();
#endif
            }

#if qHasFeature_LibCurl
            DoRegressionTests_ForConnectionFactory_ ([]() -> Connection { return Connection_LibCurl (kDefaultTestOptions_); });
#endif
#if qHasFeature_WinHTTP
            DoRegressionTests_ForConnectionFactory_ ([]() -> Connection { return Connection_WinHTTP (kDefaultTestOptions_); });
#endif
        }
    }
}

namespace {
    namespace Test3_TextStreamResponse_ {
        namespace Private_ {
            void Test_1_SimpleFetch_Google_C_ (Connection c)
            {
                c.SetURL (URL::Parse (L"http://www.google.com"));
                Response r = c.GET ();
                VerifyTestResult (r.GetSucceeded ());
                for (auto i : r.GetHeaders ()) {
                    DbgTrace (L"%s=%s", i.fKey.c_str (), i.fValue.c_str ());
                }
                InternetMediaType contentType  = r.GetContentType ().Value ();
                String            responseText = r.GetDataTextInputStream ().ReadAll ();
                DbgTrace (L"responseText = %s", responseText.c_str ());
                VerifyTestResult (responseText.Contains (L"google", Characters::CompareOptions::eCaseInsensitive));
            }
            void DoRegressionTests_ForConnectionFactory_ (Connection (*factory) ())
            {
                Test_1_SimpleFetch_Google_C_ (factory ());
            }
        }
        void DoTests_ ()
        {
            Debug::TraceContextBumper ctx ("{}::Test3_TextStreamResponse_");
            using namespace Private_;
            try {
                DoRegressionTests_ForConnectionFactory_ ([]() -> Connection { return CreateConnection (kDefaultTestOptions_); });
            }
            catch (const Execution::RequiredComponentMissingException&) {
#if !qHasFeature_LibCurl && !qHasFeature_WinHTTP
// OK to ignore. We don't wnat to call this failing a test, because there is nothing to fix.
// This is more like the absence of a feature beacuse of the missing component.
#else
                Execution::ReThrow ();
#endif
            }

#if qHasFeature_LibCurl
            DoRegressionTests_ForConnectionFactory_ ([]() -> Connection { return Connection_LibCurl (kDefaultTestOptions_); });
#endif
#if qHasFeature_WinHTTP
            DoRegressionTests_ForConnectionFactory_ ([]() -> Connection { return Connection_WinHTTP (kDefaultTestOptions_); });
#endif
        }
    }
}

namespace {
    namespace Test_4_RefDocsTests_ {
        namespace Private_ {
            void T1_get_ ()
            {
                Connection c = IO::Network::Transfer::CreateConnection (kDefaultTestOptions_);
                c.SetURL (URL::Parse (L"http://www.google.com"));
                Response r = c.GET ();
                VerifyTestResult (r.GetSucceeded ());
                VerifyTestResult (r.GetData ().size () > 1);
            }
        }
        void DoTests_ ()
        {
            Debug::TraceContextBumper ctx ("{}::Test_4_RefDocsTests_");
            try {
                Private_::T1_get_ ();
            }
            catch (const Execution::RequiredComponentMissingException&) {
#if !qHasFeature_LibCurl && !qHasFeature_WinHTTP
// OK to ignore. We don't wnat to call this failing a test, because there is nothing to fix.
// This is more like the absence of a feature beacuse of the missing component.
#else
                Execution::ReThrow ();
#endif
            }
        }
    }
}

namespace {
    namespace Test_5_SSLCertCheckTests_ {
        namespace Private_ {
            void T1_get_ (Connection::Options o)
            {
                Connection c = IO::Network::Transfer::CreateConnection (o);
                try {
                    c.SetURL (URL::Parse (L"https://testssl-valid.disig.sk/index.en.html"));
                    Response r = c.GET ();
                    VerifyTestResult (r.GetSucceeded ());
                    VerifyTestResult (r.GetData ().size () > 1);
                }
#if qHasFeature_LibCurl
                catch (const LibCurlException& lce) {
#if !qHasFeature_OpenSSL
                    if (lce.GetCode () == CURLE_UNSUPPORTED_PROTOCOL) {
                        DbgTrace ("Warning - ignored exception doing LibCurl/ssl - for now probably just no SSL support with libcurl");
                        return;
                    }
#endif
                    throw;
                }
#else
                catch (...) {
                    throw;
                }
#endif
            }
        }
        void DoTests_ ()
        {
            Debug::TraceContextBumper ctx ("{}::Test_5_SSLCertCheckTests_");
            Connection::Options       o = kDefaultTestOptions_;
            try {
                o.fFailConnectionIfSSLCertificateInvalid = true;
                Private_::T1_get_ (o);
#if qHasFeature_LibCurl && !qHasFeature_OpenSSL
// ingore this case, since we allow the failed connect above...
#else
                VerifyTestResult (false);
#endif
            }
            catch (const Execution::RequiredComponentMissingException&) {
#if !qHasFeature_LibCurl && !qHasFeature_WinHTTP
// OK to ignore. We don't wnat to call this failing a test, because there is nothing to fix.
// This is more like the absence of a feature beacuse of the missing component.
#else
                Execution::ReThrow ();
#endif
            }
            catch (...) {
                DbgTrace (L"Good - this should fail");
            }
            try {
                o.fFailConnectionIfSSLCertificateInvalid = false;
                Private_::T1_get_ (o);
            }
            catch (const Execution::RequiredComponentMissingException&) {
#if !qHasFeature_LibCurl && !qHasFeature_WinHTTP
// OK to ignore. We don't wnat to call this failing a test, because there is nothing to fix.
// This is more like the absence of a feature beacuse of the missing component.
#else
                Execution::ReThrow ();
#endif
            }
            catch (...) {
                DbgTrace (L"e=%s", Characters::ToString (current_exception ()).c_str ());
                VerifyTestResult (false);
            }
        }
    }
}

namespace {
    void DoRegressionTests_ ()
    {
        Test_1_SimpleConnnectionTests_::DoTests_ ();
        Test_2_SimpleFetch_httpbin_::DoTests_ ();
        Test3_TextStreamResponse_::DoTests_ ();
        Test_4_RefDocsTests_::DoTests_ ();
        Test_5_SSLCertCheckTests_::DoTests_ ();
    }
}

int main (int argc, const char* argv[])
{
    Stroika::TestHarness::Setup ();
#if qPlatform_POSIX
    Execution::SignalHandlerRegistry::Get ().SetSignalHandlers (SIGPIPE, Execution::SignalHandlerRegistry::kIGNORED);
#endif
    return Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
}
