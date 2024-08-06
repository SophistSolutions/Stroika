/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
//  TEST    Foundation::Containers::DataStructures::SkipList
#include "Stroika/Foundation/StroikaPreComp.h"

#include <algorithm>
#include <iostream>
#include <sstream>

#include "Stroika/Foundation/Characters/Format.h"
#include "Stroika/Foundation/Containers/DataStructures/SkipList.h"
#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/Debug/Visualizations.h"

#include "Stroika/Frameworks/Test/TestHarness.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters::Literals;
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
        EXPECT_EQ (distance (t.begin (), t.end ()), 1); // not sure this will work with my tmphakc impl of ++
    }
}

namespace {
    template <typename KEY_TYPE, typename MAPPED_TYPE, typename TRAITS>
    void BasicAddRemoveTestsHelper_ (SkipList<KEY_TYPE, MAPPED_TYPE, TRAITS> prototypeList, int tabs, size_t testLength)
    {
        SkipList<KEY_TYPE, MAPPED_TYPE, TRAITS> t = prototypeList;
        Require (t.size () == 0);
        Debug::TraceContextBumper ctx{"BasicAddRemoveTests_", "Add and remove (len={}, forward direction)"_f, testLength};
        size_t                    val = 0;
        KEY_TYPE                  biggestKey;
        for (size_t i = 1; i <= testLength; ++i) {
            KEY_TYPE key{i};
            Assert (not t.Find (key));
            t.Add (key, i);
            Assert (t.size () == i);
            Assert (t.Find (key, &val) and (val == i));
            t.Invariant ();
            strong_ordering comp = t.GetComparer () (biggestKey, key);
            if (comp == strong_ordering::greater or comp == strong_ordering::less) {
                biggestKey = key;
            }
        }
        for (size_t i = 1; i <= testLength; ++i) {
            KEY_TYPE key{i};
            Assert (t.Find (key, &val) and (val == i));
            t.Remove (key);
            Assert (not t.Find (key));
            Assert (t.size () == testLength - i);
            t.Invariant ();
        }
        Assert (t.size () == 0);
        DbgTrace ("Add and remove {} items, backwards direction"_f, testLength);

        for (size_t i = testLength; i >= 1; --i) {
            KEY_TYPE key{i};
            Assert (not t.Find (key));
            t.Add (key, i);
            Assert (t.size () == testLength - i + 1);
            Assert (t.Find (key, &val) and (val == i));
            t.Invariant ();

            int comp = t.GetComparer () (biggestKey, key);
            if (i == testLength or comp < 0) {
                biggestKey = key;
            }
        }
        for (size_t i = testLength; i >= 1; --i) {
            KEY_TYPE key{i};
            Assert (t.Find (key, &val) and (val == i));
            t.Remove (key);
            Assert (not t.Find (key));
            Assert (t.size () == i - 1);
            t.Invariant ();
        }
        Assert (t.size () == 0);
    }
    GTEST_TEST (Foundation_Containers_DataStructures_SkipList, BasicAddRemoveTest_)
    {
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
