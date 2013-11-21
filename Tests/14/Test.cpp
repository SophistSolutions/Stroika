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
#include    "Stroika/Foundation/Containers/Concrete/Set_stdset.h"
#include    "Stroika/Foundation/Debug/Assertions.h"
#include    "Stroika/Foundation/Debug/Trace.h"

#include    "../TestCommon/CommonTests_Set.h"
#include    "../TestHarness/SimpleClass.h"
#include    "../TestHarness/TestHarness.h"



using   namespace   Stroika;
using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Containers;


using   Concrete::Set_LinkedList;
using   Concrete::Set_stdset;



namespace {
    template    <typename   CONCRETE_CONTAINER>
    void    DoTestForConcreteContainer_ ()
    {
        typedef typename CONCRETE_CONTAINER::ElementType    ElementType;
        typedef typename CONCRETE_CONTAINER::TraitsType     TraitsType;
        auto extraChecksFunction = [] (const Set<ElementType, typename TraitsType::SetTraitsType>& s) {
            // only work todo on sorted sets
        };
        CommonTests::SetTests::Test_All_For_Type<CONCRETE_CONTAINER, Set<ElementType, typename TraitsType::SetTraitsType>> (extraChecksFunction);
    }
}



namespace   {
    void    DoRegressionTests_ ()
    {
        using namespace CommonTests::SetTests;

        struct  MySimpleClassWithoutComparisonOperators_CompareEquals_ {
            typedef SimpleClassWithoutComparisonOperators ElementType;
            static  bool    Equals (ElementType v1, ElementType v2) {
                return v1.GetValue () == v2.GetValue ();
            }
        };
        typedef Set_DefaultTraits<SimpleClassWithoutComparisonOperators, MySimpleClassWithoutComparisonOperators_CompareEquals_>   SimpleClassWithoutComparisonOperators_SETRAITS;

        DoTestForConcreteContainer_<Set<size_t>> ();
        DoTestForConcreteContainer_<Set<SimpleClass>> ();
        DoTestForConcreteContainer_<Set<SimpleClassWithoutComparisonOperators, SimpleClassWithoutComparisonOperators_SETRAITS>> ();

        DoTestForConcreteContainer_<Set_LinkedList<size_t>> ();
        DoTestForConcreteContainer_<Set_LinkedList<SimpleClass>> ();
        DoTestForConcreteContainer_<Set_LinkedList<SimpleClassWithoutComparisonOperators, SimpleClassWithoutComparisonOperators_SETRAITS>> ();

        DoTestForConcreteContainer_<Set_stdset<size_t>> ();
        DoTestForConcreteContainer_<Set_stdset<SimpleClass>> ();
        {
            struct  MySimpleClassWithoutComparisonOperators_ComparerWithCompare_ : MySimpleClassWithoutComparisonOperators_CompareEquals_ {
                typedef SimpleClassWithoutComparisonOperators ElementType;
                static  int    Compare (ElementType v1, ElementType v2) {
                    return static_cast<int> (v1.GetValue ()) - static_cast<int> (v2.GetValue ());
                }
            };
            typedef Concrete::Set_stdset_DefaultTraits <
            SimpleClassWithoutComparisonOperators,
            MySimpleClassWithoutComparisonOperators_CompareEquals_,
            MySimpleClassWithoutComparisonOperators_ComparerWithCompare_
            >   SimpleClassWithoutComparisonOperatorsSet_stdset_TRAITS;
            DoTestForConcreteContainer_<Set_stdset<SimpleClassWithoutComparisonOperators, SimpleClassWithoutComparisonOperatorsSet_stdset_TRAITS>> ();
        }

    }
}


int     main (int argc, const char* argv[])
{
    Stroika::TestHarness::Setup ();
    Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
    return EXIT_SUCCESS;
}

