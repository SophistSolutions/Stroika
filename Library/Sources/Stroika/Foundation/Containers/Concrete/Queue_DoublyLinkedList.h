/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_Queue_DoublyLinkedList_h_
#define _Stroika_Foundation_Containers_Concrete_Queue_DoublyLinkedList_h_

#include    "../../StroikaPreComp.h"

#include    "../Queue.h"



/**
 *  \file
 *
 *  \version    <a href="code_status.html#Alpha-Late">Alpha-Late</a>
 *
 *  TODO:
 *      @todo   Correctly implement override of Iterator<T>::IRep::Equals()
 *
 *      @todo   Finish using CONTAINER_LOCK_HELPER_START() - synchronization support
 *              THEN - MAYBE - try todo better, but at least do this as starter
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace   Concrete {


                /**
                 *  \brief   Queue_DoublyLinkedList<T, TRAITS> is an Array-based concrete implementation of the Queue<T, TRAITS> container pattern.
                 *
                 *  \note   \em Thread-Safety   <a href="thread_safety.html#Automatically-Synchronized-Thread-Safety">Automatically-Synchronized-Thread-Safety</a>
                 *
                 */
                template    <typename T, typename TRAITS = Queue_DefaultTraits<T>>
                class   Queue_DoublyLinkedList : public Queue<T, TRAITS> {
                private:
                    using   inherited   =     Queue<T, TRAITS>;

                public:
                    Queue_DoublyLinkedList ();
                    Queue_DoublyLinkedList (const Queue_DoublyLinkedList<T, TRAITS>& q);
                    template <typename CONTAINER_OF_T>
                    explicit Queue_DoublyLinkedList (const CONTAINER_OF_T& q);
                    template <typename COPY_FROM_ITERATOR_OF_T>
                    explicit Queue_DoublyLinkedList (COPY_FROM_ITERATOR_OF_T start, COPY_FROM_ITERATOR_OF_T end);

                public:
                    nonvirtual  Queue_DoublyLinkedList<T, TRAITS>& operator= (const Queue_DoublyLinkedList<T, TRAITS>& rhs);

                private:
#if     qCompilerAndStdLib_SharedPtrOfPrivateTypes_Buggy
                public:
#endif
                    class   Rep_;

                private:
                    nonvirtual  void    AssertRepValidType_ () const;
                };


            }
        }
    }
}




/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */

#include    "Queue_DoublyLinkedList.inl"

#endif  /*_Stroika_Foundation_Containers_Concrete_Queue_DoublyLinkedList_h_ */
