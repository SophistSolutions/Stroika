/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
//  TEST    Foundation::Containers::MultiSet
#include "Stroika/Foundation/StroikaPreComp.h"

#include <iostream>

#include "Stroika/Foundation/Containers/Collection.h"

#include "Stroika/Foundation/Containers/MultiSet.h"

#include "Stroika/Foundation/Containers/Concrete/MultiSet_Array.h"
#include "Stroika/Foundation/Containers/Concrete/MultiSet_LinkedList.h"
#include "Stroika/Foundation/Containers/Concrete/SortedMultiSet_SkipList.h"
#include "Stroika/Foundation/Containers/Concrete/SortedMultiSet_stdmap.h"

#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/Debug/Visualizations.h"

#include "Stroika/Frameworks/Test/ArchtypeClasses.h"
#include "Stroika/Frameworks/Test/TestHarness.h"

#include "../TestCommon/CommonTests_MultiSet.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Containers;

using namespace Stroika::Frameworks;

using Concrete::MultiSet_Array;
using Concrete::MultiSet_LinkedList;
using Concrete::SortedMultiSet_stdmap;

using Test::ArchtypeClasses::AsIntsEqualsComparer;
using Test::ArchtypeClasses::AsIntsLessComparer;
using Test::ArchtypeClasses::OnlyCopyableMoveable;
using Test::ArchtypeClasses::OnlyCopyableMoveableAndTotallyOrdered;

#if qHasFeature_GoogleTest
namespace {
    template <typename CONCRETE_CONTAINER, typename SCHEMA = CommonTests::MultiSetTests::DEFAULT_TESTING_SCHEMA<CONCRETE_CONTAINER>>
    void DoTestForConcreteContainer_ (const SCHEMA& schema = {})
    {
        Debug::TraceContextBumper ctx{"{}::DoTestForConcreteContainer_"};
        CommonTests::MultiSetTests::All_For_Type (schema);
    }
}

namespace {
    namespace ExampleCTORS_Test_2_ {
        void DoTest ()
        {
            // From MultiSet<> CTOR docs
            Collection<int>  c;
            std::vector<int> v;

            MultiSet<int> s1 = {1, 2, 3};
            MultiSet<int> s2 = s1;
            MultiSet<int> s3{s1};
            MultiSet<int> s4{s1.begin (), s1.end ()};
            MultiSet<int> s5{c};
            MultiSet<int> s6{v};
            MultiSet<int> s7{v.begin (), v.end ()};
            MultiSet<int> s8{move (s1)};
            MultiSet<int> s9{Common::DeclareEqualsComparer ([] (int l, int r) { return l == r; }), c};
        }
    }
}

namespace {
    namespace Top_Test_3_ {
        void DoTest ()
        {
            {
                MultiSet<int> test{1, 1, 5, 1, 6, 5};
                EXPECT_TRUE (test.Top ().SequentialEquals ({{1, 3}, {5, 2}, {6, 1}}));
                EXPECT_TRUE (test.Top (1).SequentialEquals ({{1, 3}}));
            }
            {
                MultiSet<int> test{1, 1, 5, 1, 6, 5};
                EXPECT_TRUE (test.TopElements ().SequentialEquals ({1, 5, 6}));
                EXPECT_TRUE (test.TopElements (1).SequentialEquals ({1}));
            }
        }
    }
}

namespace {
    GTEST_TEST (Foundation_Containers_MultiSet, all)
    {
        struct MyOnlyCopyableMoveable_ComparerWithEquals_ : Common::ComparisonRelationDeclarationBase<Common::ComparisonRelationType::eEquals> {
            bool operator() (const OnlyCopyableMoveable& lhs, const OnlyCopyableMoveable& rhs) const
            {
                return static_cast<size_t> (lhs) == static_cast<size_t> (rhs);
            }
        };
        struct MyOnlyCopyableMoveable_ComparerWithLess_ : Common::ComparisonRelationDeclarationBase<Common::ComparisonRelationType::eStrictInOrder> {
            bool operator() (const OnlyCopyableMoveable& lhs, const OnlyCopyableMoveable& rhs) const
            {
                return static_cast<size_t> (lhs) < static_cast<size_t> (rhs);
            }
        };

        {
            DoTestForConcreteContainer_<MultiSet<size_t>> ();
            DoTestForConcreteContainer_<MultiSet<OnlyCopyableMoveableAndTotallyOrdered>> ();
            auto msFactory = [] () { return MultiSet<OnlyCopyableMoveable>{MyOnlyCopyableMoveable_ComparerWithEquals_{}}; };
            DoTestForConcreteContainer_<MultiSet<OnlyCopyableMoveable>> (
                CommonTests::MultiSetTests::DEFAULT_TESTING_SCHEMA<MultiSet<OnlyCopyableMoveable>, MyOnlyCopyableMoveable_ComparerWithEquals_, decltype (msFactory)> (
                    msFactory));
        }

        {
            DoTestForConcreteContainer_<MultiSet_Array<size_t>> ();
            DoTestForConcreteContainer_<MultiSet_Array<OnlyCopyableMoveableAndTotallyOrdered>> ();
            auto msFactory = [] () { return MultiSet_Array<OnlyCopyableMoveable>{MyOnlyCopyableMoveable_ComparerWithEquals_{}}; };
            DoTestForConcreteContainer_<MultiSet_Array<OnlyCopyableMoveable>> (
                CommonTests::MultiSetTests::DEFAULT_TESTING_SCHEMA<MultiSet<OnlyCopyableMoveable>, MyOnlyCopyableMoveable_ComparerWithEquals_, decltype (msFactory)> (
                    msFactory));
        }

        {
            DoTestForConcreteContainer_<MultiSet_LinkedList<size_t>> ();
            DoTestForConcreteContainer_<MultiSet_LinkedList<OnlyCopyableMoveableAndTotallyOrdered>> ();
            auto msFactory = [] () { return MultiSet_LinkedList<OnlyCopyableMoveable>{MyOnlyCopyableMoveable_ComparerWithEquals_{}}; };
            DoTestForConcreteContainer_<MultiSet_LinkedList<OnlyCopyableMoveable>> (
                CommonTests::MultiSetTests::DEFAULT_TESTING_SCHEMA<MultiSet<OnlyCopyableMoveable>, MyOnlyCopyableMoveable_ComparerWithEquals_, decltype (msFactory)> (
                    msFactory));
        }

        {
            DoTestForConcreteContainer_<SortedMultiSet_stdmap<size_t>> ();
            DoTestForConcreteContainer_<SortedMultiSet_stdmap<OnlyCopyableMoveableAndTotallyOrdered>> ();
            auto msFactory = [] () { return SortedMultiSet_stdmap<OnlyCopyableMoveable>{MyOnlyCopyableMoveable_ComparerWithLess_{}}; };
            DoTestForConcreteContainer_<SortedMultiSet_stdmap<OnlyCopyableMoveable>> (
                CommonTests::MultiSetTests::DEFAULT_TESTING_SCHEMA<SortedMultiSet_stdmap<OnlyCopyableMoveable>,
                                                                   MyOnlyCopyableMoveable_ComparerWithEquals_, decltype (msFactory)> (msFactory));
        }

        ExampleCTORS_Test_2_::DoTest ();
        Top_Test_3_::DoTest ();

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
