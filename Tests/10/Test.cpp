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


#include    "Stroika/Foundation/Containers/Concrete/SortedMapping_stdmap.h"

using   namespace   Stroika;
using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Containers;


using   Concrete::Mapping_Array;
using   Concrete::Mapping_LinkedList;
using   Concrete::Mapping_stdmap;


namespace {
    template    <typename   CONCRETE_CONTAINER>
    void    DoTestForConcreteContainer_AllTestsWhichDontRequireComparer_For_Type_ ()
    {
        typedef typename CONCRETE_CONTAINER::KeyType                        KeyType;
        typedef typename CONCRETE_CONTAINER::ValueType                      ValueType;
        typedef typename CONCRETE_CONTAINER::TraitsType::MappingTraitsType  MappingTraitsType;
        auto extraChecksFunction = [](const Mapping<KeyType, ValueType, MappingTraitsType>& m) {
            // only work todo on sorted mappings
        };
        CommonTests::MappingTests::SimpleMappingTest_AllTestsWhichDontRequireComparer_For_Type_<CONCRETE_CONTAINER> (extraChecksFunction);
    }
    template    <typename   CONCRETE_CONTAINER>
    void    DoTestForConcreteContainer_ ()
    {
        typedef typename CONCRETE_CONTAINER::KeyType                        KeyType;
        typedef typename CONCRETE_CONTAINER::ValueType                      ValueType;
        typedef typename CONCRETE_CONTAINER::TraitsType::MappingTraitsType  MappingTraitsType;
        auto extraChecksFunction = [](const Mapping<KeyType, ValueType, MappingTraitsType>& m) {
            // only work todo on sorted mappings
        };
        CommonTests::MappingTests::SimpleMappingTest_All_For_Type<CONCRETE_CONTAINER> (extraChecksFunction);
    }
}


namespace {
    void    Test2_SimpleBaseClassConversionTraitsConfusion_ ()
    {
#if 1
        /// SERIOUS BUG - WITH HOW I DID BASE CLASS FOR Mapping_stdmap<> -.... second line needs to work!!!
        SortedMapping<int, float> xxxyy = Concrete::SortedMapping_stdmap<int, float> ();
        Mapping<int, float> xxxyy1 = Concrete::Mapping_stdmap<int, float> ();
#endif
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

        DoTestForConcreteContainer_<Mapping<size_t, size_t>> ();
        DoTestForConcreteContainer_<Mapping<SimpleClass, SimpleClass>> ();
        DoTestForConcreteContainer_AllTestsWhichDontRequireComparer_For_Type_<Mapping<SimpleClassWithoutComparisonOperators, SimpleClassWithoutComparisonOperators, SimpleClassWithoutComparisonOperators_MappingTRAITS>> ();

        DoTestForConcreteContainer_<Mapping_Array<size_t, size_t>> ();
        DoTestForConcreteContainer_<Mapping_Array<SimpleClass, SimpleClass>> ();
        DoTestForConcreteContainer_AllTestsWhichDontRequireComparer_For_Type_<Mapping_Array<SimpleClassWithoutComparisonOperators, SimpleClassWithoutComparisonOperators, SimpleClassWithoutComparisonOperators_MappingTRAITS>> ();

        DoTestForConcreteContainer_<Mapping_LinkedList<size_t, size_t>> ();
        DoTestForConcreteContainer_<Mapping_LinkedList<SimpleClass, SimpleClass>> ();
        DoTestForConcreteContainer_AllTestsWhichDontRequireComparer_For_Type_<Mapping_LinkedList<SimpleClassWithoutComparisonOperators, SimpleClassWithoutComparisonOperators, SimpleClassWithoutComparisonOperators_MappingTRAITS>> ();

        DoTestForConcreteContainer_<Mapping_stdmap<size_t, size_t>> ();
        DoTestForConcreteContainer_<Mapping_stdmap<SimpleClass, SimpleClass>> ();
        {
            struct  MySimpleClassWithoutComparisonOperators_ComparerWithCompare_ : MySimpleClassWithoutComparisonOperators_ComparerWithEquals_ {
                typedef SimpleClassWithoutComparisonOperators ElementType;
                static  int    Compare (ElementType v1, ElementType v2) {
                    return v1.GetValue () - v2.GetValue ();
                }
            };
            typedef Concrete::Mapping_stdmap_DefaultTraits <
            SimpleClassWithoutComparisonOperators,
            SimpleClassWithoutComparisonOperators,
            MySimpleClassWithoutComparisonOperators_ComparerWithEquals_,
            MySimpleClassWithoutComparisonOperators_ComparerWithEquals_,
            MySimpleClassWithoutComparisonOperators_ComparerWithCompare_
            >   SimpleClassWithoutComparisonOperators_Mapping_stdmap_TRAITS;
            DoTestForConcreteContainer_AllTestsWhichDontRequireComparer_For_Type_<Mapping_stdmap<SimpleClassWithoutComparisonOperators, SimpleClassWithoutComparisonOperators, SimpleClassWithoutComparisonOperators_Mapping_stdmap_TRAITS>> ();
        }

        Test2_SimpleBaseClassConversionTraitsConfusion_ ();
    }
}


int     main (int argc, const char* argv[])
{
    Stroika::TestHarness::Setup ();
    Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
    return EXIT_SUCCESS;
}
