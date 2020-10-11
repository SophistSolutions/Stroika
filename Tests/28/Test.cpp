/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
//  TEST    Foundation::Containers::Stack
//      STATUS  PRELIMINARY
#include "Stroika/Foundation/StroikaPreComp.h"

#include <iostream>
#include <sstream>

#include "Stroika/Foundation/Containers/Stack.h"

#include "Stroika/Foundation/Containers/Concrete/Stack_LinkedList.h"
#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/Trace.h"

#include "../TestHarness/SimpleClass.h"
#include "../TestHarness/TestHarness.h"

using namespace Stroika;
using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Containers;

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
        VerifyTestResult (s.size () == 1);
        s.Push (1);
        VerifyTestResult (s.size () == 2);
        s.Pop ();
        VerifyTestResult (s.size () == 1);
        s.RemoveAll ();
        VerifyTestResult (s.size () == 0);
    }
}

namespace {
    template <typename StackOfT>
    void SimpleTest_3_Iteration_ (StackOfT s)
    {
#if 0
        m.Add (1, 2);
        VerifyTestResult (m.size () == 1);
        for (auto i : m) {
            VerifyTestResult (i.first == 1);
            VerifyTestResult (i.second == 2);
        }
        m.Add (1, 2);
        VerifyTestResult (m.size () == 1);
        for (auto i : m) {
            VerifyTestResult (i.first == 1);
            VerifyTestResult (i.second == 2);
        }
        m.Remove (1);
        VerifyTestResult (m.size () == 0);
        for (auto i : m) {
            VerifyTestResult (false);
        }
        m.Add (1, 2);
        m.Add (2, 3);
        m.Add (3, 4);
        unsigned int cnt = 0;
        for (auto i : m) {
            cnt++;
            if (cnt == 1) {
                VerifyTestResult (i.first == 1);
                VerifyTestResult (i.second == 2);
            }
            if (cnt == 2) {
                VerifyTestResult (i.first == 2);
                VerifyTestResult (i.second == 3);
            }
            if (cnt == 3) {
                VerifyTestResult (i.first == 3);
                VerifyTestResult (i.second == 4);
            }
        }
        VerifyTestResult (cnt == 3);
#endif
        s.RemoveAll ();
        VerifyTestResult (s.size () == 0);
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
        VerifyTestResult (s.size () == 2);
        USING_STACK_CONTAINER s3 = s;
        //VerifyTestResult (s == s3);
        VerifyTestResult (typename USING_STACK_CONTAINER::template EqualsComparer<EQUALS_COMPARER>{}(s, s3));
        //VerifyTestResult (not (s != s3));

        //VerifyTestResult (s != s2);
        VerifyTestResult (not typename USING_STACK_CONTAINER::template EqualsComparer<EQUALS_COMPARER>{}(s, s2));
        //VerifyTestResult (not (s == s2));
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
    void DoRegressionTests_ ()
    {
        using COMPARE_SIZET       = equal_to<size_t>;
        using COMPARE_SimpleClass = equal_to<SimpleClass>;
        struct COMPARE_SimpleClassWithoutComparisonOperators {
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
    }
}

int main ([[maybe_unused]] int argc, [[maybe_unused]] const char* argv[])
{
    Stroika::TestHarness::Setup ();
    return Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
}
