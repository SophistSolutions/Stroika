/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
//  TEST    Foundation::Containers::SortedKeyedCollection
//      \note Code-Status:  <a href="Code-Status.md#Beta">Beta</a>
#include "Stroika/Foundation/StroikaPreComp.h"

#include <iostream>

#include "Stroika/Foundation/Containers/Concrete/SortedKeyedCollection_SkipList.h"
#include "Stroika/Foundation/Containers/SortedKeyedCollection.h"

#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/Debug/Visualizations.h"

#include "Stroika/Frameworks/Test/ArchtypeClasses.h"
#include "Stroika/Frameworks/Test/TestHarness.h"

#include "../TestCommon/CommonTests_KeyedCollection.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Containers;

using namespace Stroika::Frameworks;

using Test::ArchtypeClasses::AsIntsEqualsComparer;
using Test::ArchtypeClasses::AsIntsLessComparer;
using Test::ArchtypeClasses::OnlyCopyableMoveable;
using Test::ArchtypeClasses::OnlyCopyableMoveableAndTotallyOrdered;

#if qHasFeature_GoogleTest
namespace {
    GTEST_TEST (Foundation_Containers_SortedKeyedCollection, all)
    {
        using T1               = CommonTests::KeyedCollectionTests::Test1_Basics_::T1;
        using T1_Traits        = CommonTests::KeyedCollectionTests::Test1_Basics_::T1_Traits;
        using T1_Key_Extractor = CommonTests::KeyedCollectionTests::Test1_Basics_::T1_Key_Extractor;
        CommonTests::KeyedCollectionTests::SimpleKeyedCollectionTest_TestBasics (
            [] () { return SortedKeyedCollection<T1, int>{[] (T1 e) { return e.key; }}; }, [] (auto) {});
        CommonTests::KeyedCollectionTests::SimpleKeyedCollectionTest_TestBasics (
            [] () { return SortedKeyedCollection<T1, int, T1_Traits>{T1_Key_Extractor{}}; }, [] (auto) {});
        CommonTests::KeyedCollectionTests::SimpleKeyedCollectionTest_TestBasics (
            [] () { return SortedKeyedCollection<T1, int, T1_Traits>{}; }, [] (auto) {});
        EXPECT_TRUE (OnlyCopyableMoveableAndTotallyOrdered::GetTotalLiveCount () == 0 and OnlyCopyableMoveable::GetTotalLiveCount () == 0); // simple portable leak check
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
