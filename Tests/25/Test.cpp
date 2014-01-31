/*
 * Copyright(c) Sophist Solutions Inc. 1990-2014.  All rights reserved
 */
//      TEST    Foundation::Containers::MultiSet
#include    "Stroika/Foundation/StroikaPreComp.h"

#include    <iostream>
#include    <sstream>

#include    "Stroika/Foundation/Containers/MultiSet.h"
#include    "Stroika/Foundation/Containers/Concrete/MultiSet_Array.h"
#include    "Stroika/Foundation/Containers/Concrete/MultiSet_LinkedList.h"
#include    "Stroika/Foundation/Containers/Concrete/MultiSet_stdmap.h"
#include    "Stroika/Foundation/Debug/Assertions.h"
#include    "Stroika/Foundation/Debug/Trace.h"

#include    "../TestCommon/CommonTests_MultiSet.h"
#include    "../TestHarness/SimpleClass.h"
#include    "../TestHarness/TestHarness.h"


using   namespace   Stroika;
using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Containers;


using   Concrete::MultiSet_Array;
using   Concrete::MultiSet_LinkedList;
using   Concrete::MultiSet_stdmap;



namespace {
    template    <typename   CONCRETE_CONTAINER>
    void    DoTestForConcreteContainer_ ()
    {
        auto extraChecksFunction = [] (const typename CONCRETE_CONTAINER::ArchetypeContainerType & t) {
            // only work todo on sorted mappings
        };
        CommonTests::MultiSetTests::All_For_Type<CONCRETE_CONTAINER> (extraChecksFunction);
    }
}


namespace   {

    void    DoRegressionTests_ ()
    {
        struct  MySimpleClassWithoutComparisonOperators_ComparerWithEquals_ {
            using   ElementType     =   SimpleClassWithoutComparisonOperators;
            static  bool    Equals (ElementType v1, ElementType v2)
            {
                return v1.GetValue () == v2.GetValue ();
            }
        };
        using   SimpleClassWithoutComparisonOperators_MultiSetTRAITS    =   MultiSet_DefaultTraits<SimpleClassWithoutComparisonOperators, MySimpleClassWithoutComparisonOperators_ComparerWithEquals_>;

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
            struct  MySimpleClassWithoutComparisonOperators_ComparerWithCompare_ : MySimpleClassWithoutComparisonOperators_ComparerWithEquals_ {
                using   ElementType =   SimpleClassWithoutComparisonOperators;
                static  int    Compare (ElementType v1, ElementType v2)
                {
                    return v1.GetValue () - v2.GetValue ();
                }
            };
            using   SimpleClassWithoutComparisonOperators_Mapping_stdmap_TRAITS =   Concrete::MultiSet_stdmap_DefaultTraits <
                    SimpleClassWithoutComparisonOperators,
                    MySimpleClassWithoutComparisonOperators_ComparerWithCompare_
                    >;
            DoTestForConcreteContainer_<MultiSet_stdmap<SimpleClassWithoutComparisonOperators, SimpleClassWithoutComparisonOperators_Mapping_stdmap_TRAITS>> ();
        }
    }

}



int     main (int argc, const char* argv[])
{
    Stroika::TestHarness::Setup ();
    Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
    return EXIT_SUCCESS;
}

