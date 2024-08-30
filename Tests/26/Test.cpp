/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
//  TEST    Foundation::Containers::SortedMapping
//      \note Code-Status:  <a href="Code-Status.md#Beta">Beta</a>
#include "Stroika/Foundation/StroikaPreComp.h"

#include <iostream>
#include <sstream>

#include "Stroika/Foundation/Containers/Concrete/SortedMapping_stdmap.h"
#include "Stroika/Foundation/Containers/SortedMapping.h"
#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/Debug/Visualizations.h"

#include "Stroika/Frameworks/Test/ArchtypeClasses.h"
#include "Stroika/Frameworks/Test/TestHarness.h"

#include "../TestCommon/CommonTests_Mapping.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Containers;

using namespace Stroika::Frameworks;

using Test::ArchtypeClasses::AsIntsEqualsComparer;
using Test::ArchtypeClasses::AsIntsLessComparer;
using Test::ArchtypeClasses::OnlyCopyableMoveable;
using Test::ArchtypeClasses::OnlyCopyableMoveableAndTotallyOrdered;

using Concrete::SortedMapping_stdmap;

#if qHasFeature_GoogleTest
namespace {
    template <typename CONCRETE_CONTAINER>
    void DoTestForConcreteContainer_ ()
    {
        using namespace CommonTests::MappingTests;
        if constexpr (constructible_from<CONCRETE_CONTAINER>) {
            auto testSchema                      = DEFAULT_TESTING_SCHEMA<CONCRETE_CONTAINER>{};
            testSchema.ApplyToContainerExtraTest = [] (const typename CONCRETE_CONTAINER::ArchetypeContainerType& m) {
                // verify in sorted order
                using key_type         = typename CONCRETE_CONTAINER::key_type;
                using value_type       = typename CONCRETE_CONTAINER::value_type;
                using INORDER_COMPARER = decltype (m.GetInOrderKeyComparer ());
                optional<value_type> last;
                for (value_type i : m) {
                    if (last.has_value ()) {
                        EXPECT_TRUE ((Common::ThreeWayComparerAdapter<key_type, INORDER_COMPARER>{m.GetInOrderKeyComparer ()}(last->fKey, i.fKey) <= 0));
                    }
                    last = i;
                }
            };
            SimpleMappingTest_All_ (testSchema);
            SimpleMappingTest_WithDefaultEqCompaerer_ (testSchema);
        }
    }
    template <typename CONCRETE_CONTAINER, typename FACTORY, typename VALUE_EQUALS_COMPARER_TYPE>
    void DoTestForConcreteContainer_ (FACTORY factory, VALUE_EQUALS_COMPARER_TYPE valueEqualsComparer)
    {
        using namespace CommonTests::MappingTests;
        auto testSchema = DEFAULT_TESTING_SCHEMA<CONCRETE_CONTAINER, FACTORY, VALUE_EQUALS_COMPARER_TYPE>{factory, valueEqualsComparer};
        testSchema.ApplyToContainerExtraTest = [] (const typename CONCRETE_CONTAINER::ArchetypeContainerType& m) {
            // verify in sorted order
            using value_type       = typename CONCRETE_CONTAINER::value_type;
            using key_type         = typename CONCRETE_CONTAINER::key_type;
            using INORDER_COMPARER = decltype (m.GetInOrderKeyComparer ());
            optional<value_type> last;
            for (value_type i : m) {
                if (last.has_value ()) {
                    EXPECT_TRUE ((Common::ThreeWayComparerAdapter<key_type, INORDER_COMPARER>{m.GetInOrderKeyComparer ()}(last->fKey, i.fKey) <= 0));
                }
                last = i;
            }
        };
        SimpleMappingTest_All_ (testSchema);
    }
}

namespace {
    namespace Test3_ConvertMapping2SortedMapping {
        void TestAll ()
        {
            Mapping<int, int>       m{pair<int, int>{1, 2}, pair<int, int>{2, 4}};
            SortedMapping<int, int> ms{m};
            EXPECT_EQ (ms.size (), 2u);
            EXPECT_EQ (*ms.begin (), (pair<int, int>{1, 2}));
        }
    }
}

namespace {
    GTEST_TEST (Foundation_Containers_SortedMapping, all)
    {
        using ComparerWithEquals_ = AsIntsEqualsComparer<OnlyCopyableMoveable>;
        using ComparerWithLess_   = AsIntsLessComparer<OnlyCopyableMoveable>;

        DoTestForConcreteContainer_<SortedMapping<size_t, size_t>> ();
        DoTestForConcreteContainer_<SortedMapping<OnlyCopyableMoveableAndTotallyOrdered, OnlyCopyableMoveableAndTotallyOrdered>> ();
        DoTestForConcreteContainer_<SortedMapping<OnlyCopyableMoveable, OnlyCopyableMoveable>> (
            [] () { return SortedMapping<OnlyCopyableMoveable, OnlyCopyableMoveable> (ComparerWithLess_{}); }, ComparerWithEquals_{});

        DoTestForConcreteContainer_<SortedMapping_stdmap<size_t, size_t>> ();
        DoTestForConcreteContainer_<SortedMapping_stdmap<OnlyCopyableMoveableAndTotallyOrdered, OnlyCopyableMoveableAndTotallyOrdered>> ();
        DoTestForConcreteContainer_<SortedMapping_stdmap<OnlyCopyableMoveable, OnlyCopyableMoveable>> (
            [] () { return SortedMapping_stdmap<OnlyCopyableMoveable, OnlyCopyableMoveable> (ComparerWithLess_{}); }, ComparerWithEquals_{});

        Test3_ConvertMapping2SortedMapping::TestAll ();

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
