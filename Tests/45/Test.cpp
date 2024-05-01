/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
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
#include "Stroika/Foundation/Debug/Visualizations.h"
#include "Stroika/Foundation/Execution/Activity.h"
#include "Stroika/Foundation/Execution/RequiredComponentMissingException.h"
#include "Stroika/Foundation/Execution/SignalHandlers.h"
#include "Stroika/Foundation/Execution/Sleep.h"
#include "Stroika/Foundation/Execution/TimeOutException.h"
#if qHasFeature_LibCurl
#include "Stroika/Foundation/IO/Network/Transfer/Connection_libcurl.h"
#endif
#if qHasFeature_WinHTTP
#include "Stroika/Foundation/IO/Network/Transfer/Connection_WinHTTP.h"
#endif
#include "Stroika/Foundation/IO/Network/Transfer/ConnectionPool.h"
#include "Stroika/Foundation/Time/Duration.h"

#include "Stroika/Frameworks/Test/TestHarness.h"

using std::byte;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters::Literals;
using namespace Stroika::Foundation::DataExchange;
using namespace Stroika::Foundation::IO;
using namespace Stroika::Foundation::IO::Network;
using namespace Stroika::Foundation::IO::Network::Transfer;

using namespace Stroika::Frameworks;

#if qHasFeature_GoogleTest
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
                Response                  r = c.GET (URI{"http://www.google.com"});
                EXPECT_TRUE (r.GetSucceeded ());
                EXPECT_TRUE (r.GetData ().size () > 1);
            }
            void Test_2_SimpleFetch_SSL_Google_C_ (Connection::Ptr c)
            {
                Debug::TraceContextBumper ctx{"{}::...Test_2_SimpleFetch_SSL_Google_C_"};
                try {
                    Response r = c.GET (URI{"https://www.google.com"});
                    EXPECT_TRUE (r.GetSucceeded ());
                    EXPECT_TRUE (r.GetData ().size () > 1);
                }
                catch (const IO::Network::HTTP::Exception& e) {
                    if (e.IsServerError () or e.GetStatus () == IO::Network::HTTP::StatusCodes::kTooManyRequests) {
                        Stroika::Frameworks::Test::WarnTestIssue (Characters::Format ("Ignoring {}"_f, e).c_str ());
                    }
                    else {
                        Execution::ReThrow ();
                    }
                }
                catch (const Execution::TimeOutException& e) {
                    Stroika::Frameworks::Test::WarnTestIssue (Characters::Format ("Ignoring {}"_f, e).c_str ());
                }
#if qHasFeature_LibCurl
                catch (const system_error& lce) {
#if !qHasFeature_OpenSSL
                    if (lce.code () == error_code{CURLE_UNSUPPORTED_PROTOCOL, LibCurl::error_category ()}) {
                        DbgTrace ("Warning - ignored exception doing LibCurl/ssl - for now probably just no SSL support with libcurl"_f);
                        return;
                    }
#endif
                    //https://stroika.atlassian.net/browse/STK-679
                    if (lce.code () == error_code{CURLE_SSL_CONNECT_ERROR, LibCurl::error_category ()} and Debug::IsRunningUnderValgrind ()) {
                        DbgTrace ("Warning - ignored exception doing LibCurl/ssl - - see qCompilerAndStdLib_openssl3_helgrind_Buggy"_f);
                        return;
                    }
                    if (lce.code () == error_code{CURLE_RECV_ERROR, LibCurl::error_category ()} and Debug::IsRunningUnderValgrind ()) {
                        DbgTrace ("Warning - ignored exception doing LibCurl/ssl - - see https://stroika.atlassian.net/browse/STK-679"_f);
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
                        Stroika::Frameworks::Test::WarnTestIssue (Characters::Format ("Ignoring {}"_f, e).c_str ());
                    }
                    else {
                        Execution::ReThrow ();
                    }
                }
                catch (const Execution::TimeOutException& e) {
                    Stroika::Frameworks::Test::WarnTestIssue (Characters::Format ("Ignoring {}"_f, e).c_str ());
                }
            }
        }
        void DoTests_ ()
        {
            Debug::TraceContextBumper     ctx{"{}::Test_1_SimpleConnnectionTests_"};
            constexpr Execution::Activity kActivity_{"running Test_1_SimpleConnnectionTests_"sv};
            Execution::DeclareActivity    declareActivity{&kActivity_};
            using namespace Private_;
            try {
                DoRegressionTests_ForConnectionFactory_ ([] () -> Connection::Ptr { return Connection::New (kDefaultTestOptions_); });
            }
            catch (const Execution::RequiredComponentMissingException&) {
#if !qHasFeature_LibCurl && !qHasFeature_WinHTTP
                // OK to ignore. We don't wnat to call this failing a test, because there is nothing to fix.
                // This is more like the absence of a feature beacuse of the missing component.
                DbgTrace ("ignore RequiredComponentMissingException cuz no curl/winhttp"_f);
#else
                Execution::ReThrow ();
#endif
            }

#if qHasFeature_LibCurl
            DoRegressionTests_ForConnectionFactory_ ([] () -> Connection::Ptr { return LibCurl::Connection::New (kDefaultTestOptions_); });
#endif
#if qHasFeature_WinHTTP
            DoRegressionTests_ForConnectionFactory_ ([] () -> Connection::Ptr { return WinHTTP::Connection::New (kDefaultTestOptions_); });
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
                Response                  r = c.GET (URI{"http://httpbin.org/get"});
                EXPECT_TRUE (r.GetSucceeded ());
                EXPECT_TRUE (r.GetData ().size () > 1);
                {
                    VariantValue                  v  = Variant::JSON::Reader{}.Read (r.GetDataBinaryInputStream ());
                    Mapping<String, VariantValue> vv = v.As<Mapping<String, VariantValue>> ();
                    EXPECT_TRUE (vv.ContainsKey ("args"));
                    EXPECT_TRUE (vv["url"] == "http://httpbin.org/get" or vv["url"] == "https://httpbin.org/get");
                }
            }
            DISABLE_COMPILER_MSC_WARNING_START (4102);
            DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Wunused-label\"")
            void T2_httpbin_SimplePOST_ (Connection::Ptr c)
            {
                Debug::TraceContextBumper ctx{"T2_httpbin_SimplePOST_"};
                using Memory::BLOB;

                static mt19937 sRNG_;

                c.SetSchemeAndAuthority (URI{"http://httpbin.org"});
                BLOB roundTripTestData = [] () {
                    Memory::StackBuffer<byte> buf{Debug::IsRunningUnderValgrind () ? 100u : 1024u};
                    for (size_t i = 0; i < buf.GetSize (); ++i) {
                        buf[i] = static_cast<byte> (uniform_int_distribution<unsigned short> () (sRNG_));
                    }
                    return BLOB{buf.begin (), buf.end ()};
                }();
                optional<Response> optResp;
                [[maybe_unused]] static constexpr int kMaxTryCount_{10}; // for some reason, this fails occasionally, due to network issues or overload of target machine
                [[maybe_unused]] unsigned int tryCount{1};
#if qHasFeature_LibCurl
            again:
#endif
                try {
                    optResp = c.POST (URI{"/post"}, roundTripTestData, DataExchange::InternetMediaTypes::kOctetStream);
                }
#if qHasFeature_LibCurl
                catch (const system_error& lce) {
#if qHasFeature_OpenSSL
                    if (lce.code () == error_code{CURLE_SEND_FAIL_REWIND, LibCurl::error_category ()}) {
                        DbgTrace ("Warning - ignored failure since rewinding of the data stream failed' (status CURLE_SEND_FAIL_REWIND) - "
                                  "try again ssl link"_f);
                        c.SetSchemeAndAuthority (URI{"https://httpbin.org/"});
                        if (tryCount < kMaxTryCount_) {
                            tryCount++;
                            Execution::Sleep (500ms * tryCount);
                            goto again;
                        }
                        Execution::ReThrow ();
                    }
#endif
                    if (lce.code () == error_code{CURLE_RECV_ERROR, LibCurl::error_category ()}) {
                        // Not sure why, but we sporadically get this error in regression tests, so try to eliminate it. Probably has todo with overloaded
                        // machine we are targetting.
                        DbgTrace ("Warning - ignored  since CURLE_RECV_ERROR' (status CURLE_RECV_ERROR) - try again "_f);
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
                EXPECT_TRUE (r.GetSucceeded ());
                {
                    VariantValue                  v  = Variant::JSON::Reader {}.Read (r.GetDataBinaryInputStream ());
                    Mapping<String, VariantValue> vv = v.As<Mapping<String, VariantValue>> ();
                    DbgTrace ("POST parsed response:"_f);
                    for (auto i : vv) {
                        DbgTrace ("{} : {}"_f, i.fKey, i.fValue.As<String> ());
                    }
                    String dataValueString = Memory::NullCoalesce (vv.Lookup ("data")).As<String> ();
                    {
                        size_t i = dataValueString.Find (',').value_or (String::npos);
                        if (i != -1) {
                            dataValueString = dataValueString.SubString (i + 1);
                        }
                    }
                    BLOB resultBLOB = Cryptography::Encoding::Algorithm::Base64::Decode (dataValueString.AsUTF8 ());
                    EXPECT_TRUE (resultBLOB == roundTripTestData);
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
                Response r = c.PUT (URI{"http://httpbin.org/put"}, roundTripTestData, DataExchange::InternetMediaTypes::kOctetStream);
                EXPECT_TRUE (r.GetSucceeded ()); // because throws on failure
                {
                    VariantValue                  v  = Variant::JSON::Reader {}.Read (r.GetDataBinaryInputStream ());
                    Mapping<String, VariantValue> vv = v.As<Mapping<String, VariantValue>> ();
                    DbgTrace ("PUT parsed response:"_f);
                    for (auto i : vv) {
                        DbgTrace ("{} : {}"_f, i.fKey, i.fValue);
                    }
                    String dataValueString = Memory::NullCoalesce (vv.Lookup ("data")).As<String> ();
                    {
                        size_t i = dataValueString.Find (',').value_or (String::npos);
                        if (i != -1) {
                            dataValueString = dataValueString.SubString (i + 1);
                        }
                    }
                    BLOB resultBLOB = Cryptography::Encoding::Algorithm::Base64::Decode (dataValueString.AsUTF8 ());
                    EXPECT_TRUE (resultBLOB == roundTripTestData);
                }
            }
            void DoRegressionTests_ForConnectionFactory_ (Connection::Ptr (*factory) ())
            {
                Debug::TraceContextBumper ctx{"{}::...DoRegressionTests_ForConnectionFactory_"};
                // Ignore some server-side errors, because this service we use (http://httpbin.org/) sometimes fails
                try {
                    {
                        T1_httpbin_SimpleGET_ (factory ());
                        T2_httpbin_SimplePOST_ (factory ());
                    }
                    {
                        // Connection re-use
                        Connection::Ptr conn = factory ();
                        T1_httpbin_SimpleGET_ (conn);
                        T2_httpbin_SimplePOST_ (conn);
                        T3_httpbin_SimplePUT_ (conn);
                    }
                }
                catch (const IO::Network::HTTP::Exception& e) {
                    if (e.IsServerError () or e.GetStatus () == IO::Network::HTTP::StatusCodes::kTooManyRequests) {
                        Stroika::Frameworks::Test::WarnTestIssue (Characters::Format ("Ignoring {}"_f, e).c_str ());
                    }
                    else {
                        Execution::ReThrow ();
                    }
                }
                catch (const Execution::TimeOutException& e) {
                    Stroika::Frameworks::Test::WarnTestIssue (Characters::Format ("Ignoring {}"_f, e).c_str ());
                }
#if qHasFeature_LibCurl
                // NOTE - even though this uses non-ssl URL, it gets redirected to SSL-based url, so we must support that to test this
                catch (const system_error& lce) {
#if !qHasFeature_OpenSSL
                    if (lce.code () == error_code{CURLE_UNSUPPORTED_PROTOCOL, LibCurl::error_category ()}) {
                        DbgTrace ("Warning - ignored exception doing LibCurl/ssl - for now probably just no SSL support with libcurl"_f);
                        return;
                    }
#endif
                    Stroika::Frameworks::Test::WarnTestIssue (Characters::ToString (current_exception ()).c_str ());
                }
#endif
                catch (const Execution::RequiredComponentMissingException&) {
#if !qHasFeature_LibCurl && !qHasFeature_WinHTTP
                    // OK to ignore. We don't wnat to call this failing a test, because there is nothing to fix.
                    // This is more like the absence of a feature beacuse of the missing component.
                    DbgTrace ("ignore RequiredComponentMissingException cuz no curl/winhttp");
#else
                    Stroika::Frameworks::Test::WarnTestIssue (Characters::ToString (current_exception ()).c_str ());
#endif
                }
                catch (...) {
                    Stroika::Frameworks::Test::WarnTestIssue (Characters::ToString (current_exception ()).c_str ());
                }
            }
        }
        void DoTests_ ()
        {
            Debug::TraceContextBumper     ctx{"{}::Test_2_SimpleFetch_httpbin_"};
            constexpr Execution::Activity kActivity_{"running Test_2_SimpleFetch_httpbin_"sv};
            Execution::DeclareActivity    declareActivity{&kActivity_};
            using namespace Private_;
            try {
                DoRegressionTests_ForConnectionFactory_ ([] () -> Connection::Ptr { return Connection::New (kDefaultTestOptions_); });
            }
            catch (const Execution::RequiredComponentMissingException&) {
#if !qHasFeature_LibCurl && !qHasFeature_WinHTTP
                // OK to ignore. We don't wnat to call this failing a test, because there is nothing to fix.
                // This is more like the absence of a feature beacuse of the missing component.
                DbgTrace ("ignore RequiredComponentMissingException cuz no curl/winhttp");
#else
                Execution::ReThrow ();
#endif
            }

#if qHasFeature_LibCurl
            DoRegressionTests_ForConnectionFactory_ ([] () -> Connection::Ptr { return LibCurl::Connection::New (kDefaultTestOptions_); });
#endif
#if qHasFeature_WinHTTP
            DoRegressionTests_ForConnectionFactory_ ([] () -> Connection::Ptr { return WinHTTP::Connection::New (kDefaultTestOptions_); });
#endif
        }
    }
}

namespace {
    namespace Test3_TextStreamResponse_ {
        namespace Private_ {
            void Test_1_SimpleFetch_Google_C_ (Connection::Ptr c)
            {
                Response r = c.GET (URI{"http://www.google.com"});
                VerifyTestResultWarning (r.GetSucceeded ());
                for (auto i : r.GetHeaders ()) {
                    DbgTrace ("{}={}"_f, i.fKey, i.fValue);
                }
                InternetMediaType contentType  = r.GetContentType ().value_or (InternetMediaType{});
                String            responseText = r.GetDataTextInputStream ().ReadAll ();
                DbgTrace (L"responseText = {}"_f, responseText);
                // rarely, but sometimes, this returns text that doesn't contain the word google --LGP 2019-04-19
                VerifyTestResultWarning (responseText.Contains ("google", Characters::eCaseInsensitive));
            }
            void DoRegressionTests_ForConnectionFactory_ (Connection::Ptr (*factory) ())
            {
                try {
                    Test_1_SimpleFetch_Google_C_ (factory ());
                }
                catch (const IO::Network::HTTP::Exception& e) {
                    if (e.IsServerError () or e.GetStatus () == IO::Network::HTTP::StatusCodes::kTooManyRequests) {
                        Stroika::Frameworks::Test::WarnTestIssue (Characters::Format ("Ignoring {}"_f, e).c_str ());
                    }
                    else {
                        Execution::ReThrow ();
                    }
                }
                catch (const Execution::TimeOutException& e) {
                    Stroika::Frameworks::Test::WarnTestIssue (Characters::Format (L"Ignoring {}"_f, e).c_str ());
                }
            }
        }
        void DoTests_ ()
        {
            Debug::TraceContextBumper     ctx{"{}::Test3_TextStreamResponse_"};
            constexpr Execution::Activity kActivity_{"running Test3_TextStreamResponse_"sv};
            Execution::DeclareActivity    declareActivity{&kActivity_};
            using namespace Private_;
            try {
                DoRegressionTests_ForConnectionFactory_ ([] () -> Connection::Ptr { return Connection::New (kDefaultTestOptions_); });
            }
            catch (const Execution::RequiredComponentMissingException&) {
#if !qHasFeature_LibCurl && !qHasFeature_WinHTTP
                // OK to ignore. We don't wnat to call this failing a test, because there is nothing to fix.
                // This is more like the absence of a feature beacuse of the missing component.
                DbgTrace ("ignore RequiredComponentMissingException cuz no curl/winhttp"_f);
#else
                Execution::ReThrow ();
#endif
            }

#if qHasFeature_LibCurl
            DoRegressionTests_ForConnectionFactory_ ([] () -> Connection::Ptr { return LibCurl::Connection::New (kDefaultTestOptions_); });
#endif
#if qHasFeature_WinHTTP
            DoRegressionTests_ForConnectionFactory_ ([] () -> Connection::Ptr { return WinHTTP::Connection::New (kDefaultTestOptions_); });
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
                Response        r = c.GET (URI{"http://www.google.com"});
                VerifyTestResultWarning (r.GetSucceeded ());
                VerifyTestResultWarning (r.GetData ().size () > 1);
            }
        }
        void DoTests_ ()
        {
            Debug::TraceContextBumper     ctx{"{}::Test_4_RefDocsTests_"};
            constexpr Execution::Activity kActivity_{"running Test_4_RefDocsTests_"sv};
            Execution::DeclareActivity    declareActivity{&kActivity_};
            try {
                Private_::T1_get_ ();
            }
            catch (const IO::Network::HTTP::Exception& e) {
                if (e.IsServerError () or e.GetStatus () == IO::Network::HTTP::StatusCodes::kTooManyRequests) {
                    Stroika::Frameworks::Test::WarnTestIssue (Characters::Format ("Ignoring {}"_f, e).c_str ());
                }
                else {
                    Execution::ReThrow ();
                }
            }
            catch (const Execution::TimeOutException& e) {
                Stroika::Frameworks::Test::WarnTestIssue (Characters::Format ("Ignoring {}"_f, e).c_str ());
            }
            catch (const Execution::RequiredComponentMissingException&) {
#if !qHasFeature_LibCurl && !qHasFeature_WinHTTP
                // OK to ignore. We don't wnat to call this failing a test, because there is nothing to fix.
                // This is more like the absence of a feature beacuse of the missing component.
                DbgTrace ("ignore RequiredComponentMissingException cuz no curl/winhttp"_f);
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
                    if (lce.code () == error_code{CURLE_UNSUPPORTED_PROTOCOL, Transfer::LibCurl::error_category ()}) {
                        DbgTrace ("Warning - ignored exception doing LibCurl/ssl - for now probably just no SSL support with libcurl"_f);
                        return;
                    }
#endif
                    Execution::ReThrow ();
                }
                catch (const Execution::RequiredComponentMissingException&) {
#if !qHasFeature_LibCurl && !qHasFeature_WinHTTP
                    // OK to ignore. We don't want to call this failing a test, because there is nothing to fix.
                    // This is more like the absence of a feature beacuse of the missing component.
                    DbgTrace ("ignore RequiredComponentMissingException cuz no curl/winhttp"_f);
#else
                    Execution::ReThrow ();
#endif
                }
                catch (...) {
                    Execution::ReThrow ();
                }
            };

            constexpr Execution::Activity kActivity_{"running Test_5_SSLCertCheckTests_"sv};
            Execution::DeclareActivity    declareActivity{&kActivity_};
            Connection::Options           o = kDefaultTestOptions_;

            // GOOD SSL SITE
            const URI kGoodSite_{"https://badssl.com/"}; // ironically this is a site with good SSL cert
            try {
                o.fFailConnectionIfSSLCertificateInvalid = true;
                T1_get_ignore_SSLNotConfigured (o, kGoodSite_);
            }
            catch (const Execution::RequiredComponentMissingException&) {
#if !qHasFeature_LibCurl && !qHasFeature_WinHTTP
                // OK to ignore. We don't want to call this failing a test, because there is nothing to fix.
                // This is more like the absence of a feature beacuse of the missing component.
                DbgTrace ("ignore RequiredComponentMissingException cuz no curl/winhttp"_f);
#endif
            }
            catch (...) {
                // if transient issue, ignore
                Stroika::Frameworks::Test::WarnTestIssue (
                    Characters::Format ("badssl.com site failed with fFailConnectionIfSSLCertificateInvalid = false: {}"_f, current_exception ())
                        .c_str ());
            }
            try {
                o.fFailConnectionIfSSLCertificateInvalid = false;
                T1_get_ignore_SSLNotConfigured (o, kGoodSite_);
            }
            catch (const Execution::RequiredComponentMissingException&) {
#if !qHasFeature_LibCurl && !qHasFeature_WinHTTP
                // OK to ignore. We don't want to call this failing a test, because there is nothing to fix.
                // This is more like the absence of a feature beacuse of the missing component.
                DbgTrace ("ignore RequiredComponentMissingException cuz no curl/winhttp"_f);
#endif
            }
            catch (...) {
                // if transient issue, ignore
                Stroika::Frameworks::Test::WarnTestIssue (
                    Characters::Format (L"badssl.com site failed with fFailConnectionIfSSLCertificateInvalid = false: {}"_f, current_exception ())
                        .c_str ());
            }

            // BAD SSL SITE
            const URI kBad_Expired_Site_{"https://expired.badssl.com/"}; // see https://badssl.com/ - there are several other bads I could try

            {
                try {
                    o.fFailConnectionIfSSLCertificateInvalid = true;
                    T1_get_ignore_SSLNotConfigured (o, kBad_Expired_Site_);
                    EXPECT_TRUE (false); // getting here means our check for invalid cert didn't work, so thats bad
                }
                catch (...) {
                    DbgTrace ("Good - this should fail"_f);
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
                    DbgTrace ("ignore RequiredComponentMissingException cuz no curl/winhttp"_f);
#endif
                }
                catch (...) {
                    Stroika::Frameworks::Test::WarnTestIssue (
                        Characters::Format (L"badssl.com site failed with fFailConnectionIfSSLCertificateInvalid = false: {}"_f, current_exception ())
                            .c_str ());
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
                static const auto kInitList_ =
                    initializer_list<URI>{URI{"http://httpbin.org/get"}, URI{"http://www.google.com"}, URI{"http://www.cnn.com"}};
#endif
                for (URI u :
#if qCompilerAndStdLib_ASAN_initializerlist_scope_Buggy
                     kInitList_
#else
                     initializer_list<URI>{URI{"http://httpbin.org/get"}, URI{"http://www.google.com"}, URI{"http://www.cnn.com"}}
#endif
                ) {
                    try {
                        Response r = c.GET (u);
                        EXPECT_TRUE (not r.GetHeaders ().ContainsKey (Cache::DefaultOptions::kCachedResultHeaderDefault));
                        EXPECT_TRUE (r.GetSucceeded ());
                        EXPECT_TRUE (r.GetData ().size () > 1);
                        Response r2           = c.GET (u);
                        bool     wasFromCache = r2.GetHeaders ().ContainsKey (Cache::DefaultOptions::kCachedResultHeaderDefault);
                        EXPECT_TRUE (r.GetData () == r2.GetData () or not wasFromCache); // if not from cache, sources can give different answers
                        DbgTrace ("2nd lookup ({}) wasFromCache={}"_f, Characters::ToString (u),
                                  Characters::ToString (wasFromCache)); // cannot assert cuz some servers cachable, others not
                    }
                    catch (const IO::Network::HTTP::Exception& e) {
                        if (e.IsServerError () or e.GetStatus () == IO::Network::HTTP::StatusCodes::kTooManyRequests) {
                            Stroika::Frameworks::Test::WarnTestIssue (Characters::Format (L"Ignoring {}"_f, e).c_str ());
                        }
                        else {
                            Execution::ReThrow ();
                        }
                    }
                    catch (const Execution::TimeOutException& e) {
                        Stroika::Frameworks::Test::WarnTestIssue (Characters::Format ("Ignoring {}"_f, e).As<wstring> ().c_str ());
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
            constexpr Execution::Activity kActivity_{"running Test_6_TestWithCache_"sv};
            Execution::DeclareActivity    declareActivity{&kActivity_};
            using namespace Private_;
#if qHasFeature_LibCurl
            DoRegressionTests_ForConnectionFactory_ ([=] () -> Connection::Ptr {
                Cache::DefaultOptions cacheOptions{};
                cacheOptions.fDefaultResourceTTL = 300s;
                Cache::Ptr          cache        = Cache::CreateDefault (cacheOptions);
                Connection::Options options      = kDefaultTestOptions_;
                options.fCache                   = cache;
                return LibCurl::Connection::New (options);
            });
#endif
#if qHasFeature_WinHTTP
            DoRegressionTests_ForConnectionFactory_ ([=] () -> Connection::Ptr {
                Cache::DefaultOptions cacheOptions{};
                cacheOptions.fDefaultResourceTTL = 300s;
                Cache::Ptr          cache        = Cache::CreateDefault (cacheOptions);
                Connection::Options options      = kDefaultTestOptions_;
                options.fCache                   = cache;
                return WinHTTP::Connection::New (options);
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
                static const auto kInitList_ =
                    initializer_list<URI>{URI{"http://httpbin.org/get"}, URI{"http://www.google.com"}, URI{"http://www.cnn.com"}};
#endif
                for (URI u :
#if qCompilerAndStdLib_ASAN_initializerlist_scope_Buggy
                     kInitList_
#else
                     initializer_list<URI>{URI{"http://httpbin.org/get"}, URI{"http://www.google.com"}, URI{"http://www.cnn.com"}}
#endif
                ) {
                    Connection::Ptr c = factory (u);
                    Response        r = c.GET (u);
                    EXPECT_TRUE (r.GetSucceeded ());
                    EXPECT_TRUE (r.GetData ().size () > 1);
                    Response r2           = c.GET (u);
                    bool     wasFromCache = r2.GetHeaders ().ContainsKey (Cache::DefaultOptions::kCachedResultHeaderDefault);
                    EXPECT_TRUE (r.GetData () == r2.GetData () or not wasFromCache); // if not from cache, sources can give different answers
                    DbgTrace ("2nd lookup ({}) wasFromCache={}"_f, Characters::ToString (u),
                              Characters::ToString (wasFromCache)); // cannot assert cuz some servers cachable, others not
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
            constexpr Execution::Activity kActivity_{"running Test_7_TestWithConnectionPool_"sv};
            Execution::DeclareActivity    declareActivity{&kActivity_};
            using namespace Private_;

            Cache::DefaultOptions cacheOptions{};
            cacheOptions.fDefaultResourceTTL = 300s;
            Cache::Ptr cache                 = Cache::CreateDefault (cacheOptions);

            try {
                ConnectionPool connectionPoolWithCache{ConnectionPool::Options{3, [&] () -> Connection::Ptr {
                                                                                   Connection::Options connOpts = kDefaultTestOptions_;
                                                                                   connOpts.fCache              = cache;
                                                                                   return Connection::New (connOpts);
                                                                               }}};
                ConnectionPool connectionPoolWithoutCache{
                    ConnectionPool::Options{3, [] () -> Connection::Ptr { return Connection::New (kDefaultTestOptions_); }}};

                DoRegressionTests_ForConnectionFactory_ (
                    [&] (const URI& uriHint) -> Connection::Ptr { return connectionPoolWithoutCache.New (uriHint); });
                DoRegressionTests_ForConnectionFactory_ (
                    [&] (const URI& uriHint) -> Connection::Ptr { return connectionPoolWithCache.New (uriHint); });
            }
            catch (const IO::Network::HTTP::Exception& e) {
                if (e.IsServerError () or e.GetStatus () == IO::Network::HTTP::StatusCodes::kTooManyRequests) {
                    Stroika::Frameworks::Test::WarnTestIssue (Characters::Format ("Ignoring {}"_f, e).c_str ());
                }
                else {
                    Execution::ReThrow ();
                }
            }
            catch (const Execution::TimeOutException& e) {
                Stroika::Frameworks::Test::WarnTestIssue (Characters::Format ("Ignoring {}"_f, e).c_str ());
            }
            catch (const Execution::RequiredComponentMissingException&) {
#if !qHasFeature_LibCurl && !qHasFeature_WinHTTP
                // OK to ignore. We don't wnat to call this failing a test, because there is nothing to fix.
                // This is more like the absence of a feature beacuse of the missing component.
                DbgTrace ("ignore RequiredComponentMissingException cuz no curl/winhttp"_f);
#else
                Execution::ReThrow ();
#endif
            }
        }
    }
}

namespace {
    GTEST_TEST (Foundation_Caching, all)
    {
        Test_1_SimpleConnnectionTests_::DoTests_ ();
        Test_2_SimpleFetch_httpbin_::DoTests_ ();
        Test3_TextStreamResponse_::DoTests_ ();
        Test_4_RefDocsTests_::DoTests_ ();
        Test_5_SSLCertCheckTests_::DoTests_ ();
        Test_6_TestWithCache_::DoTests_ ();
        Test_7_TestWithConnectionPool_::DoTests_ ();
    }
}
#endif

int main (int argc, const char* argv[])
{
    Test::Setup (argc, argv);
#if qPlatform_POSIX
    Execution::SignalHandlerRegistry::Get ().SetSignalHandlers (SIGPIPE, Execution::SignalHandlerRegistry::kIGNORED);
#endif
#if qHasFeature_GoogleTest
    return RUN_ALL_TESTS ();
#else
    cerr << "Stroika regression tests require building with google test feature [  PASSED  ]" << endl;
#endif
}
