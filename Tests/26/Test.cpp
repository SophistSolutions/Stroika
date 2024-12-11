/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
//  TEST    Foundation::Containers::SortedMultiSet
//      STATUS  very minimal/incomplete
#include "Stroika/Foundation/StroikaPreComp.h"

#include <iostream>

#include "Stroika/Foundation/Containers/Concrete/SortedMultiSet_SkipList.h"
#include "Stroika/Foundation/Containers/Concrete/SortedMultiSet_stdmap.h"
#include "Stroika/Foundation/Containers/SortedMultiSet.h"
#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/Debug/Visualizations.h"

#include "Stroika/Frameworks/Test/ArchtypeClasses.h"
#include "Stroika/Frameworks/Test/TestHarness.h"

#include "../TestCommon/CommonTests_MultiSet.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Containers;

using namespace Stroika::Frameworks;

using Test::ArchtypeClasses::AsIntsEqualsComparer;
using Test::ArchtypeClasses::AsIntsLessComparer;
using Test::ArchtypeClasses::AsIntsThreeWayComparer;
using Test::ArchtypeClasses::OnlyCopyableMoveable;
using Test::ArchtypeClasses::OnlyCopyableMoveableAndTotallyOrdered;

using Concrete::SortedMultiSet_SkipList;
using Concrete::SortedMultiSet_stdmap;

#if qStroika_HasComponent_googletest
namespace {
    template <typename CONCRETE_CONTAINER>
    struct UseBasicTestingSchemas_ : CommonTests::MultiSetTests::DEFAULT_TESTING_SCHEMA<CONCRETE_CONTAINER> {
        static void ApplyToContainerExtraTest (const typename CONCRETE_CONTAINER::ArchetypeContainerType& t)
        {
            using MultiSetOfElementType = typename CONCRETE_CONTAINER::MultiSetOfElementType;
            // verify in sorted order

            //            EXPECT_TRUE (t.IsOrderedBy (m.GetKeyInOrderComparer ())); @todo something more like this... but only compare values..

            optional<MultiSetOfElementType> last;
            using COMPARER_TYPE = less<MultiSetOfElementType>;
            for (CountedValue<MultiSetOfElementType> i : t) {
                if (last.has_value ()) {
                    EXPECT_TRUE (COMPARER_TYPE{}(*last, i.fValue));
                }
                last = i.fValue;
            }
        }
    };

    template <typename CONCRETE_CONTAINER, typename SCHEMA = UseBasicTestingSchemas_<CONCRETE_CONTAINER>>
    void DoTestForConcreteContainer_ (const SCHEMA& schema = {})
    {
        Debug::TraceContextBumper ctx{"{}::DoTestForConcreteContainer_"};
        CommonTests::MultiSetTests::All_For_Type (schema);
    }
}

namespace {
    GTEST_TEST (Foundation_Containers_SortedMultiSet, FACTORY_DEFAULT)
    {
        DoTestForConcreteContainer_<SortedMultiSet<size_t>> ();
        DoTestForConcreteContainer_<SortedMultiSet<OnlyCopyableMoveableAndTotallyOrdered>> ();
        auto msFactory1 = [] () { return SortedMultiSet<OnlyCopyableMoveable>{AsIntsLessComparer<OnlyCopyableMoveable>{}}; };
        DoTestForConcreteContainer_<SortedMultiSet<OnlyCopyableMoveable>> (
            CommonTests::MultiSetTests::DEFAULT_TESTING_SCHEMA<SortedMultiSet<OnlyCopyableMoveable>, AsIntsLessComparer<OnlyCopyableMoveable>, decltype (msFactory1)> (
                msFactory1));
        auto msFactory2 = [] () { return SortedMultiSet<OnlyCopyableMoveable>{AsIntsThreeWayComparer<OnlyCopyableMoveable>{}}; };
        DoTestForConcreteContainer_<SortedMultiSet<OnlyCopyableMoveable>> (
            CommonTests::MultiSetTests::DEFAULT_TESTING_SCHEMA<SortedMultiSet<OnlyCopyableMoveable>, AsIntsLessComparer<OnlyCopyableMoveable>, decltype (msFactory2)> (
                msFactory2));
    }
}

namespace {
    GTEST_TEST (Foundation_Containers_SortedMultiSet, SortedMultiSet_stdmap)
    {
        DoTestForConcreteContainer_<SortedMultiSet_stdmap<size_t>> ();
        DoTestForConcreteContainer_<SortedMultiSet_stdmap<OnlyCopyableMoveableAndTotallyOrdered>> ();
        auto msFactory = [] () { return SortedMultiSet_stdmap<OnlyCopyableMoveable>{AsIntsLessComparer<OnlyCopyableMoveable>{}}; };
        DoTestForConcreteContainer_<SortedMultiSet_stdmap<OnlyCopyableMoveable>> (
            CommonTests::MultiSetTests::DEFAULT_TESTING_SCHEMA<SortedMultiSet<OnlyCopyableMoveable>, AsIntsLessComparer<OnlyCopyableMoveable>, decltype (msFactory)> (
                msFactory));
    }
}

namespace {
    GTEST_TEST (Foundation_Containers_SortedMultiSet, SortedMultiSet_SkipList)
    {
        DoTestForConcreteContainer_<SortedMultiSet_SkipList<size_t>> ();
        DoTestForConcreteContainer_<SortedMultiSet_SkipList<OnlyCopyableMoveableAndTotallyOrdered>> ();
        auto msFactory = [] () { return SortedMultiSet_SkipList<OnlyCopyableMoveable>{AsIntsThreeWayComparer<OnlyCopyableMoveable>{}}; };
        DoTestForConcreteContainer_<SortedMultiSet_SkipList<OnlyCopyableMoveable>> (
            CommonTests::MultiSetTests::DEFAULT_TESTING_SCHEMA<SortedMultiSet<OnlyCopyableMoveable>, AsIntsEqualsComparer<OnlyCopyableMoveable>, decltype (msFactory)> (
                msFactory));
    }
}

namespace {
    GTEST_TEST (Foundation_Containers_SortedMultiSet, CLEANUP)
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
