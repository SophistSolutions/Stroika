/*
 * Copyright(c) Sophist Solutions Inc. 1990-2014.  All rights reserved
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
#include    "Stroika/Foundation/Containers/Concrete/SortedMapping_stdmap.h"
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
    void    DoTestForConcreteContainer_AllTestsWhichDontRequireComparer_For_Type_ ()
    {
        auto extraChecksFunction = [](const typename CONCRETE_CONTAINER::ArchetypeContainerType & m) {
            // only work todo on sorted mappings
        };
        CommonTests::MappingTests::SimpleMappingTest_AllTestsWhichDontRequireComparer_For_Type_<CONCRETE_CONTAINER> (extraChecksFunction);
    }
    template    <typename   CONCRETE_CONTAINER>
    void    DoTestForConcreteContainer_ ()
    {
        auto extraChecksFunction = [](const typename CONCRETE_CONTAINER::ArchetypeContainerType & m) {
            // only work todo on sorted mappings
        };
        CommonTests::MappingTests::SimpleMappingTest_All_For_Type<CONCRETE_CONTAINER> (extraChecksFunction);
    }
}


namespace {
    void    Test2_SimpleBaseClassConversionTraitsConfusion_ ()
    {
        SortedMapping<int, float> xxxyy = Concrete::SortedMapping_stdmap<int, float> ();
        Mapping<int, float> xxxyy1 = Concrete::Mapping_stdmap<int, float> ();
    }
}


namespace {
    template <typename CONTAINER, typename COMPARER>
    void    doIt_t3_()
    {
        CommonTests::MappingTests::SimpleMappingTest_WhichRequiresExplcitValueComparer<CONTAINER, COMPARER> ([] (const CONTAINER & c) {});
    }
    void    Test3_SimpleMappingTest_WhichRequiresExplcitValueComparer ()
    {
        doIt_t3_<Mapping_LinkedList<size_t, size_t>, Common::ComparerWithEquals<size_t>> ();
    }
}



namespace {
    namespace Test4_MappingCTOROverloads_ {
        namespace xPrivate_ {
            struct A;
            struct B;
            struct A {
                A() {}
                A(const A&) {}
                A(const B&) {}
            };
            struct B {
                B() {}
                B(const A&) {}
                B(const B&) {}
            };
        }
        void DoIt ()
        {
            using namespace xPrivate_;
            Mapping<int, A> from;
#if 0
            bool xxx1 = std::is_convertible<const Mapping<int, A>*, const Mapping<int, B>*>::value;
            bool xxx2 = Configuration::IsIterableOfT<Mapping<int, A>, KeyValuePair<int, B>>::value;
            using Common::KeyValuePair;
            using KEY_TYPE = int;
            using VALUE_TYPE = B;
            using TRAITS = Mapping_DefaultTraits<KEY_TYPE, VALUE_TYPE>;
            using CONTAINER_OF_PAIR_KEY_T = Mapping<int, A>;
            bool xxxxx1 = Configuration::IsIterableOfT<CONTAINER_OF_PAIR_KEY_T, KeyValuePair<KEY_TYPE, VALUE_TYPE>>::value;
            bool xxxxx2 = (Configuration::IsIterableOfT<CONTAINER_OF_PAIR_KEY_T, KeyValuePair<KEY_TYPE, VALUE_TYPE>>::value) and not std::is_convertible<const CONTAINER_OF_PAIR_KEY_T*, const Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>*>::value;
            //bool xxxxx3 = (Configuration::IsIterableOfT<CONTAINER_OF_PAIR_KEY_T, pair<KEY_TYPE, VALUE_TYPE>>::value);
            //bool xxxxx = (Configuration::IsIterableOfT<CONTAINER_OF_PAIR_KEY_T, KeyValuePair<KEY_TYPE, VALUE_TYPE>>::value or Configuration::IsIterableOfT<CONTAINER_OF_PAIR_KEY_T, pair<KEY_TYPE, VALUE_TYPE>>::value) and not std::is_convertible<const CONTAINER_OF_PAIR_KEY_T*, const Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>*>::value;
#endif
            Mapping<int, B> to1;
            for (auto i : from) {
                to1.Add (i);
            }
            Mapping<int, B> to2 = from;
        }
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
        };
        using   SimpleClassWithoutComparisonOperators_MappingTRAITS =   Mapping_DefaultTraits <
                SimpleClassWithoutComparisonOperators,
                SimpleClassWithoutComparisonOperators,
                MySimpleClassWithoutComparisonOperators_ComparerWithEquals_
                >   ;

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
                using   ElementType =   SimpleClassWithoutComparisonOperators;
                static  int    Compare (ElementType v1, ElementType v2)
                {
                    return Common::CompareNormalizer (v1.GetValue (), v2.GetValue ());
                }
            };
            using   SimpleClassWithoutComparisonOperators_Mapping_stdmap_TRAITS =
                Concrete::Mapping_stdmap_DefaultTraits <
                SimpleClassWithoutComparisonOperators,
                SimpleClassWithoutComparisonOperators,
                MySimpleClassWithoutComparisonOperators_ComparerWithEquals_,
                MySimpleClassWithoutComparisonOperators_ComparerWithCompare_
                >;
            DoTestForConcreteContainer_AllTestsWhichDontRequireComparer_For_Type_<Mapping_stdmap<SimpleClassWithoutComparisonOperators, SimpleClassWithoutComparisonOperators, SimpleClassWithoutComparisonOperators_Mapping_stdmap_TRAITS>> ();
        }

        Test2_SimpleBaseClassConversionTraitsConfusion_ ();

        Test3_SimpleMappingTest_WhichRequiresExplcitValueComparer ();

        Test4_MappingCTOROverloads_::DoIt ();
    }
}


int     main (int argc, const char* argv[])
{
    Stroika::TestHarness::Setup ();
    Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
    return EXIT_SUCCESS;
}
