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
    struct UseBasicTestingSchemas_ : CommonTests::MultiSetTests::DEFAULT_TESTING_SCHEMA<CONCRETE_CONTAINER> {
        static void ApplyToContainerExtraTest (const typename CONCRETE_CONTAINER::ArchetypeContainerType& t)
        {
            // verify in sorted order
            Memory::Optional<MultiSetOfElementType> last;
            using COMPARER_TYPE = less<MultiSetOfElementType>;
            for (CountedValue<MultiSetOfElementType> i : t) {
                if (last.IsPresent ()) {
                    VerifyTestResult (COMPARER_TYPE () (*last, i.fValue));
                }
                last = i.fValue;
            }
        }
    };

    template <typename CONCRETE_CONTAINER, typename SCHEMA = UseBasicTestingSchemas_<CONCRETE_CONTAINER>>
    void DoTestForConcreteContainer_ (const SCHEMA& schema = {})
    {
        Debug::TraceContextBumper ctx{L"{}::DoTestForConcreteContainer_"};
        CommonTests::MultiSetTests::All_For_Type (schema);
    }
}

namespace {

    void DoRegressionTests_ ()
    {
        struct MySimpleClassWithoutComparisonOperators_ComparerWithLess_ : Common::ComparisonTraitsBase<Common::OrderingRelationType::eInOrder> {
            bool operator() (const SimpleClassWithoutComparisonOperators& lhs, const SimpleClassWithoutComparisonOperators& rhs) const
            {
                return lhs.GetValue () < rhs.GetValue ();
            }
        };

        DoTestForConcreteContainer_<SortedMultiSet<size_t>> ();
        DoTestForConcreteContainer_<SortedMultiSet<SimpleClass>> ();
        //RunTests_<SortedMultiSet<SimpleClassWithoutComparisonOperators, SimpleClassWithoutComparisonOperators_SortedMultiSetTRAITS>> ();

        DoTestForConcreteContainer_<SortedMultiSet_stdmap<size_t>> ();
        DoTestForConcreteContainer_<SortedMultiSet_stdmap<SimpleClass>> ();
        //RunTests_<SortedMultiSet_stdmap<SimpleClassWithoutComparisonOperators, SimpleClassWithoutComparisonOperators_SortedMultiSetTRAITS>> ();
    }
}

int main (int argc, const char* argv[])
{
    Stroika::TestHarness::Setup ();
    Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
    return EXIT_SUCCESS;
}
