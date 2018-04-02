/*
* Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
*/
//  TEST    Foundation::Containers::Bijection
#include "Stroika/Foundation/StroikaPreComp.h"

#include <iostream>
#include <sstream>

#include "Stroika/Foundation/Containers/Bijection.h"
#include "Stroika/Foundation/Containers/Concrete/Bijection_LinkedList.h"
#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/Trace.h"

#include "../TestCommon/CommonTests_Bijection.h"
#include "../TestHarness/SimpleClass.h"
#include "../TestHarness/TestHarness.h"

using namespace Stroika;
using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Containers;

using Concrete::Bijection_LinkedList;

namespace {
    template <typename CONCRETE_CONTAINER>
    void RunTests_ ()
    {
        auto testFunc = [](const typename CONCRETE_CONTAINER::ArchetypeContainerType& s) {
        };
        CommonTests::BijectionTests::SimpleTest_All_For_Type<CONCRETE_CONTAINER> (testFunc);
    }
}

namespace {

    void DoRegressionTests_ ()
    {
        struct MySimpleClassWithoutComparisonOperators_ComparerWithEquals_ {
            using value_type = SimpleClassWithoutComparisonOperators;
			bool operator () (value_type v1, value_type v2)
            {
                return v1.GetValue () == v2.GetValue ();
            }
        };
        //using SimpleClassWOCOMPARE_BIJECTION_TRAITS = DefaultTraits::Bijection<SimpleClassWithoutComparisonOperators, SimpleClassWithoutComparisonOperators, MySimpleClassWithoutComparisonOperators_ComparerWithEquals_, MySimpleClassWithoutComparisonOperators_ComparerWithEquals_>;

        RunTests_<Bijection<size_t, size_t>> ();
        RunTests_<Bijection<SimpleClass, SimpleClass>> ();
        //RunTests_<Bijection<SimpleClassWithoutComparisonOperators, SimpleClassWithoutComparisonOperators, SimpleClassWOCOMPARE_BIJECTION_TRAITS>> ();

        RunTests_<Bijection_LinkedList<size_t, size_t>> ();
        RunTests_<Bijection_LinkedList<SimpleClass, SimpleClass>> ();
        //RunTests_<Bijection_LinkedList<SimpleClassWithoutComparisonOperators, SimpleClassWithoutComparisonOperators, SimpleClassWOCOMPARE_BIJECTION_TRAITS>> ();
    }
}

int main (int argc, const char* argv[])
{
    Stroika::TestHarness::Setup ();
    return Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
}
