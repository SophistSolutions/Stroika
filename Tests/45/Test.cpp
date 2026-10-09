/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
//  TEST    Foundation::IO::Network::Transfer
#include "Stroika/Foundation/StroikaPreComp.h"

#include <iostream>
#include <random>

#if qStroika_HasComponent_libcurl
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
#include "Stroika/Foundation/Execution/OperationNotSupportedException.h"
#include "Stroika/Foundation/Execution/RequiredComponentMissingException.h"
#include "Stroika/Foundation/Execution/SignalHandlers.h"
#include "Stroika/Foundation/Execution/Sleep.h"
#include "Stroika/Foundation/Execution/Thread.h"
#include "Stroika/Foundation/Execution/WaitForIOReady.h"
#include "Stroika/Foundation/IO/Network/ConnectionOrientedMasterSocket.h"
#include "Stroika/Foundation/IO/Network/ConnectionOrientedStreamSocket.h"
#include "Stroika/Foundation/IO/Network/HTTP/Headers.h"
#include "Stroika/Foundation/IO/Network/HTTP/Methods.h"
#include "Stroika/Foundation/IO/Network/InternetAddress.h"
#include "Stroika/Foundation/IO/Network/SocketAddress.h"
#include "Stroika/Foundation/IO/Network/Transfer/Cache.h"
#if qStroika_HasComponent_libcurl
#include "Stroika/Foundation/IO/Network/Transfer/Connection_libcurl.h"
#endif
#if qStroika_HasComponent_WinHTTP
#include "Stroika/Foundation/IO/Network/Transfer/Connection_WinHTTP.h"
#endif
#include "Stroika/Foundation/IO/Network/Transfer/ConnectionPool.h"
#include "Stroika/Foundation/Time/Duration.h"

#include "Stroika/Frameworks/Test/TestHarness.h"

using std::byte;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters::Literals;
using namespace Stroika::Foundation::DataExchange;
using namespace Stroika::Foundation::Execution;
using namespace Stroika::Foundation::IO;
using namespace Stroika::Foundation::IO::Network;
using namespace Stroika::Foundation::IO::Network::Transfer;

using namespace Stroika::Frameworks;

#if qStroika_HasComponent_googletest
namespace {
    const Connection::Options kDefaultTestOptions_ = [] () {
        Connection::Options o;
        o.fMaxAutomaticRedirects = 2;
        return o;
    }();

    /*
     *  These tests talk to real servers (google, httpbin, cnn), so a timeout or a connection dropped mid-transfer says
     *  something about the server or the network, not about Stroika's HTTP client - such errors are warned about, not failed.
     */
    bool IsTransientNetworkError_ (const system_error& e)
    {
        // io_error: a failure sending or receiving mid-transfer (as libcurl reports CURLE_SEND_ERROR / CURLE_RECV_ERROR)
        for (errc c : {errc::timed_out, errc::connection_reset, errc::connection_aborted, errc::io_error}) {
            if (Execution::IsA (e, c)) {
                return true;
            }
        }
        return false;
    }
}

namespace {
    namespace Test_1_SimpleConnnectionTests_ {
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
                    Stroika::Frameworks::Test::WarnTestIssue ("Ignoring {}"_f(e));
                }
                else {
                    ReThrow ();
                }
            }
            catch (const system_error& e) {
#if qStroika_HasComponent_libcurl && !qStroika_HasComponent_OpenSSL
                if (e.code () == error_code{CURLE_UNSUPPORTED_PROTOCOL, LibCurl::error_category ()}) {
                    GTEST_SKIP () << "libcurl built without SSL support";
                }
#endif
                if (not IsTransientNetworkError_ (e)) {
                    ReThrow (); // only a network failure (see IsTransientNetworkError_) is tolerated here - real errors must still fail the test
                }
                Stroika::Frameworks::Test::WarnTestIssue ("Ignoring {}"_f(e));
            }
            catch (...) {
                ReThrow ();
            }
        }
        void DoRegressionTests_ForConnectionFactory_ (Connection::Ptr (*factory) ())
        {
            try {
                Test_1_SimpleFetch_Google_C_ (factory ());
                Test_2_SimpleFetch_SSL_Google_C_ (factory ());
            }
            catch (const IO::Network::HTTP::Exception& e) {
                if (e.IsServerError () or e.GetStatus () == IO::Network::HTTP::StatusCodes::kTooManyRequests) {
                    Stroika::Frameworks::Test::WarnTestIssue ("Ignoring {}"_f(e));
                }
                else {
                    ReThrow ();
                }
            }
            catch (const system_error& e) {
                if (not IsTransientNetworkError_ (e)) {
                    ReThrow (); // only a network failure (see IsTransientNetworkError_) is tolerated here - real errors must still fail the test
                }
                Stroika::Frameworks::Test::WarnTestIssue ("Ignoring {}"_f(e));
            }
        }
    }
    GTEST_TEST (Foundation_IO_Network_Transfer, SimpleConnnectionTests_)
    {
        Debug::TraceContextBumper ctx{"{}::SimpleConnnectionTests_"};
        constexpr Activity        kActivity_{"running SimpleConnnectionTests_"sv};
        DeclareActivity           declareActivity{&kActivity_};
        using namespace Test_1_SimpleConnnectionTests_;
        try {
            DoRegressionTests_ForConnectionFactory_ ([] () -> Connection::Ptr { return Connection::New (kDefaultTestOptions_); });
        }
        catch (const RequiredComponentMissingException&) {
#if !qStroika_HasComponent_libcurl && !qStroika_HasComponent_WinHTTP
            // OK to ignore. We don't wnat to call this failing a test, because there is nothing to fix.
            // This is more like the absence of a feature beacuse of the missing component.
            SkipTestPart ("no HTTP client built in (neither libcurl nor WinHTTP)");
#else
            ReThrow ();
#endif
        }

#if qStroika_HasComponent_libcurl
        DoRegressionTests_ForConnectionFactory_ ([] () -> Connection::Ptr { return LibCurl::Connection::New (kDefaultTestOptions_); });
#endif
#if qStroika_HasComponent_WinHTTP
        DoRegressionTests_ForConnectionFactory_ ([] () -> Connection::Ptr { return WinHTTP::Connection::New (kDefaultTestOptions_); });
#endif
    }
}

namespace {
    namespace Test_2_SimpleFetch_httpbin_ {
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
#if qStroika_HasComponent_libcurl
        again:
#endif
            try {
                optResp = c.POST (URI{"/post"}, TypedBLOB{roundTripTestData, DataExchange::InternetMediaTypes::kOctetStream});
            }
#if qStroika_HasComponent_libcurl
            catch (const system_error& lce) {
#if qStroika_HasComponent_OpenSSL
                if (lce.code () == error_code{CURLE_SEND_FAIL_REWIND, LibCurl::error_category ()}) {
                    DbgTrace ("Warning - ignored failure since rewinding of the data stream failed' (status CURLE_SEND_FAIL_REWIND) - "
                              "try again ssl link"_f);
                    c.SetSchemeAndAuthority (URI{"https://httpbin.org/"});
                    if (tryCount < kMaxTryCount_) {
                        tryCount++;
                        Execution::Sleep (500ms * tryCount);
                        goto again;
                    }
                    ReThrow ();
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
                ReThrow ();
            }
#endif
            catch (...) {
                ReThrow ();
            }
            Response r = *optResp;
            EXPECT_TRUE (r.GetSucceeded ());
            {
                VariantValue                  v  = Variant::JSON::Reader{}.Read (r.GetDataBinaryInputStream ());
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
            Response r = c.PUT (URI{"http://httpbin.org/put"}, TypedBLOB{roundTripTestData, DataExchange::InternetMediaTypes::kOctetStream});
            EXPECT_TRUE (r.GetSucceeded ()); // because throws on failure
            {
                VariantValue                  v  = Variant::JSON::Reader{}.Read (r.GetDataBinaryInputStream ());
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
                    Stroika::Frameworks::Test::WarnTestIssue ("Ignoring {}"_f(e));
                }
                else {
                    ReThrow ();
                }
            }
            catch (const system_error& e) {
#if qStroika_HasComponent_libcurl && !qStroika_HasComponent_OpenSSL
                // NOTE - even though this uses non-ssl URL, it gets redirected to SSL-based url, so we must support that to test this
                if (e.code () == error_code{CURLE_UNSUPPORTED_PROTOCOL, LibCurl::error_category ()}) {
                    DbgTrace ("Warning - ignored exception doing LibCurl/ssl - for now probably just no SSL support with libcurl"_f);
                    return;
                }
#endif
                if (not IsTransientNetworkError_ (e)) {
                    ReThrow (); // only a network failure (see IsTransientNetworkError_) is tolerated here - real errors must still fail the test
                }
                Stroika::Frameworks::Test::WarnTestIssue ("Ignoring {}"_f(e));
            }
            catch (const RequiredComponentMissingException&) {
#if !qStroika_HasComponent_libcurl && !qStroika_HasComponent_WinHTTP
                // OK to ignore. We don't wnat to call this failing a test, because there is nothing to fix.
                // This is more like the absence of a feature beacuse of the missing component.
                SkipTestPart ("no HTTP client built in (neither libcurl nor WinHTTP)");
#else
                Stroika::Frameworks::Test::WarnTestIssue (Characters::ToString (current_exception ()));
#endif
            }
            catch (...) {
                Stroika::Frameworks::Test::WarnTestIssue (Characters::ToString (current_exception ()));
            }
        }
    }
    GTEST_TEST (Foundation_IO_Network_Transfer, SimpleFetch_httpbin_)
    {
        Debug::TraceContextBumper ctx{"{}::SimpleFetch_httpbin_"};
        constexpr Activity        kActivity_{"running SimpleFetch_httpbin_"sv};
        DeclareActivity           declareActivity{&kActivity_};
        using namespace Test_2_SimpleFetch_httpbin_;
        try {
            DoRegressionTests_ForConnectionFactory_ ([] () -> Connection::Ptr { return Connection::New (kDefaultTestOptions_); });
        }
        catch (const RequiredComponentMissingException&) {
#if !qStroika_HasComponent_libcurl && !qStroika_HasComponent_WinHTTP
            // OK to ignore. We don't wnat to call this failing a test, because there is nothing to fix.
            // This is more like the absence of a feature beacuse of the missing component.
            SkipTestPart ("no HTTP client built in (neither libcurl nor WinHTTP)");
#else
            ReThrow ();
#endif
        }
#if qStroika_HasComponent_libcurl
        DoRegressionTests_ForConnectionFactory_ ([] () -> Connection::Ptr { return LibCurl::Connection::New (kDefaultTestOptions_); });
#endif
#if qStroika_HasComponent_WinHTTP
        DoRegressionTests_ForConnectionFactory_ ([] () -> Connection::Ptr { return WinHTTP::Connection::New (kDefaultTestOptions_); });
#endif
    }
}

namespace {
    namespace Test3_TextStreamResponse_ {
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
                    Stroika::Frameworks::Test::WarnTestIssue ("Ignoring {}"_f(e));
                }
                else {
                    ReThrow ();
                }
            }
            catch (const system_error& e) {
                if (not IsTransientNetworkError_ (e)) {
                    ReThrow (); // only a network failure (see IsTransientNetworkError_) is tolerated here - real errors must still fail the test
                }
                Stroika::Frameworks::Test::WarnTestIssue ("Ignoring {}"_f(e));
            }
        }
    }
    GTEST_TEST (Foundation_IO_Network_Transfer, TextStreamResponse_)
    {
        Debug::TraceContextBumper ctx{"{}::TextStreamResponse_"};
        constexpr Activity        kActivity_{"running TextStreamResponse_"sv};
        DeclareActivity           declareActivity{&kActivity_};
        using namespace Test3_TextStreamResponse_;
        try {
            DoRegressionTests_ForConnectionFactory_ ([] () -> Connection::Ptr { return Connection::New (kDefaultTestOptions_); });
        }
        catch (const RequiredComponentMissingException&) {
#if !qStroika_HasComponent_libcurl && !qStroika_HasComponent_WinHTTP
            // OK to ignore. We don't wnat to call this failing a test, because there is nothing to fix.
            // This is more like the absence of a feature beacuse of the missing component.
            SkipTestPart ("no HTTP client built in (neither libcurl nor WinHTTP)");
#else
            ReThrow ();
#endif
        }

#if qStroika_HasComponent_libcurl
        DoRegressionTests_ForConnectionFactory_ ([] () -> Connection::Ptr { return LibCurl::Connection::New (kDefaultTestOptions_); });
#endif
#if qStroika_HasComponent_WinHTTP
        DoRegressionTests_ForConnectionFactory_ ([] () -> Connection::Ptr { return WinHTTP::Connection::New (kDefaultTestOptions_); });
#endif
    }
}

namespace {
    namespace Test_4_RefDocsTests_ {
        void T1_get_ ()
        {
            Connection::Ptr c = IO::Network::Transfer::Connection::New (kDefaultTestOptions_);
            Response        r = c.GET (URI{"http://www.google.com"});
            VerifyTestResultWarning (r.GetSucceeded ());
            VerifyTestResultWarning (r.GetData ().size () > 1);
        }
    }
    GTEST_TEST (Foundation_IO_Network_Transfer, RefDocsTests_)
    {
        Debug::TraceContextBumper ctx{"{}::RefDocsTests_"};
        constexpr Activity        kActivity_{"running RefDocsTests_"sv};
        DeclareActivity           declareActivity{&kActivity_};
        try {
            Test_4_RefDocsTests_::T1_get_ ();
        }
        catch (const IO::Network::HTTP::Exception& e) {
            if (e.IsServerError () or e.GetStatus () == IO::Network::HTTP::StatusCodes::kTooManyRequests) {
                Stroika::Frameworks::Test::WarnTestIssue ("Ignoring {}"_f(e));
            }
            else {
                ReThrow ();
            }
        }
        catch (const system_error& e) {
            if (not IsTransientNetworkError_ (e)) {
                ReThrow (); // only a network failure (see IsTransientNetworkError_) is tolerated here - real errors must still fail the test
            }
            Stroika::Frameworks::Test::WarnTestIssue ("Ignoring {}"_f(e));
        }
        catch (const RequiredComponentMissingException&) {
#if !qStroika_HasComponent_libcurl && !qStroika_HasComponent_WinHTTP
            // OK to ignore. We don't wnat to call this failing a test, because there is nothing to fix.
            // This is more like the absence of a feature beacuse of the missing component.
            SkipTestPart ("no HTTP client built in (neither libcurl nor WinHTTP)");
#else
            ReThrow ();
#endif
        }
    }
}

namespace {
    GTEST_TEST (Foundation_IO_Network_Transfer, SSLCertCheckTests_)
    {
        Debug::TraceContextBumper ctx{"{}::SSLCertCheckTests_"};

        // Note, this code used to use https://testssl-valid.disig.sk/index.en.html, but that site started failing (bad cert) around 2020-02-01,
        // so switched to https://badssl.com/ (which seems to have good and bad ssl certs)
        auto T1_get_ignore_SSLNotConfigured = [] (Connection::Options o, const URI& uri) {
            Connection::Ptr c = IO::Network::Transfer::Connection::New (o);
            try {
                Response r = c.GET (uri);
                VerifyTestResultWarning (r.GetData ().size () > 1);
            }
            catch ([[maybe_unused]] const system_error& lce) {
#if qStroika_HasComponent_libcurl && !qStroika_HasComponent_OpenSSL
                if (lce.code () == error_code{CURLE_UNSUPPORTED_PROTOCOL, Transfer::LibCurl::error_category ()}) {
                    DbgTrace ("Warning - ignored exception doing LibCurl/ssl - for now probably just no SSL support with libcurl"_f);
                    return;
                }
#endif
                ReThrow ();
            }
            catch (const RequiredComponentMissingException&) {
#if !qStroika_HasComponent_libcurl && !qStroika_HasComponent_WinHTTP
                // OK to ignore. We don't want to call this failing a test, because there is nothing to fix.
                // This is more like the absence of a feature beacuse of the missing component.
                SkipTestPart ("no HTTP client built in (neither libcurl nor WinHTTP)");
#else
                ReThrow ();
#endif
            }
            catch (...) {
                ReThrow ();
            }
        };

        constexpr Activity  kActivity_{"running Test_5_SSLCertCheckTests_"sv};
        DeclareActivity     declareActivity{&kActivity_};
        Connection::Options o = kDefaultTestOptions_;

        // GOOD SSL SITE
        const URI kGoodSite_{"https://badssl.com/"}; // ironically this is a site with good SSL cert
        try {
            o.fFailConnectionIfSSLCertificateInvalid = true;
            T1_get_ignore_SSLNotConfigured (o, kGoodSite_);
        }
        catch (const RequiredComponentMissingException&) {
#if !qStroika_HasComponent_libcurl && !qStroika_HasComponent_WinHTTP
            // OK to ignore. We don't want to call this failing a test, because there is nothing to fix.
            // This is more like the absence of a feature beacuse of the missing component.
            SkipTestPart ("no HTTP client built in (neither libcurl nor WinHTTP)");
#endif
        }
        catch (...) {
            // if transient issue, ignore
            Stroika::Frameworks::Test::WarnTestIssue (
                "badssl.com site failed with fFailConnectionIfSSLCertificateInvalid = false: {}"_f(current_exception ()));
        }
        try {
            o.fFailConnectionIfSSLCertificateInvalid = false;
            T1_get_ignore_SSLNotConfigured (o, kGoodSite_);
        }
        catch (const RequiredComponentMissingException&) {
#if !qStroika_HasComponent_libcurl && !qStroika_HasComponent_WinHTTP
            // OK to ignore. We don't want to call this failing a test, because there is nothing to fix.
            // This is more like the absence of a feature beacuse of the missing component.
            SkipTestPart ("no HTTP client built in (neither libcurl nor WinHTTP)");
#endif
        }
        catch (...) {
            // if transient issue, ignore
            Stroika::Frameworks::Test::WarnTestIssue (
                L"badssl.com site failed with fFailConnectionIfSSLCertificateInvalid = false: {}"_f(current_exception ()));
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
            catch (const RequiredComponentMissingException&) {
#if !qStroika_HasComponent_libcurl && !qStroika_HasComponent_WinHTTP
                // OK to ignore. We don't want to call this failing a test, because there is nothing to fix.
                // This is more like the absence of a feature beacuse of the missing component.
                SkipTestPart ("no HTTP client built in (neither libcurl nor WinHTTP)");
#endif
            }
            catch (...) {
                Stroika::Frameworks::Test::WarnTestIssue (
                    "badssl.com site failed with fFailConnectionIfSSLCertificateInvalid = false: {}"_f(current_exception ()));
            }
        }
    }
}

namespace {
    namespace Test_6_TestWithCache_ {
        void SimpleGetFetch_T1 (Connection::Ptr c)
        {
            Debug::TraceContextBumper ctx{"{}::...SimpleGetFetch_T1"};
            for (URI u : initializer_list<URI>{URI{"http://httpbin.org/get"}, URI{"http://www.google.com"}, URI{"http://www.cnn.com"}}) {
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
                        Stroika::Frameworks::Test::WarnTestIssue ("Ignoring {}"_f(e));
                    }
                    else {
                        ReThrow ();
                    }
                }
                catch (const system_error& e) {
                    if (not IsTransientNetworkError_ (e)) {
                        ReThrow (); // only a network failure (see IsTransientNetworkError_) is tolerated here - real errors must still fail the test
                    }
                    Stroika::Frameworks::Test::WarnTestIssue ("Ignoring {}"_f(e));
                }
            }
        }
        void DoRegressionTests_ForConnectionFactory_ (function<Connection::Ptr ()> factory)
        {
            SimpleGetFetch_T1 (factory ());
        }
    }
    GTEST_TEST (Foundation_IO_Network_Transfer, TestWithCache_)
    {
        Debug::TraceContextBumper ctx{"{}::TestWithCache_"};
        constexpr Activity        kActivity_{"running TestWithCache_"sv};
        DeclareActivity           declareActivity{&kActivity_};
        using namespace Test_6_TestWithCache_;
#if qStroika_HasComponent_libcurl
        DoRegressionTests_ForConnectionFactory_ ([=] () -> Connection::Ptr {
            Cache::DefaultOptions cacheOptions{};
            cacheOptions.fDefaultResourceTTL = 300s;
            Cache::Ptr          cache        = Cache::CreateDefault (cacheOptions);
            Connection::Options options      = kDefaultTestOptions_;
            options.fCache                   = cache;
            return LibCurl::Connection::New (options);
        });
#endif
#if qStroika_HasComponent_WinHTTP
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

    /*
     *  A cached response gone stale is asked for again conditionally: with If-Modified-Since its Last-Modified - an HTTP date, so
     *  not in quotes - and If-None-Match its ETag (in quotes). No network: the cache's IRep, driven directly.
     */
    GTEST_TEST (Foundation_IO_Network_Transfer, Cache_StaleIsAskedForConditionally_)
    {
        Debug::TraceContextBumper ctx{"Cache_StaleIsAskedForConditionally_"};
        Cache::Ptr                cache = Cache::CreateDefault (Cache::DefaultOptions{});
        URI                       site{"http://www.example.com"sv};
        Request                   request;
        request.fMethod               = HTTP::Methods::kGet;
        request.fAuthorityRelativeURL = URI{"/a"sv};
        {
            Cache::EvalContext context;
            EXPECT_FALSE (cache->OnBeforeFetch (&context, site, &request)); // nothing cached yet
            Response response{Memory::BLOB{}, HTTP::StatusCodes::kOK,
                              Containers::Mapping<String, String>{{String{HTTP::HeaderName::kLastModified}, "Wed, 09 Jun 2021 10:18:14 GMT"sv},
                                                                  {String{HTTP::HeaderName::kETag}, "\"v1\""sv},
                                                                  {String{HTTP::HeaderName::kExpires}, "Wed, 09 Jun 2021 10:18:14 GMT"sv}}};
            cache->OnAfterFetch (context, &response);
        }
        // expired, so not answered from the cache - but asked for only if changed
        Cache::EvalContext context;
        EXPECT_FALSE (cache->OnBeforeFetch (&context, site, &request));
        EXPECT_EQ (request.fOverrideHeaders.LookupValue (String{HTTP::HeaderName::kIfModifiedSince}), "Wed, 09 Jun 2021 10:18:14 GMT"sv);
        EXPECT_EQ (request.fOverrideHeaders.LookupValue (String{HTTP::HeaderName::kIfNoneMatch}), "\"v1\""sv);
    }
}

namespace {
    namespace Test_7_TestWithConnectionPool_ {
        void SimpleGetFetch_T1 (function<Connection::Ptr (const URI& uriHint)> factory)
        {
            Debug::TraceContextBumper ctx{"{}::...SimpleGetFetch_T1"};
            for (URI u : initializer_list<URI>{URI{"http://httpbin.org/get"}, URI{"http://www.google.com"}, URI{"http://www.cnn.com"}}) {
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
    GTEST_TEST (Foundation_IO_Network_Transfer, TestWithConnectionPool_)
    {
        Debug::TraceContextBumper ctx{"{}::TestWithConnectionPool_"};
        constexpr Activity        kActivity_{"running TestWithConnectionPool_"sv};
        DeclareActivity           declareActivity{&kActivity_};
        using namespace Test_7_TestWithConnectionPool_;

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
                Stroika::Frameworks::Test::WarnTestIssue ("Ignoring {}"_f(e));
            }
            else {
                ReThrow ();
            }
        }
        catch (const system_error& e) {
            if (not IsTransientNetworkError_ (e)) {
                ReThrow (); // only a network failure (see IsTransientNetworkError_) is tolerated here - real errors must still fail the test
            }
            Stroika::Frameworks::Test::WarnTestIssue ("Ignoring {}"_f(e));
        }
        catch (const RequiredComponentMissingException&) {
#if !qStroika_HasComponent_libcurl && !qStroika_HasComponent_WinHTTP
            // OK to ignore. We don't wnat to call this failing a test, because there is nothing to fix.
            // This is more like the absence of a feature beacuse of the missing component.
            SkipTestPart ("no HTTP client built in (neither libcurl nor WinHTTP)");
#else
            ReThrow ();
#endif
        }
    }
}

namespace {
    // a Connection to nothing - enough to see what a ConnectionPool hands out, without a network or an HTTP client
    struct FakeConnectionRep_ : Connection::IRep {
        URI                         fSchemeAndAuthority;
        virtual Connection::Options GetOptions () const override
        {
            return {};
        }
        virtual URI GetSchemeAndAuthority () const override
        {
            return fSchemeAndAuthority;
        }
        virtual void SetSchemeAndAuthority (const URI& schemeAndAuthority) override
        {
            fSchemeAndAuthority = schemeAndAuthority;
        }
        virtual Time::DurationSeconds GetTimeout () const override
        {
            return 1s;
        }
        virtual void SetTimeout (Time::DurationSeconds) override
        {
        }
        virtual void Close () override
        {
        }
        virtual Response Send (const Request&) override
        {
            Execution::Throw (Execution::OperationNotSupportedException{"FakeConnectionRep_::Send"sv});
        }
    };
    GTEST_TEST (Foundation_IO_Network_Transfer, ConnectionPoolLimits_)
    {
        Debug::TraceContextBumper ctx{"{}::ConnectionPoolLimits_"};
        unsigned int              made    = 0; // called under the pool's lock
        auto                      factory = [&made] () -> Connection::Ptr {
            ++made;
            return Connection::Ptr{Memory::MakeSharedPtr<FakeConnectionRep_> ()};
        };
        {
            // with no limit given, as many as are asked for
            ConnectionPool            pool{ConnectionPool::Options{nullopt, factory}};
            optional<Connection::Ptr> a;
            optional<Connection::Ptr> b;
            EXPECT_NO_THROW (a = pool.New ()) << "no limit given, yet none handed out";
            EXPECT_NO_THROW (b = pool.New ());
            EXPECT_EQ (made, 2u);
        }
        made = 0;
        {
            // at its limit, New () waits for one to come back - and gets that one, not a new one
            ConnectionPool            pool{ConnectionPool::Options{1, factory}};
            optional<Connection::Ptr> first    = pool.New ();
            Thread::Ptr               giveBack = Thread::New (
                [&first] () {
                    Execution::Sleep (200ms);
                    first = nullopt;
                },
                Thread::eAutoStart);
            optional<Connection::Ptr> second;
            EXPECT_NO_THROW (second = pool.New (10s)) << "waited for none to come back";
            giveBack.Join ();
            EXPECT_EQ (made, 1u);
        }
        made = 0;
        {
            // and if none comes back in time, it waits that long, then throws timed_out - or, if asked, hands out one from outside
            ConnectionPool         pool{ConnectionPool::Options{1, factory}};
            Connection::Ptr        held  = pool.New ();
            Time::TimePointSeconds start = Time::GetTickCount ();
            try {
                [[maybe_unused]] Connection::Ptr c = pool.New (300ms);
                ADD_FAILURE () << "a second connection from a pool of one";
            }
            catch (const system_error& e) {
                EXPECT_TRUE (Execution::IsA (e, errc::timed_out)) << e.what ();
            }
            EXPECT_GE (Time::GetTickCount () - start, 250ms) << "did not wait";
            Connection::Ptr outside = pool.New (ConnectionPool::eAllocateGloballyIfTimeout, 100ms);
            EXPECT_EQ (made, 2u);
        }
    }
}

namespace {
    GTEST_TEST (Foundation_IO_Network_Transfer, ErrTest)
    {
        Debug::TraceContextBumper ctx{"{}::ErrTest"};
        constexpr Activity        kActivity_{"running ErrTest"sv};
        DeclareActivity           declareActivity{&kActivity_};

        try {
            using namespace Memory;
            Connection::Ptr       conn = Connection::New ();
            [[maybe_unused]] auto r =
                conn.POST (URI{"https://oauth2.googleapis.com/token"}, TypedBLOB{"aaa=bbb"_blob, InternetMediaTypes::kWWWFormURLEncoded});
        }
        catch (const IO::Network::HTTP::Exception& e) {
            DbgTrace ("e={}"_f, e);
        }
        catch (const system_error& e) {
            if (not IsTransientNetworkError_ (e)) {
                ReThrow (); // only a network failure (see IsTransientNetworkError_) is tolerated here - real errors must still fail the test
            }
            Stroika::Frameworks::Test::WarnTestIssue ("Ignoring {}"_f(e));
        }
        catch (const RequiredComponentMissingException&) {
#if !qStroika_HasComponent_libcurl && !qStroika_HasComponent_WinHTTP
            // OK to ignore. We don't wnat to call this failing a test, because there is nothing to fix.
            // This is more like the absence of a feature beacuse of the missing component.
            SkipTestPart ("no HTTP client built in (neither libcurl nor WinHTTP)");
#else
            ReThrow ();
#endif
        }
    }
}

namespace {
    /*
     *  Pins the libcurl end of the error_code -> exception contract. Deliberately needs NO network: what is
     *  being tested is the CATEGORY's mapping onto std::errc, which is what lets a libcurl error answer to a
     *  portable condition test.
     *
     *  This is the regression test for the defect that cost two release-validation re-runs in 3.0d24.
     *  CURLE_OPERATION_TIMEDOUT surfaced as a system_error in libcurl's own category, so the timeout tolerance
     *  elsewhere in THIS file - which caught Execution::TimeOutException - never matched it, and the runs
     *  failed on an unreachable www.cnn.com rather than warning. @see Execution::ThrowError.
     */
    GTEST_TEST (Foundation_IO_Network_Transfer, LibCurlErrors_MapOntoPortableConditions_)
    {
        Debug::TraceContextBumper ctx{"LibCurlErrors_MapOntoPortableConditions_"};
#if qStroika_HasComponent_libcurl
        // the category must map libcurl's own numbering onto the portable conditions ...
        EXPECT_TRUE ((error_code{CURLE_OPERATION_TIMEDOUT, LibCurl::error_category ()} == errc::timed_out));
        EXPECT_TRUE ((error_code{CURLE_OUT_OF_MEMORY, LibCurl::error_category ()} == errc::not_enough_memory));
        // ... including the network failures a caller may want to treat as transient (@see IsTransientNetworkError_)
        EXPECT_TRUE ((error_code{CURLE_SEND_ERROR, LibCurl::error_category ()} == errc::io_error));
        EXPECT_TRUE ((error_code{CURLE_RECV_ERROR, LibCurl::error_category ()} == errc::io_error));
        EXPECT_TRUE ((error_code{CURLE_GOT_NOTHING, LibCurl::error_category ()} == errc::connection_reset));
        EXPECT_TRUE ((error_code{CURLE_COULDNT_CONNECT, LibCurl::error_category ()} == errc::connection_refused));
        EXPECT_TRUE ((error_code{CURLE_COULDNT_RESOLVE_HOST, LibCurl::error_category ()} == errc::no_such_device)); // as DNS maps EAI_NONAME
        EXPECT_TRUE ((error_code{CURLE_COULDNT_RESOLVE_PROXY, LibCurl::error_category ()} == errc::no_such_device));
        EXPECT_TRUE ((error_code{CURLE_PARTIAL_FILE, LibCurl::error_category ()} == errc::io_error));
        EXPECT_TRUE ((error_code{CURLE_AGAIN, LibCurl::error_category ()} == errc::resource_unavailable_try_again));
        // ... and the rest of what has a sensible portable meaning
        EXPECT_TRUE ((error_code{CURLE_UNSUPPORTED_PROTOCOL, LibCurl::error_category ()} == errc::protocol_not_supported));
        EXPECT_TRUE ((error_code{CURLE_NOT_BUILT_IN, LibCurl::error_category ()} == errc::not_supported));
        EXPECT_TRUE ((error_code{CURLE_URL_MALFORMAT, LibCurl::error_category ()} == errc::invalid_argument));
        EXPECT_TRUE ((error_code{CURLE_BAD_FUNCTION_ARGUMENT, LibCurl::error_category ()} == errc::invalid_argument));
        EXPECT_TRUE ((error_code{CURLE_WEIRD_SERVER_REPLY, LibCurl::error_category ()} == errc::protocol_error));
        EXPECT_TRUE ((error_code{CURLE_PEER_FAILED_VERIFICATION, LibCurl::error_category ()} == errc::protocol_error));
        EXPECT_TRUE ((error_code{CURLE_REMOTE_ACCESS_DENIED, LibCurl::error_category ()} == errc::permission_denied));
        EXPECT_TRUE ((error_code{CURLE_REMOTE_FILE_NOT_FOUND, LibCurl::error_category ()} == errc::no_such_file_or_directory));
        EXPECT_TRUE ((error_code{CURLE_FILESIZE_EXCEEDED, LibCurl::error_category ()} == errc::file_too_large));
        EXPECT_TRUE ((error_code{CURLE_WRITE_ERROR, LibCurl::error_category ()} == errc::io_error));
        EXPECT_TRUE ((error_code{CURLE_READ_ERROR, LibCurl::error_category ()} == errc::io_error));
        EXPECT_TRUE ((error_code{CURLE_ABORTED_BY_CALLBACK, LibCurl::error_category ()} == errc::operation_canceled));

        // ... and throwing one must surface as something a caller can test portably, WITHOUT knowing it came
        // from libcurl. Note e.code () still reports the original libcurl code and category.
        try {
            ThrowError (error_code{CURLE_OPERATION_TIMEDOUT, LibCurl::error_category ()});
            EXPECT_TRUE (false);
        }
        catch (const system_error& e) {
            EXPECT_TRUE (Execution::IsA (e, errc::timed_out));
            EXPECT_TRUE (e.code ().value () == CURLE_OPERATION_TIMEDOUT);
            EXPECT_TRUE (e.code ().category () == LibCurl::error_category ());
        }
        catch (...) {
            EXPECT_TRUE (false);
        }

        // out-of-memory promotes clear out of the system_error hierarchy - see the ThrowError promotion table
        try {
            ThrowError (error_code{CURLE_OUT_OF_MEMORY, LibCurl::error_category ()});
            EXPECT_TRUE (false);
        }
        catch (const bad_alloc&) {
            // Good
        }
        catch (...) {
            EXPECT_TRUE (false);
        }

        // an unmapped libcurl code must NOT masquerade as a timeout (or as any of the network failures above)
        EXPECT_FALSE ((error_code{CURLE_TOO_MANY_REDIRECTS, LibCurl::error_category ()} == errc::timed_out));
        EXPECT_FALSE ((error_code{CURLE_TOO_MANY_REDIRECTS, LibCurl::error_category ()} == errc::io_error));
        EXPECT_FALSE ((error_code{CURLE_UNSUPPORTED_PROTOCOL, LibCurl::error_category ()} == errc::timed_out));
#endif
    }
}

namespace {
    /*
     *  A request whose method is neither GET, POST nor PUT - UPnP's NOTIFY, say - sends its body too, and the connection's next
     *  request is sent with its own method. Before Stroika v3.0d25 libcurl's sent no body (so a GENA NOTIFY arrived empty), and
     *  sent the next GET by the name of the method before it; WinHTTP's did both right. Against a server on loopback, which
     *  answers each request with its method, a newline, and its body.
     */
    GTEST_TEST (Foundation_IO_Network_Transfer, OtherMethodsSendTheirBody_)
    {
        Debug::TraceContextBumper           ctx{"OtherMethodsSendTheirBody_"};
        ConnectionOrientedMasterSocket::Ptr listener = ConnectionOrientedMasterSocket::New (SocketAddress::INET, Socket::STREAM);
        listener.Bind (SocketAddress{V4::kLocalhost, 0});
        listener.Listen (5);
        const URI site{"http://127.0.0.1:{}"_f(listener.GetLocalAddress ()->GetPort ())};
        // each connection, one request. Accept only once one is waiting: a thread blocked in Accept cannot be aborted on Windows
        Thread::CleanupPtr server{Thread::CleanupPtr::eAbortBeforeWaiting,
                                  Thread::New (
                                      [listener] () {
                                          while (true) {
                                              Thread::CheckForInterruption ();
                                              if (WaitForIOReady<ConnectionOrientedMasterSocket::Ptr>{listener}.WaitQuietly (100ms).empty ()) {
                                                  continue;
                                              }
                                              ConnectionOrientedStreamSocket::Ptr s = listener.Accept ();
                                              string                              request;
                                              byte                                buf[4096];
                                              auto                                readMore = [&] () {
                                                  span<byte> got = s.Read (span{buf});
                                                  request.append (reinterpret_cast<const char*> (got.data ()), got.size ());
                                                  return not got.empty ();
                                              };
                                              while (request.find ("\r\n\r\n") == string::npos and readMore ()) {
                                              }
                                              const size_t headersEnd = request.find ("\r\n\r\n");
                                              if (headersEnd == string::npos) {
                                                  continue;
                                              }
                                              size_t length = 0;
                                              for (const string& h : {"\r\nContent-Length:"s, "\r\ncontent-length:"s}) {
                                                  if (size_t at = request.find (h); at != string::npos and at < headersEnd) {
                                                      length = static_cast<size_t> (std::stoul (request.substr (at + h.size ())));
                                                  }
                                              }
                                              while (request.size () < headersEnd + 4 + length and readMore ()) {
                                              }
                                              const string answer =
                                                  request.substr (0, request.find (' ')) + "\n" + request.substr (headersEnd + 4, length);
                                              const string response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: " +
                                                                      std::to_string (answer.size ()) + "\r\nConnection: close\r\n\r\n" + answer;
                                              s.Write (as_bytes (span{response}));
                                              s.Close ();
                                          }
                                      },
                                      Thread::eAutoStart)};
        auto               check = [&] (Connection::Ptr c) {
            using namespace Memory::Literals;
            c.SetSchemeAndAuthority (site);
            Request notify;
            notify.fMethod               = "NOTIFY"sv;
            notify.fAuthorityRelativeURL = URI{"/events"sv};
            notify.fOverrideHeaders      = Containers::Mapping<String, String>{{"Content-Type"sv, "text/xml"sv}};
            notify.fData                 = "<e:propertyset/>"_blob;
            EXPECT_EQ (c.Send (notify).GetData ().As<string> (), "NOTIFY\n<e:propertyset/>");
            EXPECT_EQ (c.GET (URI{"/after"sv}).GetData ().As<string> (), "GET\n"); // the same connection's next request: a GET
        };
#if qStroika_HasComponent_libcurl
        check (LibCurl::Connection::New (kDefaultTestOptions_));
#endif
#if qStroika_HasComponent_WinHTTP
        check (WinHTTP::Connection::New (kDefaultTestOptions_));
#endif
    }
}

namespace {
    /*
     *  A username and password sent proactively go as HTTP Basic authentication (RFC 7617): "Basic ", then the base64 of
     *  username:password, on one line - the Authorization header's value, which each Connection sends as is. Before Stroika
     *  v3.0d25 it was the base64 alone, with a CRLF every 76 characters, so long credentials broke the header too.
     */
    GTEST_TEST (Foundation_IO_Network_Transfer, BasicAuthentication_)
    {
        Debug::TraceContextBumper ctx{"BasicAuthentication_"};
        using Authentication        = Connection::Options::Authentication;
        constexpr auto kProactively = Authentication::Options::eProactivelySendAuthentication;
        EXPECT_EQ ((Authentication{"Aladdin"sv, "open sesame"sv, kProactively}.GetAuthToken ()), "Basic QWxhZGRpbjpvcGVuIHNlc2FtZQ=="sv); // RFC 7617's example
        // an API token as the password, say: base64 longer than 76 characters
        const String password = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789abcdefghij"sv;
        const String token    = Authentication{"someone"sv, password, kProactively}.GetAuthToken ();
        EXPECT_FALSE (token.Contains ("\r"sv) or token.Contains ("\n"sv)) << token.AsNarrowSDKString ();
        ASSERT_TRUE (token.StartsWith ("Basic "sv)) << token.AsNarrowSDKString ();
        EXPECT_EQ (Cryptography::Encoding::Algorithm::Base64::Decode (token.SubString (6)).As<string> (), "someone:" + password.AsUTF8<string> ());
    }
}

#endif

int main (int argc, const char* argv[])
{
    Test::Setup (argc, argv);
#if qStroika_Platform_POSIX
    SignalHandlerRegistry::sThe.SetSignalHandlers (SIGPIPE, SignalHandlerRegistry::kIGNORED);
#endif
#if qStroika_HasComponent_googletest
    return RUN_ALL_TESTS ();
#else
    cerr << "[  SKIPPED ] every test - Stroika regression tests require building with google test feature" << endl;
#endif
}
