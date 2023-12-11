/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
//  TEST    Foundation::Containers::SortedAssociation
//      STATUS  Alpha-Late
#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Containers/Concrete/SortedAssociation_stdmultimap.h"
#include "Stroika/Foundation/Containers/SortedAssociation.h"
#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/Debug/Visualizations.h"
#include "Stroika/Foundation/Memory/Optional.h"

#include "Stroika/Frameworks/Test/ArchtypeClasses.h"
#include "Stroika/Frameworks/Test/TestHarness.h"

#include "../TestCommon/CommonTests_Association.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Containers;

using namespace Stroika::Frameworks;

using Test::ArchtypeClasses::SimpleClass;
using Test::ArchtypeClasses::SimpleClassWithoutComparisonOperators;

using Concrete::SortedAssociation_stdmultimap;

namespace {
    template <typename CONCRETE_CONTAINER>
    void DoTestForConcreteContainer_ ()
    {
        using namespace CommonTests::AssociationTests;
        auto testSchema                      = DEFAULT_TESTING_SCHEMA<CONCRETE_CONTAINER>{};
        testSchema.ApplyToContainerExtraTest = [] (const typename CONCRETE_CONTAINER::ArchetypeContainerType& m) {
            // verify in sorted order
            using value_type = typename CONCRETE_CONTAINER::value_type;
            optional<value_type> last;
            for (value_type i : m) {
                if (last.has_value ()) {
                    VerifyTestResult (Common::ThreeWayComparerAdapter{m.GetInOrderKeyComparer ()}(last->fKey, i.fKey) <= 0);
                }
                last = i;
            }
        };
        SimpleAssociationTest_All_ (testSchema);
        SimpleAssociationTest_WithDefaultEqCompaerer_ (testSchema);
    }
    template <typename CONCRETE_CONTAINER, typename FACTORY, typename VALUE_EQUALS_COMPARER_TYPE>
    void DoTestForConcreteContainer_ (FACTORY factory, VALUE_EQUALS_COMPARER_TYPE valueEqualsComparer)
    {
        using namespace CommonTests::AssociationTests;
        auto testSchema = DEFAULT_TESTING_SCHEMA<CONCRETE_CONTAINER, FACTORY, VALUE_EQUALS_COMPARER_TYPE>{factory, valueEqualsComparer};
        testSchema.ApplyToContainerExtraTest = [] (const typename CONCRETE_CONTAINER::ArchetypeContainerType& m) {
            // verify in sorted order
            using value_type = typename CONCRETE_CONTAINER::value_type;
            optional<value_type> last;
            for (value_type i : m) {
                if (last.has_value ()) {
                    VerifyTestResult (Common::ThreeWayComparerAdapter{m.GetInOrderKeyComparer ()}(last->fKey, i.fKey) <= 0);
                }
                last = i;
            }
        };
        SimpleAssociationTest_All_ (testSchema);
    }
}

namespace {
    namespace Test3_ConvertAssociation2SortedAssociation {
        void TestAll ()
        {
            Association<int, int>       m{pair<int, int>{1, 2}, pair<int, int>{2, 4}};
            SortedAssociation<int, int> ms{m};
            VerifyTestResult (ms.size () == 2);
            VerifyTestResult ((*ms.begin () == pair<int, int>{1, 2}));
        }
    }
}

namespace {
    void DoRegressionTests_ ()
    {
        struct MySimpleClassWithoutComparisonOperators_ComparerWithEquals_
            : Common::ComparisonRelationDeclarationBase<Common::ComparisonRelationType::eEquals> {
            using value_type = SimpleClassWithoutComparisonOperators;
            bool operator() (const value_type& v1, const value_type& v2) const
            {
                return v1.GetValue () == v2.GetValue ();
            }
        };
        struct MySimpleClassWithoutComparisonOperators_ComparerWithLess_
            : Common::ComparisonRelationDeclarationBase<Common::ComparisonRelationType::eStrictInOrder> {
            using value_type = SimpleClassWithoutComparisonOperators;
            bool operator() (const value_type& v1, const value_type& v2) const
            {
                return v1.GetValue () < v2.GetValue ();
            }
        };

        DoTestForConcreteContainer_<SortedAssociation<size_t, size_t>> ();
        DoTestForConcreteContainer_<SortedAssociation<SimpleClass, SimpleClass>> ();
        DoTestForConcreteContainer_<SortedAssociation<SimpleClassWithoutComparisonOperators, SimpleClassWithoutComparisonOperators>> (
            [] () {
                return SortedAssociation<SimpleClassWithoutComparisonOperators, SimpleClassWithoutComparisonOperators> (
                    MySimpleClassWithoutComparisonOperators_ComparerWithLess_{});
            },
            MySimpleClassWithoutComparisonOperators_ComparerWithEquals_{});

        DoTestForConcreteContainer_<SortedAssociation_stdmultimap<size_t, size_t>> ();
        DoTestForConcreteContainer_<SortedAssociation_stdmultimap<SimpleClass, SimpleClass>> ();
        DoTestForConcreteContainer_<SortedAssociation_stdmultimap<SimpleClassWithoutComparisonOperators, SimpleClassWithoutComparisonOperators>> (
            [] () {
                return SortedAssociation_stdmultimap<SimpleClassWithoutComparisonOperators, SimpleClassWithoutComparisonOperators> (
                    MySimpleClassWithoutComparisonOperators_ComparerWithLess_{});
            },
            MySimpleClassWithoutComparisonOperators_ComparerWithEquals_{});

        Test3_ConvertAssociation2SortedAssociation::TestAll ();

        VerifyTestResult (SimpleClass::GetTotalLiveCount () == 0 and SimpleClassWithoutComparisonOperators::GetTotalLiveCount () == 0); // simple portable leak check
    }
}

int main (int argc, const char* argv[])
{
    Test::Setup (argc, argv);
    return Test::PrintPassOrFail (DoRegressionTests_);
}
