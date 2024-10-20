/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
//  TEST    Foundation::Containers::Deque
//      STATUS  PRELIMINARY
#include "Stroika/Foundation/StroikaPreComp.h"

#include <iostream>

#include "Stroika/Foundation/Containers/Deque.h"

#include "Stroika/Foundation/Containers/Concrete/Deque_DoublyLinkedList.h"
#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/Debug/Visualizations.h"

#include "Stroika/Frameworks/Test/ArchtypeClasses.h"
#include "Stroika/Frameworks/Test/TestHarness.h"

#include "../TestCommon/CommonTests_Queue.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Containers;

using namespace Stroika::Frameworks;

using Test::ArchtypeClasses::AsIntsEqualsComparer;
using Test::ArchtypeClasses::OnlyCopyableMoveable;
using Test::ArchtypeClasses::OnlyCopyableMoveableAndTotallyOrdered;

using Concrete::Deque_DoublyLinkedList;

#if qHasFeature_GoogleTest
namespace {
    namespace Test1_BasicDequeTest_ {
        template <typename CONCRETE_CONTAINER, typename EQUALS_COMPARER>
        void DoAllTests_ ()
        {
            // test DQUEUE METHODS - NYI
        }
    }
}

namespace {
    template <typename CONCRETE_CONTAINER, Common::IEqualsComparer<typename CONCRETE_CONTAINER::value_type> EQUALS_COMPARER>
    void SimpleQueueTest_All_NotRequiringEquals_For_Type ()
    {
        CommonTests::QueueTests::SimpleQueueTest_All_NotRequiringEquals_For_Type<CONCRETE_CONTAINER, EQUALS_COMPARER> ();
    }
    template <typename CONCRETE_CONTAINER, Common::IEqualsComparer<typename CONCRETE_CONTAINER::value_type> EQUALS_COMPARER>
    void SimpleQueueTest_All_For_Type ()
    {
        CommonTests::QueueTests::SimpleQueueTest_All_For_Type<CONCRETE_CONTAINER, EQUALS_COMPARER> ();
        Test1_BasicDequeTest_::DoAllTests_<CONCRETE_CONTAINER, EQUALS_COMPARER> ();
    }
}

namespace {
    GTEST_TEST (Foundation_Deque, all)
    {
        using COMPARE_SIZET       = equal_to<size_t>;
        using COMPARE_SimpleClass = equal_to<OnlyCopyableMoveableAndTotallyOrdered>;

        using COMPARE_OnlyCopyableMoveable = AsIntsEqualsComparer<OnlyCopyableMoveable>;

        SimpleQueueTest_All_For_Type<Deque<size_t>, COMPARE_SIZET> ();
        SimpleQueueTest_All_For_Type<Deque<OnlyCopyableMoveableAndTotallyOrdered>, COMPARE_SimpleClass> ();
        SimpleQueueTest_All_NotRequiringEquals_For_Type<Deque<OnlyCopyableMoveable>, COMPARE_OnlyCopyableMoveable> ();
        SimpleQueueTest_All_For_Type<Deque<OnlyCopyableMoveable>, COMPARE_OnlyCopyableMoveable> ();

        SimpleQueueTest_All_For_Type<Deque_DoublyLinkedList<size_t>, COMPARE_SIZET> ();
        SimpleQueueTest_All_For_Type<Deque_DoublyLinkedList<OnlyCopyableMoveableAndTotallyOrdered>, COMPARE_SimpleClass> ();
        SimpleQueueTest_All_NotRequiringEquals_For_Type<Deque_DoublyLinkedList<OnlyCopyableMoveable>, COMPARE_OnlyCopyableMoveable> ();
        SimpleQueueTest_All_For_Type<Deque_DoublyLinkedList<OnlyCopyableMoveable>, COMPARE_OnlyCopyableMoveable> ();

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
