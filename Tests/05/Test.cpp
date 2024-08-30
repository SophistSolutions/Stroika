/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
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

using Test::ArchtypeClasses::OnlyCopyableMoveableAndTotallyOrdered;

using DataStructures::Array;

#if qHasFeature_GoogleTest
namespace {
    static void Test1 ()
    {
        Array<size_t> someArray;

        const size_t kBigSize = 1001;

        EXPECT_TRUE (someArray.size () == 0);

        someArray.SetLength (kBigSize, 0);
        someArray.RemoveAll ();
        someArray.SetLength (kBigSize, 0);
        someArray.SetLength (10, 0);
        someArray.SetLength (kBigSize, 0);

        EXPECT_TRUE (someArray.size () == kBigSize);
        someArray[55] = 55;
        EXPECT_TRUE (someArray[55] == 55);
        EXPECT_TRUE (someArray[55] != 56);

        someArray.InsertAt (100, 1);
        EXPECT_TRUE (someArray.size () == kBigSize + 1);
        EXPECT_TRUE (someArray[100] == 1);

        someArray[101] = someArray[100] + 10;
        EXPECT_TRUE (someArray[101] == 11);
        someArray.RemoveAt (0);
        EXPECT_TRUE (someArray[100] == 11);
        someArray.RemoveAt (1);

        EXPECT_TRUE (someArray[99] == 11);
    }

    static void Test2 ()
    {
        {
            Array<OnlyCopyableMoveableAndTotallyOrdered> someArray;
            someArray.InsertAt (0, 100);
            // for (size_t i = 0; i < someArray.size (); ++i) { cerr << "someArray[" << i << "] = " << someArray[i].GetValue () << endl; }
            someArray.RemoveAt (0);
            someArray.InsertAt (0, 2);
            someArray.InsertAt (0, 1);
            someArray.InsertAt (0, 3);
            someArray.InsertAt (someArray.size (), 4);
            someArray.RemoveAt (someArray.size () - 1);
            someArray.RemoveAt (1);
        }

        Array<OnlyCopyableMoveableAndTotallyOrdered> someArray;

        const size_t kBigSize = 1001;

        EXPECT_TRUE (someArray.size () == 0);
        someArray.SetLength (kBigSize, 0);
        someArray.SetLength (0, 0);
        someArray.SetLength (kBigSize, 0);
        someArray.SetLength (10, 0);
        someArray.SetLength (kBigSize, 0);

        EXPECT_TRUE (someArray.size () == kBigSize);
        someArray[55] = 55;
        EXPECT_TRUE (someArray[55] == 55);
        EXPECT_TRUE (not(someArray[55] == 56));

        someArray.RemoveAt (100);

        while (someArray.size () > 0) {
            someArray.RemoveAt (0);
        }
        while (someArray.size () < kBigSize) {
            someArray.InsertAt (someArray.size (), 1);
        }

        someArray.InsertAt (100, 1);
        EXPECT_TRUE (someArray.size () == kBigSize + 1);
        EXPECT_TRUE (someArray[100] == 1);
        someArray[101] = 1 + static_cast<size_t> (someArray[100]);
        someArray.RemoveAt (1);
        EXPECT_EQ (static_cast<size_t> (someArray[100]), 2u);
    }
}

namespace {
    GTEST_TEST (Foundation_Caching, all)
    {
        Test1 ();
        Test2 ();
    }
}
#endif

int main (int argc, const char* argv[])
{
    Test::Setup (argc, argv);
#if qHasFeature_GoogleTest
    return RUN_ALL_TESTS ();
#else
    cerr << "Stroika regression tests require building with google test feature [  PASSED  ]" << endl;
#endif
}
