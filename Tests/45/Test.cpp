/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
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
#include "Stroika/Foundation/DataExchange/InternetMediaTypeRegistry.h"
#include "Stroika/Foundation/DataExchange/Variant/JSON/Reader.h"
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/Debug/Valgrind.h"
#include "Stroika/Foundation/Execution/Activity.h"
#include "Stroika/Foundation/Execution/RequiredComponentMissingException.h"
#include "Stroika/Foundation/Execution/SignalHandlers.h"
#include "Stroika/Foundation/Execution/Sleep.h"
#if qHasFeature_LibCurl
#include "Stroika/Foundation/IO/Network/Transfer/Connection_libcurl.h"
#endif
#if qHasFeature_WinHTTP
#include "Stroika/Foundation/IO/Network/Transfer/Connection_WinHTTP.h"
#endif
#include "Stroika/Foundation/IO/Network/Transfer/ConnectionPool.h"
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
        o.fMaxAutomaticRedirects = 2;
        return o;
    }();
}

namespace {
    namespace Test_1_SimpleConnnectionTests_ {
        namespace Private_ {
            void Test_1_SimpleFetch_Google_C_ (Connection::Ptr c)
            {
                Debug::TraceContextBumper ctx{"{}::...Test_1_SimpleFetch_Google_C_"};
                Response                  r = c.GET (URI{L"http://www.google.com"});
                VerifyTestResult (r.GetSucceeded ());
                VerifyTestResult (r.GetData ().size () > 1);
            }
            void Test_2_SimpleFetch_SSL_Google_C_ (Connection::Ptr c)
            {
                Debug::TraceContextBumper ctx{"{}::...Test_2_SimpleFetch_SSL_Google_C_"};
                try {
                    Response r = c.GET (URI{L"https://www.google.com"});
                    VerifyTestResult (r.GetSucceeded ());
                    VerifyTestResult (r.GetData ().size () > 1);
                }
                catch (const IO::Network::HTTP::Exception& e) {
                    if (e.IsServerError () or e.GetStatus () == IO::Network::HTTP::StatusCodes::kTooManyRequests) {
                        Stroika::TestHarness::WarnTestIssue (Characters::Format (L"Ignorning %s", Characters::ToString (e).c_str ()).c_str ());
                    }
                    else {
                        Execution::ReThrow ();
                    }
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
                    if (lce.code () == error_code{CURLE_SSL_CONNECT_ERROR, LibCurl_error_category ()} and Debug::IsRunningUnderValgrind ()) {
                        DbgTrace ("Warning - ignored exception doing LibCurl/ssl - - see qCompilerAndStdLib_openssl3_helgrind_Buggy");
                        return;
                    }
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
            void DoRegressionTests_ForConnectionFactory_ (Connection::Ptr (*factory) ())
            {
                try {
                    Test_1_SimpleFetch_Google_C_ (factory ());
#if qCompilerAndStdLib_arm_openssl_valgrind_Buggy
                    if (not Debug::IsRunningUnderValgrind ()) {
                        Test_2_SimpleFetch_SSL_Google_C_ (factory ());
                    }
#else
                    Test_2_SimpleFetch_SSL_Google_C_ (factory ());
#endif
                }
                catch (const IO::Network::HTTP::Exception& e) {
                    if (e.IsServerError () or e.GetStatus () == IO::Network::HTTP::StatusCodes::kTooManyRequests) {
                        Stroika::TestHarness::WarnTestIssue (Characters::Format (L"Ignorning %s", Characters::ToString (e).c_str ()).c_str ());
                    }
                    else {
                        Execution::ReThrow ();
                    }
                }
            }
        }
        void DoTests_ ()
        {
            Debug::TraceContextBumper     ctx{"{}::Test_1_SimpleConnnectionTests_"};
            constexpr Execution::Activity kActivity_{L"running Test_1_SimpleConnnectionTests_"sv};
            Execution::DeclareActivity    declareActivity{&kActivity_};
            using namespace Private_;
            try {
                DoRegressionTests_ForConnectionFactory_ ([] () -> Connection::Ptr { return Connection::New (kDefaultTestOptions_); });
            }
            catch (const Execution::RequiredComponentMissingException&) {
#if !qHasFeature_LibCurl && !qHasFeature_WinHTTP
                // OK to ignore. We don't wnat to call this failing a test, because there is nothing to fix.
                // This is more like the absence of a feature beacuse of the missing component.
                DbgTrace (L"ignore RequiredComponentMissingException cuz no curl/winhttp");
#else
                Execution::ReThrow ();
#endif
            }

#if qHasFeature_LibCurl
            DoRegressionTests_ForConnectionFactory_ ([] () -> Connection::Ptr { return Connection_LibCurl::New (kDefaultTestOptions_); });
#endif
#if qHasFeature_WinHTTP
            DoRegressionTests_ForConnectionFactory_ ([] () -> Connection::Ptr { return Connection_WinHTTP::New (kDefaultTestOptions_); });
#endif
        }
    }
}

namespace {
    namespace Test_2_SimpleFetch_httpbin_ {
        namespace Private_ {
            void T1_httpbin_SimpleGET_ (Connection::Ptr c)
            {
                Debug::TraceContextBumper ctx{"T1_httpbin_SimpleGET_"};
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
            void T2_httpbin_SimplePOST_ (Connection::Ptr c)
            {
                Debug::TraceContextBumper ctx{"T2_httpbin_SimplePOST_"};
                using Memory::BLOB;

                static mt19937 sRNG_;

                c.SetSchemeAndAuthority (URI{L"http://httpbin.org"});
                BLOB roundTripTestData = [] () {
                    Memory::StackBuffer<byte> buf{Debug::IsRunningUnderValgrind () ? 100u : 1024u};
                    for (size_t i = 0; i < buf.GetSize (); ++i) {
                        buf[i] = static_cast<byte> (uniform_int_distribution<unsigned short> () (sRNG_));
                    }
                    return BLOB{buf.begin (), buf.end ()};
                }();
                optional<Response>                    optResp;
                [[maybe_unused]] static constexpr int kMaxTryCount_{10}; // for some reason, this fails occasionally, due to network issues or overload of target machine
                [[maybe_unused]] unsigned int         tryCount{1};
#if qHasFeature_LibCurl
            again:
#endif
                try {
                    optResp = c.POST (URI{L"/post"}, roundTripTestData, DataExchange::InternetMediaTypes::kOctetStream);
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
                    String dataValueString = Memory::NullCoalesce (vv.Lookup (L"data")).As<String> ();
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
            void T3_httpbin_SimplePUT_ (Connection::Ptr c)
            {
                Debug::TraceContextBumper ctx{"T3_httpbin_SimplePUT_"};
                using Memory::BLOB;

                static mt19937 sRNG_;

                BLOB roundTripTestData = [] () {
                    Memory::StackBuffer<byte> buf{Debug::IsRunningUnderValgrind () ? 100u : 1024u};
                    for (size_t i = 0; i < buf.GetSize (); ++i) {
                        buf[i] = static_cast<byte> (uniform_int_distribution<unsigned short> () (sRNG_));
                    }
                    return BLOB (buf.begin (), buf.end ());
                }();
                Response r = c.PUT (URI{L"http://httpbin.org/put"}, roundTripTestData, DataExchange::InternetMediaTypes::kOctetStream);
                VerifyTestResult (r.GetSucceeded ()); // because throws on failure
                {
                    VariantValue                  v  = Variant::JSON::Reader ().Read (r.GetDataBinaryInputStream ());
                    Mapping<String, VariantValue> vv = v.As<Mapping<String, VariantValue>> ();
                    DbgTrace ("PUT parsed response:");
                    for (auto i : vv) {
                        DbgTrace (L"%s : %s", i.fKey.c_str (), i.fValue.As<String> ().c_str ());
                    }
                    String dataValueString = Memory::NullCoalesce (vv.Lookup (L"data")).As<String> ();
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
            void DoRegressionTests_ForConnectionFactory_ (Connection::Ptr (*factory) ())
            {
                Debug::TraceContextBumper ctx{"{}::...DoRegressionTests_ForConnectionFactory_"};
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
                        Connection::Ptr conn = factory ();
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
                catch (const IO::Network::HTTP::Exception& e) {
                    if (e.IsServerError () or e.GetStatus () == IO::Network::HTTP::StatusCodes::kTooManyRequests) {
                        Stroika::TestHarness::WarnTestIssue (Characters::Format (L"Ignorning %s", Characters::ToString (e).c_str ()).c_str ());
                    }
                    else {
                        Execution::ReThrow ();
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
                    Stroika::TestHarness::WarnTestIssue (Characters::ToString (current_exception ()).c_str ());
                }
#endif
                catch (const Execution::RequiredComponentMissingException&) {
#if !qHasFeature_LibCurl && !qHasFeature_WinHTTP
                    // OK to ignore. We don't wnat to call this failing a test, because there is nothing to fix.
                    // This is more like the absence of a feature beacuse of the missing component.
                    DbgTrace (L"ignore RequiredComponentMissingException cuz no curl/winhttp");
#else
                    Stroika::TestHarness::WarnTestIssue (Characters::ToString (current_exception ()).c_str ());
#endif
                }
                catch (...) {
                    Stroika::TestHarness::WarnTestIssue (Characters::ToString (current_exception ()).c_str ());
                }
            }
        }
        void DoTests_ ()
        {
            Debug::TraceContextBumper     ctx{"{}::Test_2_SimpleFetch_httpbin_"};
            constexpr Execution::Activity kActivity_{L"running Test_2_SimpleFetch_httpbin_"sv};
            Execution::DeclareActivity    declareActivity{&kActivity_};
            using namespace Private_;
            try {
                DoRegressionTests_ForConnectionFactory_ ([] () -> Connection::Ptr { return Connection::New (kDefaultTestOptions_); });
            }
            catch (const Execution::RequiredComponentMissingException&) {
#if !qHasFeature_LibCurl && !qHasFeature_WinHTTP
                // OK to ignore. We don't wnat to call this failing a test, because there is nothing to fix.
                // This is more like the absence of a feature beacuse of the missing component.
                DbgTrace (L"ignore RequiredComponentMissingException cuz no curl/winhttp");
#else
                Execution::ReThrow ();
#endif
            }

#if qHasFeature_LibCurl
            DoRegressionTests_ForConnectionFactory_ ([] () -> Connection::Ptr { return Connection_LibCurl::New (kDefaultTestOptions_); });
#endif
#if qHasFeature_WinHTTP
            DoRegressionTests_ForConnectionFactory_ ([] () -> Connection::Ptr { return Connection_WinHTTP::New (kDefaultTestOptions_); });
#endif
        }
    }
}

namespace {
    namespace Test3_TextStreamResponse_ {
        namespace Private_ {
            void Test_1_SimpleFetch_Google_C_ (Connection::Ptr c)
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
            void DoRegressionTests_ForConnectionFactory_ (Connection::Ptr (*factory) ())
            {
                Test_1_SimpleFetch_Google_C_ (factory ());
            }
        }
        void DoTests_ ()
        {
            Debug::TraceContextBumper     ctx{"{}::Test3_TextStreamResponse_"};
            constexpr Execution::Activity kActivity_{L"running Test3_TextStreamResponse_"sv};
            Execution::DeclareActivity    declareActivity{&kActivity_};
            using namespace Private_;
            try {
                DoRegressionTests_ForConnectionFactory_ ([] () -> Connection::Ptr { return Connection::New (kDefaultTestOptions_); });
            }
            catch (const IO::Network::HTTP::Exception& e) {
                if (e.IsServerError () or e.GetStatus () == IO::Network::HTTP::StatusCodes::kTooManyRequests) {
                    Stroika::TestHarness::WarnTestIssue (Characters::Format (L"Ignorning %s", Characters::ToString (e).c_str ()).c_str ());
                }
                else {
                    Execution::ReThrow ();
                }
            }
            catch (const Execution::RequiredComponentMissingException&) {
#if !qHasFeature_LibCurl && !qHasFeature_WinHTTP
                // OK to ignore. We don't wnat to call this failing a test, because there is nothing to fix.
                // This is more like the absence of a feature beacuse of the missing component.
                DbgTrace (L"ignore RequiredComponentMissingException cuz no curl/winhttp");
#else
                Execution::ReThrow ();
#endif
            }

#if qHasFeature_LibCurl
            DoRegressionTests_ForConnectionFactory_ ([] () -> Connection::Ptr { return Connection_LibCurl::New (kDefaultTestOptions_); });
#endif
#if qHasFeature_WinHTTP
            DoRegressionTests_ForConnectionFactory_ ([] () -> Connection::Ptr { return Connection_WinHTTP::New (kDefaultTestOptions_); });
#endif
        }
    }
}

namespace {
    namespace Test_4_RefDocsTests_ {
        namespace Private_ {
            void T1_get_ ()
            {
                Connection::Ptr c = IO::Network::Transfer::Connection::New (kDefaultTestOptions_);
                Response        r = c.GET (URI{L"http://www.google.com"});
                VerifyTestResultWarning (r.GetSucceeded ());
                VerifyTestResultWarning (r.GetData ().size () > 1);
            }
        }
        void DoTests_ ()
        {
            Debug::TraceContextBumper     ctx{"{}::Test_4_RefDocsTests_"};
            constexpr Execution::Activity kActivity_{L"running Test_4_RefDocsTests_"sv};
            Execution::DeclareActivity    declareActivity{&kActivity_};
            try {
                Private_::T1_get_ ();
            }
            catch (const IO::Network::HTTP::Exception& e) {
                if (e.IsServerError () or e.GetStatus () == IO::Network::HTTP::StatusCodes::kTooManyRequests) {
                    Stroika::TestHarness::WarnTestIssue (Characters::Format (L"Ignorning %s", Characters::ToString (e).c_str ()).c_str ());
                }
                else {
                    Execution::ReThrow ();
                }
            }
            catch (const Execution::RequiredComponentMissingException&) {
#if !qHasFeature_LibCurl && !qHasFeature_WinHTTP
                // OK to ignore. We don't wnat to call this failing a test, because there is nothing to fix.
                // This is more like the absence of a feature beacuse of the missing component.
                DbgTrace (L"ignore RequiredComponentMissingException cuz no curl/winhttp");
#else
                Execution::ReThrow ();
#endif
            }
        }
    }
}

namespace {
    namespace Test_5_SSLCertCheckTests_ {
        void DoTests_ ()
        {
            Debug::TraceContextBumper ctx{"{}::Test_5_SSLCertCheckTests_"};

            // Note, this code used to use https://testssl-valid.disig.sk/index.en.html, but that site started failing (bad cert) around 2020-02-01,
            // so switched to https://badssl.com/ (which seems to have good and bad ssl certs)
            auto T1_get_ignore_SSLNotConfigured = [] (Connection::Options o, const URI& uri) {
                Connection::Ptr c = IO::Network::Transfer::Connection::New (o);
                try {
                    Response r = c.GET (uri);
                    VerifyTestResultWarning (r.GetData ().size () > 1);
                }
                catch ([[maybe_unused]] const system_error& lce) {
#if qHasFeature_LibCurl && !qHasFeature_OpenSSL
                    if (lce.code () == error_code{CURLE_UNSUPPORTED_PROTOCOL, LibCurl_error_category ()}) {
                        DbgTrace ("Warning - ignored exception doing LibCurl/ssl - for now probably just no SSL support with libcurl");
                        return;
                    }
#endif
                    Execution::ReThrow ();
                }
                catch (const Execution::RequiredComponentMissingException&) {
#if !qHasFeature_LibCurl && !qHasFeature_WinHTTP
                    // OK to ignore. We don't want to call this failing a test, because there is nothing to fix.
                    // This is more like the absence of a feature beacuse of the missing component.
                    DbgTrace (L"ignore RequiredComponentMissingException cuz no curl/winhttp");
#else
                    Execution::ReThrow ();
#endif
                }
                catch (...) {
                    Execution::ReThrow ();
                }
            };

            constexpr Execution::Activity kActivity_{L"running Test_5_SSLCertCheckTests_"sv};
            Execution::DeclareActivity    declareActivity{&kActivity_};
            Connection::Options           o = kDefaultTestOptions_;

            // GOOD SSL SITE
            const URI kGoodSite_{L"https://badssl.com/"}; // ironically this is a site with good SSL cert
            try {
                o.fFailConnectionIfSSLCertificateInvalid = true;
                T1_get_ignore_SSLNotConfigured (o, kGoodSite_);
            }
            catch (const Execution::RequiredComponentMissingException&) {
#if !qHasFeature_LibCurl && !qHasFeature_WinHTTP
                // OK to ignore. We don't want to call this failing a test, because there is nothing to fix.
                // This is more like the absence of a feature beacuse of the missing component.
                DbgTrace (L"ignore RequiredComponentMissingException cuz no curl/winhttp");
#endif
            }
            catch (...) {
                // if transient issue, ignore
                Stroika::TestHarness::WarnTestIssue (Characters::Format (L"badssl.com site failed with fFailConnectionIfSSLCertificateInvalid = false: %s", Characters::ToString (current_exception ()).c_str ()).c_str ());
            }
            try {
                o.fFailConnectionIfSSLCertificateInvalid = false;
                T1_get_ignore_SSLNotConfigured (o, kGoodSite_);
            }
            catch (const Execution::RequiredComponentMissingException&) {
#if !qHasFeature_LibCurl && !qHasFeature_WinHTTP
                // OK to ignore. We don't want to call this failing a test, because there is nothing to fix.
                // This is more like the absence of a feature beacuse of the missing component.
                DbgTrace (L"ignore RequiredComponentMissingException cuz no curl/winhttp");
#endif
            }
            catch (...) {
                // if transient issue, ignore
                Stroika::TestHarness::WarnTestIssue (Characters::Format (L"badssl.com site failed with fFailConnectionIfSSLCertificateInvalid = false: %s", Characters::ToString (current_exception ()).c_str ()).c_str ());
            }

            // BAD SSL SITE
            const URI kBad_Expired_Site_{L"https://expired.badssl.com/"}; // see https://badssl.com/ - there are several other bads I could try

#if qCompilerAndStdLib_ASAN_windows_http_badheader_Buggy
            bool doTest = not Debug::kBuiltWithAddressSanitizer;
#else
            bool doTest = true;
#endif
            if (doTest) {
                try {
                    o.fFailConnectionIfSSLCertificateInvalid = true;
                    T1_get_ignore_SSLNotConfigured (o, kBad_Expired_Site_);
                    VerifyTestResult (false); // getting here means our check for invalid cert didn't work, so thats bad
                }
                catch (...) {
                    DbgTrace (L"Good - this should fail");
                }
                try {
                    o.fFailConnectionIfSSLCertificateInvalid = false;
                    T1_get_ignore_SSLNotConfigured (o, kBad_Expired_Site_);
                    // Getting here is fine - we should be able to ignore the invalid CERT
                }
                catch (const Execution::RequiredComponentMissingException&) {
#if !qHasFeature_LibCurl && !qHasFeature_WinHTTP
                    // OK to ignore. We don't want to call this failing a test, because there is nothing to fix.
                    // This is more like the absence of a feature beacuse of the missing component.
                    DbgTrace (L"ignore RequiredComponentMissingException cuz no curl/winhttp");
#endif
                }
                catch (...) {
                    Stroika::TestHarness::WarnTestIssue (Characters::Format (L"badssl.com site failed with fFailConnectionIfSSLCertificateInvalid = false: %s", Characters::ToString (current_exception ()).c_str ()).c_str ());
                }
            }
        }
    }
}

namespace {
    namespace Test_6_TestWithCache_ {
        namespace Private_ {
            void SimpleGetFetch_T1 (Connection::Ptr c)
            {
                Debug::TraceContextBumper ctx{"{}::...SimpleGetFetch_T1"};
#if qCompilerAndStdLib_ASAN_initializerlist_scope_Buggy
                static const auto kInitList_ = initializer_list<URI>{URI{L"http://httpbin.org/get"}, URI{L"http://www.google.com"}, URI{L"http://www.cnn.com"}};
#endif
                for (URI u :
#if qCompilerAndStdLib_ASAN_initializerlist_scope_Buggy
                     kInitList_
#else
                     initializer_list<URI>{URI{L"http://httpbin.org/get"}, URI{L"http://www.google.com"}, URI{L"http://www.cnn.com"}}
#endif
                ) {
#if qCompilerAndStdLib_arm_openssl_valgrind_Buggy
                    // Not SURE this is the same bug (openssl related) but could be due to redirect?) Anyhow - both are raspberrypi only - and valgrind only
                    if (u == URI{L"http://www.cnn.com"} and Debug::IsRunningUnderValgrind ()) {
                        continue; // sigill in c.GET (u) under valgrind, just on raspberrypi. just with valgrind, inside libcurl code, so not obviously our bug
                    }
#endif
                    try {
                        Response r = c.GET (u);
                        VerifyTestResult (not r.GetHeaders ().ContainsKey (Cache::DefaultOptions::kCachedResultHeaderDefault));
                        VerifyTestResult (r.GetSucceeded ());
                        VerifyTestResult (r.GetData ().size () > 1);
                        Response r2           = c.GET (u);
                        bool     wasFromCache = r2.GetHeaders ().ContainsKey (Cache::DefaultOptions::kCachedResultHeaderDefault);
                        VerifyTestResult (r.GetData () == r2.GetData () or not wasFromCache);                                                           // if not from cache, sources can give different answers
                        DbgTrace (L"2nd lookup (%s) wasFromCache=%s", Characters::ToString (u).c_str (), Characters::ToString (wasFromCache).c_str ()); // cannot assert cuz some servers cachable, others not
                    }
                    catch (const IO::Network::HTTP::Exception& e) {
                        if (e.IsServerError () or e.GetStatus () == IO::Network::HTTP::StatusCodes::kTooManyRequests) {
                            Stroika::TestHarness::WarnTestIssue (Characters::Format (L"Ignorning %s", Characters::ToString (e).c_str ()).c_str ());
                        }
                        else {
                            Execution::ReThrow ();
                        }
                    }
                }
            }
            void DoRegressionTests_ForConnectionFactory_ (function<Connection::Ptr ()> factory)
            {
                SimpleGetFetch_T1 (factory ());
            }
        }
        void DoTests_ ()
        {
            Debug::TraceContextBumper     ctx{"{}::Test_6_TestWithCache_"};
            constexpr Execution::Activity kActivity_{L"running Test_6_TestWithCache_"sv};
            Execution::DeclareActivity    declareActivity{&kActivity_};
            using namespace Private_;
#if qHasFeature_LibCurl
            DoRegressionTests_ForConnectionFactory_ ([=] () -> Connection::Ptr {
                Cache::DefaultOptions cacheOptions{};
                cacheOptions.fDefaultResourceTTL = 300s;
                Cache::Ptr          cache        = Cache::CreateDefault (cacheOptions);
                Connection::Options options      = kDefaultTestOptions_;
                options.fCache                   = cache;
                return Connection_LibCurl::New (options);
            });
#endif
#if qHasFeature_WinHTTP
            DoRegressionTests_ForConnectionFactory_ ([=] () -> Connection::Ptr {
                Cache::DefaultOptions cacheOptions{};
                cacheOptions.fDefaultResourceTTL = 300s;
                Cache::Ptr          cache        = Cache::CreateDefault (cacheOptions);
                Connection::Options options      = kDefaultTestOptions_;
                options.fCache                   = cache;
                return Connection_WinHTTP::New (options);
            });
#endif
        }
    }
}

namespace {
    namespace Test_7_TestWithConnectionPool_ {
        namespace Private_ {
            void SimpleGetFetch_T1 (function<Connection::Ptr (const URI& uriHint)> factory)
            {
                Debug::TraceContextBumper ctx{"{}::...SimpleGetFetch_T1"};
#if qCompilerAndStdLib_ASAN_initializerlist_scope_Buggy
                static const auto kInitList_ = initializer_list<URI>{URI{L"http://httpbin.org/get"}, URI{L"http://www.google.com"}, URI{L"http://www.cnn.com"}};
#endif
                for (URI u :
#if qCompilerAndStdLib_ASAN_initializerlist_scope_Buggy
                     kInitList_
#else
                     initializer_list<URI>{URI{L"http://httpbin.org/get"}, URI{L"http://www.google.com"}, URI{L"http://www.cnn.com"}}
#endif
                ) {
#if qCompilerAndStdLib_arm_openssl_valgrind_Buggy
                    // Not SURE this is the same bug (openssl related) but could be due to redirect?) Anyhow - both are raspberrypi only - and valgrind only
                    if (u == URI{L"http://www.cnn.com"} and Debug::IsRunningUnderValgrind ()) {
                        continue; // sigill in c.GET (u) under valgrind, just on raspberrypi. just with valgrind, inside libcurl code, so not obviously our bug
                    }
#endif
                    Connection::Ptr c = factory (u);
                    Response        r = c.GET (u);
                    VerifyTestResult (r.GetSucceeded ());
                    VerifyTestResult (r.GetData ().size () > 1);
                    Response r2           = c.GET (u);
                    bool     wasFromCache = r2.GetHeaders ().ContainsKey (Cache::DefaultOptions::kCachedResultHeaderDefault);
                    VerifyTestResult (r.GetData () == r2.GetData () or not wasFromCache);                                                           // if not from cache, sources can give different answers
                    DbgTrace (L"2nd lookup (%s) wasFromCache=%s", Characters::ToString (u).c_str (), Characters::ToString (wasFromCache).c_str ()); // cannot assert cuz some servers cachable, others not
                }
            }
            void DoRegressionTests_ForConnectionFactory_ (function<Connection::Ptr (const URI& uriHint)> factory)
            {
                SimpleGetFetch_T1 (factory);
                SimpleGetFetch_T1 (factory);
            }
        }
        void DoTests_ ()
        {
            Debug::TraceContextBumper     ctx{"{}::Test_7_TestWithConnectionPool_"};
            constexpr Execution::Activity kActivity_{L"running Test_7_TestWithConnectionPool_"sv};
            Execution::DeclareActivity    declareActivity{&kActivity_};
            using namespace Private_;

            Cache::DefaultOptions cacheOptions{};
            cacheOptions.fDefaultResourceTTL = 300s;
            Cache::Ptr cache                 = Cache::CreateDefault (cacheOptions);

            try {
                ConnectionPool connectionPoolWithCache{
                    ConnectionPool::Options{
                        3,
                        [&] () -> Connection::Ptr {
                            Connection::Options connOpts = kDefaultTestOptions_;
                            connOpts.fCache              = cache;
                            return Connection::New (connOpts);
                        }}};
                ConnectionPool connectionPoolWithoutCache{
                    ConnectionPool::Options{
                        3,
                        [] () -> Connection::Ptr {
                            return Connection::New (kDefaultTestOptions_);
                        }}};

                DoRegressionTests_ForConnectionFactory_ ([&] (const URI& uriHint) -> Connection::Ptr {
                    return connectionPoolWithoutCache.New (uriHint);
                });
                DoRegressionTests_ForConnectionFactory_ ([&] (const URI& uriHint) -> Connection::Ptr {
                    return connectionPoolWithCache.New (uriHint);
                });
            }
            catch (const IO::Network::HTTP::Exception& e) {
                if (e.IsServerError () or e.GetStatus () == IO::Network::HTTP::StatusCodes::kTooManyRequests) {
                    Stroika::TestHarness::WarnTestIssue (Characters::Format (L"Ignorning %s", Characters::ToString (e).c_str ()).c_str ());
                }
                else {
                    Execution::ReThrow ();
                }
            }
            catch (const Execution::RequiredComponentMissingException&) {
#if !qHasFeature_LibCurl && !qHasFeature_WinHTTP
                // OK to ignore. We don't wnat to call this failing a test, because there is nothing to fix.
                // This is more like the absence of a feature beacuse of the missing component.
                DbgTrace (L"ignore RequiredComponentMissingException cuz no curl/winhttp");
#else
                Execution::ReThrow ();
#endif
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
        Test_6_TestWithCache_::DoTests_ ();
        Test_7_TestWithConnectionPool_::DoTests_ ();
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
