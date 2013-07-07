/*
* Copyright(c) Sophist Solutions Inc. 1990-2013.  All rights reserved
*/
//  TEST    Foundation::Containers::Bag
#include    "Stroika/Foundation/StroikaPreComp.h"

#include    <iostream>
#include    <sstream>

#include    "Stroika/Foundation/Containers/Bag.h"
#include    "Stroika/Foundation/Debug/Assertions.h"
#include    "Stroika/Foundation/Debug/Trace.h"

#include    "../TestCommon/CommonTests_Bag.h"
#include    "../TestHarness/SimpleClass.h"
#include    "../TestHarness/TestHarness.h"

#include    "Stroika/Foundation/Containers/Concrete/Bag_Array.h"
#include    "Stroika/Foundation/Containers/Concrete/Bag_LinkedList.h"
#include    "Stroika/Foundation/Containers/Concrete/Bag_stdforward_list.h"


using   namespace   Stroika;
using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Containers;

using   Concrete::Bag_Array;
using   Concrete::Bag_LinkedList;
using   Concrete::Bag_stdforward_list;



namespace {
    template    <typename CONCRETE_CONTAINER>
    void     RunTests_ ()
    {
        typedef typename CONCRETE_CONTAINER::ElementType    T;
        typedef typename CONCRETE_CONTAINER::TraitsType     TraitsType;
        auto testFunc = [] (const Bag<T, TraitsType>& s) {
        };
        CommonTests::BagTests::SimpleBagTest_All_For_Type<CONCRETE_CONTAINER> (testFunc);
    }
}


namespace   {

    void    DoRegressionTests_ ()
    {
        struct  MySimpleClassWithoutComparisonOperators_ComparerWithEquals_ {
            typedef SimpleClassWithoutComparisonOperators ElementType;
            static  bool    Equals (ElementType v1, ElementType v2) {
                return v1.GetValue () == v2.GetValue ();
            }
        };
        RunTests_<Bag<size_t>> ();
        RunTests_<Bag<SimpleClass>> ();
        RunTests_<Bag<SimpleClassWithoutComparisonOperators, Bag_DefaultTraits<SimpleClassWithoutComparisonOperators, MySimpleClassWithoutComparisonOperators_ComparerWithEquals_>>> ();

        RunTests_<Bag_LinkedList<size_t>> ();
        RunTests_<Bag_LinkedList<SimpleClass>> ();
        RunTests_<Bag_LinkedList<SimpleClassWithoutComparisonOperators, Bag_DefaultTraits<SimpleClassWithoutComparisonOperators, MySimpleClassWithoutComparisonOperators_ComparerWithEquals_>>> ();

        RunTests_<Bag_Array<size_t>> ();
        RunTests_<Bag_Array<SimpleClass>> ();
        RunTests_<Bag_Array<SimpleClassWithoutComparisonOperators, Bag_DefaultTraits<SimpleClassWithoutComparisonOperators, MySimpleClassWithoutComparisonOperators_ComparerWithEquals_>>> ();

        RunTests_<Bag_stdforward_list<size_t>> ();
        RunTests_<Bag_stdforward_list<SimpleClass>> ();
        RunTests_<Bag_stdforward_list<SimpleClassWithoutComparisonOperators, Bag_DefaultTraits<SimpleClassWithoutComparisonOperators, MySimpleClassWithoutComparisonOperators_ComparerWithEquals_>>> ();
    }

}



int main (int argc, const char* argv[])
{
    Stroika::TestHarness::Setup ();
    Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
    return EXIT_SUCCESS;
}

