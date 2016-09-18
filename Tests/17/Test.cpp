/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
//  TEST    Foundation::Containers::PriorityQueue
//      STATUS  TOTALLY WRONG - PLACEHOLDER
#include    "Stroika/Foundation/StroikaPreComp.h"

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
    return Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
}
