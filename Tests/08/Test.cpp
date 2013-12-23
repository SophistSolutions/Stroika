/*
* Copyright(c) Sophist Solutions Inc. 1990-2013.  All rights reserved
*/
//  TEST    Foundation::Containers::Bijection
#include    "Stroika/Foundation/StroikaPreComp.h"

#include    <iostream>
#include    <sstream>

#include    "Stroika/Foundation/Containers/Bijection.h"
#include    "Stroika/Foundation/Containers/Concrete/Bijection_LinkedList.h"
#include    "Stroika/Foundation/Debug/Assertions.h"
#include    "Stroika/Foundation/Debug/Trace.h"

#include    "../TestCommon/CommonTests_Bijection.h"
#include    "../TestHarness/SimpleClass.h"
#include    "../TestHarness/TestHarness.h"



using   namespace   Stroika;
using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Containers;

using   Concrete::Bijection_LinkedList;


namespace {
    template    <typename CONCRETE_CONTAINER>
    void     RunTests_ ()
    {
        auto testFunc = [] (const typename CONCRETE_CONTAINER::ArchetypeContainerType & s) {
        };
        CommonTests::BijectionTests::SimpleBagTest_All_For_Type<CONCRETE_CONTAINER> (testFunc);
    }
}


namespace   {

    void    DoRegressionTests_ ()
    {
        struct  MySimpleClassWithoutComparisonOperators_ComparerWithEquals_ {
            typedef SimpleClassWithoutComparisonOperators ElementType;
            static  bool    Equals (ElementType v1, ElementType v2)
            {
                return v1.GetValue () == v2.GetValue ();
            }
        };
        typedef Bijection_DefaultTraits<SimpleClassWithoutComparisonOperators, SimpleClassWithoutComparisonOperators, MySimpleClassWithoutComparisonOperators_ComparerWithEquals_, MySimpleClassWithoutComparisonOperators_ComparerWithEquals_>   SimpleClassWithoutComparisonOperators_BAGTRAITS;

        RunTests_<Bijection<size_t, size_t>> ();
        RunTests_<Bijection<SimpleClass, SimpleClass>> ();
        RunTests_<Bijection<SimpleClassWithoutComparisonOperators, SimpleClassWithoutComparisonOperators, SimpleClassWithoutComparisonOperators_BAGTRAITS>> ();

        RunTests_<Bijection_LinkedList<size_t, size_t>> ();
        RunTests_<Bijection_LinkedList<SimpleClass, SimpleClass>> ();
        RunTests_<Bijection_LinkedList<SimpleClassWithoutComparisonOperators, SimpleClassWithoutComparisonOperators, SimpleClassWithoutComparisonOperators_BAGTRAITS>> ();
    }

}



int main (int argc, const char* argv[])
{
    Stroika::TestHarness::Setup ();
    Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
    return EXIT_SUCCESS;
}

