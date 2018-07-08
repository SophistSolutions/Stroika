/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include "../Sequence.h"

#ifndef _Stroika_Foundation_Containers_Concrete_Sequence_DoublyLinkedList_h_
#define _Stroika_Foundation_Containers_Concrete_Sequence_DoublyLinkedList_h_

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
                 *  \brief   Sequence_DoublyLinkedList<T> is an Array-based concrete implementation of the Sequence<T> container pattern.
                 *
                 *  \note   \em Thread-Safety   <a href="thread_safety.html#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
                 *
                 */
                template <typename T>
                class Sequence_DoublyLinkedList : public Sequence<T> {
                private:
                    using inherited = Sequence<T>;

                public:
                    Sequence_DoublyLinkedList ();
                    Sequence_DoublyLinkedList (const Sequence_DoublyLinkedList<T>& src);
                    Sequence_DoublyLinkedList (const initializer_list<T>& src);
                    Sequence_DoublyLinkedList (const vector<T>& src);
                    template <typename CONTAINER_OF_T, typename ENABLE_IF = enable_if_t<Configuration::has_beginend<CONTAINER_OF_T>::value and !is_convertible_v<const CONTAINER_OF_T*, const Sequence_DoublyLinkedList<T>*>>>
                    explicit Sequence_DoublyLinkedList (const CONTAINER_OF_T& src);
                    template <typename COPY_FROM_ITERATOR_OF_T>
                    explicit Sequence_DoublyLinkedList (COPY_FROM_ITERATOR_OF_T start, COPY_FROM_ITERATOR_OF_T end);

                public:
                    /**
                     */
                    nonvirtual Sequence_DoublyLinkedList<T>& operator= (const Sequence_DoublyLinkedList<T>& rhs) = default;

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

#include "Sequence_DoublyLinkedList.inl"

#endif /*_Stroika_Foundation_Containers_Concrete_Sequence_DoublyLinkedList_h_ */
