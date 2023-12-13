/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
//  TEST    Foundation::Containers::SortedMultiSet
//      STATUS  very minimal/incomplete
#include "Stroika/Foundation/StroikaPreComp.h"

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

using Test::ArchtypeClasses::SimpleClass;
using Test::ArchtypeClasses::SimpleClassWithoutComparisonOperators;

using Concrete::SortedMultiSet_stdmap;

namespace {
    template <typename CONCRETE_CONTAINER>
    struct UseBasicTestingSchemas_ : CommonTests::MultiSetTests::DEFAULT_TESTING_SCHEMA<CONCRETE_CONTAINER> {
        static void ApplyToContainerExtraTest (const typename CONCRETE_CONTAINER::ArchetypeContainerType& t)
        {
            using MultiSetOfElementType = typename CONCRETE_CONTAINER::MultiSetOfElementType;
            // verify in sorted order
            optional<MultiSetOfElementType> last;
            using COMPARER_TYPE = less<MultiSetOfElementType>;
            for (CountedValue<MultiSetOfElementType> i : t) {
                if (last.has_value ()) {
                    VerifyTestResult (COMPARER_TYPE{}(*last, i.fValue));
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
#if qHasFeature_GoogleTest
    GTEST_TEST (Foundation_Caching, all)
#else
    void DoRegressionTests_ ()
#endif
    {
        struct MySimpleClassWithoutComparisonOperators_ComparerWithLess_
            : Common::ComparisonRelationDeclarationBase<Common::ComparisonRelationType::eStrictInOrder> {
            bool operator() (const SimpleClassWithoutComparisonOperators& lhs, const SimpleClassWithoutComparisonOperators& rhs) const
            {
                return lhs.GetValue () < rhs.GetValue ();
            }
        };

        {
            DoTestForConcreteContainer_<SortedMultiSet<size_t>> ();
            DoTestForConcreteContainer_<SortedMultiSet<SimpleClass>> ();
            auto msFactory = [] () {
                return SortedMultiSet<SimpleClassWithoutComparisonOperators>{MySimpleClassWithoutComparisonOperators_ComparerWithLess_{}};
            };
            DoTestForConcreteContainer_<SortedMultiSet<SimpleClassWithoutComparisonOperators>> (
                CommonTests::MultiSetTests::DEFAULT_TESTING_SCHEMA<SortedMultiSet<SimpleClassWithoutComparisonOperators>, MySimpleClassWithoutComparisonOperators_ComparerWithLess_,
                                                                   decltype (msFactory)> (msFactory));
        }

        {
            DoTestForConcreteContainer_<SortedMultiSet_stdmap<size_t>> ();
            DoTestForConcreteContainer_<SortedMultiSet_stdmap<SimpleClass>> ();
            auto msFactory = [] () {
                return SortedMultiSet_stdmap<SimpleClassWithoutComparisonOperators>{MySimpleClassWithoutComparisonOperators_ComparerWithLess_{}};
            };
            DoTestForConcreteContainer_<SortedMultiSet_stdmap<SimpleClassWithoutComparisonOperators>> (
                CommonTests::MultiSetTests::DEFAULT_TESTING_SCHEMA<SortedMultiSet<SimpleClassWithoutComparisonOperators>, MySimpleClassWithoutComparisonOperators_ComparerWithLess_,
                                                                   decltype (msFactory)> (msFactory));
        }

        VerifyTestResult (SimpleClass::GetTotalLiveCount () == 0 and SimpleClassWithoutComparisonOperators::GetTotalLiveCount () == 0); // simple portable leak check
    }
}

int main (int argc, const char* argv[])
{
    Test::Setup (argc, argv);
#if qHasFeature_GoogleTest
    return RUN_ALL_TESTS ();
#else
    return Stroika::Frameworks::Test::PrintPassOrFail (DoRegressionTests_);
#endif
}
