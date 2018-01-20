/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
//  TEST    Foundation::Containers::MultiSet
#include "Stroika/Foundation/StroikaPreComp.h"

#include <iostream>
#include <sstream>

#include "Stroika/Foundation/Containers/Collection.h"

#include "Stroika/Foundation/Containers/MultiSet.h"

#include "Stroika/Foundation/Containers/Concrete/MultiSet_Array.h"
#include "Stroika/Foundation/Containers/Concrete/MultiSet_LinkedList.h"
#include "Stroika/Foundation/Containers/Concrete/MultiSet_stdmap.h"

#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/Trace.h"

#include "../TestCommon/CommonTests_MultiSet.h"
#include "../TestHarness/SimpleClass.h"
#include "../TestHarness/TestHarness.h"

using namespace Stroika;
using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Containers;

using Concrete::MultiSet_Array;
using Concrete::MultiSet_LinkedList;
using Concrete::MultiSet_stdmap;

namespace {
    template <typename CONCRETE_CONTAINER>
    void DoTestForConcreteContainer_ ()
    {
        Debug::TraceContextBumper ctx{L"{}::DoTestForConcreteContainer_"};
        auto                      extraChecksFunction = [](const typename CONCRETE_CONTAINER::ArchetypeContainerType& t) {
            // only work todo on sorted mappings
        };
        CommonTests::MultiSetTests::All_For_Type<CONCRETE_CONTAINER> (extraChecksFunction);
    }
}

namespace {
    namespace ExampleCTORS_Test_2_ {
        void DoTest ()
        {
            // From Set<> CTOR docs
            Collection<int>  c;
            std::vector<int> v;

            MultiSet<int> s1 = {1, 2, 3};
            MultiSet<int> s2 = s1;
            MultiSet<int> s3{s1};
            MultiSet<int> s4{s1.begin (), s1.end ()};
            MultiSet<int> s5{c};
            MultiSet<int> s6{v};
            //@todo - make this work - use ITERATOR NOT PTR -  MultiSet<int> s7{v.begin (), v.end ()};
            MultiSet<int> s8{move (s1)};
        }
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
        };
        using SimpleClassWithoutComparisonOperators_MultiSetTRAITS = DefaultTraits::MultiSet<SimpleClassWithoutComparisonOperators, MySimpleClassWithoutComparisonOperators_ComparerWithEquals_>;

        DoTestForConcreteContainer_<MultiSet<size_t>> ();
        DoTestForConcreteContainer_<MultiSet<SimpleClass>> ();
        DoTestForConcreteContainer_<MultiSet<SimpleClassWithoutComparisonOperators, SimpleClassWithoutComparisonOperators_MultiSetTRAITS>> ();

        DoTestForConcreteContainer_<MultiSet_Array<size_t>> ();
        DoTestForConcreteContainer_<MultiSet_Array<SimpleClass>> ();
        DoTestForConcreteContainer_<MultiSet_Array<SimpleClassWithoutComparisonOperators, SimpleClassWithoutComparisonOperators_MultiSetTRAITS>> ();

        DoTestForConcreteContainer_<MultiSet_LinkedList<size_t>> ();
        DoTestForConcreteContainer_<MultiSet_LinkedList<SimpleClass>> ();
        DoTestForConcreteContainer_<MultiSet_LinkedList<SimpleClassWithoutComparisonOperators, SimpleClassWithoutComparisonOperators_MultiSetTRAITS>> ();

        DoTestForConcreteContainer_<MultiSet_stdmap<size_t>> ();
        DoTestForConcreteContainer_<MultiSet_stdmap<SimpleClass>> ();
        {
            struct MySimpleClassWithoutComparisonOperators_ComparerWithCompare_ : MySimpleClassWithoutComparisonOperators_ComparerWithEquals_ {
                using value_type = SimpleClassWithoutComparisonOperators;
                static int Compare (value_type v1, value_type v2)
                {
                    return Common::CompareNormalizer (v1.GetValue (), v2.GetValue ());
                }
            };
            using SimpleClassWithoutComparisonOperators_Mapping_stdmap_TRAITS = Concrete::MultiSet_stdmap_DefaultTraits<
                SimpleClassWithoutComparisonOperators,
                MySimpleClassWithoutComparisonOperators_ComparerWithCompare_>;
            DoTestForConcreteContainer_<MultiSet_stdmap<SimpleClassWithoutComparisonOperators, SimpleClassWithoutComparisonOperators_Mapping_stdmap_TRAITS>> ();
        }

        ExampleCTORS_Test_2_::DoTest ();
    }
}

int main (int argc, const char* argv[])
{
    Stroika::TestHarness::Setup ();
    return Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
}
