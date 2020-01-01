/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
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

#include "../TestCommon/CommonTests_Queue.h"
#include "../TestHarness/SimpleClass.h"
#include "../TestHarness/TestHarness.h"

using namespace Stroika;
using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Containers;

using Concrete::Queue_Array;
using Concrete::Queue_DoublyLinkedList;

namespace {
    template <typename CONCRETE_CONTAINER, typename EQUALS_COMPARER>
    void SimpleQueueTest_All_NotRequiringEquals_For_Type ()
    {
        CommonTests::QueueTests::SimpleQueueTest_All_NotRequiringEquals_For_Type<CONCRETE_CONTAINER, EQUALS_COMPARER> ();
    }

    template <typename CONCRETE_CONTAINER, typename EQUALS_COMPARER>
    void SimpleQueueTest_All_For_Type ()
    {
        CommonTests::QueueTests::SimpleQueueTest_All_For_Type<CONCRETE_CONTAINER, EQUALS_COMPARER> ();
    }
}

namespace {
    void DoRegressionTests_ ()
    {
        using COMPARE_SIZET       = std::equal_to<size_t>;
        using COMPARE_SimpleClass = std::equal_to<SimpleClass>;
        struct COMPARE_SimpleClassWithoutComparisonOperators {
            using value_type = SimpleClassWithoutComparisonOperators;
            bool operator() (value_type v1, value_type v2) const
            {
                return v1.GetValue () == v2.GetValue ();
            }
        };

        SimpleQueueTest_All_For_Type<Queue<size_t>, COMPARE_SIZET> ();
        SimpleQueueTest_All_For_Type<Queue<SimpleClass>, COMPARE_SimpleClass> ();
        SimpleQueueTest_All_NotRequiringEquals_For_Type<Queue<SimpleClassWithoutComparisonOperators>, COMPARE_SimpleClassWithoutComparisonOperators> ();
        SimpleQueueTest_All_For_Type<Queue<SimpleClassWithoutComparisonOperators>, COMPARE_SimpleClassWithoutComparisonOperators> ();

        SimpleQueueTest_All_For_Type<Queue_Array<size_t>, COMPARE_SIZET> ();
        SimpleQueueTest_All_For_Type<Queue_Array<SimpleClass>, COMPARE_SimpleClass> ();
        SimpleQueueTest_All_NotRequiringEquals_For_Type<Queue_Array<SimpleClassWithoutComparisonOperators>, COMPARE_SimpleClassWithoutComparisonOperators> ();
        SimpleQueueTest_All_For_Type<Queue_Array<SimpleClassWithoutComparisonOperators>, COMPARE_SimpleClassWithoutComparisonOperators> ();

        SimpleQueueTest_All_For_Type<Queue_DoublyLinkedList<size_t>, COMPARE_SIZET> ();
        SimpleQueueTest_All_For_Type<Queue_DoublyLinkedList<SimpleClass>, COMPARE_SimpleClass> ();
        SimpleQueueTest_All_NotRequiringEquals_For_Type<Queue_DoublyLinkedList<SimpleClassWithoutComparisonOperators>, COMPARE_SimpleClassWithoutComparisonOperators> ();
        SimpleQueueTest_All_For_Type<Queue_DoublyLinkedList<SimpleClassWithoutComparisonOperators>, COMPARE_SimpleClassWithoutComparisonOperators> ();
    }
}

int main ([[maybe_unused]] int argc, [[maybe_unused]] const char* argv[])
{
    Stroika::TestHarness::Setup ();
    return Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
}
