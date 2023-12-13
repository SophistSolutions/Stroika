/*
* Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
*/
//  TEST    Foundation::Containers::Bijection
#include "Stroika/Foundation/StroikaPreComp.h"

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

using Test::ArchtypeClasses::SimpleClass;
using Test::ArchtypeClasses::SimpleClassWithoutComparisonOperators;

using Concrete::Bijection_LinkedList;

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
#if qHasFeature_GoogleTest
    GTEST_TEST (Foundation_Containers_Bijection, all)
#else
    void DoRegressionTests_ ()
#endif
    {
        struct MySimpleClassWithoutComparisonOperators_ComparerWithEquals_
            : Common::ComparisonRelationDeclarationBase<Common::ComparisonRelationType::eEquals> {
            using value_type = SimpleClassWithoutComparisonOperators;
            bool operator() (value_type v1, value_type v2) const
            {
                return v1.GetValue () == v2.GetValue ();
            }
        };

        RunTests_<Bijection<size_t, size_t>> ();
        RunTests_<Bijection<SimpleClass, SimpleClass>> ();
        RunTests_<Bijection<SimpleClassWithoutComparisonOperators, SimpleClassWithoutComparisonOperators>> ([] () {
            return Bijection<SimpleClassWithoutComparisonOperators, SimpleClassWithoutComparisonOperators>{
                MySimpleClassWithoutComparisonOperators_ComparerWithEquals_{}, MySimpleClassWithoutComparisonOperators_ComparerWithEquals_{}};
        });

        RunTests_<Bijection_LinkedList<size_t, size_t>> ();
        RunTests_<Bijection_LinkedList<SimpleClass, SimpleClass>> ();
        RunTests_<Bijection_LinkedList<SimpleClassWithoutComparisonOperators, SimpleClassWithoutComparisonOperators>> ([] () {
            return Bijection_LinkedList<SimpleClassWithoutComparisonOperators, SimpleClassWithoutComparisonOperators>{
                MySimpleClassWithoutComparisonOperators_ComparerWithEquals_{}, MySimpleClassWithoutComparisonOperators_ComparerWithEquals_{}};
        });

        EXPECT_TRUE (SimpleClass::GetTotalLiveCount () == 0 and SimpleClassWithoutComparisonOperators::GetTotalLiveCount () == 0); // simple portable leak check
    }
}

int main (int argc, const char* argv[])
{
    Stroika::Frameworks::Test::Setup (argc, argv);
#if qHasFeature_GoogleTest
    return RUN_ALL_TESTS ();
#else
    cerr << "Stroika regression tests require building with google test feature" << endl;
#endif
}
