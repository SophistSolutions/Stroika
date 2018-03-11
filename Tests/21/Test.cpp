/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
//  TEST    Foundation::Containers::Set
//      STATUS  PRELIMINARY
#include "Stroika/Foundation/StroikaPreComp.h"

#include <iostream>
#include <sstream>

#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/Containers/Collection.h"
#include "Stroika/Foundation/Containers/Concrete/Set_LinkedList.h"
#include "Stroika/Foundation/Containers/Concrete/Set_stdset.h"
#include "Stroika/Foundation/Containers/Set.h"
#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/Trace.h"

#include "../TestCommon/CommonTests_Set.h"
#include "../TestHarness/SimpleClass.h"
#include "../TestHarness/TestHarness.h"

using namespace Stroika;
using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Containers;

using Concrete::Set_LinkedList;
using Concrete::Set_stdset;

namespace {
    template <typename CONCRETE_CONTAINER>
    void DoTestForConcreteContainer_ ()
    {
        using value_type         = typename CONCRETE_CONTAINER::value_type;
        using TraitsType         = typename CONCRETE_CONTAINER::TraitsType;
        auto extraChecksFunction = [](const Set<value_type, typename TraitsType::SetTraitsType>& s) {
            // only work todo on sorted sets
        };
        CommonTests::SetTests::Test_All_For_Type<CONCRETE_CONTAINER, Set<value_type, typename TraitsType::SetTraitsType>> (extraChecksFunction);
    }
}

namespace {
    namespace ExampleCTORS_Test_2_ {
        void DoTest ()
        {
            // From Set<> CTOR docs
            Collection<int>  c;
            std::vector<int> v;

            Set<int> s1 = {1, 2, 3};
            Set<int> s2 = s1;
            Set<int> s3{s1};
            Set<int> s4{s1.begin (), s1.end ()};
            Set<int> s5{c};
            Set<int> s6{v};
            Set<int> s7{v.begin (), v.end ()};
            Set<int> s8{move (s1)};
        }
    }
}

namespace {
    namespace Where_Test_3_ {
        void DoAll ()
        {
            Set<int> s{1, 2, 3, 4, 5};
            VerifyTestResult ((s.Where ([](int i) { return Math::IsPrime (i); }) == Set<int>{2, 3, 5}));
        }
    }
}

namespace {
    void DoRegressionTests_ ()
    {
        using namespace CommonTests::SetTests;

        struct MySimpleClassWithoutComparisonOperators_CompareEquals_ {
            using value_type = SimpleClassWithoutComparisonOperators;
            static bool Equals (value_type v1, value_type v2)
            {
                return v1.GetValue () == v2.GetValue ();
            }
        };
        using SimpleClassWithoutComparisonOperators_SETRAITS = DefaultTraits::Set<SimpleClassWithoutComparisonOperators, MySimpleClassWithoutComparisonOperators_CompareEquals_>;

        DoTestForConcreteContainer_<Set<size_t>> ();
        DoTestForConcreteContainer_<Set<SimpleClass>> ();
        DoTestForConcreteContainer_<Set<SimpleClassWithoutComparisonOperators, SimpleClassWithoutComparisonOperators_SETRAITS>> ();

        DoTestForConcreteContainer_<Set_LinkedList<size_t>> ();
        DoTestForConcreteContainer_<Set_LinkedList<SimpleClass>> ();
        DoTestForConcreteContainer_<Set_LinkedList<SimpleClassWithoutComparisonOperators, SimpleClassWithoutComparisonOperators_SETRAITS>> ();

        DoTestForConcreteContainer_<Set_stdset<size_t>> ();
        DoTestForConcreteContainer_<Set_stdset<SimpleClass>> ();
        {
            struct MySimpleClassWithoutComparisonOperators_ComparerWithCompare_ : MySimpleClassWithoutComparisonOperators_CompareEquals_ {
                using value_type = SimpleClassWithoutComparisonOperators;
                static int Compare (value_type v1, value_type v2)
                {
                    return static_cast<int> (v1.GetValue ()) - static_cast<int> (v2.GetValue ());
                }
            };
            using SimpleClassWithoutComparisonOperatorsSet_stdset_TRAITS = Concrete::Set_stdset_DefaultTraits<
                SimpleClassWithoutComparisonOperators,
                MySimpleClassWithoutComparisonOperators_CompareEquals_,
                MySimpleClassWithoutComparisonOperators_ComparerWithCompare_>;
            DoTestForConcreteContainer_<Set_stdset<SimpleClassWithoutComparisonOperators, SimpleClassWithoutComparisonOperatorsSet_stdset_TRAITS>> ();

            ExampleCTORS_Test_2_::DoTest ();
        }

        Where_Test_3_::DoAll ();
    }
}

int main (int argc, const char* argv[])
{
    Stroika::TestHarness::Setup ();
    return Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
}
