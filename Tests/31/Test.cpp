/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
//  TEST    Foundation::Execution::Exceptions
#include    "Stroika/Foundation/StroikaPreComp.h"

#include    <iostream>
#include    <sstream>

#if     qPlatform_Windows
#include    <Windows.h>
#include    <winerror.h>
#include    <wininet.h>     // for error codes
#endif

#include    "Stroika/Foundation/Execution/Exceptions.h"
#if     qPlatform_Windows
#include    "Stroika/Foundation/Execution/Platform/Windows/Exception.h"
#endif

#include    "../TestHarness/TestHarness.h"



using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Execution;






namespace   {
    void    RegressionTest1_ ()
    {
#if         qPlatform_Windows
        VerifyTestResult (Platform::Windows::Exception::kERROR_INTERNET_TIMEOUT == ERROR_INTERNET_TIMEOUT);
        VerifyTestResult (Platform::Windows::Exception::kERROR_INTERNET_INVALID_URL == ERROR_INTERNET_INVALID_URL);
        VerifyTestResult (Platform::Windows::Exception::kERROR_INTERNET_UNRECOGNIZED_SCHEME == ERROR_INTERNET_UNRECOGNIZED_SCHEME);
        VerifyTestResult (Platform::Windows::Exception::kERROR_INTERNET_NAME_NOT_RESOLVED == ERROR_INTERNET_NAME_NOT_RESOLVED);
        VerifyTestResult (Platform::Windows::Exception::kERROR_INTERNET_PROTOCOL_NOT_FOUND == ERROR_INTERNET_PROTOCOL_NOT_FOUND);
        VerifyTestResult (Platform::Windows::Exception::kERROR_INTERNET_CANNOT_CONNECT == ERROR_INTERNET_CANNOT_CONNECT);
#endif
    }
}



namespace   {

    void    DoRegressionTests_ ()
    {
        RegressionTest1_ ();
    }

}





int     main (int argc, const char* argv[])
{
    Stroika::TestHarness::Setup ();
    Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
    return EXIT_SUCCESS;
}

