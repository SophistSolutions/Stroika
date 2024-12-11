/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
//  TEST    Foundation::Containers::SortedKeyedCollection
//      \note Code-Status:  <a href="Code-Status.md#Beta">Beta</a>
#include "Stroika/Foundation/StroikaPreComp.h"

#include <iostream>

#include "Stroika/Foundation/Containers/Concrete/SortedKeyedCollection_SkipList.h"
#include "Stroika/Foundation/Containers/Concrete/SortedKeyedCollection_stdset.h"
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

using namespace CommonTests::KeyedCollectionTests::Test1_Basics_;

#if qStroika_HasComponent_googletest
namespace {
    GTEST_TEST (Foundation_Containers_SortedKeyedCollection, FACTORY_DEFAULT)
    {
        CommonTests::KeyedCollectionTests::SimpleKeyedCollectionTest_TestBasics (
            [] () { return SortedKeyedCollection<T1, int>{[] (T1 e) { return e.key; }}; }, [] (auto) {});
        CommonTests::KeyedCollectionTests::SimpleKeyedCollectionTest_TestBasics (
            [] () { return SortedKeyedCollection<T1, int, T1_Traits>{T1_Key_Extractor{}}; }, [] (auto) {});
        CommonTests::KeyedCollectionTests::SimpleKeyedCollectionTest_TestBasics (
            [] () { return SortedKeyedCollection<T1, int, T1_Traits>{}; }, [] (auto) {});
    }
}

namespace {
    GTEST_TEST (Foundation_Containers_SortedKeyedCollection, SortedKeyedCollection_stdset)
    {
        CommonTests::KeyedCollectionTests::SimpleKeyedCollectionTest_TestBasics (
            [] () { return Concrete::SortedKeyedCollection_stdset<T1, int>{[] (T1 e) { return e.key; }}; }, [] (auto) {});
        CommonTests::KeyedCollectionTests::SimpleKeyedCollectionTest_TestBasics (
            [] () { return Concrete::SortedKeyedCollection_stdset<T1, int, T1_Traits>{T1_Key_Extractor{}}; }, [] (auto) {});
        CommonTests::KeyedCollectionTests::SimpleKeyedCollectionTest_TestBasics (
            [] () { return Concrete::SortedKeyedCollection_stdset<T1, int, T1_Traits>{}; }, [] (auto) {});
    }
}

namespace {
    GTEST_TEST (Foundation_Containers_SortedKeyedCollection, SortedKeyedCollection_SkipList)
    {
        CommonTests::KeyedCollectionTests::SimpleKeyedCollectionTest_TestBasics (
            [] () { return Concrete::SortedKeyedCollection_SkipList<T1, int>{[] (T1 e) { return e.key; }}; }, [] (auto) {});
        CommonTests::KeyedCollectionTests::SimpleKeyedCollectionTest_TestBasics (
            [] () { return Concrete::SortedKeyedCollection_SkipList<T1, int, T1_Traits>{T1_Key_Extractor{}}; }, [] (auto) {});
        CommonTests::KeyedCollectionTests::SimpleKeyedCollectionTest_TestBasics (
            [] () { return Concrete::SortedKeyedCollection_SkipList<T1, int, T1_Traits>{}; }, [] (auto) {});
    }
}

namespace {
    GTEST_TEST (Foundation_Containers_SortedKeyedCollection, CLEANUP)
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
