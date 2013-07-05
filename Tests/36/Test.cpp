/*
 * Copyright(c) Records For Living, Inc. 2004-2012.  All rights reserved
 */
//  TEST    Foundation::Traveral
#include    "Stroika/Foundation/StroikaPreComp.h"

#include    "Stroika/Foundation/Debug/Assertions.h"
#include    "Stroika/Foundation/Configuration/Enumeration.h"
#include    "Stroika/Foundation/Traversal/DiscreteRange.h"
#include    "Stroika/Foundation/Traversal/Range.h"

#include    "../TestHarness/TestHarness.h"




using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Traversal;


namespace   {

    void    Test_1_BasicRange_ ()
    {
        {
            Range<int> r (3, 5);
            VerifyTestResult (not r.empty ());
            VerifyTestResult (r.Contains (3));
        }
        {
            Range<int> r1 (3, 5);
            Range<int> r2 (5, 6);
            VerifyTestResult (not r1.Overlaps (r2));
            VerifyTestResult (not r2.Overlaps (r1));
            Range<int> r3  = r1;
            VerifyTestResult (r1.Overlaps (r3));
            VerifyTestResult (r3.Overlaps (r1));
        }
    }

    void    Test_2_BasicDiscreteRangeIteration_ ()
    {
#if 0
        {
            DiscreteRange<int> r (3, 5);
            VerifyTestResult (not r.empty ());
            VerifyTestResult (r.Contains (3));
        } {
            int nItemsHit = 0;
            int lastItemHit = 0;
            for (auto i : DiscreteRange<int> (3, 5)) {
                nItemsHit++;
                VerifyTestResult (lastItemHit < i);
                lastItemHit = i;
            }
            VerifyTestResult (lastItemHit == 5);    /// IN DISCUSSION - OPEN ENDED RHS?
        }
#endif

    }

}


namespace {
    void    Test_3_SimpleDiscreteRangeWithEnumsTest_ ()
    {
        // NYI - but do stuff like:

        enum class Color {
            red, blue, green,

            Define_Start_End_Count (red, green)
        };

#if 0
        {
            int nItemsHit = 0;
            Color lastItemHit = Color::red;
            for (auto i : DiscreteRange<Color> (Optional<Color> (), Optional<Color> ())) {
                nItemsHit++;
                VerifyTestResult (lastItemHit < i);
                lastItemHit = i;
            }
            nItemsHit (lastItemHit == 3);
            VerifyTestResult (lastItemHit == Color::green);
        }
        {
            int nItemsHit = 0;
            Color lastItemHit = Color::red;
            DiscreteRange<Color> (Optional<Color> (), Optional<Color> ()).Apply ([&nItemsHit, &lastItemHit] (Color c) {
                nItemsHit++;
                VerifyTestResult (lastItemHit < i);
                lastItemHit = i;
            });
            nItemsHit (lastItemHit == 3);
            VerifyTestResult (lastItemHit == Color::green);
        }
#endif
    }
}



namespace   {

    void    DoRegressionTests_ ()
    {
        Test_1_BasicRange_ ();
        Test_2_BasicDiscreteRangeIteration_ ();
        Test_3_SimpleDiscreteRangeWithEnumsTest_ ();
    }
}




int     main (int argc, const char* argv[])
{
    Stroika::TestHarness::Setup ();
    Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
    return EXIT_SUCCESS;
}

