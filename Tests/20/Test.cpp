/*
 * Copyright(c) Sophist Solutions Inc. 1990-2013.  All rights reserved
 */
//      TEST    Foundation::Containers::Tally
#include    "Stroika/Foundation/StroikaPreComp.h"

#include    <iostream>
#include    <sstream>

#include    "Stroika/Foundation/Containers/Tally.h"
#include    "Stroika/Foundation/Containers/Concrete/Tally_Array.h"
#include    "Stroika/Foundation/Containers/Concrete/Tally_LinkedList.h"
#include    "Stroika/Foundation/Containers/Concrete/Tally_stdmap.h"
#include    "Stroika/Foundation/Debug/Assertions.h"
#include    "Stroika/Foundation/Debug/Trace.h"

#include    "../TestCommon/CommonTests_Tally.h"
#include    "../TestHarness/SimpleClass.h"
#include    "../TestHarness/TestHarness.h"


using   namespace   Stroika;
using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Containers;


using   Concrete::Tally_Array;
using   Concrete::Tally_LinkedList;
using   Concrete::Tally_stdmap;



namespace {
    template    <typename   CONCRETE_CONTAINER>
    void    DoTestForConcreteContainer_ ()
    {
        typedef typename CONCRETE_CONTAINER::TallyOfElementType     TallyOfElementType;
        typedef typename CONCRETE_CONTAINER::TraitsType             TraitsType;
        auto extraChecksFunction = [] (const Tally<TallyOfElementType, TraitsType>& t) {
            // only work todo on sorted mappings
        };
        CommonTests::TallyTests::All_For_Type<CONCRETE_CONTAINER> (extraChecksFunction);
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
        typedef Tally_DefaultTraits <
        SimpleClassWithoutComparisonOperators,
        MySimpleClassWithoutComparisonOperators_ComparerWithEquals_
        >   SimpleClassWithoutComparisonOperators_TallyTRAITS;

        DoTestForConcreteContainer_<Tally<size_t>> ();
        DoTestForConcreteContainer_<Tally<SimpleClass>> ();
        DoTestForConcreteContainer_<Tally<SimpleClassWithoutComparisonOperators, SimpleClassWithoutComparisonOperators_TallyTRAITS>> ();

        DoTestForConcreteContainer_<Tally_Array<size_t>> ();
        DoTestForConcreteContainer_<Tally_Array<SimpleClass>> ();
        DoTestForConcreteContainer_<Tally_Array<SimpleClassWithoutComparisonOperators, SimpleClassWithoutComparisonOperators_TallyTRAITS>> ();

        DoTestForConcreteContainer_<Tally_LinkedList<size_t>> ();
        DoTestForConcreteContainer_<Tally_LinkedList<SimpleClass>> ();
        DoTestForConcreteContainer_<Tally_LinkedList<SimpleClassWithoutComparisonOperators, SimpleClassWithoutComparisonOperators_TallyTRAITS>> ();

        DoTestForConcreteContainer_<Tally_stdmap<size_t>> ();
        DoTestForConcreteContainer_<Tally_stdmap<SimpleClass>> ();
        {
            struct  MySimpleClassWithoutComparisonOperators_ComparerWithCompare_ : MySimpleClassWithoutComparisonOperators_ComparerWithEquals_ {
                typedef SimpleClassWithoutComparisonOperators ElementType;
                static  int    Compare (ElementType v1, ElementType v2) {
                    return v1.GetValue () - v2.GetValue ();
                }
            };
            typedef Concrete::Tally_stdmap_DefaultTraits <
            SimpleClassWithoutComparisonOperators,
            MySimpleClassWithoutComparisonOperators_ComparerWithCompare_
            >   SimpleClassWithoutComparisonOperators_Mapping_stdmap_TRAITS;
			DoTestForConcreteContainer_<Tally_stdmap<SimpleClassWithoutComparisonOperators, SimpleClassWithoutComparisonOperators_Mapping_stdmap_TRAITS>> ();
        }

    }

}



int     main (int argc, const char* argv[])
{
    Stroika::TestHarness::Setup ();
    Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
    return EXIT_SUCCESS;
}

