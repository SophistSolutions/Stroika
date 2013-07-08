/*
 * Copyright(c) Sophist Solutions Inc. 1990-2013.  All rights reserved
 */
//      TEST    Foundation::Containers::Set
//      STATUS  PRELIMINARY
#include    "Stroika/Foundation/StroikaPreComp.h"

#include    <iostream>
#include    <sstream>

#include    "Stroika/Foundation/Containers/Set.h"
#include    "Stroika/Foundation/Containers/Concrete/Set_LinkedList.h"
#include    "Stroika/Foundation/Debug/Assertions.h"
#include    "Stroika/Foundation/Debug/Trace.h"

#include    "../TestCommon/CommonTests_Set.h"
#include    "../TestHarness/SimpleClass.h"
#include    "../TestHarness/TestHarness.h"



using   namespace   Stroika;
using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Containers;


using   Concrete::Set_LinkedList;



namespace   {
    void    DoRegressionTests_ ()
    {
        using namespace CommonTests::SetTests;

        struct  MySimpleClassWithoutComparisonOperators_Comparer_ {
            typedef SimpleClassWithoutComparisonOperators ElementType;
            static  bool    Equals (ElementType v1, ElementType v2) {
                return v1.GetValue () == v2.GetValue ();
            }
        };
        typedef Set_DefaultTraits<SimpleClassWithoutComparisonOperators, MySimpleClassWithoutComparisonOperators_Comparer_>   SimpleClassWithoutComparisonOperators_SETRAITS;

        auto testFunc1 = [] (const Set<size_t>& s) {
        };
        auto testFunc2 = [] (const Set<SimpleClass>& s) {
        };
        auto testFunc3 = [] (const Set<SimpleClassWithoutComparisonOperators, SimpleClassWithoutComparisonOperators_SETRAITS>& s) {
        };

        Test_All_For_Type<Set_LinkedList<size_t>, Set<size_t>> (testFunc1);
        Test_All_For_Type<Set_LinkedList<SimpleClass>, Set<SimpleClass>> (testFunc2);
        Test_All_For_Type<Set_LinkedList<SimpleClassWithoutComparisonOperators, SimpleClassWithoutComparisonOperators_SETRAITS>, Set<SimpleClassWithoutComparisonOperators, SimpleClassWithoutComparisonOperators_SETRAITS>> (testFunc3);

        Test_All_For_Type<Set<size_t>, Set<size_t>> (testFunc1);
        Test_All_For_Type<Set<SimpleClass>, Set<SimpleClass>> (testFunc2);
        Test_All_For_Type<Set<SimpleClassWithoutComparisonOperators, SimpleClassWithoutComparisonOperators_SETRAITS>, Set<SimpleClassWithoutComparisonOperators, SimpleClassWithoutComparisonOperators_SETRAITS>> (testFunc3);
    }
}


int     main (int argc, const char* argv[])
{
    Stroika::TestHarness::Setup ();
    Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
    return EXIT_SUCCESS;
}

