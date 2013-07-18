/*
 * Copyright(c) Sophist Solutions Inc. 1990-2013.  All rights reserved
 */
//      TEST    Foundation::Containers::Mapping
//      STATUS  Alpha-Late
#include    "Stroika/Foundation/StroikaPreComp.h"

#include    <iostream>
#include    <sstream>

#include    "Stroika/Foundation/Containers/Mapping.h"
#include    "Stroika/Foundation/Containers/Concrete/Mapping_Array.h"
#include    "Stroika/Foundation/Containers/Concrete/Mapping_LinkedList.h"
#include    "Stroika/Foundation/Containers/Concrete/Mapping_stdmap.h"
#include    "Stroika/Foundation/Debug/Assertions.h"
#include    "Stroika/Foundation/Debug/Trace.h"


#include    "../TestCommon/CommonTests_Mapping.h"
#include    "../TestHarness/SimpleClass.h"
#include    "../TestHarness/TestHarness.h"



using   namespace   Stroika;
using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Containers;


using   Concrete::Mapping_Array;
using   Concrete::Mapping_LinkedList;
using   Concrete::Mapping_stdmap;


namespace {
    template    <typename   CONCRETE_CONTAINER>
    void    DoTestForConcreteContainer_ ()
    {
        typedef typename CONCRETE_CONTAINER::KeyType    KeyType;
        typedef typename CONCRETE_CONTAINER::ValueType  ValueType;
        typedef typename CONCRETE_CONTAINER::TraitsType TraitsType;
        auto extraChecksFunction = [] (const Mapping<KeyType, ValueType, TraitsType>& m) {
            // only work todo on sorted mappings
        };
        CommonTests::MappingTests::SimpleMappingTest_All_For_Type<CONCRETE_CONTAINER> (extraChecksFunction);
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
        typedef Mapping_DefaultTraits <
        SimpleClassWithoutComparisonOperators,
        SimpleClassWithoutComparisonOperators,
        MySimpleClassWithoutComparisonOperators_ComparerWithEquals_,
        MySimpleClassWithoutComparisonOperators_ComparerWithEquals_
        >   SimpleClassWithoutComparisonOperators_MappingTRAITS;

#if 0
        // must breakup SimpleMappingTest_All_For_Type as we did for other stuf - this STL thing cannot work unless operaotr< or some other emchanim so speicyt right stl comparer!
        template <typename USING_MAPPING_CONTAINER, typename TEST_FUNCTION>
        void    SimpleMappingTest_All_For_Type (TEST_FUNCTION applyToContainer) {
            ...            Test5_ToFromSTLMap::DoAllTests_<USING_MAPPING_CONTAINER> (applyToContainer);
        }
#endif


        DoTestForConcreteContainer_<Mapping<size_t, size_t>> ();
        DoTestForConcreteContainer_<Mapping<SimpleClass, SimpleClass>> ();
        //DoTestForConcreteContainer_<Mapping<SimpleClassWithoutComparisonOperators, SimpleClassWithoutComparisonOperators, SimpleClassWithoutComparisonOperators_MappingTRAITS>> ();

        DoTestForConcreteContainer_<Mapping_Array<size_t, size_t>> ();
        DoTestForConcreteContainer_<Mapping_Array<SimpleClass, SimpleClass>> ();
        //DoTestForConcreteContainer_<Mapping_Array<SimpleClassWithoutComparisonOperators, SimpleClassWithoutComparisonOperators, SimpleClassWithoutComparisonOperators_MappingTRAITS>> ();

        DoTestForConcreteContainer_<Mapping_LinkedList<size_t, size_t>> ();
        DoTestForConcreteContainer_<Mapping_LinkedList<SimpleClass, SimpleClass>> ();
        //DoTestForConcreteContainer_<Mapping_LinkedList<SimpleClassWithoutComparisonOperators, SimpleClassWithoutComparisonOperators, SimpleClassWithoutComparisonOperators_MappingTRAITS>> ();

        DoTestForConcreteContainer_<Mapping_stdmap<size_t, size_t>> ();
        DoTestForConcreteContainer_<Mapping_stdmap<SimpleClass, SimpleClass>> ();
        //needs work - @todo - must add TRIATS stuff to SortedMapping first (or move this code to not depend on sorted mapping)
        //DoTestForConcreteContainer_<Mapping_stdmap<SimpleClassWithoutComparisonOperators, SimpleClassWithoutComparisonOperators, SimpleClassWithoutComparisonOperators_MappingTRAITS>> ();
    }
}


int     main (int argc, const char* argv[])
{
    Stroika::TestHarness::Setup ();
    Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
    return EXIT_SUCCESS;
}

