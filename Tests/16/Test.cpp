/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
//  TEST    Foundation::Containers::KeyedCollection
//      \note Code-Status:  <a href="Code-Status.md#Beta">Beta</a>
#include "Stroika/Foundation/StroikaPreComp.h"

#include <iostream>
#include <typeindex>

#include "Stroika/Foundation/Containers/Concrete/KeyedCollection_Array.h"
#include "Stroika/Foundation/Containers/Concrete/KeyedCollection_LinkedList.h"
#include "Stroika/Foundation/Containers/Concrete/KeyedCollection_stdhashset.h"
#include "Stroika/Foundation/Containers/Concrete/SortedKeyedCollection_SkipList.h"
#include "Stroika/Foundation/Containers/Concrete/SortedKeyedCollection_stdset.h"
#include "Stroika/Foundation/Containers/KeyedCollection.h"
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

//using T1               = CommonTests::KeyedCollectionTests::Test1_Basics_::T1;
//using T1_Traits        = CommonTests::KeyedCollectionTests::Test1_Basics_::T1_Traits;
//using T1_Key_Extractor = CommonTests::KeyedCollectionTests::Test1_Basics_::T1_Key_Extractor;

#if qHasFeature_GoogleTest
namespace {
    namespace Test_KeyedCollectionTypeIndexUsesStdSet_ {
        struct Obj_ {
            type_index fTypeIndex;  // KEY
            int        otherData{}; //
        };
        using My_Extractor_ = decltype ([] (const Obj_& t) -> type_index { return t.fTypeIndex; });
        using My_Traits_    = Containers::KeyedCollection_DefaultTraits<Obj_, type_index, My_Extractor_>;
    }
    GTEST_TEST (Foundation_Containers_KeyedCollection, Test_KeyedCollectionTypeIndexUsesStdSet_)
    {
        using namespace Test_KeyedCollectionTypeIndexUsesStdSet_;
        {
            Concrete::SortedKeyedCollection_stdset<Obj_, type_index, My_Traits_> s1;
            KeyedCollection<Obj_, type_index, My_Traits_>                        s2;
            s2.Add (Obj_{typeid (int)});
            s2.Add (Obj_{typeid (long int)});
        }
        {
            // Or slightly more flexibility, but less efficiently (cuz extractor stored in std::function and invoked through that wrapper)
            KeyedCollection<Obj_, type_index> s2{My_Extractor_{}};
            s2.Add (Obj_{typeid (int)});
            s2.Add (Obj_{typeid (long int)});
        }
    }
}

namespace {
    GTEST_TEST (Foundation_Containers_KeyedCollection, FACTORY_DEFAULT)
    {
        CommonTests::KeyedCollectionTests::SimpleKeyedCollectionTest_TestBasics (
            [] () { return KeyedCollection<T1, int>{[] (T1 e) { return e.key; }}; }, [] (auto) {});
        CommonTests::KeyedCollectionTests::SimpleKeyedCollectionTest_TestBasics (
            [] () { return KeyedCollection<T1, int, T1_Traits>{T1_Key_Extractor{}}; }, [] (auto) {});
        CommonTests::KeyedCollectionTests::SimpleKeyedCollectionTest_TestBasics ([] () { return KeyedCollection<T1, int, T1_Traits>{}; },
                                                                                 [] (auto) {});
    }
}
namespace {
    GTEST_TEST (Foundation_Containers_KeyedCollection, KeyedCollection_Array)
    {
        CommonTests::KeyedCollectionTests::SimpleKeyedCollectionTest_TestBasics (
            [] () { return Concrete::KeyedCollection_Array<T1, int>{[] (T1 e) { return e.key; }}; }, [] (auto) {});
    }
}
namespace {
    GTEST_TEST (Foundation_Containers_KeyedCollection, KeyedCollection_LinkedList)
    {
        CommonTests::KeyedCollectionTests::SimpleKeyedCollectionTest_TestBasics (
            [] () { return Concrete::KeyedCollection_LinkedList<T1, int>{[] (T1 e) { return e.key; }}; }, [] (auto) {});
    }
}

namespace {
    GTEST_TEST (Foundation_Containers_KeyedCollection, SortedKeyedCollection_stdset)
    {
        CommonTests::KeyedCollectionTests::SimpleKeyedCollectionTest_TestBasics (
            [] () { return Concrete::SortedKeyedCollection_stdset<T1, int>{[] (T1 e) { return e.key; }}; }, [] (auto) {});
    }
}

namespace {
    GTEST_TEST (Foundation_Containers_KeyedCollection, KeyedCollection_stdhashset)
    {
        CommonTests::KeyedCollectionTests::SimpleKeyedCollectionTest_TestBasics (
            [] () { return Concrete::KeyedCollection_stdhashset<T1, int>{[] (T1 e) { return e.key; }}; }, [] (auto) {});
    }
}

namespace {
    GTEST_TEST (Foundation_Containers_KeyedCollection, KeyedCollection_SkipList)
    {
        CommonTests::KeyedCollectionTests::SimpleKeyedCollectionTest_TestBasics (
            [] () { return Concrete::SortedKeyedCollection_SkipList<T1, int>{[] (T1 e) { return e.key; }}; }, [] (auto) {});
        {
            Concrete::SortedKeyedCollection_SkipList<T1, int> x{[] (T1 e) { return e.key; }};
            x.ReBalance ();
        }
    }
}

namespace {
    GTEST_TEST (Foundation_Containers_KeyedCollection, thoughtISawBugWithThisCopyScenarioButSeemsFine)
    {
        struct A {
            int f;
        };
        KeyedCollection<A, int> obj{[] (A e) { return e.f; }};
        obj.Add (A{.f = 3});

        auto objCopy = obj;
        obj.Add (A{.f = 4});
        obj = objCopy;
    }
}

namespace {
    GTEST_TEST (Foundation_Containers_KeyedCollection, CLEANUP)
    {
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
