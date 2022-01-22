/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
//  TEST    Foundation::Containers::Association
//      STATUS  Alpha-Late
#include "Stroika/Foundation/StroikaPreComp.h"

#include <iostream>
#include <sstream>

#include "Stroika/Foundation/Containers/Collection.h"

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Containers/Association.h"
#include "Stroika/Foundation/Containers/Concrete/Association_Array.h"
#include "Stroika/Foundation/Containers/Concrete/Association_LinkedList.h"
#include "Stroika/Foundation/Containers/Concrete/Association_stdmultimap.h"
#include "Stroika/Foundation/Containers/Concrete/SortedAssociation_stdmultimap.h"
#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/Trace.h"

#include "../TestCommon/CommonTests_Association.h"
#include "../TestHarness/SimpleClass.h"
#include "../TestHarness/TestHarness.h"

using namespace Stroika;
using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Containers;

using Concrete::Association_Array;
using Concrete::Association_LinkedList;
using Concrete::Association_stdmultimap;

namespace {
    template <typename CONCRETE_CONTAINER>
    void DoTestForConcreteContainer_ ()
    {
        using namespace CommonTests::AssociationTests;
        SimpleAssociationTest_All_ (DEFAULT_TESTING_SCHEMA<CONCRETE_CONTAINER>{});
        SimpleAssociationTest_WithDefaultEqCompaerer_ (DEFAULT_TESTING_SCHEMA<CONCRETE_CONTAINER>{});
    }
    template <typename CONCRETE_CONTAINER, typename FACTORY, typename VALUE_EQUALS_COMPARER_TYPE>
    void DoTestForConcreteContainer_ (FACTORY factory, VALUE_EQUALS_COMPARER_TYPE valueEqualsComparer)
    {
        using namespace CommonTests::AssociationTests;
        auto testschema = DEFAULT_TESTING_SCHEMA<CONCRETE_CONTAINER, FACTORY, VALUE_EQUALS_COMPARER_TYPE>{factory, valueEqualsComparer};
        SimpleAssociationTest_All_ (testschema);
    }
}

namespace {
    void Test2_SimpleBaseClassConversionTraitsConfusion_ ()
    {
        Debug::TraceContextBumper     ctx{L"{}::Test2_SimpleBaseClassConversionTraitsConfusion_"};
        SortedAssociation<int, float> xxxyy  = Concrete::SortedAssociation_stdmultimap<int, float> ();
        Association<int, float>       xxxyy1 = Concrete::Association_stdmultimap<int, float> ();
    }
}

namespace {
    namespace Test4_AssociationCTOROverloads_ {
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
            using CONTAINER_OF_PAIR_KEY_T = Association<int, A>;
            using T                       = KeyValuePair<KEY_TYPE, VALUE_TYPE>;
        }
        void DoIt ()
        {
            Debug::TraceContextBumper ctx{L"{}::Test4_AssociationCTOROverloads_"};
            using namespace xPrivate_;
            Association<int, A> from;

            static_assert (Configuration::IsIterableOfT_v<Association<int, A>, KeyValuePair<int, A>>);
            static_assert (Configuration::IsIterableOfT_v<Association<int, B>, KeyValuePair<int, B>>);

            Association<int, B> to1;
            for (auto i : from) {
                to1.Add (i);
            }
            Association<int, B> to2{from};
        }
    }
}

namespace {
    namespace ExampleCTORS_Test_5_ {
        void DoTest ()
        {
            Debug::TraceContextBumper ctx{L"{}::ExampleCTORS_Test_5_"};
            // From Association<> CTOR docs
            Collection<pair<int, int>> c;
            std::multimap<int, int>    m;

            Association<int, int> m1 = {{1, 1}, {2, 2}, {3, 2}};
            Association<int, int> m2 = m1;
            Association<int, int> m3{m1};
            Association<int, int> m4{m1.begin (), m1.end ()};
            Association<int, int> m5{c};
            Association<int, int> m6{m};
            Association<int, int> m7{m.begin (), m.end ()};
            Association<int, int> m8{move (m1)};
            Association<int, int> m9{Common::DeclareEqualsComparer ([] (int l, int r) { return l == r; })};
        }
    }
}

namespace {
    namespace Where_Test_6_ {
        void DoAll ()
        {
            {
                Association<int, int> m{KeyValuePair<int, int>{1, 3}, KeyValuePair<int, int>{2, 4}, KeyValuePair<int, int>{3, 5}, KeyValuePair<int, int>{4, 5}, KeyValuePair<int, int>{5, 7}};
                VerifyTestResult ((m.Where ([] (const KeyValuePair<int, int>& value) { return Math::IsPrime (value.fKey); }) == Association<int, int>{KeyValuePair<int, int>{2, 4}, KeyValuePair<int, int>{3, 5}, KeyValuePair<int, int>{5, 7}}));
                VerifyTestResult ((m.Where ([] (int key) { return Math::IsPrime (key); }) == Association<int, int>{KeyValuePair<int, int>{2, 4}, KeyValuePair<int, int>{3, 5}, KeyValuePair<int, int>{5, 7}}));
            }
            {
                // same but using pair<>
                Association<int, int> m{pair<int, int>{1, 3}, pair<int, int>{2, 4}, pair<int, int>{3, 5}, pair<int, int>{4, 5}, pair<int, int>{5, 7}};
                VerifyTestResult ((m.Where ([] (const KeyValuePair<int, int>& value) { return Math::IsPrime (value.fKey); }) == Association<int, int>{pair<int, int>{2, 4}, pair<int, int>{3, 5}, pair<int, int>{5, 7}}));
                VerifyTestResult ((m.Where ([] (int key) { return Math::IsPrime (key); }) == Association<int, int>{pair<int, int>{2, 4}, pair<int, int>{3, 5}, pair<int, int>{5, 7}}));
            }
            {
                // simular but example has duplicates
                Association<int, int> m{pair<int, int>{1, 3}, pair<int, int>{2, 3}, pair<int, int>{2, 4}, pair<int, int>{3, 5}, pair<int, int>{4, 5}, pair<int, int>{5, 7}};
                VerifyTestResult ((m.Where ([] (const KeyValuePair<int, int>& value) { return Math::IsPrime (value.fKey); }) == Association<int, int>{pair<int, int>{2, 3}, pair<int, int>{2, 4}, pair<int, int>{3, 5}, pair<int, int>{5, 7}}));
                VerifyTestResult ((m.Where ([] (int key) { return Math::IsPrime (key); }) == Association<int, int>{pair<int, int>{2, 3}, pair<int, int>{2, 4}, pair<int, int>{3, 5}, pair<int, int>{5, 7}}));
            }
        }
    }
}

namespace {
    namespace WithKeys_Test_7_ {
        void DoAll ()
        {
            Association<int, int> m{KeyValuePair<int, int>{1, 3}, KeyValuePair<int, int>{2, 4}, KeyValuePair<int, int>{3, 5}, KeyValuePair<int, int>{4, 5}, KeyValuePair<int, int>{5, 7}};
            VerifyTestResult ((m.WithKeys (initializer_list<int>{2, 5}) == Association<int, int>{KeyValuePair<int, int>{2, 4}, KeyValuePair<int, int>{5, 7}}));
        }
    }
}

namespace {
    namespace ClearBug_Test_8_ {
        void DoAll ()
        {
            // https://stroika.atlassian.net/browse/STK-541
            Association<int, int> m{KeyValuePair<int, int>{1, 3}, KeyValuePair<int, int>{2, 4}, KeyValuePair<int, int>{3, 5}, KeyValuePair<int, int>{4, 5}, KeyValuePair<int, int>{5, 7}};
            Association<int, int> mm{move (m)};
            // SEE https://stroika.atlassian.net/browse/STK-541  - this call to clear is ILLEGAL - after m has been moved from
            //m.clear ();
        }
    }
}

namespace {
    namespace BasicNewAssociationRules_Test_9_ {
        void DoAll ()
        {
            {
                Association<int, int> m;
                m.Add (1, 2);
                m.Add (1, 2);
                VerifyTestResult (m.size () == 2);
                VerifyTestResult ((m.Lookup (1).MultiSetEquals (Traversal::Iterable<int>{2, 2})));
                VerifyTestResult (m.Lookup (2).empty ());
                m.Add (1, 3);
                VerifyTestResult ((m.Lookup (1).MultiSetEquals (Traversal::Iterable<int>{2, 3, 2})));

                Association<int, int> m2;
                m2.Add (1, 3);
                m2.Add (1, 2);
                VerifyTestResult (m != m2);
                m2.Add (1, 2);
                VerifyTestResult (m == m2);
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
                Association<String, String> parameters{String::EqualsComparer{Characters::CompareOptions::eCaseInsensitive}};
                // https://stroika.atlassian.net/browse/STK-738 (and see other workarounds in other files)
                Association<String, String> parameters2{String::EqualsComparer{Characters::CompareOptions::eCaseInsensitive}, parameters};
            }
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

        DoTestForConcreteContainer_<Association<size_t, size_t>> ();
        DoTestForConcreteContainer_<Association<SimpleClass, SimpleClass>> ();
        DoTestForConcreteContainer_<Association<SimpleClassWithoutComparisonOperators, SimpleClassWithoutComparisonOperators>> (
            [] () { return Association<SimpleClassWithoutComparisonOperators, SimpleClassWithoutComparisonOperators> (MySimpleClassWithoutComparisonOperators_ComparerWithEquals_{}); },
            MySimpleClassWithoutComparisonOperators_ComparerWithEquals_{});

        DoTestForConcreteContainer_<Association_Array<size_t, size_t>> ();
        DoTestForConcreteContainer_<Association_Array<SimpleClass, SimpleClass>> ();
        DoTestForConcreteContainer_<Association_Array<SimpleClassWithoutComparisonOperators, SimpleClassWithoutComparisonOperators>> (
            [] () { return Association_Array<SimpleClassWithoutComparisonOperators, SimpleClassWithoutComparisonOperators> (MySimpleClassWithoutComparisonOperators_ComparerWithEquals_{}); },
            MySimpleClassWithoutComparisonOperators_ComparerWithEquals_{});

        DoTestForConcreteContainer_<Association_LinkedList<size_t, size_t>> ();
        DoTestForConcreteContainer_<Association_LinkedList<SimpleClass, SimpleClass>> ();
        // DoTestForConcreteContainer_AllTestsWhichDontRequireComparer_For_Type_<Association_LinkedList<SimpleClassWithoutComparisonOperators, SimpleClassWithoutComparisonOperators, SimpleClassWithoutComparisonOperators_AssociationTRAITS>> ();
        DoTestForConcreteContainer_<Association_LinkedList<SimpleClassWithoutComparisonOperators, SimpleClassWithoutComparisonOperators>> (
            [] () { return Association_LinkedList<SimpleClassWithoutComparisonOperators, SimpleClassWithoutComparisonOperators> (MySimpleClassWithoutComparisonOperators_ComparerWithEquals_{}); },
            MySimpleClassWithoutComparisonOperators_ComparerWithEquals_{});

        DoTestForConcreteContainer_<Association_stdmultimap<size_t, size_t>> ();
        DoTestForConcreteContainer_<Association_stdmultimap<SimpleClass, SimpleClass>> ();
        {
            struct MySimpleClassWithoutComparisonOperators_ComparerWithLess_ : Common::ComparisonRelationDeclaration<Common::ComparisonRelationType::eStrictInOrder> {
                using value_type = SimpleClassWithoutComparisonOperators;
                bool operator() (const value_type& v1, const value_type& v2) const
                {
                    return v1.GetValue () < v2.GetValue ();
                }
            };
            DoTestForConcreteContainer_<Association_stdmultimap<SimpleClassWithoutComparisonOperators, SimpleClassWithoutComparisonOperators>> (
                [] () { return Association_stdmultimap<SimpleClassWithoutComparisonOperators, SimpleClassWithoutComparisonOperators> (MySimpleClassWithoutComparisonOperators_ComparerWithLess_{}); },
                //Common::mkEqualsComparerAdapter (MySimpleClassWithoutComparisonOperators_ComparerWithLess_{})
                MySimpleClassWithoutComparisonOperators_ComparerWithEquals_{});
        }

        Test2_SimpleBaseClassConversionTraitsConfusion_ ();

        //Test3_SimpleAssociationTest_WhichRequiresExplcitValueComparer ();

        Test4_AssociationCTOROverloads_::DoIt ();

        ExampleCTORS_Test_5_::DoTest ();

        Where_Test_6_::DoAll ();
        WithKeys_Test_7_::DoAll ();
        ClearBug_Test_8_::DoAll ();
        BasicNewAssociationRules_Test_9_::DoAll ();
        CTORWithComparerAndContainer_Test_10_::DoAll ();

        VerifyTestResult (SimpleClass::GetTotalLiveCount () == 0 and SimpleClassWithoutComparisonOperators::GetTotalLiveCount () == 0); // simple portable leak check
    }
}

int main ([[maybe_unused]] int argc, [[maybe_unused]] const char* argv[])
{
    Stroika::TestHarness::Setup ();
    return Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
}
