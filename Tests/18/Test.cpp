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
#include "Stroika/Foundation/Containers/Concrete/MultiSet_stdmap.h"

#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/Debug/Visualizations.h"

#include "Stroika/Frameworks/Test/ArchtypeClasses.h"
#include "Stroika/Frameworks/Test/TestHarness.h"

#include "../TestCommon/CommonTests_MultiSet.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Containers;

using namespace Stroika::Frameworks;

using Test::ArchtypeClasses::SimpleClass;
using Test::ArchtypeClasses::SimpleClassWithoutComparisonOperators;

using Concrete::MultiSet_Array;
using Concrete::MultiSet_LinkedList;
using Concrete::MultiSet_stdmap;

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
#if qHasFeature_GoogleTest
    GTEST_TEST (Foundation_Caching, all)
#else
    void DoRegressionTests_ ()
#endif
    {
        struct MySimpleClassWithoutComparisonOperators_ComparerWithEquals_
            : Common::ComparisonRelationDeclarationBase<Common::ComparisonRelationType::eEquals> {
            bool operator() (const SimpleClassWithoutComparisonOperators& lhs, const SimpleClassWithoutComparisonOperators& rhs) const
            {
                return lhs.GetValue () == rhs.GetValue ();
            }
        };
        struct MySimpleClassWithoutComparisonOperators_ComparerWithLess_
            : Common::ComparisonRelationDeclarationBase<Common::ComparisonRelationType::eStrictInOrder> {
            bool operator() (const SimpleClassWithoutComparisonOperators& lhs, const SimpleClassWithoutComparisonOperators& rhs) const
            {
                return lhs.GetValue () < rhs.GetValue ();
            }
        };

        {
            DoTestForConcreteContainer_<MultiSet<size_t>> ();
            DoTestForConcreteContainer_<MultiSet<SimpleClass>> ();
            auto msFactory = [] () {
                return MultiSet<SimpleClassWithoutComparisonOperators>{MySimpleClassWithoutComparisonOperators_ComparerWithEquals_{}};
            };
            DoTestForConcreteContainer_<MultiSet<SimpleClassWithoutComparisonOperators>> (
                CommonTests::MultiSetTests::DEFAULT_TESTING_SCHEMA<MultiSet<SimpleClassWithoutComparisonOperators>, MySimpleClassWithoutComparisonOperators_ComparerWithEquals_,
                                                                   decltype (msFactory)> (msFactory));
        }

        {
            DoTestForConcreteContainer_<MultiSet_Array<size_t>> ();
            DoTestForConcreteContainer_<MultiSet_Array<SimpleClass>> ();
            auto msFactory = [] () {
                return MultiSet_Array<SimpleClassWithoutComparisonOperators>{MySimpleClassWithoutComparisonOperators_ComparerWithEquals_{}};
            };
            DoTestForConcreteContainer_<MultiSet_Array<SimpleClassWithoutComparisonOperators>> (
                CommonTests::MultiSetTests::DEFAULT_TESTING_SCHEMA<MultiSet<SimpleClassWithoutComparisonOperators>, MySimpleClassWithoutComparisonOperators_ComparerWithEquals_,
                                                                   decltype (msFactory)> (msFactory));
        }

        {
            DoTestForConcreteContainer_<MultiSet_LinkedList<size_t>> ();
            DoTestForConcreteContainer_<MultiSet_LinkedList<SimpleClass>> ();
            auto msFactory = [] () {
                return MultiSet_LinkedList<SimpleClassWithoutComparisonOperators>{MySimpleClassWithoutComparisonOperators_ComparerWithEquals_{}};
            };
            DoTestForConcreteContainer_<MultiSet_LinkedList<SimpleClassWithoutComparisonOperators>> (
                CommonTests::MultiSetTests::DEFAULT_TESTING_SCHEMA<MultiSet<SimpleClassWithoutComparisonOperators>, MySimpleClassWithoutComparisonOperators_ComparerWithEquals_,
                                                                   decltype (msFactory)> (msFactory));
        }

        {
            DoTestForConcreteContainer_<MultiSet_stdmap<size_t>> ();
            DoTestForConcreteContainer_<MultiSet_stdmap<SimpleClass>> ();
            auto msFactory = [] () {
                return MultiSet_stdmap<SimpleClassWithoutComparisonOperators>{MySimpleClassWithoutComparisonOperators_ComparerWithLess_{}};
            };
            DoTestForConcreteContainer_<MultiSet_stdmap<SimpleClassWithoutComparisonOperators>> (
                CommonTests::MultiSetTests::DEFAULT_TESTING_SCHEMA<MultiSet_stdmap<SimpleClassWithoutComparisonOperators>, MySimpleClassWithoutComparisonOperators_ComparerWithEquals_,
                                                                   decltype (msFactory)> (msFactory));
        }

        ExampleCTORS_Test_2_::DoTest ();
        Top_Test_3_::DoTest ();

        EXPECT_TRUE (SimpleClass::GetTotalLiveCount () == 0 and SimpleClassWithoutComparisonOperators::GetTotalLiveCount () == 0); // simple portable leak check
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
