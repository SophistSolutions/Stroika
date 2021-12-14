/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include "../Deque.h"

#ifndef _Stroika_Foundation_Containers_Concrete_Deque_DoublyLinkedList_h_
#define _Stroika_Foundation_Containers_Concrete_Deque_DoublyLinkedList_h_

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 *
 */

namespace Stroika::Foundation::Containers::Concrete {

    /**
     *  \brief   Deque_DoublyLinkedList<T> is an Array-based concrete implementation of the Deque<T> container pattern.
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     *
     * \note Performance Notes:
     *      o   GetLength () is O(N)
     */
    template <typename T>
    class Deque_DoublyLinkedList : public Deque<T> {
    private:
        using inherited = Deque<T>;

    public:
        template <typename POTENTIALLY_ADDABLE_T>
        static constexpr bool IsAddable_v = inherited::template IsAddable_v<POTENTIALLY_ADDABLE_T>;
        using value_type                  = typename inherited::value_type;

    public:
        /**
         *  \see docs on Deque<T> constructor
         */
        Deque_DoublyLinkedList ();
        Deque_DoublyLinkedList (Deque_DoublyLinkedList&& src) noexcept      = default;
        Deque_DoublyLinkedList (const Deque_DoublyLinkedList& src) noexcept = default;
        Deque_DoublyLinkedList (const initializer_list<value_type>& src);
        template <typename ITERABLE_OF_ADDABLE, enable_if_t<Configuration::IsIterable_v<ITERABLE_OF_ADDABLE> and not is_base_of_v<Deque_DoublyLinkedList<T>, decay_t<ITERABLE_OF_ADDABLE>>>* = nullptr>
        explicit Deque_DoublyLinkedList (ITERABLE_OF_ADDABLE&& src);
        template <typename ITERATOR_OF_ADDABLE>
        Deque_DoublyLinkedList (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end);

    public:
        nonvirtual Deque_DoublyLinkedList<T>& operator= (Deque_DoublyLinkedList<T>&& rhs) = default;
        nonvirtual Deque_DoublyLinkedList<T>& operator= (const Deque_DoublyLinkedList<T>& rhs) = default;

    protected:
        using _IterableRepSharedPtr = typename inherited::_IterableRepSharedPtr;
        using _QueueRepSharedPtr    = typename Queue<T>::_IRepSharedPtr;

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

#include "Deque_DoublyLinkedList.inl"

#endif /*_Stroika_Foundation_Containers_Concrete_Deque_DoublyLinkedList_h_ */
