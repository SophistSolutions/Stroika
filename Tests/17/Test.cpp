/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
//  TEST    Foundation::Containers::Mapping
//      \note Code-Status:  <a href="Code-Status.md#Beta">Beta</a>
#include "Stroika/Foundation/StroikaPreComp.h"

#include <iostream>

#include "Stroika/Foundation/Containers/Collection.h"

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Containers/Concrete/Mapping_Array.h"
#include "Stroika/Foundation/Containers/Concrete/Mapping_LinkedList.h"
#include "Stroika/Foundation/Containers/Concrete/Mapping_SkipList.h"
#include "Stroika/Foundation/Containers/Concrete/Mapping_stdhashmap.h"
#include "Stroika/Foundation/Containers/Concrete/Mapping_stdmap.h"
#include "Stroika/Foundation/Containers/Concrete/SortedMapping_stdmap.h"
#include "Stroika/Foundation/Containers/Mapping.h"
#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/Debug/Visualizations.h"

#include "Stroika/Frameworks/Test/ArchtypeClasses.h"
#include "Stroika/Frameworks/Test/TestHarness.h"

#include "../TestCommon/CommonTests_Mapping.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Containers;

using namespace Stroika::Frameworks;

using Test::ArchtypeClasses::SimpleClass;
using Test::ArchtypeClasses::SimpleClassWithoutComparisonOperators;

using Concrete::Mapping_Array;
using Concrete::Mapping_LinkedList;
using Concrete::Mapping_SkipList;
using Concrete::Mapping_stdhashmap;
using Concrete::Mapping_stdmap;

#if qHasFeature_GoogleTest
namespace {
    template <typename CONCRETE_CONTAINER>
    void DoTestForConcreteContainer_ ()
    {
        using namespace CommonTests::MappingTests;
        if constexpr (constructible_from<CONCRETE_CONTAINER>) {
            SimpleMappingTest_All_ (DEFAULT_TESTING_SCHEMA<CONCRETE_CONTAINER>{});
            SimpleMappingTest_WithDefaultEqCompaerer_ (DEFAULT_TESTING_SCHEMA<CONCRETE_CONTAINER>{});
        }
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
    GTEST_TEST (Foundation_Containers_Mapping, Test2_SimpleBaseClassConversionTraitsConfusion_)
    {
        Debug::TraceContextBumper ctx{"{}::Test2_SimpleBaseClassConversionTraitsConfusion_"};
        SortedMapping<int, float> xxxyy  = Concrete::SortedMapping_stdmap<int, float>{};
        Mapping<int, float>       xxxyy1 = Concrete::Mapping_stdmap<int, float>{};
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
                A (const B&)
                {
                }
            };
            struct B {
                B () = default;
                B (const A&)
                {
                }
                B (const B&) = default;
            };
            using Common::KeyValuePair;
            using KEY_TYPE                = int;
            using VALUE_TYPE              = B;
            using CONTAINER_OF_PAIR_KEY_T = Mapping<int, A>;
            using T                       = KeyValuePair<KEY_TYPE, VALUE_TYPE>;
        }
    }

    GTEST_TEST (Foundation_Containers_Mapping, Test4_MappingCTOROverloads_)
    {
        Debug::TraceContextBumper ctx{"{}::Test4_MappingCTOROverloads_"};
        using namespace Test4_MappingCTOROverloads_::xPrivate_;
        Mapping<int, A> from;
        static_assert (Traversal::IIterableOf<Mapping<int, A>, KeyValuePair<int, A>>);
        static_assert (Traversal::IIterableOf<Mapping<int, B>, KeyValuePair<int, B>>);
        Mapping<int, B> to1;
        for (auto i : from) {
            to1.Add (i);
        }
        Mapping<int, B> to2{from};
    }
}

namespace {
    namespace ExampleCTORS_Test_5_ {
        void DoTest ()
        {
            Debug::TraceContextBumper ctx{"{}::ExampleCTORS_Test_5_"};
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
            Mapping<int, int> m9{Common::DeclareEqualsComparer ([] (int l, int r) { return l == r; })};

            {
                using Characters::String;
                const auto kReference1a_ = Mapping<String, String>{{KeyValuePair<String, String>{"Content-Length", "3"}}};
                const auto kReference1b_ =
                    Mapping<String, String>{{KeyValuePair<String, String>{"Content-Length", "3"}, KeyValuePair<String, String>{"xx", "3"}}};
                const auto kReference1c_ = Mapping<String, String>{KeyValuePair<String, String>{L"Content-Length", L"3"}};
                const auto kReference2a_ = Mapping<String, String>{{pair<String, String>{L"Content-Length", "3"}}};
                const auto kReference2b_ = Mapping<String, String>{{pair<String, String>{"Content-Length", "3"}, pair<String, String>{"xx", "3"}}};
                const auto kReference2c_ = Mapping<String, String>{pair<String, String>{"Content-Length", "3"}, pair<String, String>{"xx", "3"}};
                const auto kReference3a_ = Mapping<String, String>{{{"Content-Length", "3"}}};
                EXPECT_EQ (kReference3a_.size (), 1u);
                using Characters::operator""_k;
                const auto kReference3b_ = Mapping<String, String>{{{"Content-Length"_k, "3"_k}, {"x"_k, "3"_k}}}; // need _k on some compilers to avoid error due to invoke explicit String/2 (g++10) - not sure if bug or not but easy to avoid ambiguity
                EXPECT_EQ (kReference3b_.size (), 2u);
                const auto kReference3c_ = Mapping<String, String>{{"Content-Length", "3"}, {"x", "3"}};
                EXPECT_EQ (kReference3c_.size (), 2u);
            }
        }
    }
}

namespace {
    namespace Where_Test_6_ {
        void DoAll ()
        {
            Mapping<int, int> m{{1, 3}, {2, 4}, {3, 5}, {4, 5}, {5, 7}};
            EXPECT_EQ (m.Where ([] (const KeyValuePair<int, int>& value) { return Math::IsPrime (value.fKey); }),
                       (Mapping<int, int>{{2, 4}, {3, 5}, {5, 7}}));
            EXPECT_EQ (m.Where ([] (int key) { return Math::IsPrime (key); }), (Mapping<int, int>{{2, 4}, {3, 5}, {5, 7}}));
        }
    }
}

namespace {
    namespace WithKeys_Test_7_ {
        void DoAll ()
        {
            Mapping<int, int> m{{1, 3}, {2, 4}, {3, 5}, {4, 5}, {5, 7}};
            EXPECT_EQ (m.WithKeys ({2, 5}), (Mapping<int, int>{{2, 4}, {5, 7}}));
        }
    }
}

namespace {
    namespace ClearBug_Test_8_ {
        void DoAll ()
        {
            // http://stroika-bugs.sophists.com/browse/STK-541
            Mapping<int, int> m{KeyValuePair<int, int>{1, 3}, KeyValuePair<int, int>{2, 4}, KeyValuePair<int, int>{3, 5},
                                KeyValuePair<int, int>{4, 5}, KeyValuePair<int, int>{5, 7}};
            Mapping<int, int> mm{move (m)};
            // SEE http://stroika-bugs.sophists.com/browse/STK-541  - this call to clear is ILLEGAL - after m has been moved from
            //m.clear ();
        }
    }
}

namespace {
    namespace AddVsAddIf_Test_9_ {
        void DoAll ()
        {
            {
                Mapping<int, int> m;
                m.Add (1, 2);
                EXPECT_TRUE (m[1] == 2);
                m.Add (1, 3);
                EXPECT_TRUE (m[1] == 3);
                EXPECT_TRUE (not m.Add (1, 4, AddReplaceMode::eAddIfMissing));
                EXPECT_TRUE (m[1] == 3);
                EXPECT_TRUE (m.Add (2, 3, AddReplaceMode::eAddIfMissing));
                EXPECT_TRUE (m[2] == 3);
            }
        }
    }
}

namespace {
    namespace CTORWithComparerAndContainer_Test_10_ {
        void DoAll ()
        {
            using namespace Characters;
            {
                Mapping<String, String> parameters{String::EqualsComparer{Characters::eCaseInsensitive}};
                // http://stroika-bugs.sophists.com/browse/STK-738 (and see other workarounds in other files)
                Mapping<String, String> parameters2{String::EqualsComparer{Characters::eCaseInsensitive}, parameters};
            }
        }
    }
}

namespace {
    GTEST_TEST (Foundation_Containers_Mapping, all)
    {
        struct MySimpleClassWithoutComparisonOperators_ComparerWithEquals_
            : Common::ComparisonRelationDeclarationBase<Common::ComparisonRelationType::eEquals> {
            using value_type = SimpleClassWithoutComparisonOperators;
            bool operator() (const value_type& v1, const value_type& v2) const
            {
                return v1.GetValue () == v2.GetValue ();
            }
        };

        DoTestForConcreteContainer_<Mapping<size_t, size_t>> ();
        DoTestForConcreteContainer_<Mapping<SimpleClass, SimpleClass>> ();
        DoTestForConcreteContainer_<Mapping<SimpleClassWithoutComparisonOperators, SimpleClassWithoutComparisonOperators>> (
            [] () {
                return Mapping<SimpleClassWithoutComparisonOperators, SimpleClassWithoutComparisonOperators>{
                    MySimpleClassWithoutComparisonOperators_ComparerWithEquals_{}};
            },
            MySimpleClassWithoutComparisonOperators_ComparerWithEquals_{});

        DoTestForConcreteContainer_<Mapping_Array<size_t, size_t>> ();
        DoTestForConcreteContainer_<Mapping_Array<SimpleClass, SimpleClass>> ();
        DoTestForConcreteContainer_<Mapping_Array<SimpleClassWithoutComparisonOperators, SimpleClassWithoutComparisonOperators>> (
            [] () {
                return Mapping_Array<SimpleClassWithoutComparisonOperators, SimpleClassWithoutComparisonOperators>{
                    MySimpleClassWithoutComparisonOperators_ComparerWithEquals_{}};
            },
            MySimpleClassWithoutComparisonOperators_ComparerWithEquals_{});

        DoTestForConcreteContainer_<Mapping_SkipList<size_t, size_t>> ();
        DoTestForConcreteContainer_<Mapping_SkipList<SimpleClass, SimpleClass>> ();
        DoTestForConcreteContainer_<Mapping_SkipList<SimpleClassWithoutComparisonOperators, SimpleClassWithoutComparisonOperators>> (
            [] () {
                return Mapping_SkipList<SimpleClassWithoutComparisonOperators, SimpleClassWithoutComparisonOperators>{
                    [] (SimpleClassWithoutComparisonOperators l, SimpleClassWithoutComparisonOperators r) -> strong_ordering {
                        return l.GetValue () <=> r.GetValue ();
                    }};
            },
            MySimpleClassWithoutComparisonOperators_ComparerWithEquals_{});

        DoTestForConcreteContainer_<Mapping_LinkedList<size_t, size_t>> ();
        DoTestForConcreteContainer_<Mapping_LinkedList<SimpleClass, SimpleClass>> ();
        DoTestForConcreteContainer_<Mapping_LinkedList<SimpleClassWithoutComparisonOperators, SimpleClassWithoutComparisonOperators>> (
            [] () {
                return Mapping_LinkedList<SimpleClassWithoutComparisonOperators, SimpleClassWithoutComparisonOperators> (
                    MySimpleClassWithoutComparisonOperators_ComparerWithEquals_{});
            },
            MySimpleClassWithoutComparisonOperators_ComparerWithEquals_{});

        DoTestForConcreteContainer_<Mapping_stdmap<size_t, size_t>> ();
        DoTestForConcreteContainer_<Mapping_stdmap<SimpleClass, SimpleClass>> ();
        {
            struct MySimpleClassWithoutComparisonOperators_ComparerWithLess_
                : Common::ComparisonRelationDeclarationBase<Common::ComparisonRelationType::eStrictInOrder> {
                using value_type = SimpleClassWithoutComparisonOperators;
                bool operator() (const value_type& v1, const value_type& v2) const
                {
                    return v1.GetValue () < v2.GetValue ();
                }
            };
            DoTestForConcreteContainer_<Mapping_stdmap<SimpleClassWithoutComparisonOperators, SimpleClassWithoutComparisonOperators>> (
                [] () {
                    return Mapping_stdmap<SimpleClassWithoutComparisonOperators, SimpleClassWithoutComparisonOperators> (
                        MySimpleClassWithoutComparisonOperators_ComparerWithLess_{});
                },
                //Common::mkEqualsComparerAdapter (MySimpleClassWithoutComparisonOperators_ComparerWithLess_{})
                MySimpleClassWithoutComparisonOperators_ComparerWithEquals_{});
        }

        {
            {
                Mapping_stdhashmap<size_t, size_t> fred{};
                fred.Add (1, 2);
                fred.Add (1, 3);
                EXPECT_TRUE (fred.size () == 1);
            }
            {
                Mapping_stdhashmap<size_t, size_t> fred{};
                fred.Add (1, 2);
                fred.Add (2, 4);
                fred.Add (3, 9);
                EXPECT_TRUE (fred.size () == 3);
                EXPECT_TRUE ((fred == Mapping<size_t, size_t>{{1, 2}, {2, 4}, {3, 9}}));
            }
            {
                static_assert (Cryptography::Digest::IHashFunction<std::hash<size_t>, size_t>);
                Mapping_stdhashmap<size_t, size_t> fred{std::hash<size_t>{}, std::equal_to<size_t>{}};
                EXPECT_TRUE ((fred == Mapping_stdhashmap<size_t, size_t>{}));
            }
            {
                using Characters::String;
                Mapping_stdhashmap<String, String> m{};
                m.Add ("a", "alpha");
                m.Add ("b", "beta");
                m.Add ("c", "gamma");
                m.Add ("d", "delta");
                EXPECT_TRUE (m.size () == 4);
                EXPECT_TRUE ((m == Mapping<String, String>{{"a", "alpha"}, {"b", "beta"}, {"c", "gamma"}, {"d", "delta"}}));
            }

            DoTestForConcreteContainer_<Mapping_stdhashmap<size_t, size_t>> ();
            //DoTestForConcreteContainer_<Mapping_stdhashmap<SimpleClass, SimpleClass>> (); // -- wont work cuz not hashable
#if 0
            DoTestForConcreteContainer_<Mapping<SimpleClassWithoutComparisonOperators, SimpleClassWithoutComparisonOperators>> (
                [] () { return Mapping<SimpleClassWithoutComparisonOperators, SimpleClassWithoutComparisonOperators> (MySimpleClassWithoutComparisonOperators_ComparerWithEquals_{}); },
                MySimpleClassWithoutComparisonOperators_ComparerWithEquals_{});
#endif
        }

        ExampleCTORS_Test_5_::DoTest ();

        Where_Test_6_::DoAll ();
        WithKeys_Test_7_::DoAll ();
        ClearBug_Test_8_::DoAll ();
        AddVsAddIf_Test_9_::DoAll ();
        CTORWithComparerAndContainer_Test_10_::DoAll ();

        EXPECT_TRUE (SimpleClass::GetTotalLiveCount () == 0 and SimpleClassWithoutComparisonOperators::GetTotalLiveCount () == 0); // simple portable leak check
    }
}
#endif

int main (int argc, const char* argv[])
{
    Test::Setup (argc, argv);
#if qHasFeature_GoogleTest
    return RUN_ALL_TESTS ();
#else
    cerr << "Stroika regression tests require building with google test feature [  PASSED  ]" << endl;
#endif
}
