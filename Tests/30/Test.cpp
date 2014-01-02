/*
 * Copyright(c) Sophist Solutions Inc. 1990-2014.  All rights reserved
 */
//  TEST    Foundation::IO::Network
#include    "Stroika/Foundation/StroikaPreComp.h"

#include    "Stroika/Foundation/Debug/Assertions.h"
#include    "Stroika/Foundation/Debug/Trace.h"
#include    "Stroika/Foundation/Memory/Optional.h"
#include    "Stroika/Foundation/IO/Network/URL.h"

#include    "../TestHarness/SimpleClass.h"
#include    "../TestHarness/TestHarness.h"


using   namespace   Stroika;
using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::IO::Network;


namespace   {
    void    Test1_URL_ ()
    {
        {
            const   wchar_t kTestURL_[] = L"http://www.x.com/foo?bar=3";
            VerifyTestResult (URL (kTestURL_).GetFullURL () == kTestURL_);
            VerifyTestResult (URL (kTestURL_).GetHost () == L"www.x.com");
            VerifyTestResult (URL (kTestURL_).GetHostRelativePath () == L"foo");
            VerifyTestResult (URL (kTestURL_).GetQueryString () == L"bar=3");
            VerifyTestResult (URL (kTestURL_) == URL (L"http", L"www.x.com", L"foo", L"bar=3"));
        }
    }
}


namespace   {
    void    DoRegressionTests_ ()
    {
        Test1_URL_ ();
    }
}



int main (int argc, const char* argv[])
{
    Stroika::TestHarness::Setup ();
    Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
    return EXIT_SUCCESS;
}



