/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
//  TEST    Foundation::Containers::SortedMultiSet
//      STATUS  very minimal/incomplete
#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Containers/Concrete/SortedMultiSet_stdmap.h"
#include "Stroika/Foundation/Containers/SortedMultiSet.h"
#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/Memory/Optional.h"

#include "../TestCommon/CommonTests_MultiSet.h"
#include "../TestHarness/SimpleClass.h"
#include "../TestHarness/TestHarness.h"

#include "../TestCommon/CommonTests_MultiSet.h"
#include "../TestHarness/SimpleClass.h"
#include "../TestHarness/TestHarness.h"

using namespace Stroika;
using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Containers;

using Concrete::SortedMultiSet_stdmap;

namespace {
    template <typename CONCRETE_CONTAINER>
    void DoTestForConcreteContainer_ ()
    {
        using TraitsType            = typename CONCRETE_CONTAINER::TraitsType;
        using MultiSetOfElementType = typename CONCRETE_CONTAINER::MultiSetOfElementType;
        auto extraChecksFunction    = [](const typename CONCRETE_CONTAINER::ArchetypeContainerType& t) {
            // verify in sorted order
            Memory::Optional<MultiSetOfElementType> last;
            for (CountedValue<MultiSetOfElementType> i : t) {
                if (last.IsPresent ()) {
                    VerifyTestResult (TraitsType::WellOrderCompareFunctionType::Compare (*last, i.fValue) <= 0);
                }
                last = i.fValue;
            }
        };
        CommonTests::MultiSetTests::All_For_Type<CONCRETE_CONTAINER> (extraChecksFunction);
    }
}

namespace {

    void DoRegressionTests_ ()
    {
        struct MySimpleClassWithoutComparisonOperators_ComparerWithComparer_ {
            using value_type = SimpleClassWithoutComparisonOperators;
            static bool Equals (value_type v1, value_type v2)
            {
                return v1.GetValue () == v2.GetValue ();
            }
            static int Compare (value_type v1, value_type v2)
            {
                return Common::CompareNormalizer (v1.GetValue (), v2.GetValue ());
            }
        };
        using SimpleClassWithoutComparisonOperators_SortedMultiSetTRAITS = DefaultTraits::SortedMultiSet<
            SimpleClassWithoutComparisonOperators,
            MySimpleClassWithoutComparisonOperators_ComparerWithComparer_>;

        DoTestForConcreteContainer_<SortedMultiSet<size_t>> ();
        DoTestForConcreteContainer_<SortedMultiSet<SimpleClass>> ();
        DoTestForConcreteContainer_<SortedMultiSet<SimpleClassWithoutComparisonOperators, SimpleClassWithoutComparisonOperators_SortedMultiSetTRAITS>> ();

        DoTestForConcreteContainer_<SortedMultiSet_stdmap<size_t>> ();
        DoTestForConcreteContainer_<SortedMultiSet_stdmap<SimpleClass>> ();
        DoTestForConcreteContainer_<SortedMultiSet_stdmap<SimpleClassWithoutComparisonOperators, SimpleClassWithoutComparisonOperators_SortedMultiSetTRAITS>> ();
    }
}

int main (int argc, const char* argv[])
{
    Stroika::TestHarness::Setup ();
    Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
    return EXIT_SUCCESS;
}
