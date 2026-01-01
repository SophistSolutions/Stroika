/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
//  TEST    Foundation::Containers::DataStructures::Array
#include "Stroika/Foundation/StroikaPreComp.h"

#include <iostream>

#include "Stroika/Foundation/Containers/DataStructures/Array.h"
#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/Debug/Visualizations.h"

#include "Stroika/Frameworks/Test/ArchtypeClasses.h"
#include "Stroika/Frameworks/Test/TestHarness.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Containers;

using namespace Stroika::Frameworks;

using Test::ArchtypeClasses::OnlyCopyableMoveable;
using Test::ArchtypeClasses::OnlyCopyableMoveableAndTotallyOrdered;

using DataStructures::Array;

#if qStroika_HasComponent_googletest
namespace {
    GTEST_TEST (Foundation_Containers_DataStructures_Array, VerySimpleTestsOnArrayOfSize_t)
    {
        Array<size_t> someArray;

        const size_t kBigSize = 1001;

        EXPECT_EQ (someArray.size (), 0u);

        someArray.SetLength (kBigSize, 0);
        someArray.clear ();
        someArray.SetLength (kBigSize, 0);
        someArray.SetLength (10, 0);
        someArray.SetLength (kBigSize, 0);

        EXPECT_EQ (someArray.size (), kBigSize);
        someArray[55] = 55;
        EXPECT_EQ (someArray[55], 55u);
        EXPECT_NE (someArray[55], 56u);

        someArray.Insert (100, 1);
        EXPECT_EQ (someArray.size (), kBigSize + 1u);
        EXPECT_EQ (someArray[100], 1u);

        someArray[101] = someArray[100] + 10;
        EXPECT_EQ (someArray[101], 11u);
        someArray.Remove (0u);
        EXPECT_EQ (someArray[100], 11u);
        someArray.Remove (1u);
        EXPECT_EQ (someArray[99], 11u);
    }
}

namespace {
    GTEST_TEST (Foundation_Containers_DataStructures_Array, VerySimpleTestsOnArrayOfOnlyCopyableMoveableAndTotallyOrdered)
    {
        {
            Array<OnlyCopyableMoveableAndTotallyOrdered> someArray;
            someArray.Insert (0, 100);
            // for (size_t i = 0; i < someArray.size (); ++i) { cerr << "someArray[" << i << "] = " << someArray[i].GetValue () << endl; }
            someArray.Remove (0u);
            someArray.Insert (0, 2);
            someArray.Insert (0, 1);
            someArray.Insert (0, 3);
            someArray.Insert (someArray.size (), 4);
            someArray.Remove (someArray.size () - 1u);
            someArray.Remove (1u);
        }

        Array<OnlyCopyableMoveableAndTotallyOrdered> someArray;

        const size_t kBigSize = 1001;

        EXPECT_EQ (someArray.size (), 0u);
        someArray.SetLength (kBigSize, 0);
        someArray.SetLength (0, 0);
        someArray.SetLength (kBigSize, 0);
        someArray.SetLength (10, 0);
        someArray.SetLength (kBigSize, 0);

        EXPECT_EQ (someArray.size (), kBigSize);
        someArray[55] = 55;
        EXPECT_EQ (someArray[55], 55u);
        EXPECT_TRUE (not(someArray[55] == 56));

        someArray.Remove (100u);

        while (someArray.size () > 0) {
            someArray.Remove (0u);
        }
        while (someArray.size () < kBigSize) {
            someArray.Insert (someArray.size (), 1);
        }

        someArray.Insert (100, 1);
        EXPECT_EQ (someArray.size (), kBigSize + 1u);
        EXPECT_EQ (someArray[100], 1u);
        someArray[101] = 1 + static_cast<size_t> (someArray[100]);
        someArray.Remove (1u);
        EXPECT_EQ (static_cast<size_t> (someArray[100]), 2u);
    }
}

namespace {
    GTEST_TEST (Foundation_Containers_DataStructures_Array, RangedInsertAndRemove)
    {
        Array<OnlyCopyableMoveable> someArray;
        EXPECT_EQ (someArray.size (), 0u);
        someArray.Insert (0, 3);
        EXPECT_EQ (someArray.size (), 1u);
        {
            const OnlyCopyableMoveable kTest_[] = {2, 3, 4}; // not static cuz of 'cleanup' test at end
            someArray.Insert (1, span{kTest_});
        }
        EXPECT_EQ (someArray.size (), 4u);
        someArray.Remove (1, 3);
        EXPECT_EQ (someArray.size (), 2u);
    }
}

namespace {
    GTEST_TEST (Foundation_Containers_DataStructures_Array, RangedInsertAndRemove2)
    {
        Array<OnlyCopyableMoveableAndTotallyOrdered> someArray;
        EXPECT_EQ (someArray.size (), 0u);
        someArray.Insert (0, 3);
        EXPECT_EQ (someArray.size (), 1u);
        EXPECT_EQ (distance (someArray.begin (), someArray.end ()), 1);
        {
            const OnlyCopyableMoveableAndTotallyOrdered kTest_[] = {2, 3, 4}; // not static cuz of 'cleanup' test at end
            someArray.Insert (1, span{kTest_});
        }
        EXPECT_EQ (someArray.size (), 4u);
        EXPECT_EQ (distance (someArray.begin (), someArray.end ()), 4);
        someArray.Remove (1, 4);
        EXPECT_EQ (someArray.size (), 1u);
        EXPECT_EQ (someArray[0], 3u);
        someArray[0] = 0;
        {
            const OnlyCopyableMoveableAndTotallyOrdered kTest_[] = {2, 3, 4}; // not static cuz of 'cleanup' test at end
            someArray.Insert (0, span{kTest_});
            EXPECT_EQ (someArray[0], 2u);
            EXPECT_EQ (someArray[1], 3u);
            EXPECT_EQ (someArray[2], 4u);
            EXPECT_EQ (someArray[3], 0u);
            someArray.Insert (2, span{kTest_});
            EXPECT_EQ (someArray[0], 2u);
            EXPECT_EQ (someArray[1], 3u);
            EXPECT_EQ (someArray[2], 2u);
            EXPECT_EQ (someArray[3], 3u);
            EXPECT_EQ (someArray[4], 4u);
            EXPECT_EQ (someArray[5], 4u);
            EXPECT_EQ (someArray[6], 0u);
        }
    }
}

namespace {
    GTEST_TEST (Foundation_Containers_DataStructures_Array, Cleanup)
    {
        EXPECT_TRUE (OnlyCopyableMoveableAndTotallyOrdered::GetTotalLiveCount () == 0 and OnlyCopyableMoveable::GetTotalLiveCount () == 0); // simple portable leak check
    }
}
#endif

int main (int argc, const char* argv[])
{
    Test::Setup (argc, argv);
#if qStroika_HasComponent_googletest
    return RUN_ALL_TESTS ();
#else
    cerr << "Stroika regression tests require building with google test feature [  PASSED  ]" << endl;
#endif
}
