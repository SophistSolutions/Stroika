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
    void DoTestForConcreteContainer_ ()
    {
        using namespace CommonTests::MappingTests;
        SimpleMappingTest_All_ (DEFAULT_TESTING_SCHEMA<CONCRETE_CONTAINER>{});
        SimpleMappingTest_WithDefaultEqCompaerer_ (DEFAULT_TESTING_SCHEMA<CONCRETE_CONTAINER>{});
    }
    template <typename CONCRETE_CONTAINER, typename FACTORY, typename VALUE_EQUALS_COMPARER_TYPE>
    void DoTestForConcreteContainer_ (FACTORY factory, VALUE_EQUALS_COMPARER_TYPE valueEqualsComparer)
    {
        using namespace CommonTests::MappingTests;
        auto testschema = DEFAULT_TESTING_SCHEMA<CONCRETE_CONTAINER, FACTORY, VALUE_EQUALS_COMPARER_TYPE>{factory, valueEqualsComparer};
        SimpleMappingTest_All_ (testschema);
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
    namespace Test4_MappingCTOROverloads_ {
        namespace xPrivate_ {
            struct A;
            struct B;
            struct A {
                A ()         = default;
                A (const A&) = default;
                A (const B&) {}
            };
            struct B {
                B () = default;
                B (const A&) {}
                B (const B&) = default;
            };
            using Common::KeyValuePair;
            using KEY_TYPE                = int;
            using VALUE_TYPE              = B;
            using CONTAINER_OF_PAIR_KEY_T = Mapping<int, A>;
            using T                       = KeyValuePair<KEY_TYPE, VALUE_TYPE>;
        }
        void DoIt ()
        {
            Debug::TraceContextBumper ctx{L"{}::Test4_MappingCTOROverloads_"};
            using namespace xPrivate_;
            Mapping<int, A> from;

            static_assert (Configuration::IsIterableOfT_v<Mapping<int, A>, KeyValuePair<int, A>>);
            static_assert (Configuration::IsIterableOfT_v<Mapping<int, B>, KeyValuePair<int, B>>);

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
            VerifyTestResult ((m.Where ([](const KeyValuePair<int, int>& value) { return Math::IsPrime (value.fKey); }) == Mapping<int, int>{KeyValuePair<int, int>{2, 4}, KeyValuePair<int, int>{3, 5}, KeyValuePair<int, int>{5, 7}}));
            VerifyTestResult ((m.Where ([](int key) { return Math::IsPrime (key); }) == Mapping<int, int>{KeyValuePair<int, int>{2, 4}, KeyValuePair<int, int>{3, 5}, KeyValuePair<int, int>{5, 7}}));
        }
    }
}

namespace {
    namespace WithKeys_Test_7_ {
        void DoAll ()
        {
            Mapping<int, int> m{KeyValuePair<int, int>{1, 3}, KeyValuePair<int, int>{2, 4}, KeyValuePair<int, int>{3, 5}, KeyValuePair<int, int>{4, 5}, KeyValuePair<int, int>{5, 7}};
            VerifyTestResult ((m.WithKeys (initializer_list<int>{2, 5}) == Mapping<int, int>{KeyValuePair<int, int>{2, 4}, KeyValuePair<int, int>{5, 7}}));
        }
    }
}

namespace {
    void DoRegressionTests_ ()
    {
        struct MySimpleClassWithoutComparisonOperators_ComparerWithEquals_ : Common::ComparisonRelationDeclaration<Common::ComparisonRelationType::eEquals> {
            using value_type = SimpleClassWithoutComparisonOperators;
            bool operator() (const value_type& v1, const value_type& v2) const
            {
                return v1.GetValue () == v2.GetValue ();
            }
        };

        DoTestForConcreteContainer_<Mapping<size_t, size_t>> ();
        DoTestForConcreteContainer_<Mapping<SimpleClass, SimpleClass>> ();
        DoTestForConcreteContainer_<Mapping<SimpleClassWithoutComparisonOperators, SimpleClassWithoutComparisonOperators>> (
            []() { return Mapping<SimpleClassWithoutComparisonOperators, SimpleClassWithoutComparisonOperators> (MySimpleClassWithoutComparisonOperators_ComparerWithEquals_{}); },
            MySimpleClassWithoutComparisonOperators_ComparerWithEquals_{});

        DoTestForConcreteContainer_<Mapping_Array<size_t, size_t>> ();
        DoTestForConcreteContainer_<Mapping_Array<SimpleClass, SimpleClass>> ();
        DoTestForConcreteContainer_<Mapping_Array<SimpleClassWithoutComparisonOperators, SimpleClassWithoutComparisonOperators>> (
            []() { return Mapping_Array<SimpleClassWithoutComparisonOperators, SimpleClassWithoutComparisonOperators> (MySimpleClassWithoutComparisonOperators_ComparerWithEquals_{}); },
            MySimpleClassWithoutComparisonOperators_ComparerWithEquals_{});

        DoTestForConcreteContainer_<Mapping_LinkedList<size_t, size_t>> ();
        DoTestForConcreteContainer_<Mapping_LinkedList<SimpleClass, SimpleClass>> ();
        // DoTestForConcreteContainer_AllTestsWhichDontRequireComparer_For_Type_<Mapping_LinkedList<SimpleClassWithoutComparisonOperators, SimpleClassWithoutComparisonOperators, SimpleClassWithoutComparisonOperators_MappingTRAITS>> ();
        DoTestForConcreteContainer_<Mapping_LinkedList<SimpleClassWithoutComparisonOperators, SimpleClassWithoutComparisonOperators>> (
            []() { return Mapping_LinkedList<SimpleClassWithoutComparisonOperators, SimpleClassWithoutComparisonOperators> (MySimpleClassWithoutComparisonOperators_ComparerWithEquals_{}); },
            MySimpleClassWithoutComparisonOperators_ComparerWithEquals_{});

        DoTestForConcreteContainer_<Mapping_stdmap<size_t, size_t>> ();
        DoTestForConcreteContainer_<Mapping_stdmap<SimpleClass, SimpleClass>> ();
        {
            struct MySimpleClassWithoutComparisonOperators_ComparerWithLess_ : Common::ComparisonRelationDeclaration<Common::ComparisonRelationType::eStrictInOrder> {
                using value_type = SimpleClassWithoutComparisonOperators;
                bool operator() (const value_type& v1, const value_type& v2) const
                {
                    return v1.GetValue () < v2.GetValue ();
                }
            };
            DoTestForConcreteContainer_<Mapping_stdmap<SimpleClassWithoutComparisonOperators, SimpleClassWithoutComparisonOperators>> (
                []() { return Mapping_stdmap<SimpleClassWithoutComparisonOperators, SimpleClassWithoutComparisonOperators> (MySimpleClassWithoutComparisonOperators_ComparerWithLess_{}); },
                //Common::mkEqualsComparerAdapter (MySimpleClassWithoutComparisonOperators_ComparerWithLess_{})
                MySimpleClassWithoutComparisonOperators_ComparerWithEquals_{});
        }

        Test2_SimpleBaseClassConversionTraitsConfusion_ ();

        //Test3_SimpleMappingTest_WhichRequiresExplcitValueComparer ();

        Test4_MappingCTOROverloads_::DoIt ();

        ExampleCTORS_Test_5_::DoTest ();

        Where_Test_6_::DoAll ();
        WithKeys_Test_7_::DoAll ();
    }
}

int main ([[maybe_unused]] int argc, [[maybe_unused]] const char* argv[])
{
    Stroika::TestHarness::Setup ();
    return Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
}
