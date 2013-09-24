/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
//  TEST    Foundation::Traveral
#include    "Stroika/Foundation/StroikaPreComp.h"

#include    "Stroika/Foundation/Debug/Assertions.h"
#include    "Stroika/Foundation/Containers/Sequence.h"
#include    "Stroika/Foundation/Containers/Mapping.h"
#include    "Stroika/Foundation/Configuration/Enumeration.h"
#include    "Stroika/Foundation/Traversal/DiscreteRange.h"
#include    "Stroika/Foundation/Traversal/MapReduce.h"
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
        {
            typedef DefaultIntegerRangeTraits < int, int, int, -3, 100 > RT;
            Range<int, RT> x;
            VerifyTestResult (x.begin () == -3);
            VerifyTestResult (x.end () == 100);
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




namespace {
    void    Test4_MapTest_ ()
    {
        using   namespace   Containers;
        {
            Sequence<int>   n;
            n.Append (1);
            n.Append (2);
            n.Append (3);
            Sequence<int>   n1 = Traversal::Map (n, [] (int i) -> int { return i + 1;});
            VerifyTestResult (n1.size () == 3);
            VerifyTestResult (n1[0] == 2);
            VerifyTestResult (n1[1] == 3);
            VerifyTestResult (n1[2] == 4);
        }
    }
}







namespace {
    void    Test5_ReduceTest_ ()
    {
        using   namespace   Containers;
        {
            Sequence<int>   n;
            n.Append (1);
            n.Append (2);
            n.Append (3);
            int sum = Traversal::Reduce (n, [] (int l, int r) -> int { return l + r; }, 0);
            VerifyTestResult (sum == 6);
        }
    }
}



namespace   {

    void    DoRegressionTests_ ()
    {
        Test_1_BasicRange_ ();
        Test_2_BasicDiscreteRangeIteration_ ();
        Test_3_SimpleDiscreteRangeWithEnumsTest_ ();
        Test4_MapTest_ ();
        Test5_ReduceTest_ ();
    }
}




int     main (int argc, const char* argv[])
{
    Stroika::TestHarness::Setup ();
    Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
    return EXIT_SUCCESS;
}

