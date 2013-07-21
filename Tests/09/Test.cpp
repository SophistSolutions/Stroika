/*
 * Copyright(c) Sophist Solutions Inc. 1990-2013.  All rights reserved
 */
//      STATUS  TOTALLY WRONG - DISCARD
#include    "Stroika/Foundation/StroikaPreComp.h"

#include    <iostream>
#include    <sstream>
#include    "Stroika/Foundation/Debug/Assertions.h"
#include    "Stroika/Foundation/Debug/Trace.h"


#include    "../TestHarness/SimpleClass.h"
#include    "../TestHarness/TestHarness.h"



using   namespace   Stroika;
using   namespace   Stroika::Foundation;



namespace   {
    void    DoRegressionTests_ ()
    {
    }
}


int     main (int argc, const char* argv[])
{
    Stroika::TestHarness::Setup ();
    Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
    return EXIT_SUCCESS;
}

