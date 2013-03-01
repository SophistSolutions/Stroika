/*
 * Copyright(c) Sophist Solutions Inc. 1990-2013.  All rights reserved
 */
//  TEST    Foundation::Math
#include    "Stroika/Foundation/StroikaPreComp.h"

#include    "Stroika/Foundation/Debug/Assertions.h"
#include    "Stroika/Foundation/Debug/Trace.h"

#include    "Stroika/Foundation/Math/Overlap.h"

#include    "../TestHarness/SimpleClass.h"
#include    "../TestHarness/TestHarness.h"




using   namespace   Stroika;
using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Math;


namespace {
    // test helper to assure answer for (A,B) is same as (B,A) - commutative
    template    <typename T>
    bool    VerifyOverlapIsCommutative_ (const pair<T, T>& p1, const pair<T, T>& p2)
    {
        bool r  = Overlaps<T> (p1, p2);
        VerifyTestResult (r == Overlaps<T> (p2, p1));
        return r;
    }
}


namespace   {
    void    Test1_Overlap_ ()
    {
        VerifyTestResult (VerifyOverlapIsCommutative_<int> (pair<int, int> (1, 3), pair<int, int> (2, 2)));
        VerifyTestResult (not VerifyOverlapIsCommutative_<int> (pair<int, int> (1, 3), pair<int, int> (3, 4)));
        VerifyTestResult (not VerifyOverlapIsCommutative_<int> (pair<int, int> (1, 3), pair<int, int> (0, 1)));
        VerifyTestResult (not VerifyOverlapIsCommutative_<int> (pair<int, int> (1, 3), pair<int, int> (1, 1)));
        VerifyTestResult (VerifyOverlapIsCommutative_<int> (pair<int, int> (1, 10), pair<int, int> (3, 4)));
        VerifyTestResult (VerifyOverlapIsCommutative_<int> (pair<int, int> (1, 10), pair<int, int> (3, 3)));
    }
}


namespace   {
    void    DoRegressionTests_ ()
    {
        Test1_Overlap_ ();
    }
}



int main (int argc, const char* argv[])
{
    Stroika::TestHarness::Setup ();
    Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
    return EXIT_SUCCESS;
}



