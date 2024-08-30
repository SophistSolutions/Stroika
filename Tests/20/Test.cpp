/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
//  TEST    Foundation::Containers::Queue
//      STATUS  PRELIMINARY
#include "Stroika/Foundation/StroikaPreComp.h"

#include <iostream>
#include <sstream>

#include "Stroika/Foundation/Containers/Concrete/Queue_Array.h"
#include "Stroika/Foundation/Containers/Concrete/Queue_DoublyLinkedList.h"
#include "Stroika/Foundation/Containers/Queue.h"
#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/Debug/Visualizations.h"

#include "Stroika/Frameworks/Test/ArchtypeClasses.h"
#include "Stroika/Frameworks/Test/TestHarness.h"

#include "../TestCommon/CommonTests_Queue.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Containers;

using namespace Stroika::Frameworks;

using Test::ArchtypeClasses::OnlyCopyableMoveable;
using Test::ArchtypeClasses::OnlyCopyableMoveableAndTotallyOrdered;

using Concrete::Queue_Array;
using Concrete::Queue_DoublyLinkedList;

#if qHasFeature_GoogleTest
namespace {
    template <typename CONCRETE_CONTAINER, typename EQUALS_COMPARER>
    void SimpleQueueTest_All_NotRequiringEquals_For_Type ()
    {
        CommonTests::QueueTests::SimpleQueueTest_All_NotRequiringEquals_For_Type<CONCRETE_CONTAINER, EQUALS_COMPARER> ();
    }

    template <typename CONCRETE_CONTAINER, Common::IEqualsComparer<typename CONCRETE_CONTAINER::value_type> EQUALS_COMPARER>
    void SimpleQueueTest_All_For_Type ()
    {
        CommonTests::QueueTests::SimpleQueueTest_All_For_Type<CONCRETE_CONTAINER, EQUALS_COMPARER> ();
    }
}

namespace {
    GTEST_TEST (Foundation_Containers_Queue, all)
    {
        using COMPARE_SIZET       = std::equal_to<size_t>;
        using COMPARE_SimpleClass = std::equal_to<OnlyCopyableMoveableAndTotallyOrdered>;
        struct COMPARE_OnlyCopyableMoveable : Common::ComparisonRelationDeclarationBase<Common::ComparisonRelationType::eEquals> {
            using value_type = OnlyCopyableMoveable;
            bool operator() (value_type v1, value_type v2) const
            {
                return static_cast<size_t> (v1) == static_cast<size_t> (v2);
            }
        };

        SimpleQueueTest_All_For_Type<Queue<size_t>, COMPARE_SIZET> ();
        SimpleQueueTest_All_For_Type<Queue<OnlyCopyableMoveableAndTotallyOrdered>, COMPARE_SimpleClass> ();
        SimpleQueueTest_All_NotRequiringEquals_For_Type<Queue<OnlyCopyableMoveable>, COMPARE_OnlyCopyableMoveable> ();
        SimpleQueueTest_All_For_Type<Queue<OnlyCopyableMoveable>, COMPARE_OnlyCopyableMoveable> ();

        SimpleQueueTest_All_For_Type<Queue_Array<size_t>, COMPARE_SIZET> ();
        SimpleQueueTest_All_For_Type<Queue_Array<OnlyCopyableMoveableAndTotallyOrdered>, COMPARE_SimpleClass> ();
        SimpleQueueTest_All_NotRequiringEquals_For_Type<Queue_Array<OnlyCopyableMoveable>, COMPARE_OnlyCopyableMoveable> ();
        SimpleQueueTest_All_For_Type<Queue_Array<OnlyCopyableMoveable>, COMPARE_OnlyCopyableMoveable> ();

        SimpleQueueTest_All_For_Type<Queue_DoublyLinkedList<size_t>, COMPARE_SIZET> ();
        SimpleQueueTest_All_For_Type<Queue_DoublyLinkedList<OnlyCopyableMoveableAndTotallyOrdered>, COMPARE_SimpleClass> ();
        SimpleQueueTest_All_NotRequiringEquals_For_Type<Queue_DoublyLinkedList<OnlyCopyableMoveable>, COMPARE_OnlyCopyableMoveable> ();
        SimpleQueueTest_All_For_Type<Queue_DoublyLinkedList<OnlyCopyableMoveable>, COMPARE_OnlyCopyableMoveable> ();

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
