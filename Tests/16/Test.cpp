/*
 * Copyright(c) Sophist Solutions Inc. 1990-2013.  All rights reserved
 */
//      TEST    Foundation::Containers::SortedMapping
//      STATUS  Alpha-Late
#include    "Stroika/Foundation/StroikaPreComp.h"

#include    <iostream>
#include    <sstream>

#include    "Stroika/Foundation/Containers/SortedMapping.h"
#include    "Stroika/Foundation/Containers/Concrete/SortedMapping_stdmap.h"
#include    "Stroika/Foundation/Debug/Assertions.h"
#include    "Stroika/Foundation/Debug/Trace.h"
#include    "Stroika/Foundation/Memory/Optional.h"


#include    "../TestCommon/CommonTests_Mapping.h"
#include    "../TestHarness/SimpleClass.h"
#include    "../TestHarness/TestHarness.h"



using   namespace   Stroika;
using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Containers;

using   Memory::Optional;

using   Concrete::SortedMapping_stdmap;


namespace {
    template    <typename   CONCRETE_CONTAINER>
    void    DoTestForConcreteContainer_AllTestsWhichDontRequireComparer_For_Type_ ()
    {
        typedef typename CONCRETE_CONTAINER::KeyType        KeyType;
        typedef typename CONCRETE_CONTAINER::ValueType      ValueType;
        typedef typename CONCRETE_CONTAINER::ElementType    ElementType;
        typedef typename CONCRETE_CONTAINER::TraitsType     TraitsType;
        auto extraChecksFunction = [] (const SortedMapping<KeyType, ValueType, TraitsType>& m) {
            // verify in sorted order
            Optional<ElementType> last;
            for (ElementType i : m) {
                if (last.IsPresent ()) {
                    VerifyTestResult (TraitsType::KeyWellOrderCompareFunctionType::Compare (last->first, i.first) <= 0);
                }
                last = i;
            }
        };
        CommonTests::MappingTests::SimpleMappingTest_AllTestsWhichDontRequireComparer_For_Type_<CONCRETE_CONTAINER> (extraChecksFunction);
    }
    template    <typename   CONCRETE_CONTAINER>
    void    DoTestForConcreteContainer_ ()
    {
        typedef typename CONCRETE_CONTAINER::KeyType        KeyType;
        typedef typename CONCRETE_CONTAINER::ValueType      ValueType;
        typedef typename CONCRETE_CONTAINER::ElementType    ElementType;
        typedef typename CONCRETE_CONTAINER::TraitsType     TraitsType;
        auto extraChecksFunction = [] (const SortedMapping<KeyType, ValueType, TraitsType>& m) {
            // verify in sorted order
            Optional<ElementType> last;
            for (ElementType i : m) {
                if (last.IsPresent ()) {
                    VerifyTestResult (TraitsType::KeyWellOrderCompareFunctionType::Compare (last->first, i.first) <= 0);
                }
                last = i;
            }
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
            static  int    Compare (ElementType v1, ElementType v2) {
                return v1.GetValue () - v2.GetValue ();
            }
        };
        typedef SortedMapping_DefaultTraits <
        SimpleClassWithoutComparisonOperators,
        SimpleClassWithoutComparisonOperators,
        MySimpleClassWithoutComparisonOperators_ComparerWithEquals_,
        MySimpleClassWithoutComparisonOperators_ComparerWithEquals_
        >   SimpleClassWithoutComparisonOperators_MappingTRAITS;

        DoTestForConcreteContainer_<SortedMapping<size_t, size_t>> ();
        DoTestForConcreteContainer_<SortedMapping<SimpleClass, SimpleClass>> ();
        DoTestForConcreteContainer_AllTestsWhichDontRequireComparer_For_Type_<SortedMapping<SimpleClassWithoutComparisonOperators, SimpleClassWithoutComparisonOperators, SimpleClassWithoutComparisonOperators_MappingTRAITS>> ();

        DoTestForConcreteContainer_<SortedMapping_stdmap<size_t, size_t>> ();
        DoTestForConcreteContainer_<SortedMapping_stdmap<SimpleClass, SimpleClass>> ();
        DoTestForConcreteContainer_AllTestsWhichDontRequireComparer_For_Type_<SortedMapping_stdmap<SimpleClassWithoutComparisonOperators, SimpleClassWithoutComparisonOperators, SimpleClassWithoutComparisonOperators_MappingTRAITS>> ();
    }
}


int     main (int argc, const char* argv[])
{
    Stroika::TestHarness::Setup ();
    Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
    return EXIT_SUCCESS;
}

