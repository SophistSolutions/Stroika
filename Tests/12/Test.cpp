/*
* Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
*/
//  TEST    Foundation::Containers::Bijection
#include "Stroika/Foundation/StroikaPreComp.h"

#include <iostream>

#include "Stroika/Foundation/Containers/Bijection.h"
#include "Stroika/Foundation/Containers/Concrete/Bijection_LinkedList.h"
#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/Debug/Visualizations.h"

#include "Stroika/Frameworks/Test/ArchtypeClasses.h"
#include "Stroika/Frameworks/Test/TestHarness.h"

#include "../TestCommon/CommonTests_Bijection.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Containers;

using namespace Stroika::Frameworks;

using Test::ArchtypeClasses::AsIntsEqualsComparer;
using Test::ArchtypeClasses::AsIntsLessComparer;
using Test::ArchtypeClasses::OnlyCopyableMoveable;
using Test::ArchtypeClasses::OnlyCopyableMoveableAndTotallyOrdered;

using Concrete::Bijection_LinkedList;

#if qHasFeature_GoogleTest
namespace {
    template <typename CONCRETE_CONTAINER>
    void RunTests_ ()
    {
        using namespace CommonTests::BijectionTests;
        All_For_Type (DEFAULT_TESTING_SCHEMA<CONCRETE_CONTAINER>{});
        For_TypesWithDefaultFactory (DEFAULT_TESTING_SCHEMA<CONCRETE_CONTAINER>{});
    }
    template <typename CONCRETE_CONTAINER, typename FACTORY>
    void RunTests_ (const FACTORY& factory)
    {
        using namespace CommonTests::BijectionTests;
        All_For_Type (DEFAULT_TESTING_SCHEMA<CONCRETE_CONTAINER, FACTORY> (factory));
    }
}

namespace {
    GTEST_TEST (Foundation_Containers_Bijection, all)
    {
        struct MyComparerWithEquals_ : Common::ComparisonRelationDeclarationBase<Common::ComparisonRelationType::eEquals> {
            using value_type = OnlyCopyableMoveable;
            bool operator() (value_type v1, value_type v2) const
            {
                return static_cast<size_t> (v1) == static_cast<size_t> (v2);
            }
        };

        RunTests_<Bijection<size_t, size_t>> ();
        RunTests_<Bijection<OnlyCopyableMoveableAndTotallyOrdered, OnlyCopyableMoveableAndTotallyOrdered>> ();
        RunTests_<Bijection<OnlyCopyableMoveable, OnlyCopyableMoveable>> ([] () {
            return Bijection<OnlyCopyableMoveable, OnlyCopyableMoveable>{MyComparerWithEquals_{}, MyComparerWithEquals_{}};
        });

        RunTests_<Bijection_LinkedList<size_t, size_t>> ();
        RunTests_<Bijection_LinkedList<OnlyCopyableMoveableAndTotallyOrdered, OnlyCopyableMoveableAndTotallyOrdered>> ();
        RunTests_<Bijection_LinkedList<OnlyCopyableMoveable, OnlyCopyableMoveable>> ([] () {
            return Bijection_LinkedList<OnlyCopyableMoveable, OnlyCopyableMoveable>{MyComparerWithEquals_{}, MyComparerWithEquals_{}};
        });

        EXPECT_TRUE (OnlyCopyableMoveableAndTotallyOrdered::GetTotalLiveCount () == 0 and OnlyCopyableMoveable::GetTotalLiveCount () == 0); // simple portable leak check
    }
}
#endif

int main (int argc, const char* argv[])
{
    Stroika::Frameworks::Test::Setup (argc, argv);
#if qHasFeature_GoogleTest
    return RUN_ALL_TESTS ();
#else
    cerr << "Stroika regression tests require building with google test feature [  PASSED  ]" << endl;
#endif
}
