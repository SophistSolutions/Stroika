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
#include "Stroika/Foundation/Containers/Concrete/SortedCollection_stdmultiset.h"

#include "Stroika/Foundation/Containers/SortedMultiSet.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Containers;

using namespace Stroika::Frameworks;

using Test::ArchtypeClasses::AsIntsEqualsComparer;
using Test::ArchtypeClasses::AsIntsLessComparer;
using Test::ArchtypeClasses::OnlyCopyableMoveable;
using Test::ArchtypeClasses::OnlyCopyableMoveableAndTotallyOrdered;

using Concrete::Collection_Array;
using Concrete::Collection_LinkedList;
using Concrete::Collection_stdforward_list;
using Concrete::SortedCollection_stdmultiset;

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
    namespace ExampleCTORS_Test_2_ {
        void DoTest ()
        {
            Debug::TraceContextBumper ctx{"{}::ExampleCTORS_Test_2_"};
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
}

namespace {
    namespace RemoveAndUpdateIteratorUpdate_Test3 {
        void DoTest ()
        {
            Debug::TraceContextBumper ctx{"{}::RemoveAndUpdateIteratorUpdate_Test3"};
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
                EXPECT_TRUE (sum == 10); // verify we still hit all items
                EXPECT_TRUE (c.Sum () == 7);
            }
        }
    }
}

namespace {
    namespace SB_SEPARETEFILE_CONTAINERS_MISC_Adder {
        void DoTest ()
        {
            using namespace Containers::Adapters;
            static_assert (IAddableTo<set<int>>);
            static_assert (IAddableTo<MultiSet<int>>);
            static_assert (IAddableTo<Containers::SortedMultiSet<int>>);
        }
    }
}

namespace {
    namespace ExampleUsingWhere {
        void DoTest ()
        {
            Debug::TraceContextBumper ctx{"{}::ExampleUsingWhere"};
            {
                Collection<int> c{1, 2, 3, 4, 5, 6};
                EXPECT_TRUE (c.Where ([] (int i) { return i % 2 == 0; }).SetEquals (Iterable<int>{2, 4, 6}));
                EXPECT_TRUE (c.Where<Iterable<int>> ([] (int i) { return i % 2 == 0; }).SetEquals (Iterable<int>{2, 4, 6})); // to get lazy evaluation
            }
        }
    }
}

namespace {
    GTEST_TEST (Foundation_Containers_Collection, all)
    {
        struct MyOnlyCopyableMoveable_ComparerWithEquals_ {
            using value_type = OnlyCopyableMoveable;
            static bool Equals (value_type v1, value_type v2)
            {
                return static_cast<size_t> (v1) == static_cast<size_t> (v2);
            }
        };
        struct MyOnlyCopyableMoveable_LESS_ : Common::ComparisonRelationDeclarationBase<Common::ComparisonRelationType::eStrictInOrder> {
            bool operator() (const OnlyCopyableMoveable& lhs, const OnlyCopyableMoveable& rhs) const
            {
                return static_cast<size_t> (lhs) < static_cast<size_t> (rhs);
            }
        };
        RunTests_<Collection<size_t>> ();
        RunTestsWithEquals_<Collection<size_t>, equal_to<size_t>> ();
        RunTests_<Collection<OnlyCopyableMoveableAndTotallyOrdered>> ();
        RunTests_<Collection<OnlyCopyableMoveable>> ();

        RunTests_<Collection_LinkedList<size_t>> ();
        RunTestsWithEquals_<Collection_LinkedList<size_t>, equal_to<size_t>> ();
        RunTests_<Collection_LinkedList<OnlyCopyableMoveableAndTotallyOrdered>> ();
        RunTests_<Collection_LinkedList<OnlyCopyableMoveable>> ();

        RunTests_<Collection_Array<size_t>> ();
        RunTestsWithEquals_<Collection_Array<size_t>, equal_to<size_t>> ();
        RunTests_<Collection_Array<OnlyCopyableMoveableAndTotallyOrdered>> ();
        RunTests_<Collection_Array<OnlyCopyableMoveable>> ();

        RunTests_<Collection_stdforward_list<size_t>> ();
        RunTestsWithEquals_<Collection_stdforward_list<size_t>, equal_to<size_t>> ();
        RunTests_<Collection_stdforward_list<OnlyCopyableMoveableAndTotallyOrdered>> ();
        RunTests_<Collection_stdforward_list<OnlyCopyableMoveable>> ();

        RunTests_<SortedCollection_stdmultiset<size_t>> ();
        RunTestsWithEquals_<SortedCollection_stdmultiset<size_t>, equal_to<size_t>> ();
        RunTests_<SortedCollection_stdmultiset<OnlyCopyableMoveableAndTotallyOrdered>> ();
        RunTests_<SortedCollection_stdmultiset<OnlyCopyableMoveable>> (
            [] () { return SortedCollection_stdmultiset<OnlyCopyableMoveable> (MyOnlyCopyableMoveable_LESS_ ()); });

        ExampleCTORS_Test_2_::DoTest ();
        RemoveAndUpdateIteratorUpdate_Test3::DoTest ();
        ExampleUsingWhere::DoTest ();

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
