/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
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
#include "Stroika/Foundation/Debug/Valgrind.h"
#include "Stroika/Foundation/Execution/Activity.h"
#include "Stroika/Foundation/Execution/RequiredComponentMissingException.h"
#include "Stroika/Foundation/Execution/SignalHandlers.h"
#include "Stroika/Foundation/Execution/Sleep.h"
#include "Stroika/Foundation/IO/Network/Transfer/Client.h"
#if qHasFeature_LibCurl
#include "Stroika/Foundation/IO/Network/Transfer/Client_libcurl.h"
#endif
#if qHasFeature_WinHTTP
#include "Stroika/Foundation/IO/Network/Transfer/Client_WinHTTP.h"
#endif
#include "Stroika/Foundation/Memory/Optional.h"
#include "Stroika/Foundation/Time/Duration.h"

#include "../TestHarness/TestHarness.h"

using std::byte;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::DataExchange;
using namespace Stroika::Foundation::IO;
using namespace Stroika::Foundation::IO::Network;
using namespace Stroika::Foundation::IO::Network::Transfer;

namespace {
    const Connection::Options kDefaultTestOptions_ = [] () {
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
                Debug::TraceContextBumper ctx ("{}::...Test_1_SimpleFetch_Google_C_");
                Response                  r = c.GET (URI{L"http://www.google.com"});
                VerifyTestResult (r.GetSucceeded ());
                VerifyTestResult (r.GetData ().size () > 1);
            }
            void Test_2_SimpleFetch_SSL_Google_C_ (Connection c)
            {
                Debug::TraceContextBumper ctx ("{}::...Test_2_SimpleFetch_SSL_Google_C_");
                try {
                    Response r = c.GET (URI{L"https://www.google.com"});
                    VerifyTestResult (r.GetSucceeded ());
                    VerifyTestResult (r.GetData ().size () > 1);
                }
#if qHasFeature_LibCurl
                catch (const system_error& lce) {
#if !qHasFeature_OpenSSL
                    if (lce.code () == error_code{CURLE_UNSUPPORTED_PROTOCOL, LibCurl_error_category ()}) {
                        DbgTrace ("Warning - ignored exception doing LibCurl/ssl - for now probably just no SSL support with libcurl");
                        return;
                    }
#endif
                    //https://stroika.atlassian.net/browse/STK-679
                    // MAYBE RELATED TO/SAME AS qCompilerAndStdLib_arm_openssl_valgrind_Buggy
                    if (lce.code () == error_code{CURLE_RECV_ERROR, LibCurl_error_category ()} and Debug::IsRunningUnderValgrind ()) {
                        DbgTrace ("Warning - ignored exception doing LibCurl/ssl - - see https://stroika.atlassian.net/browse/STK-679");
                        return;
                    }
                    Execution::ReThrow ();
                }
#endif
                catch (...) {
                    Execution::ReThrow ();
                }
            }
            void DoRegressionTests_ForConnectionFactory_ (Connection (*factory) ())
            {
                Test_1_SimpleFetch_Google_C_ (factory ());
#if qCompilerAndStdLib_arm_openssl_valgrind_Buggy
                if (not Debug::IsRunningUnderValgrind ()) {
                    Test_2_SimpleFetch_SSL_Google_C_ (factory ());
                }
#else
                Test_2_SimpleFetch_SSL_Google_C_ (factory ());
#endif
            }
        }
        void DoTests_ ()
        {
            Debug::TraceContextBumper     ctx ("{}::Test_1_SimpleConnnectionTests_");
            constexpr Execution::Activity kActivity_{L"running Test_1_SimpleConnnectionTests_"sv};
            Execution::DeclareActivity    declareActivity{&kActivity_};
            using namespace Private_;
            try {
                DoRegressionTests_ForConnectionFactory_ ([] () -> Connection { return CreateConnection (kDefaultTestOptions_); });
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
            DoRegressionTests_ForConnectionFactory_ ([] () -> Connection { return Connection_LibCurl (kDefaultTestOptions_); });
#endif
#if qHasFeature_WinHTTP
            DoRegressionTests_ForConnectionFactory_ ([] () -> Connection { return Connection_WinHTTP (kDefaultTestOptions_); });
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
                Response                  r = c.GET (URI{L"http://httpbin.org/get"});
                VerifyTestResult (r.GetSucceeded ());
                VerifyTestResult (r.GetData ().size () > 1);
                {
                    VariantValue                  v  = Variant::JSON::Reader ().Read (r.GetDataBinaryInputStream ());
                    Mapping<String, VariantValue> vv = v.As<Mapping<String, VariantValue>> ();
                    VerifyTestResult (vv.ContainsKey (L"args"));
                    VerifyTestResult (vv[L"url"] == L"http://httpbin.org/get" or vv[L"url"] == L"https://httpbin.org/get");
                }
            }
            DISABLE_COMPILER_MSC_WARNING_START (4102);
            DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Wunused-label\"")
            void T2_httpbin_SimplePOST_ (Connection c)
            {
                Debug::TraceContextBumper ctx ("T2_httpbin_SimplePOST_");
                using Memory::BLOB;

                static mt19937 sRNG_;

                c.SetSchemeAndAuthority (URI{L"http://httpbin.org"});
                BLOB roundTripTestData = [] () {
                    Memory::SmallStackBuffer<byte> buf (1024);
                    for (size_t i = 0; i < buf.GetSize (); ++i) {
                        buf[i] = static_cast<byte> (uniform_int_distribution<unsigned short> () (sRNG_));
                    }
                    return BLOB (buf.begin (), buf.end ());
                }();
                optional<Response>            optResp;
                static constexpr int          kMaxTryCount_{10}; // for some reason, this fails occasionally, due to network issues or overload of target machine
                [[maybe_unused]] unsigned int tryCount{1};
            again:
                try {
                    optResp = c.POST (URI{L"/post"}, roundTripTestData, DataExchange::PredefinedInternetMediaType::kOctetStream);
                }
#if qHasFeature_LibCurl
                catch (const system_error& lce) {
#if qHasFeature_OpenSSL
                    if (lce.code () == error_code{CURLE_SEND_FAIL_REWIND, LibCurl_error_category ()}) {
                        DbgTrace ("Warning - ignored failure since rewinding of the data stream failed' (status CURLE_SEND_FAIL_REWIND) - try again ssl link");
                        c.SetSchemeAndAuthority (URI{L"https://httpbin.org/"});
                        if (tryCount < kMaxTryCount_) {
                            tryCount++;
                            Execution::Sleep (500ms * tryCount);
                            goto again;
                        }
                        Execution::ReThrow ();
                    }
#endif
                    if (lce.code () == error_code{CURLE_RECV_ERROR, LibCurl_error_category ()}) {
                        // Not sure why, but we sporadically get this error in regression tests, so try to eliminate it. Probably has todo with overloaded
                        // machine we are targetting.
                        DbgTrace ("Warning - ignored  since CURLE_RECV_ERROR' (status CURLE_RECV_ERROR) - try again ");
                        if (tryCount < kMaxTryCount_) {
                            tryCount++;
                            Execution::Sleep (500ms * tryCount);
                            goto again;
                        }
                    }
                    Execution::ReThrow ();
                }
#endif
                catch (...) {
                    Execution::ReThrow ();
                }
                Response r = *optResp;
                VerifyTestResult (r.GetSucceeded ());
                {
                    VariantValue                  v  = Variant::JSON::Reader ().Read (r.GetDataBinaryInputStream ());
                    Mapping<String, VariantValue> vv = v.As<Mapping<String, VariantValue>> ();
                    DbgTrace ("POST parsed response:");
                    for (auto i : vv) {
                        DbgTrace (L"%s : %s", i.fKey.c_str (), i.fValue.As<String> ().c_str ());
                    }
                    String dataValueString = Memory::ValueOrDefault (vv.Lookup (L"data")).As<String> ();
                    {
                        size_t i = dataValueString.Find (',').value_or (String::npos);
                        if (i != -1) {
                            dataValueString = dataValueString.SubString (i + 1);
                        }
                    }
                    BLOB resultBLOB = Cryptography::Encoding::Algorithm::DecodeBase64 (dataValueString.AsUTF8 ());
                    VerifyTestResult (resultBLOB == roundTripTestData);
                }
            }
            DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Wunused-label\"")
            DISABLE_COMPILER_MSC_WARNING_END (4102);
            void T3_httpbin_SimplePUT_ (Connection c)
            {
                Debug::TraceContextBumper ctx ("T3_httpbin_SimplePUT_");
                using Memory::BLOB;

                static mt19937 sRNG_;

                BLOB roundTripTestData = [] () {
                    Memory::SmallStackBuffer<byte> buf (1024);
                    for (size_t i = 0; i < buf.GetSize (); ++i) {
                        buf[i] = static_cast<byte> (uniform_int_distribution<unsigned short> () (sRNG_));
                    }
                    return BLOB (buf.begin (), buf.end ());
                }();
                Response r = c.PUT (URI{L"http://httpbin.org/put"}, roundTripTestData, DataExchange::PredefinedInternetMediaType::kOctetStream);
                VerifyTestResult (r.GetSucceeded ()); // because throws on failure
                {
                    VariantValue                  v  = Variant::JSON::Reader ().Read (r.GetDataBinaryInputStream ());
                    Mapping<String, VariantValue> vv = v.As<Mapping<String, VariantValue>> ();
                    DbgTrace ("PUT parsed response:");
                    for (auto i : vv) {
                        DbgTrace (L"%s : %s", i.fKey.c_str (), i.fValue.As<String> ().c_str ());
                    }
                    String dataValueString = Memory::ValueOrDefault (vv.Lookup (L"data")).As<String> ();
                    {
                        size_t i = dataValueString.Find (',').value_or (String::npos);
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
                Debug::TraceContextBumper ctx ("{}::...DoRegressionTests_ForConnectionFactory_");
                // Ignore some server-side errors, because this service we use (http://httpbin.org/) sometimes fails
                try {
                    {
                        T1_httpbin_SimpleGET_ (factory ());
#if qCompilerAndStdLib_arm_openssl_valgrind_Buggy
                        if (not Debug::IsRunningUnderValgrind ()) {
                            T2_httpbin_SimplePOST_ (factory ());
                        }
#else
                        T2_httpbin_SimplePOST_ (factory ());
#endif
                    }
                    {
                        // Connection re-use
                        Connection conn = factory ();
                        T1_httpbin_SimpleGET_ (conn);
#if qCompilerAndStdLib_arm_openssl_valgrind_Buggy
                        if (not Debug::IsRunningUnderValgrind ()) {
                            T2_httpbin_SimplePOST_ (conn);
                        }
#else
                        T2_httpbin_SimplePOST_ (conn);
#endif
                        T3_httpbin_SimplePUT_ (conn);
                    }
                }
#if qHasFeature_LibCurl
                // NOTE - even though this uses non-ssl URL, it gets redirected to SSL-based url, so we must support that to test this
                catch (const system_error& lce) {
#if !qHasFeature_OpenSSL
                    if (lce.code () == error_code{CURLE_UNSUPPORTED_PROTOCOL, LibCurl_error_category ()}) {
                        DbgTrace ("Warning - ignored exception doing LibCurl/ssl - for now probably just no SSL support with libcurl");
                        return;
                    }
#endif
                    Stroika::TestHarness::WarnTestIssue (Characters::ToString (current_exception ()).AsNarrowSDKString ().c_str ());
                }
#endif
                catch (const Execution::RequiredComponentMissingException&) {
#if !qHasFeature_LibCurl && !qHasFeature_WinHTTP
                    // OK to ignore. We don't wnat to call this failing a test, because there is nothing to fix.
                    // This is more like the absence of a feature beacuse of the missing component.
#else
                    Stroika::TestHarness::WarnTestIssue (Characters::ToString (current_exception ()).AsNarrowSDKString ().c_str ());
#endif
                }
                catch (...) {
                    Stroika::TestHarness::WarnTestIssue (Characters::ToString (current_exception ()).AsNarrowSDKString ().c_str ());
                }
            }
        }
        void DoTests_ ()
        {
            Debug::TraceContextBumper     ctx ("{}::Test_2_SimpleFetch_httpbin_");
            constexpr Execution::Activity kActivity_{L"running Test_2_SimpleFetch_httpbin_"sv};
            Execution::DeclareActivity    declareActivity{&kActivity_};
            using namespace Private_;
            try {
                DoRegressionTests_ForConnectionFactory_ ([] () -> Connection { return CreateConnection (kDefaultTestOptions_); });
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
            DoRegressionTests_ForConnectionFactory_ ([] () -> Connection { return Connection_LibCurl (kDefaultTestOptions_); });
#endif
#if qHasFeature_WinHTTP
            DoRegressionTests_ForConnectionFactory_ ([] () -> Connection { return Connection_WinHTTP (kDefaultTestOptions_); });
#endif
        }
    }
}

namespace {
    namespace Test3_TextStreamResponse_ {
        namespace Private_ {
            void Test_1_SimpleFetch_Google_C_ (Connection c)
            {
                Response r = c.GET (URI{L"http://www.google.com"});
                VerifyTestResultWarning (r.GetSucceeded ());
                for (auto i : r.GetHeaders ()) {
                    DbgTrace (L"%s=%s", i.fKey.c_str (), i.fValue.c_str ());
                }
                InternetMediaType contentType  = r.GetContentType ().value_or (InternetMediaType{});
                String            responseText = r.GetDataTextInputStream ().ReadAll ();
                DbgTrace (L"responseText = %s", responseText.c_str ());
                // rarely, but sometimes, this returns text that doesn't contain the word google --LGP 2019-04-19
                VerifyTestResultWarning (responseText.Contains (L"google", Characters::CompareOptions::eCaseInsensitive));
            }
            void DoRegressionTests_ForConnectionFactory_ (Connection (*factory) ())
            {
                Test_1_SimpleFetch_Google_C_ (factory ());
            }
        }
        void DoTests_ ()
        {
            Debug::TraceContextBumper     ctx ("{}::Test3_TextStreamResponse_");
            constexpr Execution::Activity kActivity_{L"running Test3_TextStreamResponse_"sv};
            Execution::DeclareActivity    declareActivity{&kActivity_};
            using namespace Private_;
            try {
                DoRegressionTests_ForConnectionFactory_ ([] () -> Connection { return CreateConnection (kDefaultTestOptions_); });
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
            DoRegressionTests_ForConnectionFactory_ ([] () -> Connection { return Connection_LibCurl (kDefaultTestOptions_); });
#endif
#if qHasFeature_WinHTTP
            DoRegressionTests_ForConnectionFactory_ ([] () -> Connection { return Connection_WinHTTP (kDefaultTestOptions_); });
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
                Response   r = c.GET (URI{L"http://www.google.com"});
                VerifyTestResultWarning (r.GetSucceeded ());
                VerifyTestResultWarning (r.GetData ().size () > 1);
            }
        }
        void DoTests_ ()
        {
            Debug::TraceContextBumper     ctx ("{}::Test_4_RefDocsTests_");
            constexpr Execution::Activity kActivity_{L"running Test_4_RefDocsTests_"sv};
            Execution::DeclareActivity    declareActivity{&kActivity_};
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
                    Response r = c.GET (URI{L"https://testssl-valid.disig.sk/index.en.html"});
                    VerifyTestResultWarning (r.GetSucceeded ());
                    VerifyTestResultWarning (r.GetData ().size () > 1);
                }
#if qHasFeature_LibCurl
                catch (const system_error& lce) {
#if !qHasFeature_OpenSSL
                    if (lce.code () == error_code{CURLE_UNSUPPORTED_PROTOCOL, LibCurl_error_category ()}) {
                        DbgTrace ("Warning - ignored exception doing LibCurl/ssl - for now probably just no SSL support with libcurl");
                        return;
                    }
#endif
                    Execution::ReThrow ();
                }
#endif
                catch (...) {
                    Execution::ReThrow ();
                }
            }
        }
        void DoTests_ ()
        {
            Debug::TraceContextBumper     ctx ("{}::Test_5_SSLCertCheckTests_");
            constexpr Execution::Activity kActivity_{L"running Test_5_SSLCertCheckTests_"sv};
            Execution::DeclareActivity    declareActivity{&kActivity_};
            Connection::Options           o = kDefaultTestOptions_;
            try {
                o.fFailConnectionIfSSLCertificateInvalid = true;
                Private_::T1_get_ (o);
#if qHasFeature_LibCurl && !qHasFeature_OpenSSL
// ingore this case, since we allow the failed connect above...
#else
                VerifyTestResultWarning (false);
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
            catch (const system_error& lce) {
                if (lce.code () == errc::timed_out
#if qHasFeature_LibCurl
                    or (lce.code () == error_code{CURLE_COULDNT_CONNECT, LibCurl_error_category ()})
#endif
                ) {
                    DbgTrace (L"Ignored '%s' on Test_5_SSLCertCheckTests_", Characters::ToString (current_exception ()).c_str ());
                    cerr << endl
                         << "WARNING: Ignored '" << Characters::ToString (current_exception ()).AsNarrowSDKString () << "' on Test_5_SSLCertCheckTests_"
                         << endl;
                }
                else {
                    Execution::ReThrow ();
                }
            }
            catch (...) {
                DbgTrace (L"e=%s", Characters::ToString (current_exception ()).c_str ());
                VerifyTestResultWarning (false);
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
#if qCompilerAndStdLib_arm_openssl_valgrind_Buggy
        if (not Debug::IsRunningUnderValgrind ()) {
            Test_5_SSLCertCheckTests_::DoTests_ ();
        }
#else
        Test_5_SSLCertCheckTests_::DoTests_ ();
#endif
    }
}

int main ([[maybe_unused]] int argc, [[maybe_unused]] const char* argv[])
{
    Stroika::TestHarness::Setup ();
#if qPlatform_POSIX
    Execution::SignalHandlerRegistry::Get ().SetSignalHandlers (SIGPIPE, Execution::SignalHandlerRegistry::kIGNORED);
#endif
    return Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
}
