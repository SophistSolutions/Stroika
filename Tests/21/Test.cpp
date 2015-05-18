/*
 * Copyright(c) Sophist Solutions Inc. 1990-2014.  All rights reserved
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
        using   ElementType     =   typename CONCRETE_CONTAINER::ElementType;
        using   TraitsType      =   typename CONCRETE_CONTAINER::TraitsType;
        auto extraChecksFunction = [] (const typename CONCRETE_CONTAINER::ArchetypeContainerType & m) {
            // verify in sorted order
            Optional<ElementType> last;
            for (ElementType i : m) {
                if (last.IsPresent ()) {
                    VerifyTestResult (TraitsType::KeyWellOrderCompareFunctionType::Compare (last->fKey, i.fKey) <= 0);
                }
                last = i;
            }
        };
        CommonTests::MappingTests::SimpleMappingTest_AllTestsWhichDontRequireComparer_For_Type_<CONCRETE_CONTAINER> (extraChecksFunction);
    }
    template    <typename   CONCRETE_CONTAINER>
    void    DoTestForConcreteContainer_ ()
    {
        using   ElementType =   typename CONCRETE_CONTAINER::ElementType;
        using   TraitsType  =   typename CONCRETE_CONTAINER::TraitsType;
        auto extraChecksFunction = [] (const typename CONCRETE_CONTAINER::ArchetypeContainerType & m) {
            // verify in sorted order
            Optional<ElementType> last;
            for (ElementType i : m) {
                if (last.IsPresent ()) {
                    VerifyTestResult (TraitsType::KeyWellOrderCompareFunctionType::Compare (last->fKey, i.fKey) <= 0);
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
            using   ElementType =   SimpleClassWithoutComparisonOperators;
            static  bool    Equals (ElementType v1, ElementType v2)
            {
                return v1.GetValue () == v2.GetValue ();
            }
            static  int    Compare (ElementType v1, ElementType v2)
            {
                return Common::CompareNormalizer (v1.GetValue () - v2.GetValue ());
            }
        };
        struct  MySimpleClassWithoutComparisonOperators_ComparerWithCompare_ : MySimpleClassWithoutComparisonOperators_ComparerWithEquals_ {
            using   ElementType =   SimpleClassWithoutComparisonOperators;
            static  int    Compare (ElementType v1, ElementType v2)
            {
                return Common::CompareNormalizer (v1.GetValue () - v2.GetValue ());
            }
        };
        using   SimpleClassWithoutComparisonOperators_MappingTRAITS =   SortedMapping_DefaultTraits <
                SimpleClassWithoutComparisonOperators,
                SimpleClassWithoutComparisonOperators,
                MySimpleClassWithoutComparisonOperators_ComparerWithEquals_,
                MySimpleClassWithoutComparisonOperators_ComparerWithCompare_
                >;

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

