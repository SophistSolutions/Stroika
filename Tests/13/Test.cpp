/*
* Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
*/
//  TEST    Foundation::Containers::Collection
#include "Stroika/Foundation/StroikaPreComp.h"

#include <iostream>

#include "Stroika/Foundation/Containers/Collection.h"
#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/Debug/Visualizations.h"

#include "Stroika/Frameworks/Test/ArchtypeClasses.h"
#include "Stroika/Frameworks/Test/TestHarness.h"

#include "../TestCommon/CommonTests_Collection.h"

#include "Stroika/Foundation/Containers/Concrete/Collection_Array.h"
#include "Stroika/Foundation/Containers/Concrete/Collection_LinkedList.h"
#include "Stroika/Foundation/Containers/Concrete/Collection_stdforward_list.h"
#include "Stroika/Foundation/Containers/Concrete/SortedCollection_SkipList.h"
#include "Stroika/Foundation/Containers/Concrete/SortedCollection_stdmultiset.h"

#include "Stroika/Foundation/Containers/SortedMultiSet.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Containers;

using namespace Stroika::Frameworks;

using Test::ArchtypeClasses::AsIntsEqualsComparer;
using Test::ArchtypeClasses::AsIntsLessComparer;
using Test::ArchtypeClasses::AsIntsThreeWayComparer;
using Test::ArchtypeClasses::OnlyCopyableMoveable;
using Test::ArchtypeClasses::OnlyCopyableMoveableAndTotallyOrdered;

using Concrete::Collection_Array;
using Concrete::Collection_LinkedList;
using Concrete::Collection_stdforward_list;
using Concrete::SortedCollection_SkipList;
using Concrete::SortedCollection_stdmultiset;

using MyOnlyCopyableMoveable_ComparerWithEquals_ = AsIntsEqualsComparer<OnlyCopyableMoveable>;
using MyOnlyCopyableMoveable_LESS_               = AsIntsLessComparer<OnlyCopyableMoveable>;
using MyOnlyCopyableMoveable_THREEWAY_           = AsIntsThreeWayComparer<OnlyCopyableMoveable>;

#if qHasFeature_GoogleTest
namespace {
    template <typename CONCRETE_CONTAINER, typename CONCRETE_CONTAINER_FACTORY>
    void RunTests_ (CONCRETE_CONTAINER_FACTORY factory)
    {
        Debug::TraceContextBumper ctx{"{}::RunTests_"};
        CommonTests::CollectionTests::SimpleCollectionTest_Generic<CONCRETE_CONTAINER> (
            factory, [] ([[maybe_unused]] const typename CONCRETE_CONTAINER::ArchetypeContainerType& c) {});
    }
    template <typename CONCRETE_CONTAINER>
    void RunTests_ ()
    {
        RunTests_<CONCRETE_CONTAINER> ([] () { return CONCRETE_CONTAINER{}; });
    }
}

namespace {
    template <typename CONCRETE_CONTAINER, typename EQUALS_COMPARER>
    void RunTestsWithEquals_ ()
    {
        Debug::TraceContextBumper ctx{"{}::RunTests_"};
        CommonTests::CollectionTests::SimpleCollectionTest_TestsWhichRequireEquals<CONCRETE_CONTAINER, EQUALS_COMPARER> ();
    }
}

namespace {
    namespace SB_SEPARETEFILE_CONTAINERS_MISC_Adder_ {
        using namespace Containers::Adapters;
        static_assert (IAddableTo<set<int>>);
        static_assert (IAddableTo<MultiSet<int>>);
        static_assert (IAddableTo<Containers::SortedMultiSet<int>>);
    }
}

namespace {
    GTEST_TEST (Foundation_Containers_Collection, DEFAULT_FACTORY_TESTS)
    {
        Debug::TraceContextBumper ctx{"{}::DEFAULT_FACTORY_TESTS"};
        RunTests_<Collection<size_t>> ();
        RunTestsWithEquals_<Collection<size_t>, equal_to<size_t>> ();
        RunTests_<Collection<OnlyCopyableMoveableAndTotallyOrdered>> ();
        RunTests_<Collection<OnlyCopyableMoveable>> ();
    }
}

namespace {
    GTEST_TEST (Foundation_Containers_Collection, Collection_LinkedList)
    {
        Debug::TraceContextBumper ctx{"{}::Collection_LinkedList"};
        RunTests_<Collection_LinkedList<size_t>> ();
        RunTestsWithEquals_<Collection_LinkedList<size_t>, equal_to<size_t>> ();
        RunTests_<Collection_LinkedList<OnlyCopyableMoveableAndTotallyOrdered>> ();
        RunTests_<Collection_LinkedList<OnlyCopyableMoveable>> ();
    }
}

namespace {
    GTEST_TEST (Foundation_Containers_Collection, Collection_Array)
    {
        Debug::TraceContextBumper ctx{"{}::Collection_Array"};
        RunTests_<Collection_Array<size_t>> ();
        RunTestsWithEquals_<Collection_Array<size_t>, equal_to<size_t>> ();
        RunTests_<Collection_Array<OnlyCopyableMoveableAndTotallyOrdered>> ();
        RunTests_<Collection_Array<OnlyCopyableMoveable>> ();
    }
}

namespace {
    GTEST_TEST (Foundation_Containers_Collection, Collection_stdforward_list)
    {
        Debug::TraceContextBumper ctx{"{}::Collection_stdforward_list"};
        RunTests_<Collection_stdforward_list<size_t>> ();
        RunTestsWithEquals_<Collection_stdforward_list<size_t>, equal_to<size_t>> ();
        RunTests_<Collection_stdforward_list<OnlyCopyableMoveableAndTotallyOrdered>> ();
        RunTests_<Collection_stdforward_list<OnlyCopyableMoveable>> ();
    }
}

namespace {
    GTEST_TEST (Foundation_Containers_Collection, SortedCollection_SkipList)
    {
        Debug::TraceContextBumper ctx{"{}::SortedCollection_SkipList"};
        RunTests_<SortedCollection_SkipList<size_t>> ();
        RunTestsWithEquals_<SortedCollection_SkipList<size_t>, equal_to<size_t>> ();
        RunTests_<SortedCollection_SkipList<OnlyCopyableMoveableAndTotallyOrdered>> ();
        RunTests_<SortedCollection_SkipList<OnlyCopyableMoveable>> (
            [] () { return SortedCollection_SkipList<OnlyCopyableMoveable> (MyOnlyCopyableMoveable_THREEWAY_{}); });
        {
            SortedCollection_SkipList<size_t> x;
            x.ReBalance ();
        }
    }
}

namespace {
    GTEST_TEST (Foundation_Containers_Collection, SortedCollection_stdmultiset)
    {
        Debug::TraceContextBumper ctx{"{}::SortedCollection_stdmultiset"};
        RunTests_<SortedCollection_stdmultiset<size_t>> ();
        RunTestsWithEquals_<SortedCollection_stdmultiset<size_t>, equal_to<size_t>> ();
        RunTests_<SortedCollection_stdmultiset<OnlyCopyableMoveableAndTotallyOrdered>> ();
        RunTests_<SortedCollection_stdmultiset<OnlyCopyableMoveable>> (
            [] () { return SortedCollection_stdmultiset<OnlyCopyableMoveable> (MyOnlyCopyableMoveable_LESS_{}); });
    }
}

namespace {
    GTEST_TEST (Foundation_Containers_Collection, ExampleCTORS_Tests_)
    {
        Debug::TraceContextBumper ctx{"{}::ExampleCTORS_Tests_"};
        // From Collection<> CTOR docs
        Sequence<int> s;
        vector<int>   v;

        Collection<int> c1 = {1, 2, 3};
        Collection<int> c2 = c1;
        Collection<int> c3{c1};
        Collection<int> c4{c1.begin (), c1.end ()};
        Collection<int> c5{s};
        Collection<int> c6{v};
        Collection<int> c7{v.begin (), v.end ()};
        Collection<int> c8{move (c1)};
    }
}

namespace {
    GTEST_TEST (Foundation_Containers_Collection, RemoveAndUpdateIteratorUpdate)
    {
        Debug::TraceContextBumper ctx{"{}::RemoveAndUpdateIteratorUpdate"};
        {
            Collection<int> c   = {1, 2, 3, 4};
            int             sum = 0;
            c                   = {1, 2, 3, 4};
            for (Iterator<int> i = c.begin (); i != c.end ();) {
                sum += *i;
                if (*i == 3) {
                    c.Remove (i, &i);
                }
                else {
                    ++i;
                }
            }
            EXPECT_EQ (sum, 10); // verify we still hit all items
            EXPECT_EQ (c.Sum (), 7);
        }
    }
}
namespace {
    GTEST_TEST (Foundation_Containers_Collection, ExampleUsingWhere)
    {
        Debug::TraceContextBumper ctx{"{}::ExampleUsingWhere"};
        {
            Collection<int> c{1, 2, 3, 4, 5, 6};
            EXPECT_TRUE (c.Where ([] (int i) { return i % 2 == 0; }).SetEquals (Iterable<int>{2, 4, 6}));
            EXPECT_TRUE (c.Where<Iterable<int>> ([] (int i) { return i % 2 == 0; }).SetEquals (Iterable<int>{2, 4, 6})); // to get lazy evaluation
        }
    }
}

namespace {
    GTEST_TEST (Foundation_Containers_Collection, CLEANUP)
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
