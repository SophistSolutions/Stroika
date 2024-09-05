/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
//  TEST    Foundation::Containers::Set
#include "Stroika/Foundation/StroikaPreComp.h"

#include <iostream>

#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/Containers/Collection.h"
#include "Stroika/Foundation/Containers/Concrete/Set_Array.h"
#include "Stroika/Foundation/Containers/Concrete/Set_LinkedList.h"
#include "Stroika/Foundation/Containers/Concrete/SortedSet_SkipList.h"
#include "Stroika/Foundation/Containers/Concrete/SortedSet_stdset.h"
#include "Stroika/Foundation/Containers/Set.h"
#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/Debug/Visualizations.h"

#include "Stroika/Frameworks/Test/ArchtypeClasses.h"
#include "Stroika/Frameworks/Test/TestHarness.h"

#include "../TestCommon/CommonTests_Set.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Containers;

using namespace Stroika::Frameworks;

using Test::ArchtypeClasses::AsIntsEqualsComparer;
using Test::ArchtypeClasses::AsIntsLessComparer;
using Test::ArchtypeClasses::AsIntsThreeWayComparer;
using Test::ArchtypeClasses::OnlyCopyableMoveable;
using Test::ArchtypeClasses::OnlyCopyableMoveableAndTotallyOrdered;

using Concrete::Set_Array;
using Concrete::Set_LinkedList;
using Concrete::SortedSet_SkipList;
using Concrete::SortedSet_stdset;

using namespace CommonTests::SetTests;

using MyOnlyCopyableMoveable_EQUAL_TO_ = AsIntsEqualsComparer<OnlyCopyableMoveable>;
using MyOnlyCopyableMoveable_LESS_     = AsIntsLessComparer<OnlyCopyableMoveable>;
using MyOnlyCopyableMoveable_THREEWAY_ = AsIntsThreeWayComparer<OnlyCopyableMoveable>;

#if qHasFeature_GoogleTest
namespace {
    template <typename CONCRETE_CONTAINER, typename CONCRETE_CONTAINER_FACTORY>
    void DoTestForConcreteContainer_ (CONCRETE_CONTAINER_FACTORY factory)
    {
        using T                  = typename CONCRETE_CONTAINER::value_type;
        auto extraChecksFunction = [] ([[maybe_unused]] const Set<T>& s) {};
        CommonTests::SetTests::Test_All_For_Type<CONCRETE_CONTAINER, Set<T>> (factory, extraChecksFunction);
    }
    template <typename CONCRETE_CONTAINER>
    void DoTestForConcreteContainer_ ()
    {
        DoTestForConcreteContainer_<CONCRETE_CONTAINER> ([] () { return CONCRETE_CONTAINER{}; });
    }
}

namespace {
    namespace ExampleCTORS_Tests_ {
        void DoTest_examples_from_docs_ ()
        {
            // From Set<> CTOR docs
            Collection<int> c;
            vector<int>     v;

            Set<int> s1 = {1, 2, 3};
            Set<int> s2 = s1;
            Set<int> s3{s1};
            Set<int> s4{s1.begin (), s1.end ()};
            Set<int> s5{c};
            Set<int> s6{v};
            Set<int> s7{v.begin (), v.end ()};
            Set<int> s8{move (s1)};
            Set<int> s9{1, 2, 3};
            EXPECT_EQ (s9.size (), 3u);
            Set<int> s10{Common::DeclareEqualsComparer ([] (int l, int r) { return l == r; }), c};
        }
        void TestCTORFromOtherContainer_ ()
        {
            using Characters::Character;
            using Characters::String;
            Set<String> tmp1 = Set<String>{Sequence<String>{}};
            Set<String> tmp2 = Set<String>{String ().Trim ().Tokenize ({';', ' '})};
        }
        void DoTest ()
        {
            DoTest_examples_from_docs_ ();
            TestCTORFromOtherContainer_ ();
        }
    }
}

namespace {
    GTEST_TEST (Foundation_Containers_Set, DEFAULT_SET_FACTORY)
    {
        Debug::TraceContextBumper ctx{"DEFAULT_SET_FACTORY"};
        DoTestForConcreteContainer_<Set<size_t>> ();
        DoTestForConcreteContainer_<Set<OnlyCopyableMoveableAndTotallyOrdered>> ();
        DoTestForConcreteContainer_<Set<OnlyCopyableMoveable>> (
            [] () { return Set<OnlyCopyableMoveable> (MyOnlyCopyableMoveable_EQUAL_TO_{}); });
    }
}

namespace {
    GTEST_TEST (Foundation_Containers_Set, Set_LinkedList)
    {
        Debug::TraceContextBumper ctx{"Set_LinkedList"};
        DoTestForConcreteContainer_<Set_LinkedList<size_t>> ();
        DoTestForConcreteContainer_<Set_LinkedList<OnlyCopyableMoveableAndTotallyOrdered>> ();
        DoTestForConcreteContainer_<Set_LinkedList<OnlyCopyableMoveable>> (
            [] () { return Set_LinkedList<OnlyCopyableMoveable> (MyOnlyCopyableMoveable_EQUAL_TO_{}); });
    }
}

namespace {
    GTEST_TEST (Foundation_Containers_Set, Set_Array)
    {
        Debug::TraceContextBumper ctx{"Set_Array"};
        DoTestForConcreteContainer_<Set_Array<size_t>> ();
        DoTestForConcreteContainer_<Set_Array<OnlyCopyableMoveableAndTotallyOrdered>> ();
        DoTestForConcreteContainer_<Set_Array<OnlyCopyableMoveable>> (
            [] () { return Set_Array<OnlyCopyableMoveable> (MyOnlyCopyableMoveable_EQUAL_TO_{}); });
    }
}

namespace {
    GTEST_TEST (Foundation_Containers_Set, SortedSet_stdset)
    {
        Debug::TraceContextBumper ctx{"SortedSet_stdset"};
        DoTestForConcreteContainer_<SortedSet_stdset<size_t>> ();
        DoTestForConcreteContainer_<SortedSet_stdset<OnlyCopyableMoveableAndTotallyOrdered>> ();
        DoTestForConcreteContainer_<SortedSet_stdset<OnlyCopyableMoveable>> (
            [] () { return SortedSet_stdset<OnlyCopyableMoveable> (MyOnlyCopyableMoveable_LESS_{}); });
    }
}
namespace {
    GTEST_TEST (Foundation_Containers_Set, SortedSet_SkipList)
    {
        Debug::TraceContextBumper ctx{"SortedSet_SkipList"};
        DoTestForConcreteContainer_<SortedSet_SkipList<size_t>> ();
        DoTestForConcreteContainer_<SortedSet_SkipList<OnlyCopyableMoveableAndTotallyOrdered>> ();
        DoTestForConcreteContainer_<SortedSet_SkipList<OnlyCopyableMoveable>> (
            [] () { return SortedSet_SkipList<OnlyCopyableMoveable> (MyOnlyCopyableMoveable_THREEWAY_{}); });
    }
}

namespace {
    GTEST_TEST (Foundation_Containers_Set, ExampleCTORS_Tests_)
    {
        Debug::TraceContextBumper ctx{"ExampleCTORS_Tests_"};
        ExampleCTORS_Tests_::DoTest ();
    }
}

namespace {
    GTEST_TEST (Foundation_Containers_Set, Where_Tests_)
    {
        Debug::TraceContextBumper ctx{"Where_Tests_"};
        Set<int>                  s{1, 2, 3, 4, 5};
        EXPECT_EQ (s.Where ([] (int i) { return Math::IsPrime (i); }), (Set<int>{2, 3, 5}));
    }
}

namespace {
    GTEST_TEST (Foundation_Containers_Set, EqualsTests_)
    {
        Debug::TraceContextBumper ctx{"EqualsTests_"};
        using Characters::String;
        constexpr auto kHeaderNameEqualsComparer = String::EqualsComparer{Characters::eCaseInsensitive};
        Set<String>    m;
        auto           m1 = Set<String>{decltype (kHeaderNameEqualsComparer) (kHeaderNameEqualsComparer), m};
        auto m2 = Set<String>{kHeaderNameEqualsComparer, m}; // http://stroika-bugs.sophists.com/browse/STK-720 failed to compile before fix in 2.1b10x
        EXPECT_EQ (m, m1);
    }
}

namespace {
    GTEST_TEST (Foundation_Containers_Set, CLEANUP)
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
