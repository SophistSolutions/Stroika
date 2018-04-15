/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
//  TEST    Foundation::Containers::SortedMapping
//      STATUS  Alpha-Late
#include "Stroika/Foundation/StroikaPreComp.h"

#include <iostream>
#include <sstream>

#include "Stroika/Foundation/Containers/Concrete/SortedMapping_stdmap.h"
#include "Stroika/Foundation/Containers/SortedMapping.h"
#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/Memory/Optional.h"

#include "../TestCommon/CommonTests_Mapping.h"
#include "../TestHarness/SimpleClass.h"
#include "../TestHarness/TestHarness.h"

using namespace Stroika;
using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Containers;

using Memory::Optional;

using Concrete::SortedMapping_stdmap;

namespace {
    template <typename CONCRETE_CONTAINER>
    void DoTestForConcreteContainer_AllTestsWhichDontRequireComparer_For_Type_ ()
    {
#if 0
        using value_type         = typename CONCRETE_CONTAINER::value_type;
        using TraitsType         = typename CONCRETE_CONTAINER::TraitsType;
        auto extraChecksFunction = [](const typename CONCRETE_CONTAINER::ArchetypeContainerType& m) {
            // verify in sorted order
            Optional<value_type> last;
            for (value_type i : m) {
                if (last.IsPresent ()) {
                    VerifyTestResult (Common::mkThreeWayComparerAdapter (m.GetInOrderKeyComparer ()) (last->fKey, i.fKey) <= 0);
                }
                last = i;
            }
        };
        CommonTests::MappingTests::SimpleMappingTest_All_<CONCRETE_CONTAINER> (extraChecksFunction);
#endif
    }
    template <typename CONCRETE_CONTAINER>
    void DoTestForConcreteContainer_ ()
    {
        using namespace CommonTests::MappingTests;
        auto testSchema                      = DEFAULT_TESTING_SCHEMA<CONCRETE_CONTAINER>{};
        testSchema.ApplyToContainerExtraTest = [](const typename CONCRETE_CONTAINER::ArchetypeContainerType& m) {
            // verify in sorted order
            using value_type = typename CONCRETE_CONTAINER::value_type;
            Optional<value_type> last;
            for (value_type i : m) {
                if (last.IsPresent ()) {
                    VerifyTestResult (Common::mkThreeWayComparerAdapter (m.GetInOrderKeyComparer ()) (last->fKey, i.fKey) <= 0);
                }
                last = i;
            }
        };
        SimpleMappingTest_All_ (testSchema);
#if 0
        using value_type         = typename CONCRETE_CONTAINER::value_type;
        auto extraChecksFunction = [](const typename CONCRETE_CONTAINER::ArchetypeContainerType& m) {
            // verify in sorted order
            Optional<value_type> last;
            for (value_type i : m) {
                if (last.IsPresent ()) {
                    VerifyTestResult (Common::mkThreeWayComparerAdapter (m.GetInOrderKeyComparer ()) (last->fKey, i.fKey) <= 0);
                }
                last = i;
            }
        };
        CommonTests::MappingTests::SimpleMappingTest_All_<CONCRETE_CONTAINER> (extraChecksFunction);
#endif
    }
}

namespace {
    void DoRegressionTests_ ()
    {
        struct MySimpleClassWithoutComparisonOperators_ComparerWithEquals_ {
            using value_type = SimpleClassWithoutComparisonOperators;
            static bool Equals (value_type v1, value_type v2)
            {
                return v1.GetValue () == v2.GetValue ();
            }
            static int Compare (value_type v1, value_type v2)
            {
                return Common::ThreeWayCompareNormalizer (v1.GetValue (), v2.GetValue ());
            }
        };
        struct MySimpleClassWithoutComparisonOperators_ComparerWithCompare_ : MySimpleClassWithoutComparisonOperators_ComparerWithEquals_ {
            using value_type = SimpleClassWithoutComparisonOperators;
            static int Compare (value_type v1, value_type v2)
            {
                return Common::ThreeWayCompareNormalizer (v1.GetValue (), v2.GetValue ());
            }
        };
#if 0
        // @todo FIX this case
        using SimpleClassWithoutComparisonOperators_MappingTRAITS = DefaultTraits::SortedMapping<
            SimpleClassWithoutComparisonOperators,
            SimpleClassWithoutComparisonOperators,
            MySimpleClassWithoutComparisonOperators_ComparerWithEquals_,
            MySimpleClassWithoutComparisonOperators_ComparerWithCompare_>;
#endif

        DoTestForConcreteContainer_<SortedMapping<size_t, size_t>> ();
        DoTestForConcreteContainer_<SortedMapping<SimpleClass, SimpleClass>> ();
        //DoTestForConcreteContainer_AllTestsWhichDontRequireComparer_For_Type_<SortedMapping<SimpleClassWithoutComparisonOperators, SimpleClassWithoutComparisonOperators, SimpleClassWithoutComparisonOperators_MappingTRAITS>> ();

        DoTestForConcreteContainer_<SortedMapping_stdmap<size_t, size_t>> ();
        DoTestForConcreteContainer_<SortedMapping_stdmap<SimpleClass, SimpleClass>> ();
        // DoTestForConcreteContainer_AllTestsWhichDontRequireComparer_For_Type_<SortedMapping_stdmap<SimpleClassWithoutComparisonOperators, SimpleClassWithoutComparisonOperators, SimpleClassWithoutComparisonOperators_MappingTRAITS>> ();
    }
}

int main (int argc, const char* argv[])
{
    Stroika::TestHarness::Setup ();
    return Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
}
