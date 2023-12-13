/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
//  TEST    Foundation::Containers::Stack
//      STATUS  PRELIMINARY
#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Containers/Stack.h"

#include "Stroika/Foundation/Containers/Concrete/Stack_LinkedList.h"
#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/Debug/Visualizations.h"

#include "Stroika/Frameworks/Test/ArchtypeClasses.h"
#include "Stroika/Frameworks/Test/TestHarness.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Containers;

using namespace Stroika::Frameworks;

using Test::ArchtypeClasses::SimpleClass;
using Test::ArchtypeClasses::SimpleClassWithoutComparisonOperators;

using Concrete::Stack_LinkedList;

namespace {
    template <typename StackOfT>
    void SimpleTest_1_ (StackOfT s)
    {
        StackOfT s2;
        StackOfT s3 = s;
    }
}

namespace {
    template <typename StackOfT>
    void SimpleTest_2_ (StackOfT s)
    {
        s.Push (1);
        EXPECT_TRUE (s.size () == 1);
        s.Push (2);
        EXPECT_TRUE (s.size () == 2);
        s.Pop ();
        EXPECT_TRUE (s.size () == 1);
        s.RemoveAll ();
        EXPECT_TRUE (s.size () == 0);
    }
}

namespace {
    template <typename StackOfT>
    void SimpleTest_3_Iteration_ (StackOfT s)
    {
#if 0
        m.Add (1, 2);
        EXPECT_TRUE (m.size () == 1);
        for (auto i : m) {
            EXPECT_TRUE (i.first == 1);
            EXPECT_TRUE (i.second == 2);
        }
        m.Add (1, 2);
        EXPECT_TRUE (m.size () == 1);
        for (auto i : m) {
            EXPECT_TRUE (i.first == 1);
            EXPECT_TRUE (i.second == 2);
        }
        m.Remove (1);
        EXPECT_TRUE (m.size () == 0);
        for (auto i : m) {
            EXPECT_TRUE (false);
        }
        m.Add (1, 2);
        m.Add (2, 3);
        m.Add (3, 4);
        unsigned int cnt = 0;
        for (auto i : m) {
            ++cnt;
            if (cnt == 1) {
                EXPECT_TRUE (i.first == 1);
                EXPECT_TRUE (i.second == 2);
            }
            if (cnt == 2) {
                EXPECT_TRUE (i.first == 2);
                EXPECT_TRUE (i.second == 3);
            }
            if (cnt == 3) {
                EXPECT_TRUE (i.first == 3);
                EXPECT_TRUE (i.second == 4);
            }
        }
        EXPECT_TRUE (cnt == 3);
#endif
        s.RemoveAll ();
        EXPECT_TRUE (s.size () == 0);
    }
}

namespace Test4_Equals {
    template <typename USING_STACK_CONTAINER, typename EQUALS_COMPARER>
    void DoAllTests_ ()
    {
        USING_STACK_CONTAINER s;
        USING_STACK_CONTAINER s2 = s;
        s.Push (1);
        s.Push (2);
        EXPECT_TRUE (s.size () == 2);
        USING_STACK_CONTAINER s3 = s;
        EXPECT_TRUE (typename USING_STACK_CONTAINER::template EqualsComparer<EQUALS_COMPARER>{}(s, s3));
        EXPECT_TRUE (not typename USING_STACK_CONTAINER::template EqualsComparer<EQUALS_COMPARER>{}(s, s2));
        EXPECT_TRUE (EQUALS_COMPARER{}(s.Pop (), 2));
    }
}

namespace {

    template <typename CONCRETE_STACK_TYPE, typename EQUALS_COMPARER>
    void Tests_All_For_Type_WhichDontRequireComparer_For_Type_ ()
    {
        CONCRETE_STACK_TYPE s;
        SimpleTest_1_<CONCRETE_STACK_TYPE> (s);
        SimpleTest_2_<CONCRETE_STACK_TYPE> (s);
        SimpleTest_3_Iteration_<CONCRETE_STACK_TYPE> (s);
    }

    template <typename CONCRETE_STACK_TYPE, typename EQUALS_COMPARER>
    void Tests_All_For_Type_ ()
    {
        Tests_All_For_Type_WhichDontRequireComparer_For_Type_<CONCRETE_STACK_TYPE, EQUALS_COMPARER> ();
        Test4_Equals::DoAllTests_<CONCRETE_STACK_TYPE, EQUALS_COMPARER> ();
    }
}

namespace {
    namespace Test3_StackContructionByValue_ {

        void Test ()
        {
            Stack<int> a;
            a.Push (1);
            a.Push (2);
            vector<int> aa (a.begin (), a.end ());
            Stack<int>  b{aa};
            EXPECT_TRUE (b.size () == 2);
            EXPECT_TRUE (b.Pop () == 2);
            EXPECT_TRUE (b.Pop () == 1);
        }
    }
}

namespace {
#if qHasFeature_GoogleTest
    GTEST_TEST (Foundation_Caching, all)
#else
    void DoRegressionTests_ ()
#endif
    {
        using COMPARE_SIZET       = equal_to<size_t>;
        using COMPARE_SimpleClass = equal_to<SimpleClass>;
        struct COMPARE_SimpleClassWithoutComparisonOperators : Common::ComparisonRelationDeclarationBase<Common::ComparisonRelationType::eEquals> {
            using value_type = SimpleClassWithoutComparisonOperators;
            bool operator() (value_type v1, value_type v2) const
            {
                return v1.GetValue () == v2.GetValue ();
            }
        };

        Tests_All_For_Type_<Stack<size_t>, COMPARE_SIZET> ();
        Tests_All_For_Type_<Stack<SimpleClass>, COMPARE_SimpleClass> ();
        Tests_All_For_Type_WhichDontRequireComparer_For_Type_<Stack<SimpleClassWithoutComparisonOperators>, COMPARE_SimpleClassWithoutComparisonOperators> ();
        Tests_All_For_Type_<Stack<SimpleClassWithoutComparisonOperators>, COMPARE_SimpleClassWithoutComparisonOperators> ();

        Tests_All_For_Type_<Stack_LinkedList<size_t>, COMPARE_SIZET> ();
        Tests_All_For_Type_<Stack_LinkedList<SimpleClass>, COMPARE_SimpleClass> ();
        Tests_All_For_Type_WhichDontRequireComparer_For_Type_<Stack_LinkedList<SimpleClassWithoutComparisonOperators>, COMPARE_SimpleClassWithoutComparisonOperators> ();
        Tests_All_For_Type_<Stack_LinkedList<SimpleClassWithoutComparisonOperators>, COMPARE_SimpleClassWithoutComparisonOperators> ();

        Test3_StackContructionByValue_::Test ();

        EXPECT_TRUE (SimpleClass::GetTotalLiveCount () == 0 and SimpleClassWithoutComparisonOperators::GetTotalLiveCount () == 0); // simple portable leak check
    }
}

int main (int argc, const char* argv[])
{
    Test::Setup (argc, argv);
#if qHasFeature_GoogleTest
    return RUN_ALL_TESTS ();
#else
    cerr << "Stroika regression tests require building with google test feature" << endl;
#endif
}
