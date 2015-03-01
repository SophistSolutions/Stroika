/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
//  TEST    Foundation::IO::Network::Transfer
#include    "Stroika/Foundation/StroikaPreComp.h"

#include    <iostream>
#include    <random>

#include    "Stroika/Foundation/Cryptography/Encoding/Algorithm/Base64.h"
#include    "Stroika/Foundation/DataExchange/JSON/Reader.h"
#include    "Stroika/Foundation/Execution/RequiredComponentMissingException.h"
#include    "Stroika/Foundation/IO/Network/Transfer/Client.h"
#if     qHasFeature_libcurl
#include    "Stroika/Foundation/IO/Network/Transfer/Client_libcurl.h"
#endif
#if     qHasFeature_WinHTTP
#include    "Stroika/Foundation/IO/Network/Transfer/Client_WinHTTP.h"
#endif

#include    "../TestHarness/TestHarness.h"




using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::DataExchange;
using   namespace   Stroika::Foundation::IO;
using   namespace   Stroika::Foundation::IO::Network;
using   namespace   Stroika::Foundation::IO::Network::Transfer;









namespace {
    namespace Test_1_SimpleConnnectionTests_ {
        namespace Private_ {
            void    Test_1_SimpleFetch_Google_C_ (Connection c)
            {
                c.SetURL (URL::Parse (L"http://www.google.com"));
                Response    r   =   c.GET ();
                VerifyTestResult (r.GetSucceeded ());
                VerifyTestResult (r.GetData ().size () > 1);
            }
            void    Test_2_SimpleFetch_SSL_Google_C_ (Connection c)
            {
                c.SetURL (URL::Parse (L"https://www.google.com"));
                Response    r   =   c.GET ();
                VerifyTestResult (r.GetSucceeded ());
                VerifyTestResult (r.GetData ().size () > 1);
            }
            void    DoRegressionTests_ForConnectionFactory_ (Connection (*factory) ())
            {
                Test_1_SimpleFetch_Google_C_ (factory ());
                Test_2_SimpleFetch_SSL_Google_C_ (factory ());
            }
        }
        void    DoTests_ ()
        {
            using namespace Private_;
            try {
                DoRegressionTests_ForConnectionFactory_ ([]() -> Connection { return CreateConnection (); });
            }
            catch (const Execution::RequiredComponentMissingException&) {
#if     !qHasFeature_libcurl && !qHasFeature_WinHTTP
                // OK to ignore. We don't wnat to call this failing a test, because there is nothing to fix.
                // This is more like the absence of a feature beacuse of the missing component.
#else
                Execution::DoReThrow ();
#endif
            }

#if     qHasFeature_libcurl
            DoRegressionTests_ForConnectionFactory_ ([]() -> Connection { return Connection_LibCurl (); });
#endif
#if     qHasFeature_WinHTTP
            DoRegressionTests_ForConnectionFactory_ ([]() -> Connection { return Connection_WinHTTP (); });
#endif
        }
    }
}









namespace {
    namespace Test_2_SimpleFetch_httpbin_ {
        namespace Private_ {
            void    T1_httpbin_SimpleGET_ (Connection c)
            {
                Debug::TraceContextBumper ctx ("T1_httpbin_SimpleGET_");
                c.SetURL (URL::Parse (L"http://httpbin.org/get"));
                Response    r   =   c.GET ();
                VerifyTestResult (r.GetSucceeded ());
                VerifyTestResult (r.GetData ().size () > 1);
                {
                    VariantValue v = JSON::Reader ().Read (r.GetDataBinaryInputStream ());
                    Mapping<String, VariantValue> vv = v.As<Mapping<String, VariantValue>> ();
                    VerifyTestResult (vv.ContainsKey (L"args"));
                    VerifyTestResult (vv[L"url"] == L"http://httpbin.org/get");
                }
            }
            void    T2_httpbin_SimplePOST_ (Connection c)
            {
                Debug::TraceContextBumper ctx ("T2_httpbin_SimplePOST_");
                using   Memory::BLOB;

                static   mt19937 sRNG_;

                c.SetURL (URL::Parse (L"http://httpbin.org/post"));
                BLOB    roundTripTestData = [] () {
                    Memory::SmallStackBuffer<Byte> buf (1024);
                    for (size_t i = 0; i < buf.GetSize (); ++i) {
                        buf[i] = static_cast<Byte> (uniform_int_distribution<unsigned short>()(sRNG_));
                    }
                    return BLOB (buf.begin (), buf.end ());
                } ();
                Response    r   =   c.POST (roundTripTestData, DataExchange::PredefinedInternetMediaType::OctetStream_CT ());
                VerifyTestResult (r.GetSucceeded ());
                {
                    VariantValue v = JSON::Reader ().Read (r.GetDataBinaryInputStream ());
                    Mapping<String, VariantValue> vv = v.As<Mapping<String, VariantValue>> ();
                    DbgTrace ("POST parsed response:");
                    for (auto i : vv) {
                        DbgTrace (L"%s : %s", i.fKey.c_str (), i.fValue.As<String> ().c_str ());
                    }
                    String  dataValueString = vv.Lookup (L"data").Value ().As<String> ();
                    {
                        size_t i = dataValueString.Find (',');
                        if (i != -1) {
                            dataValueString = dataValueString.SubString (i + 1);
                        }
                    }
                    BLOB    resultBLOB = Cryptography::Encoding::Algorithm::DecodeBase64 (dataValueString.AsUTF8 ());
                    VerifyTestResult (resultBLOB == roundTripTestData);
                }
            }
            void    T3_httpbin_SimplePUT_ (Connection c)
            {
                Debug::TraceContextBumper ctx ("T3_httpbin_SimplePUT_");
                using   Memory::BLOB;

                static   mt19937 sRNG_;

                c.SetURL (URL::Parse (L"http://httpbin.org/put"));
                BLOB    roundTripTestData = [] () {
                    Memory::SmallStackBuffer<Byte> buf (1024);
                    for (size_t i = 0; i < buf.GetSize (); ++i) {
                        buf[i] = static_cast<Byte> (uniform_int_distribution<unsigned short>()(sRNG_));
                    }
                    return BLOB (buf.begin (), buf.end ());
                } ();
                Response    r   =   c.PUT (roundTripTestData, DataExchange::PredefinedInternetMediaType::OctetStream_CT ());
                VerifyTestResult (r.GetSucceeded ());
                {
                    VariantValue v = JSON::Reader ().Read (r.GetDataBinaryInputStream ());
                    Mapping<String, VariantValue> vv = v.As<Mapping<String, VariantValue>> ();
                    DbgTrace ("PUT parsed response:");
                    for (auto i : vv) {
                        DbgTrace (L"%s : %s", i.fKey.c_str (), i.fValue.As<String> ().c_str ());
                    }
                    String  dataValueString = vv.Lookup (L"data").Value ().As<String> ();
                    {
                        size_t i = dataValueString.Find (',');
                        if (i != -1) {
                            dataValueString = dataValueString.SubString (i + 1);
                        }
                    }
                    BLOB    resultBLOB = Cryptography::Encoding::Algorithm::DecodeBase64 (dataValueString.AsUTF8 ());
                    VerifyTestResult (resultBLOB == roundTripTestData);
                }
            }
            void    DoRegressionTests_ForConnectionFactory_ (Connection (*factory) ())
            {
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
        }
        void    DoTests_ ()
        {
            using namespace Private_;
            try {
                DoRegressionTests_ForConnectionFactory_ ([]() -> Connection { return CreateConnection (); });
            }
            catch (const Execution::RequiredComponentMissingException&) {
#if     !qHasFeature_libcurl && !qHasFeature_WinHTTP
                // OK to ignore. We don't wnat to call this failing a test, because there is nothing to fix.
                // This is more like the absence of a feature beacuse of the missing component.
#else
                Execution::DoReThrow ();
#endif
            }

#if     qHasFeature_libcurl
            DoRegressionTests_ForConnectionFactory_ ([]() -> Connection { return Connection_LibCurl (); });
#endif
#if     qHasFeature_WinHTTP
            DoRegressionTests_ForConnectionFactory_ ([]() -> Connection { return Connection_WinHTTP (); });
#endif
        }
    }
}






namespace {
    namespace Test3_TextStreamResponse_ {
        namespace Private_ {
            void    Test_1_SimpleFetch_Google_C_ (Connection c)
            {
                c.SetURL (URL::Parse (L"http://www.google.com"));
                Response    r   =   c.GET ();
                VerifyTestResult (r.GetSucceeded ());
                for (auto i : r.GetHeaders ()) {
                    DbgTrace (L"%s=%s", i.fKey.c_str (), i.fValue.c_str ());
                }
                InternetMediaType contentType = r.GetContentType ();
                String responseText = r.GetDataTextInputStream ().ReadAll ();
                DbgTrace (L"responseText = %s", responseText.c_str ());
                VerifyTestResult (responseText.Contains (L"google"));
            }
            void    DoRegressionTests_ForConnectionFactory_ (Connection (*factory) ())
            {
                Test_1_SimpleFetch_Google_C_ (factory ());
            }
        }
        void    DoTests_ ()
        {
            using namespace Private_;
            try {
                DoRegressionTests_ForConnectionFactory_ ([]() -> Connection { return CreateConnection (); });
            }
            catch (const Execution::RequiredComponentMissingException&) {
#if     !qHasFeature_libcurl && !qHasFeature_WinHTTP
                // OK to ignore. We don't wnat to call this failing a test, because there is nothing to fix.
                // This is more like the absence of a feature beacuse of the missing component.
#else
                Execution::DoReThrow ();
#endif
            }

#if     qHasFeature_libcurl
            DoRegressionTests_ForConnectionFactory_ ([]() -> Connection { return Connection_LibCurl (); });
#endif
#if     qHasFeature_WinHTTP
            DoRegressionTests_ForConnectionFactory_ ([]() -> Connection { return Connection_WinHTTP (); });
#endif
        }
    }
}








namespace   {
    void    DoRegressionTests_ ()
    {
        Test_1_SimpleConnnectionTests_::DoTests_ ();
        Test_2_SimpleFetch_httpbin_::DoTests_ ();
        Test3_TextStreamResponse_::DoTests_ ();
    }
}





int main (int argc, const char* argv[])
{
    Stroika::TestHarness::Setup ();
    Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
    return EXIT_SUCCESS;
}
