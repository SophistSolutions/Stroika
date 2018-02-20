/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
//  TEST    Foundation::Containers::Mapping
//      STATUS  Alpha-Late
#include "Stroika/Foundation/StroikaPreComp.h"

#include <iostream>
#include <sstream>

#include "Stroika/Foundation/Containers/Collection.h"

#include "Stroika/Foundation/Containers/Concrete/Mapping_Array.h"
#include "Stroika/Foundation/Containers/Concrete/Mapping_LinkedList.h"
#include "Stroika/Foundation/Containers/Concrete/Mapping_stdmap.h"
#include "Stroika/Foundation/Containers/Concrete/SortedMapping_stdmap.h"
#include "Stroika/Foundation/Containers/Mapping.h"
#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/Trace.h"

#include "../TestCommon/CommonTests_Mapping.h"
#include "../TestHarness/SimpleClass.h"
#include "../TestHarness/TestHarness.h"

using namespace Stroika;
using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Containers;

using Concrete::Mapping_Array;
using Concrete::Mapping_LinkedList;
using Concrete::Mapping_stdmap;

namespace {
    template <typename CONCRETE_CONTAINER>
    void DoTestForConcreteContainer_AllTestsWhichDontRequireComparer_For_Type_ ()
    {
        auto extraChecksFunction = [](const typename CONCRETE_CONTAINER::ArchetypeContainerType& m) {
            // only work todo on sorted mappings
        };
        CommonTests::MappingTests::SimpleMappingTest_AllTestsWhichDontRequireComparer_For_Type_<CONCRETE_CONTAINER> (extraChecksFunction);
    }
    template <typename CONCRETE_CONTAINER>
    void DoTestForConcreteContainer_ ()
    {
        auto extraChecksFunction = [](const typename CONCRETE_CONTAINER::ArchetypeContainerType& m) {
            // only work todo on sorted mappings
        };
        CommonTests::MappingTests::SimpleMappingTest_All_For_Type<CONCRETE_CONTAINER> (extraChecksFunction);
    }
}

namespace {
    void Test2_SimpleBaseClassConversionTraitsConfusion_ ()
    {
        Debug::TraceContextBumper ctx{L"{}::Test2_SimpleBaseClassConversionTraitsConfusion_"};
        SortedMapping<int, float> xxxyy  = Concrete::SortedMapping_stdmap<int, float> ();
        Mapping<int, float>       xxxyy1 = Concrete::Mapping_stdmap<int, float> ();
    }
}

namespace {
    template <typename CONTAINER, typename COMPARER>
    void doIt_t3_ ()
    {
        CommonTests::MappingTests::SimpleMappingTest_WhichRequiresExplcitValueComparer<CONTAINER, COMPARER> ([](const CONTAINER& c) {});
    }
    void Test3_SimpleMappingTest_WhichRequiresExplcitValueComparer ()
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
                A () {}
                A (const A&) {}
                A (const B&) {}
            };
            struct B {
                B () {}
                B (const A&) {}
                B (const B&) {}
            };

            using Common::KeyValuePair;
            using KEY_TYPE                = int;
            using VALUE_TYPE              = B;
            using TRAITS                  = DefaultTraits::Mapping<KEY_TYPE, VALUE_TYPE>;
            using CONTAINER_OF_PAIR_KEY_T = Mapping<int, A>;
            using T                       = KeyValuePair<KEY_TYPE, VALUE_TYPE>;

            struct aaaaaa {
                //              template    <typename X>
                using X = T;
                static auto check (const X& x) -> std::conditional<
                    Configuration::has_beginend<CONTAINER_OF_PAIR_KEY_T>::value and
                        std::is_convertible<typename std::iterator_traits<Configuration::begin_result<CONTAINER_OF_PAIR_KEY_T>>::value_type, T>::value,
                    Configuration::substitution_succeeded<T>,
                    Configuration::substitution_failure>::type;
                //                  static Configuration::substitution_failure check (...);
                using type = decltype (check (declval<T> ()));
            };
        }
        void DoIt ()
        {
            Debug::TraceContextBumper ctx{L"{}::Test4_MappingCTOROverloads_"};
            using namespace xPrivate_;
            Mapping<int, A> from;

            using Common::KeyValuePair;
            using KEY_TYPE                = int;
            using VALUE_TYPE              = B;
            using TRAITS                  = DefaultTraits::Mapping<KEY_TYPE, VALUE_TYPE>;
            using CONTAINER_OF_PAIR_KEY_T = Mapping<int, A>;
            bool n1                       = Configuration::IsIterableOfT<CONTAINER_OF_PAIR_KEY_T, KeyValuePair<KEY_TYPE, VALUE_TYPE>>::value;
            //bool n2 = Configuration::IsIterableOfT2<CONTAINER_OF_PAIR_KEY_T, KeyValuePair<KEY_TYPE, VALUE_TYPE>>::value;

            using T = KeyValuePair<KEY_TYPE, VALUE_TYPE>;
            using ttt =
                std::conditional<
                    Configuration::has_beginend<CONTAINER_OF_PAIR_KEY_T>::value and
                        std::is_convertible<typename std::iterator_traits<Configuration::begin_result<CONTAINER_OF_PAIR_KEY_T>>::value_type, T>::value,
                    Configuration::substitution_succeeded<T>,
                    Configuration::substitution_failure>::type;
            const bool n3 = is_same<ttt, Configuration::substitution_failure>::value;

            using aaaa3   = Configuration::Private_::IsIterableOfT_Impl2_<CONTAINER_OF_PAIR_KEY_T, T>::type;
            const bool n4 = is_same<aaaa3, Configuration::substitution_failure>::value;

            const bool n5 = is_same<aaaaaa::type, Configuration::substitution_failure>::value;

#if 0
            bool xxx1 = std::is_convertible<const Mapping<int, A>*, const Mapping<int, B>*>::value;
            bool xxx2 = Configuration::IsIterableOfT<Mapping<int, A>, KeyValuePair<int, B>>::value;
            using Common::KeyValuePair;
            using KEY_TYPE = int;
            using VALUE_TYPE = B;
            using TRAITS = DefaultTraits::Mapping<KEY_TYPE, VALUE_TYPE>;
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

namespace {
    namespace ExampleCTORS_Test_5_ {
        void DoTest ()
        {
            Debug::TraceContextBumper ctx{L"{}::ExampleCTORS_Test_5_"};
            // From Mapping<> CTOR docs
            Collection<pair<int, int>> c;
            std::map<int, int>         m;

            Mapping<int, int> m1 = {pair<int, int>{1, 1}, pair<int, int>{2, 2}, pair<int, int>{3, 2}};
            Mapping<int, int> m2 = m1;
            Mapping<int, int> m3{m1};
            Mapping<int, int> m4{m1.begin (), m1.end ()};
            Mapping<int, int> m5{c};
            Mapping<int, int> m6{m};
            Mapping<int, int> m7{m.begin (), m.end ()};
            Mapping<int, int> m8{move (m1)};
        }
    }
}

namespace {
    namespace Where_Test_6_ {
        void DoAll ()
        {
            Mapping<int, int> m{KeyValuePair<int, int>{1, 3}, KeyValuePair<int, int>{2, 4}, KeyValuePair<int, int>{3, 5}, KeyValuePair<int, int>{4, 5}, KeyValuePair<int, int>{5, 7}};
            VerifyTestResult ((m.WithKeys (initializer_list<int>{2, 5}) == Mapping<int, int>{KeyValuePair<int, int>{2, 4}, KeyValuePair<int, int>{3, 5}, KeyValuePair<int, int>{5, 7}}));
        }
    }
}

namespace {
    namespace Where_Test_7_ {
        void DoAll ()
        {
            Mapping<int, int> m{KeyValuePair<int, int>{1, 3}, KeyValuePair<int, int>{2, 4}, KeyValuePair<int, int>{3, 5}, KeyValuePair<int, int>{4, 5}, KeyValuePair<int, int>{5, 7}};
            VerifyTestResult ((m.WithKeys (initializer_list<int>{2, 5}) == Mapping<int, int>{KeyValuePair<int, int>{2, 4}, KeyValuePair<int, int>{3, 5}, KeyValuePair<int, int>{5, 7}}));
        }
    }
}

namespace {
    void DoRegressionTests_ ()
    {
        struct MySimpleClassWithoutComparisonOperators_ComparerWithEquals_ {
            using value_type = SimpleClassWithoutComparisonOperators;
            static bool Equals (value_type v1, value_type v2)
            {
                return v1.GetValue () == v2.GetValue ();
            }
        };
        using SimpleClassWithoutComparisonOperators_MappingTRAITS = DefaultTraits::Mapping<
            SimpleClassWithoutComparisonOperators,
            SimpleClassWithoutComparisonOperators,
            MySimpleClassWithoutComparisonOperators_ComparerWithEquals_>;

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
            struct MySimpleClassWithoutComparisonOperators_ComparerWithCompare_ : MySimpleClassWithoutComparisonOperators_ComparerWithEquals_ {
                using value_type = SimpleClassWithoutComparisonOperators;
                static int Compare (value_type v1, value_type v2)
                {
                    return Common::CompareNormalizer (v1.GetValue (), v2.GetValue ());
                }
            };
            using SimpleClassWithoutComparisonOperators_Mapping_stdmap_TRAITS =
                Concrete::Mapping_stdmap_DefaultTraits<
                    SimpleClassWithoutComparisonOperators,
                    SimpleClassWithoutComparisonOperators,
                    MySimpleClassWithoutComparisonOperators_ComparerWithEquals_,
                    MySimpleClassWithoutComparisonOperators_ComparerWithCompare_>;
            DoTestForConcreteContainer_AllTestsWhichDontRequireComparer_For_Type_<Mapping_stdmap<SimpleClassWithoutComparisonOperators, SimpleClassWithoutComparisonOperators, SimpleClassWithoutComparisonOperators_Mapping_stdmap_TRAITS>> ();
        }

        Test2_SimpleBaseClassConversionTraitsConfusion_ ();

        Test3_SimpleMappingTest_WhichRequiresExplcitValueComparer ();

        Test4_MappingCTOROverloads_::DoIt ();

        ExampleCTORS_Test_5_::DoTest ();

        Where_Test_6_::DoAll ();
        Where_Test_7_::DoAll ();
    }
}

int main (int argc, const char* argv[])
{
    Stroika::TestHarness::Setup ();
    return Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
}
