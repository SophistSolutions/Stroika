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
        SimpleMappingTest_WithDefaultEqCompaerer_ (testSchema);
    }
    template <typename CONCRETE_CONTAINER, typename FACTORY, typename VALUE_EQUALS_COMPARER_TYPE>
    void DoTestForConcreteContainer_ (FACTORY factory, VALUE_EQUALS_COMPARER_TYPE valueEqualsComparer)
    {
        using namespace CommonTests::MappingTests;
        auto testSchema                      = DEFAULT_TESTING_SCHEMA<CONCRETE_CONTAINER, FACTORY, VALUE_EQUALS_COMPARER_TYPE>{factory, valueEqualsComparer};
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
    }
}

namespace {
    void DoRegressionTests_ ()
    {
        struct MySimpleClassWithoutComparisonOperators_ComparerWithEquals_ : Common::ComparisonRelationDeclaration<Common::ComparisonRelationType::eEquals> {
            using value_type = SimpleClassWithoutComparisonOperators;
            bool operator() (const value_type& v1, const value_type& v2) const
            {
                return v1.GetValue () == v2.GetValue ();
            }
        };
        struct MySimpleClassWithoutComparisonOperators_ComparerWithLess_ : Common::ComparisonRelationDeclaration<Common::ComparisonRelationType::eStrictInOrder> {
            using value_type = SimpleClassWithoutComparisonOperators;
            bool operator() (const value_type& v1, const value_type& v2) const
            {
                return v1.GetValue () < v2.GetValue ();
            }
        };

        DoTestForConcreteContainer_<SortedMapping<size_t, size_t>> ();
        DoTestForConcreteContainer_<SortedMapping<SimpleClass, SimpleClass>> ();
        DoTestForConcreteContainer_<SortedMapping<SimpleClassWithoutComparisonOperators, SimpleClassWithoutComparisonOperators>> (
            []() { return SortedMapping<SimpleClassWithoutComparisonOperators, SimpleClassWithoutComparisonOperators> (MySimpleClassWithoutComparisonOperators_ComparerWithLess_{}); },
            MySimpleClassWithoutComparisonOperators_ComparerWithEquals_{});

        DoTestForConcreteContainer_<SortedMapping_stdmap<size_t, size_t>> ();
        DoTestForConcreteContainer_<SortedMapping_stdmap<SimpleClass, SimpleClass>> ();
        DoTestForConcreteContainer_<SortedMapping_stdmap<SimpleClassWithoutComparisonOperators, SimpleClassWithoutComparisonOperators>> (
            []() { return SortedMapping_stdmap<SimpleClassWithoutComparisonOperators, SimpleClassWithoutComparisonOperators> (MySimpleClassWithoutComparisonOperators_ComparerWithLess_{}); },
            MySimpleClassWithoutComparisonOperators_ComparerWithEquals_{});
    }
}

int main ([[maybe_unused]] int argc, [[maybe_unused]] const char* argv[])
{
    Stroika::TestHarness::Setup ();
    return Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
}
