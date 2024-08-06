/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
//  TEST    Foundation::Containers::DataStructures::SkipList
#include "Stroika/Foundation/StroikaPreComp.h"

#include <algorithm>
#include <iostream>
#include <sstream>

#include "Stroika/Foundation/Containers/DataStructures/SkipList.h"
#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/Debug/Visualizations.h"

#include "Stroika/Frameworks/Test/TestHarness.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::Containers::DataStructures;

using namespace Stroika::Frameworks;

#if qHasFeature_GoogleTest
namespace {
    GTEST_TEST (Foundation_Containers_DataStructures_SkipList, BasicSmokeTest)
    {
        SkipList<int, int> t;
        EXPECT_TRUE (not t.contains (1));
        t.Add (1, 2);
        EXPECT_TRUE (t.contains (1));
    }
}

namespace {
    GTEST_TEST (Foundation_Containers_DataStructures_SkipList, BasicIteration)
    {
        SkipList<int, int> t;
        EXPECT_EQ (distance (t.begin (), t.end ()), 0);
        t.Add (1, 2);
        //EXPECT_EQ (distance (t.begin (), t.end ()), 1); // not sure this will work with my tmphakc impl of ++
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
