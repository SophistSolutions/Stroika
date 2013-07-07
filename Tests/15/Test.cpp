/*
* Copyright(c) Sophist Solutions Inc. 1990-2013.  All rights reserved
*/
//  TEST    Foundation::Containers::SortedBag
#include    "Stroika/Foundation/StroikaPreComp.h"

#include    <iostream>
#include    <sstream>

#include    "Stroika/Foundation/Containers/SortedBag.h"
#include    "Stroika/Foundation/Debug/Assertions.h"
#include    "Stroika/Foundation/Debug/Trace.h"

#include    "../TestCommon/CommonTests_Bag.h"
#include    "../TestHarness/SimpleClass.h"
#include    "../TestHarness/TestHarness.h"

#include    "Stroika/Foundation/Containers/Concrete/SortedBag_LinkedList.h"



using   namespace   Stroika;
using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Containers;

using   Concrete::SortedBag_LinkedList;




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
        typedef Bag_DefaultTraits<SimpleClassWithoutComparisonOperators, MySimpleClassWithoutComparisonOperators_ComparerWithEquals_>   SimpleClassWithoutComparisonOperators_BAGTRAITS;

        RunTests_<SortedBag<size_t>> ();
        RunTests_<SortedBag<SimpleClass>> ();
        //RunTests_<SortedBag<SimpleClassWithoutComparisonOperators, SimpleClassWithoutComparisonOperators_BAGTRAITS>> ();

        RunTests_<SortedBag_LinkedList<size_t>> ();
        RunTests_<SortedBag_LinkedList<SimpleClass>> ();
        //RunTests_<SortedBag_LinkedList<SimpleClassWithoutComparisonOperators, SimpleClassWithoutComparisonOperators_BAGTRAITS>> ();
    }

}




int main (int argc, const char* argv[])
{
    Stroika::TestHarness::Setup ();
    Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
    return EXIT_SUCCESS;
}

