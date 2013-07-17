/*
 * Copyright(c) Sophist Solutions Inc. 1990-2013.  All rights reserved
 */
//      TEST    Foundation::Containers::Deque
//      STATUS  PRELIMINARY
#include    "Stroika/Foundation/StroikaPreComp.h"

#include    <iostream>
#include    <sstream>

#include    "Stroika/Foundation/Containers/Deque.h"
#include    "Stroika/Foundation/Containers/Concrete/Deque_DoublyLinkedList.h"
#include    "Stroika/Foundation/Debug/Assertions.h"
#include    "Stroika/Foundation/Debug/Trace.h"


#include    "../TestHarness/SimpleClass.h"
#include    "../TestCommon/CommonTests_Queue.h"
#include    "../TestHarness/TestHarness.h"



using   namespace   Stroika;
using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Containers;

using   Concrete::Deque_DoublyLinkedList;





namespace {
    namespace Test1_BasicDequeTest_ {
        template <typename CONCRETE_CONTAINER, typename EQUALS_COMPARER>
        void    DoAllTests_ ()
        {
            // test DQUEUE METHODS - NYI
        }
    }
}




namespace {
    template <typename CONCRETE_CONTAINER, typename EQUALS_COMPARER>
    void    SimpleQueueTest_All_NotRequiringEquals_For_Type ()
    {
        CommonTests::QueueTests::SimpleQueueTest_All_NotRequiringEquals_For_Type<CONCRETE_CONTAINER, EQUALS_COMPARER> ();
    }
    template <typename CONCRETE_CONTAINER, typename EQUALS_COMPARER>
    void    SimpleQueueTest_All_For_Type ()
    {
        CommonTests::QueueTests::SimpleQueueTest_All_For_Type<CONCRETE_CONTAINER, EQUALS_COMPARER> ();
        Test1_BasicDequeTest_::DoAllTests_<CONCRETE_CONTAINER, EQUALS_COMPARER> ();
    }
}





namespace   {
    void    DoRegressionTests_ ()
    {
        typedef Common::ComparerWithEquals<size_t>  COMPARE_SIZET;
        typedef Common::ComparerWithEquals<SimpleClass>  COMPARE_SimpleClass;
        struct  COMPARE_SimpleClassWithoutComparisonOperators {
            typedef SimpleClassWithoutComparisonOperators ElementType;
            static  bool    Equals (ElementType v1, ElementType v2) {
                return v1.GetValue () == v2.GetValue ();
            }
        };

        typedef Deque_DefaultTraits<SimpleClassWithoutComparisonOperators, COMPARE_SimpleClassWithoutComparisonOperators> Deque_SimpleClassWithoutComparisonOperators_Comparer_Traits;

        SimpleQueueTest_All_For_Type<Deque<size_t>, COMPARE_SIZET> ();
        SimpleQueueTest_All_For_Type<Deque<SimpleClass>, COMPARE_SimpleClass> ();
        SimpleQueueTest_All_NotRequiringEquals_For_Type<Deque<SimpleClassWithoutComparisonOperators>, COMPARE_SimpleClassWithoutComparisonOperators> ();
        SimpleQueueTest_All_For_Type<Deque<SimpleClassWithoutComparisonOperators, Deque_SimpleClassWithoutComparisonOperators_Comparer_Traits>, COMPARE_SimpleClassWithoutComparisonOperators> ();

        SimpleQueueTest_All_For_Type<Deque_DoublyLinkedList<size_t>, COMPARE_SIZET> ();
        SimpleQueueTest_All_For_Type<Deque_DoublyLinkedList<SimpleClass>, COMPARE_SimpleClass> ();
        SimpleQueueTest_All_NotRequiringEquals_For_Type<Deque_DoublyLinkedList<SimpleClassWithoutComparisonOperators>, COMPARE_SimpleClassWithoutComparisonOperators> ();
        SimpleQueueTest_All_For_Type<Deque_DoublyLinkedList<SimpleClassWithoutComparisonOperators, Deque_SimpleClassWithoutComparisonOperators_Comparer_Traits>, COMPARE_SimpleClassWithoutComparisonOperators> ();
    }
}




int     main (int argc, const char* argv[])
{
    Stroika::TestHarness::Setup ();
    Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
    return EXIT_SUCCESS;
}

