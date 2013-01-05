/*
 * Copyright(c) Sophist Solutions Inc. 1990-2013.  All rights reserved
 */
#include    "Stroika/Foundation/StroikaPreComp.h"

#include    "Stroika/Foundation/Debug/Assertions.h"
#include    "Stroika/Foundation/Debug/Trace.h"

#include    "Stroika/Foundation/Memory/Optional.h"
#include    "Stroika/Foundation/Memory/SharedByValue.h"
#include    "Stroika/Foundation/Memory/VariantValue.h"

#include    "../TestHarness/SimpleClass.h"
#include    "../TestHarness/TestHarness.h"





using   namespace   Stroika;
using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Memory;


//TODO: DOES IT EVEN NEED TO BE SAID? THese tests are a bit sparse ;-)

namespace   {
    void    Test1_Optional ()
    {
        {
            Optional<int>   x;
            VerifyTestResult (x.empty ());
            x = 1;
            VerifyTestResult (not x.empty ());
            VerifyTestResult (*x == 1);
        }
    }
    void    Test1_SharedByValue ()
    {
    }
    void    Test1_VariantValue ()
    {
    }
}


namespace   {

    void    DoRegressionTests_ ()
    {
        Test1_Optional ();
        Test1_SharedByValue ();
        Test1_VariantValue ();
    }
}



int main (int argc, const char* argv[])
{
    Stroika::TestHarness::Setup ();
    Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
    return EXIT_SUCCESS;
}



