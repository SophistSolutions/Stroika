/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include "../Queue.h"

#ifndef _Stroika_Foundation_Containers_Concrete_Queue_DoublyLinkedList_h_
#define _Stroika_Foundation_Containers_Concrete_Queue_DoublyLinkedList_h_

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 *
 */

namespace Stroika::Foundation::Containers::Concrete {

    /**
     *  \brief   Queue_DoublyLinkedList<T> is an Array-based concrete implementation of the Queue<T> container pattern.
     *
     * \note Performance Notes:
     *      o   size () is O(N)
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     */
    template <typename T>
    class Queue_DoublyLinkedList : public Queue<T> {
    private:
        using inherited = Queue<T>;

    public:
        template <typename POTENTIALLY_ADDABLE_T>
        static constexpr bool IsAddable_v = inherited::template IsAddable_v<POTENTIALLY_ADDABLE_T>;
        using value_type                  = typename inherited::value_type;

    public:
        /**
         *  \see docs on Queue<T> constructor
         */
        Queue_DoublyLinkedList ();
        Queue_DoublyLinkedList (Queue_DoublyLinkedList&& src) noexcept      = default;
        Queue_DoublyLinkedList (const Queue_DoublyLinkedList& src) noexcept = default;
        Queue_DoublyLinkedList (const initializer_list<value_type>& src);
        template <typename ITERABLE_OF_ADDABLE, enable_if_t<Configuration::IsIterable_v<ITERABLE_OF_ADDABLE> and not is_base_of_v<Queue_DoublyLinkedList<T>, decay_t<ITERABLE_OF_ADDABLE>>>* = nullptr>
        explicit Queue_DoublyLinkedList (ITERABLE_OF_ADDABLE&& src);
        template <typename ITERATOR_OF_ADDABLE>
        Queue_DoublyLinkedList (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end);

    public:
        nonvirtual Queue_DoublyLinkedList& operator= (Queue_DoublyLinkedList&& rhs) noexcept = default;
        nonvirtual Queue_DoublyLinkedList& operator= (const Queue_DoublyLinkedList& rhs) = default;

    protected:
        using _IterableRepSharedPtr = typename inherited::_IterableRepSharedPtr;
        using _QueueRepSharedPtr    = typename inherited::_IRepSharedPtr;

    private:
        class Rep_;

    private:
        nonvirtual void AssertRepValidType_ () const;
    };

}

/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */

#include "Queue_DoublyLinkedList.inl"

#endif /*_Stroika_Foundation_Containers_Concrete_Queue_DoublyLinkedList_h_ */
