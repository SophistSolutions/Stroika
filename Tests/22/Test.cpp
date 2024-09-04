/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
//  TEST    Foundation::Containers::Set
#include "Stroika/Foundation/StroikaPreComp.h"

#include <iostream>
#include <sstream>

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
using Test::ArchtypeClasses::OnlyCopyableMoveable;
using Test::ArchtypeClasses::OnlyCopyableMoveableAndTotallyOrdered;

using Concrete::Set_Array;
using Concrete::Set_LinkedList;
using Concrete::SortedSet_SkipList;
using Concrete::SortedSet_stdset;

#if qHasFeature_GoogleTest
namespace {
    template <typename CONCRETE_CONTAINER, typename CONCRETE_CONTAINER_FACTORY>
    void DoTestForConcreteContainer_ (CONCRETE_CONTAINER_FACTORY factory)
    {
        using T                  = typename CONCRETE_CONTAINER::value_type;
        auto extraChecksFunction = [] ([[maybe_unused]] const Set<T>& s) {
            // only work todo on sorted sets
        };
        CommonTests::SetTests::Test_All_For_Type<CONCRETE_CONTAINER, Set<T>> (factory, extraChecksFunction);
    }
    template <typename CONCRETE_CONTAINER>
    void DoTestForConcreteContainer_ ()
    {
        DoTestForConcreteContainer_<CONCRETE_CONTAINER> ([] () { return CONCRETE_CONTAINER{}; });
    }
}

namespace {
    namespace ExampleCTORS_Test_2_ {
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
    namespace Where_Test_3_ {
        void DoAll ()
        {
            Set<int> s{1, 2, 3, 4, 5};
            EXPECT_EQ (s.Where ([] (int i) { return Math::IsPrime (i); }), (Set<int>{2, 3, 5}));
        }
    }
}

namespace {
    namespace EqualsTests_Test_4_ {
        void DoAll ()
        {
            using Characters::String;
            constexpr auto kHeaderNameEqualsComparer = String::EqualsComparer{Characters::eCaseInsensitive};
            Set<String>    m;
            auto           m1 = Set<String>{decltype (kHeaderNameEqualsComparer) (kHeaderNameEqualsComparer), m};
            auto m2 = Set<String>{kHeaderNameEqualsComparer, m}; // http://stroika-bugs.sophists.com/browse/STK-720 failed to compile before fix in 2.1b10x
        }
    }
}

namespace {
    GTEST_TEST (Foundation_Containers_Set, all)
    {
        using namespace CommonTests::SetTests;

        using MyOnlyCopyableMoveable_EQUAL_TO_ = AsIntsEqualsComparer<OnlyCopyableMoveable>;
        using MyOnlyCopyableMoveable_LESS_     = AsIntsLessComparer<OnlyCopyableMoveable>;

        DoTestForConcreteContainer_<Set<size_t>> ();
        DoTestForConcreteContainer_<Set<OnlyCopyableMoveableAndTotallyOrdered>> ();
        DoTestForConcreteContainer_<Set<OnlyCopyableMoveable>> (
            [] () { return Set<OnlyCopyableMoveable> (MyOnlyCopyableMoveable_EQUAL_TO_{}); });

        DoTestForConcreteContainer_<Set_LinkedList<size_t>> ();
        DoTestForConcreteContainer_<Set_LinkedList<OnlyCopyableMoveableAndTotallyOrdered>> ();
        DoTestForConcreteContainer_<Set_LinkedList<OnlyCopyableMoveable>> (
            [] () { return Set_LinkedList<OnlyCopyableMoveable> (MyOnlyCopyableMoveable_EQUAL_TO_{}); });

        DoTestForConcreteContainer_<Set_Array<size_t>> ();
        DoTestForConcreteContainer_<Set_Array<OnlyCopyableMoveableAndTotallyOrdered>> ();
        DoTestForConcreteContainer_<Set_Array<OnlyCopyableMoveable>> (
            [] () { return Set_Array<OnlyCopyableMoveable> (MyOnlyCopyableMoveable_EQUAL_TO_{}); });

        DoTestForConcreteContainer_<SortedSet_stdset<size_t>> ();
        DoTestForConcreteContainer_<SortedSet_stdset<OnlyCopyableMoveableAndTotallyOrdered>> ();
        DoTestForConcreteContainer_<SortedSet_stdset<OnlyCopyableMoveable>> (
            [] () { return SortedSet_stdset<OnlyCopyableMoveable> (MyOnlyCopyableMoveable_LESS_{}); });

        DataStructures::SkipList<size_t, void> xx;
        SortedSet_SkipList<size_t>             tmp;
        //DoTestForConcreteContainer_<SortedSet_SkipList<size_t>> ();
        //DoTestForConcreteContainer_<SortedSet_stdset<OnlyCopyableMoveableAndTotallyOrdered>> ();
        //DoTestForConcreteContainer_<SortedSet_stdset<OnlyCopyableMoveable>> (
        //    [] () { return SortedSet_stdset<OnlyCopyableMoveable> (MyOnlyCopyableMoveable_LESS_{}); });

        ExampleCTORS_Test_2_::DoTest ();

        Where_Test_3_::DoAll ();
        EqualsTests_Test_4_::DoAll ();

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
