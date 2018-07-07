/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include "../Queue.h"

#ifndef _Stroika_Foundation_Containers_Concrete_Queue_DoublyLinkedList_h_
#define _Stroika_Foundation_Containers_Concrete_Queue_DoublyLinkedList_h_

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Late</a>
 *
 *  TODO:
 *      @todo   Correctly implement override of Iterator<T>::IRep::Equals()
 *
 *      @todo   Finish using CONTAINER_LOCK_HELPER_START() - synchronization support
 *              THEN - MAYBE - try todo better, but at least do this as starter
 */

namespace Stroika {
    namespace Foundation {
        namespace Containers {
            namespace Concrete {

                /**
                 *  \brief   Queue_DoublyLinkedList<T> is an Array-based concrete implementation of the Queue<T> container pattern.
                 *
                 *  \note   \em Thread-Safety   <a href="thread_safety.html#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
                 *
                 */
                template <typename T>
                class Queue_DoublyLinkedList : public Queue<T> {
                private:
                    using inherited = Queue<T>;

                public:
                    /**
                     */
                    Queue_DoublyLinkedList ();
                    Queue_DoublyLinkedList (const Queue_DoublyLinkedList<T>& src);
                    template <typename CONTAINER_OF_T, typename ENABLE_IF = enable_if_t<Configuration::has_beginend<CONTAINER_OF_T>::value && !std::is_convertible<const CONTAINER_OF_T*, const Queue_DoublyLinkedList<T>*>::value>>
                    explicit Queue_DoublyLinkedList (const CONTAINER_OF_T& src);
                    template <typename COPY_FROM_ITERATOR_OF_T>
                    explicit Queue_DoublyLinkedList (COPY_FROM_ITERATOR_OF_T start, COPY_FROM_ITERATOR_OF_T end);

                public:
                    nonvirtual Queue_DoublyLinkedList<T>& operator= (const Queue_DoublyLinkedList<T>& rhs) = default;

                private:
                    class Rep_;

                private:
                    nonvirtual void AssertRepValidType_ () const;
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

#include "Queue_DoublyLinkedList.inl"

#endif /*_Stroika_Foundation_Containers_Concrete_Queue_DoublyLinkedList_h_ */
