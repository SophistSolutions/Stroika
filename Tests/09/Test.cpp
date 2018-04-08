/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
//  TEST    Foundation::Containers::Association
//      STATUS  Alpha-Late
#include "Stroika/Foundation/StroikaPreComp.h"

#include <iostream>
#include <sstream>

#include "Stroika/Foundation/Containers/Association.h"
#include "Stroika/Foundation/Containers/Concrete/Association_Array.h"
#include "Stroika/Foundation/Containers/Concrete/Association_LinkedList.h"
#include "Stroika/Foundation/Containers/Concrete/Association_stdmultimap.h"
//#include    "Stroika/Foundation/Containers/Concrete/SortedAssociation_stdmap.h"
#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/Trace.h"

#include "../TestCommon/CommonTests_Association.h"
#include "../TestHarness/SimpleClass.h"
#include "../TestHarness/TestHarness.h"

using namespace Stroika;
using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Containers;

using Concrete::Association_Array;
using Concrete::Association_LinkedList;
using Concrete::Association_stdmultimap;

namespace {
    template <typename CONCRETE_CONTAINER>
    void DoTestForConcreteContainer_AllTestsWhichDontRequireComparer_For_Type_ ()
    {
        auto extraChecksFunction = [](const typename CONCRETE_CONTAINER::ArchetypeContainerType& m) {
            // only work todo on sorted Associations
        };
        CommonTests::AssociationTests::SimpleAssociationTest_AllTestsWhichDontRequireComparer_For_Type_<CONCRETE_CONTAINER> (extraChecksFunction);
    }
    template <typename CONCRETE_CONTAINER>
    void DoTestForConcreteContainer_ ()
    {
        auto extraChecksFunction = [](const typename CONCRETE_CONTAINER::ArchetypeContainerType& m) {
            // only work todo on sorted Associations
        };
        CommonTests::AssociationTests::SimpleAssociationTest_All_For_Type<CONCRETE_CONTAINER> (extraChecksFunction);
    }
}

namespace {
    void Test2_SimpleBaseClassConversionTraitsConfusion_ ()
    {
#if 0
/// @todo sorted assoc NYI
        SortedAssociation<int, float> xxxyy = Concrete::SortedAssociation_stdmap<int, float> ();
        Association<int, float> xxxyy1 = Concrete::Association_stdmap<int, float> ();
#endif
    }
}

namespace {
    template <typename CONTAINER, typename COMPARER>
    void doIt_t3_ ()
    {
        CommonTests::AssociationTests::SimpleAssociationTest_WhichRequiresExplcitValueComparer<CONTAINER, COMPARER> ([](const CONTAINER& c) {});
    }
    void Test3_SimpleAssociationTest_WhichRequiresExplcitValueComparer ()
    {
        doIt_t3_<Association_LinkedList<size_t, size_t>, std::equal_to<size_t>> ();
    }
}

namespace {
    void DoRegressionTests_ ()
    {
#if 0
        struct MySimpleClassWithoutComparisonOperators_ComparerWithEquals_ {
            using value_type = SimpleClassWithoutComparisonOperators;
            static bool Equals (value_type v1, value_type v2)
            {
                return v1.GetValue () == v2.GetValue ();
            }
        };
        using SimpleClassWithoutComparisonOperators_AssociationTRAITS = DefaultTraits::Association<
            SimpleClassWithoutComparisonOperators,
            SimpleClassWithoutComparisonOperators,
            MySimpleClassWithoutComparisonOperators_ComparerWithEquals_>;

        DoTestForConcreteContainer_<Association<size_t, size_t>> ();
        DoTestForConcreteContainer_<Association<SimpleClass, SimpleClass>> ();
        DoTestForConcreteContainer_AllTestsWhichDontRequireComparer_For_Type_<Association<SimpleClassWithoutComparisonOperators, SimpleClassWithoutComparisonOperators, SimpleClassWithoutComparisonOperators_AssociationTRAITS>> ();

        DoTestForConcreteContainer_<Association_Array<size_t, size_t>> ();
        DoTestForConcreteContainer_<Association_Array<SimpleClass, SimpleClass>> ();
        DoTestForConcreteContainer_AllTestsWhichDontRequireComparer_For_Type_<Association_Array<SimpleClassWithoutComparisonOperators, SimpleClassWithoutComparisonOperators, SimpleClassWithoutComparisonOperators_AssociationTRAITS>> ();

        DoTestForConcreteContainer_<Association_LinkedList<size_t, size_t>> ();
        DoTestForConcreteContainer_<Association_LinkedList<SimpleClass, SimpleClass>> ();
        DoTestForConcreteContainer_AllTestsWhichDontRequireComparer_For_Type_<Association_LinkedList<SimpleClassWithoutComparisonOperators, SimpleClassWithoutComparisonOperators, SimpleClassWithoutComparisonOperators_AssociationTRAITS>> ();

        DoTestForConcreteContainer_<Association_stdmultimap<size_t, size_t>> ();
        DoTestForConcreteContainer_<Association_stdmultimap<SimpleClass, SimpleClass>> ();
        {
            struct MySimpleClassWithoutComparisonOperators_ComparerWithCompare_ : MySimpleClassWithoutComparisonOperators_ComparerWithEquals_ {
                using value_type = SimpleClassWithoutComparisonOperators;
                static int Compare (value_type v1, value_type v2)
                {
                    return Common::ThreeWayCompareNormalizer (v1.GetValue (), v2.GetValue ());
                }
            };
            using SimpleClassWithoutComparisonOperators_Association_stdmultimap_TRAITS =
                Concrete::Association_stdmultimap_DefaultTraits<
                    SimpleClassWithoutComparisonOperators,
                    SimpleClassWithoutComparisonOperators,
                    MySimpleClassWithoutComparisonOperators_ComparerWithEquals_,
                    MySimpleClassWithoutComparisonOperators_ComparerWithCompare_>;
            DoTestForConcreteContainer_AllTestsWhichDontRequireComparer_For_Type_<Association_stdmultimap<SimpleClassWithoutComparisonOperators, SimpleClassWithoutComparisonOperators, SimpleClassWithoutComparisonOperators_Association_stdmultimap_TRAITS>> ();
        }

        Test2_SimpleBaseClassConversionTraitsConfusion_ ();

        Test3_SimpleAssociationTest_WhichRequiresExplcitValueComparer ();
#endif
    }
}

int main (int argc, const char* argv[])
{
    Stroika::TestHarness::Setup ();
    return Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
}
