/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
//  TEST    Foundation::IO::Network::Transfer
#include    "Stroika/Foundation/StroikaPreComp.h"

#include    <iostream>

#include    "Stroika/Foundation/IO/Network/Transfer/Client.h"
#if     qHasFeature_libcurl
#include    "Stroika/Foundation/IO/Network/Transfer/Client_libcurl.h"
#endif
#if     qHasFeature_WinHTTP
#include    "Stroika/Foundation/IO/Network/Transfer/Client_WinHTTP.h"
#endif

#include    "../TestHarness/TestHarness.h"




using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::IO;
using   namespace   Stroika::Foundation::IO::Network;
using   namespace   Stroika::Foundation::IO::Network::Transfer;





namespace {
    void    TestURLParsing_ ()
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



namespace   {
    void    Test_1_SimpleFetch_Google_C_ (Connection c)
    {
        c.SetURL (URL (L"http://www.google.com"));
        Response    r   =   c.Get ();
        VerifyTestResult (r.GetSucceeded ());
        VerifyTestResult (r.fData.size () > 1);
    }
    void    Test_2_SimpleFetch_SSL_Google_C_ (Connection c)
    {
        c.SetURL (URL (L"https://www.google.com"));
        Response    r   =   c.Get ();
        VerifyTestResult (r.GetSucceeded ());
        VerifyTestResult (r.fData.size () > 1);
    }
}




//// CREATE TEMPLATE THAT ITERATES OVER ALL USEFUL CONNECTION TYPES (default one, and special chosen depending on available defines - so eventually
// on windows, we test BTOH win32 and curl (if avaialble).

namespace   {
    void    DoRegressionTests_ForConnectionFactory_ (Connection (*factory) ())
    {
        Test_1_SimpleFetch_Google_C_ ((factory) ());
        Test_2_SimpleFetch_SSL_Google_C_ ((factory) ());
    }

#if     !qCompilerAndStdLib_lamba_closureCvtToFunctionPtrSupported
#if     qHasFeature_WinHTTP
    Connection  mk_WinHTTP_ ()
    {
        return Connection_WinHTTP ();
    }
#endif
#if     qHasFeature_libcurl
    Connection  mk_LIBCURL_ ()
    {
        return Connection_LibCurl ();
    }
#endif
#endif
    void    DoRegressionTests_ ()
    {
        TestURLParsing_ ();

        DoRegressionTests_ForConnectionFactory_ (&CreateConnection);

#if     qHasFeature_libcurl
#if     qCompilerAndStdLib_lamba_closureCvtToFunctionPtrSupported
        DoRegressionTests_ForConnectionFactory_ ([]() -> Connection { return Connection_LibCurl (); });
#else
        DoRegressionTests_ForConnectionFactory_ (&mk_LIBCURL_);
#endif
#endif
#if     qHasFeature_WinHTTP
#if     qCompilerAndStdLib_lamba_closureCvtToFunctionPtrSupported
        DoRegressionTests_ForConnectionFactory_ ([]() -> Connection { return Connection_WinHTTP (); });
#else
        DoRegressionTests_ForConnectionFactory_ (&mk_WinHTTP_);
#endif
#endif
    }
}





int main (int argc, const char* argv[])
{
    Stroika::TestHarness::Setup ();
    Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
    return EXIT_SUCCESS;
}
