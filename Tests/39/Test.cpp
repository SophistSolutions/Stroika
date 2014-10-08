/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
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
    namespace Test1_URL_Parsing_ {
        void    DoTests_ ()
        {
            {
                URL url (L"http:/StyleSheet.css?ThemeName=Cupertino");
                VerifyTestResult (url.GetEffectivePortNumber () == 80);
                VerifyTestResult (url.GetQueryString () == L"ThemeName=Cupertino");
                VerifyTestResult (url.GetHost ().empty ());
                VerifyTestResult (url.GetHostRelativePath () == L"StyleSheet.css");
                VerifyTestResult (url.GetFragment ().empty ());
                VerifyTestResult (url.GetProtocol () == L"http");
            }
            {
                URL url (L"http://www.recordsforliving.com/");
                VerifyTestResult (url.GetEffectivePortNumber () == 80);
                VerifyTestResult (url.GetQueryString ().empty ());
                VerifyTestResult (url.GetFragment ().empty ());
                VerifyTestResult (url.GetHostRelativePath ().empty ());
                VerifyTestResult (url.GetHost () == L"www.recordsforliving.com");
                VerifyTestResult (url.GetProtocol () == L"http");
                VerifyTestResult (not url.IsSecure ());
            }
            {
                URL url (L"https://xxx.recordsforliving.com/");
                VerifyTestResult (url.GetEffectivePortNumber () == 443);
                VerifyTestResult (url.GetQueryString ().empty ());
                VerifyTestResult (url.GetFragment ().empty ());
                VerifyTestResult (url.GetHostRelativePath ().empty ());
                VerifyTestResult (url.GetHost () == L"xxx.recordsforliving.com");
                VerifyTestResult (url.GetProtocol () == L"https");
                VerifyTestResult (url.IsSecure ());
            }
        }
    }
}




namespace {
    namespace Test2_SimpleConnnectionTests_ {
        namespace Private_ {
            void    Test_1_SimpleFetch_Google_C_ (Connection c)
            {
                c.SetURL (URL (L"http://www.google.com"));
                Response    r   =   c.GET ();
                VerifyTestResult (r.GetSucceeded ());
                VerifyTestResult (r.GetData ().size () > 1);
            }
            void    Test_2_SimpleFetch_SSL_Google_C_ (Connection c)
            {
                c.SetURL (URL (L"https://www.google.com"));
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
    namespace Test_3_SimpleFetch_httpbin_ {
        namespace Private_ {
            void    T1_httpbin_SimpleGET_ (Connection c)
            {
                c.SetURL (URL (L"http://httpbin.org/get"));
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
                using   Memory::BLOB;

                static   mt19937 sRNG_;

                c.SetURL (URL (L"http://httpbin.org/post"));
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
                    for (auto i : vv) {
#if 0
                        DbgTrace (L"i.frist=%s", i.fKey.c_str ());
                        DbgTrace (L"i.second=%s", i.fValue.As<String> ().c_str ());
#endif
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
                using   Memory::BLOB;

                static   mt19937 sRNG_;

                c.SetURL (URL (L"http://httpbin.org/put"));
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
                    for (auto i : vv) {
#if 0
                        DbgTrace (L"i.frist=%s", i.fKey.c_str ());
                        DbgTrace (L"i.second=%s", i.fValue.As<String> ().c_str ());
#endif
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





namespace   {
    void    DoRegressionTests_ ()
    {
        Test1_URL_Parsing_::DoTests_ ();
        Test2_SimpleConnnectionTests_::DoTests_ ();
        Test_3_SimpleFetch_httpbin_::DoTests_ ();
    }
}





int main (int argc, const char* argv[])
{
    Stroika::TestHarness::Setup ();
    Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
    return EXIT_SUCCESS;
}
