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
        using value_type = typename inherited::value_type;

    public:
        /**
         *  @todo - https://stroika.atlassian.net/browse/STK-652 - add COMPARER constructor overloads like the archetype base class
         */
        Deque_DoublyLinkedList ();
        Deque_DoublyLinkedList (const Deque_DoublyLinkedList& src);
        template <typename CONTAINER_OF_T, enable_if_t<Configuration::IsIterable_v<CONTAINER_OF_T> and not is_convertible_v<const CONTAINER_OF_T*, const Deque_DoublyLinkedList<T>*>>* = nullptr>
        explicit Deque_DoublyLinkedList (const CONTAINER_OF_T& src);
        template <typename COPY_FROM_ITERATOR_OF_T>
        explicit Deque_DoublyLinkedList (COPY_FROM_ITERATOR_OF_T start, COPY_FROM_ITERATOR_OF_T end);

    public:
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
