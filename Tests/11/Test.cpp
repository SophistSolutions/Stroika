/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
//  TEST    Foundation::Containers::Association
//      \note Code-Status:  <a href="Code-Status.md#Beta">Beta</a>
#include "Stroika/Foundation/StroikaPreComp.h"

#include <iostream>

#include "Stroika/Foundation/Containers/Collection.h"

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Containers/Association.h"
#include "Stroika/Foundation/Containers/Concrete/Association_Array.h"
#include "Stroika/Foundation/Containers/Concrete/Association_LinkedList.h"
#include "Stroika/Foundation/Containers/Concrete/SortedAssociation_SkipList.h"
#include "Stroika/Foundation/Containers/Concrete/SortedAssociation_stdmultimap.h"
#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/Debug/Visualizations.h"

#include "Stroika/Frameworks/Test/ArchtypeClasses.h"
#include "Stroika/Frameworks/Test/TestHarness.h"

#include "../TestCommon/CommonTests_Association.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Containers;

using namespace Stroika::Frameworks;

using Test::ArchtypeClasses::AsIntsEqualsComparer;
using Test::ArchtypeClasses::AsIntsLessComparer;
using Test::ArchtypeClasses::AsIntsThreeWayComparer;
using Test::ArchtypeClasses::OnlyCopyableMoveable;
using Test::ArchtypeClasses::OnlyCopyableMoveableAndTotallyOrdered;

using Concrete::Association_Array;
using Concrete::Association_LinkedList;
using Concrete::SortedAssociation_SkipList;
using Concrete::SortedAssociation_stdmultimap;

#if qHasFeature_GoogleTest
namespace {
    template <typename CONCRETE_CONTAINER>
    void DoTestForConcreteContainer_ ()
    {
        using namespace CommonTests::AssociationTests;
        SimpleAssociationTest_All_ (DEFAULT_TESTING_SCHEMA<CONCRETE_CONTAINER>{});
        SimpleAssociationTest_WithDefaultEqComparer_ (DEFAULT_TESTING_SCHEMA<CONCRETE_CONTAINER>{});
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
    GTEST_TEST (Foundation_Containers_Association, FACTORY_DEFAULT)
    {
        Debug::TraceContextBumper ctx{"{}::FACTORY_DEFAULT"};
        DoTestForConcreteContainer_<Association<size_t, size_t>> ();
        DoTestForConcreteContainer_<Association<OnlyCopyableMoveableAndTotallyOrdered, OnlyCopyableMoveableAndTotallyOrdered>> ();
        DoTestForConcreteContainer_<Association<OnlyCopyableMoveable, OnlyCopyableMoveable>> (
            [] () { return Association<OnlyCopyableMoveable, OnlyCopyableMoveable> (AsIntsEqualsComparer<OnlyCopyableMoveable>{}); },
            AsIntsEqualsComparer<OnlyCopyableMoveable>{});
    }
}

GTEST_TEST (Foundation_Containers_Association, Association_Array)
{
    Debug::TraceContextBumper ctx{"{}::Association_Array"};
    DoTestForConcreteContainer_<Association_Array<size_t, size_t>> ();
    DoTestForConcreteContainer_<Association_Array<OnlyCopyableMoveableAndTotallyOrdered, OnlyCopyableMoveableAndTotallyOrdered>> ();
    DoTestForConcreteContainer_<Association_Array<OnlyCopyableMoveable, OnlyCopyableMoveable>> (
        [] () { return Association_Array<OnlyCopyableMoveable, OnlyCopyableMoveable> (AsIntsEqualsComparer<OnlyCopyableMoveable>{}); },
        AsIntsEqualsComparer<OnlyCopyableMoveable>{});
}

GTEST_TEST (Foundation_Containers_Association, Association_LinkedList)
{
    Debug::TraceContextBumper ctx{"{}::Association_LinkedList"};
    DoTestForConcreteContainer_<Association_LinkedList<size_t, size_t>> ();
    DoTestForConcreteContainer_<Association_LinkedList<OnlyCopyableMoveableAndTotallyOrdered, OnlyCopyableMoveableAndTotallyOrdered>> ();
    // DoTestForConcreteContainer_AllTestsWhichDontRequireComparer_For_Type_<Association_LinkedList<OnlyCopyableMoveable, SimpleClassWithoutComparisonOperators, SimpleClassWithoutComparisonOperators_AssociationTRAITS>> ();
    DoTestForConcreteContainer_<Association_LinkedList<OnlyCopyableMoveable, OnlyCopyableMoveable>> (
        [] () { return Association_LinkedList<OnlyCopyableMoveable, OnlyCopyableMoveable> (AsIntsEqualsComparer<OnlyCopyableMoveable>{}); },
        AsIntsEqualsComparer<OnlyCopyableMoveable>{});
}

GTEST_TEST (Foundation_Containers_Association, SortedAssociation_stdmultimap)
{
    Debug::TraceContextBumper ctx{"{}::SortedAssociation_stdmultimap"};
    DoTestForConcreteContainer_<SortedAssociation_stdmultimap<size_t, size_t>> ();
    DoTestForConcreteContainer_<SortedAssociation_stdmultimap<OnlyCopyableMoveableAndTotallyOrdered, OnlyCopyableMoveableAndTotallyOrdered>> ();
    DoTestForConcreteContainer_<SortedAssociation_stdmultimap<OnlyCopyableMoveable, OnlyCopyableMoveable>> (
        [] () {
            return SortedAssociation_stdmultimap<OnlyCopyableMoveable, OnlyCopyableMoveable> (AsIntsLessComparer<OnlyCopyableMoveable>{});
        },
        AsIntsEqualsComparer<OnlyCopyableMoveable>{});
}

GTEST_TEST (Foundation_Containers_Association, SortedAssociation_SkipList)
{
    Debug::TraceContextBumper ctx{"{}::SortedAssociation_SkipList"};
    DoTestForConcreteContainer_<SortedAssociation_SkipList<size_t, size_t>> ();
    DoTestForConcreteContainer_<SortedAssociation_SkipList<OnlyCopyableMoveableAndTotallyOrdered, OnlyCopyableMoveableAndTotallyOrdered>> ();
    DoTestForConcreteContainer_<SortedAssociation_SkipList<OnlyCopyableMoveable, OnlyCopyableMoveable>> (
        [] () {
            return SortedAssociation_SkipList<OnlyCopyableMoveable, OnlyCopyableMoveable> (AsIntsThreeWayComparer<OnlyCopyableMoveable>{});
        },
        AsIntsEqualsComparer<OnlyCopyableMoveable>{});
    {
        SortedAssociation_SkipList<size_t, size_t> x;
        x.ReBalance (); // just to assure compiles - no easy way to test decently...
    }
}

GTEST_TEST (Foundation_Containers_Association, SimpleBaseClassConversionTraitsConfusion_)
{
    Debug::TraceContextBumper     ctx{"{}::SimpleBaseClassConversionTraitsConfusion_"};
    SortedAssociation<int, float> xxxyy  = Concrete::SortedAssociation_stdmultimap<int, float> ();
    Association<int, float>       xxxyy1 = Concrete::SortedAssociation_stdmultimap<int, float> ();
}

namespace {
    namespace Test4_AssociationCTOROverloads_::xPrivate_ {
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
        using CONTAINER_OF_PAIR_KEY_T = Association<int, A>;
        using T                       = KeyValuePair<KEY_TYPE, VALUE_TYPE>;
    }
    GTEST_TEST (Foundation_Containers_Association, AssociationCTOROverloads_)
    {
        Debug::TraceContextBumper ctx{"{}::AssociationCTOROverloads_"};
        using namespace Test4_AssociationCTOROverloads_::xPrivate_;
        Association<int, A> from;

        static_assert (Traversal::IIterableOf<Association<int, A>, KeyValuePair<int, A>>);
        static_assert (Traversal::IIterableOf<Association<int, B>, KeyValuePair<int, B>>);

        Association<int, B> to1;
        for (auto i : from) {
            to1.Add (i);
        }
        Association<int, B> to2{from};
    }
}

namespace {
    GTEST_TEST (Foundation_Containers_Association, ExampleCTORS_)
    {
        Debug::TraceContextBumper ctx{"{}::ExampleCTORS_"};
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

namespace {
    GTEST_TEST (Foundation_Containers_Association, Where)
    {
        Debug::TraceContextBumper ctx{"{}::Where"};
        {
            Association<int, int> m{KeyValuePair<int, int>{1, 3}, KeyValuePair<int, int>{2, 4}, KeyValuePair<int, int>{3, 5},
                                    KeyValuePair<int, int>{4, 5}, KeyValuePair<int, int>{5, 7}};
            EXPECT_TRUE ((m.Where ([] (const KeyValuePair<int, int>& value) { return Math::IsPrime (value.fKey); }) ==
                          Association<int, int>{KeyValuePair<int, int>{2, 4}, KeyValuePair<int, int>{3, 5}, KeyValuePair<int, int>{5, 7}}));
            EXPECT_TRUE ((m.Where ([] (int key) { return Math::IsPrime (key); }) ==
                          Association<int, int>{KeyValuePair<int, int>{2, 4}, KeyValuePair<int, int>{3, 5}, KeyValuePair<int, int>{5, 7}}));
        }
        {
            // same but letting system guess type of arg to association
            Association<int, int> m{{1, 3}, {2, 4}, {3, 5}, {4, 5}, {5, 7}};
            EXPECT_TRUE ((m.Where ([] (const KeyValuePair<int, int>& value) { return Math::IsPrime (value.fKey); }) ==
                          Association<int, int>{{2, 4}, {3, 5}, {5, 7}}));
            EXPECT_TRUE ((m.Where ([] (int key) { return Math::IsPrime (key); }) ==
                          Association<int, int>{KeyValuePair<int, int>{2, 4}, KeyValuePair<int, int>{3, 5}, KeyValuePair<int, int>{5, 7}}));
        }
        {
            // same but using pair<>
            Association<int, int> m{pair<int, int>{1, 3}, pair<int, int>{2, 4}, pair<int, int>{3, 5}, pair<int, int>{4, 5}, pair<int, int>{5, 7}};
            EXPECT_TRUE ((m.Where ([] (const KeyValuePair<int, int>& value) { return Math::IsPrime (value.fKey); }) ==
                          Association<int, int>{pair<int, int>{2, 4}, pair<int, int>{3, 5}, pair<int, int>{5, 7}}));
            EXPECT_TRUE ((m.Where ([] (int key) { return Math::IsPrime (key); }) ==
                          Association<int, int>{pair<int, int>{2, 4}, pair<int, int>{3, 5}, pair<int, int>{5, 7}}));
        }
        {
            // similar but example has duplicates
            Association<int, int> m{pair<int, int>{1, 3}, pair<int, int>{2, 3}, pair<int, int>{2, 4},
                                    pair<int, int>{3, 5}, pair<int, int>{4, 5}, pair<int, int>{5, 7}};
            EXPECT_TRUE ((m.Where ([] (const KeyValuePair<int, int>& value) { return Math::IsPrime (value.fKey); }) ==
                          Association<int, int>{pair<int, int>{2, 3}, pair<int, int>{2, 4}, pair<int, int>{3, 5}, pair<int, int>{5, 7}}));
            EXPECT_TRUE ((m.Where ([] (int key) { return Math::IsPrime (key); }) ==
                          Association<int, int>{pair<int, int>{2, 3}, pair<int, int>{2, 4}, pair<int, int>{3, 5}, pair<int, int>{5, 7}}));
        }
    }
}

namespace {
    GTEST_TEST (Foundation_Containers_Association, WithKeys)
    {
        Debug::TraceContextBumper ctx{"{}::WithKeys"};
        Association<int, int>     m{{1, 3}, {2, 4}, {3, 5}, {4, 5}, {5, 7}};
        EXPECT_EQ (m.WithKeys ({2, 5}), (Association<int, int>{{2, 4}, {5, 7}}));
    }
}

namespace {
    GTEST_TEST (Foundation_Containers_Association, ClearBug)
    {
        Debug::TraceContextBumper ctx{"{}::ClearBug"};
        // http://stroika-bugs.sophists.com/browse/STK-541
        Association<int, int> m{KeyValuePair<int, int>{1, 3}, KeyValuePair<int, int>{2, 4}, KeyValuePair<int, int>{3, 5},
                                KeyValuePair<int, int>{4, 5}, KeyValuePair<int, int>{5, 7}};
        Association<int, int> mm{move (m)};
        // SEE http://stroika-bugs.sophists.com/browse/STK-541  - this call to clear is ILLEGAL - after m has been moved from
        //m.clear ();
    }
}

namespace {
    GTEST_TEST (Foundation_Containers_Association, BasicNewAssociationRules)
    {
        Debug::TraceContextBumper ctx{"{}::BasicNewAssociationRules"};
        Association<int, int>     m;
        m.Add (1, 2);
        m.Add (1, 2);
        EXPECT_EQ (m.size (), 2u);
        EXPECT_TRUE ((m.Lookup (1).MultiSetEquals (Traversal::Iterable<int>{2, 2})));
        EXPECT_TRUE (m.Lookup (2).empty ());
        m.Add (1, 3);
        EXPECT_TRUE ((m.Lookup (1).MultiSetEquals (Traversal::Iterable<int>{2, 3, 2})));

        Association<int, int> m2;
        m2.Add (1, 3);
        m2.Add (1, 2);
        EXPECT_TRUE (m != m2);
        m2.Add (1, 2);
        EXPECT_EQ (m, m2);
    }
}

namespace {
    GTEST_TEST (Foundation_Containers_Association, CTORWithComparerAndContainer)
    {
        Debug::TraceContextBumper ctx{"{}::CTORWithComparerAndContainer"};
        using namespace Characters;
        {
            Association<String, String> parameters{String::EqualsComparer{Characters::eCaseInsensitive}};
            // http://stroika-bugs.sophists.com/browse/STK-738 (and see other workarounds in other files)
            Association<String, String> parameters2{String::EqualsComparer{Characters::eCaseInsensitive}, parameters};
        }
    }
}

namespace {
    GTEST_TEST (Foundation_Containers_Association, Association_Array_ArrayAPITests_)
    {
        Debug::TraceContextBumper   ctx{"{}::Association_Array_ArrayAPITests_"};
        Association_Array<int, int> a;
        a.reserve (3);
        EXPECT_EQ (a.capacity (), 3u);
        a.shrink_to_fit ();
        EXPECT_EQ (a.capacity (), 0u);
    }
}

namespace {
    GTEST_TEST (Foundation_Containers_Association, Cleanups)
    {
        EXPECT_TRUE (OnlyCopyableMoveableAndTotallyOrdered::GetTotalLiveCount () == 0 and OnlyCopyableMoveable::GetTotalLiveCount () == 0); // simple portable leak check
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
