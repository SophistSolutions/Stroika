/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
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
     *      o   size () is O(N)
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
        template <IIterableOfT<T> ITERABLE_OF_ADDABLE>
        explicit Deque_DoublyLinkedList (ITERABLE_OF_ADDABLE&& src)
            requires (not is_base_of_v<Deque_DoublyLinkedList<T>, decay_t<ITERABLE_OF_ADDABLE>>)
#if qCompilerAndStdLib_RequiresNotMatchInlineOutOfLineForTemplateClassBeingDefined_Buggy
        {
            this->AddAllToTail (forward<ITERABLE_OF_ADDABLE> (src));
            AssertRepValidType_ ();
        }
#endif
        ;
        template <IInputIteratorOfT<T> ITERATOR_OF_ADDABLE>
        Deque_DoublyLinkedList (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end);

    public:
        nonvirtual Deque_DoublyLinkedList<T>& operator= (Deque_DoublyLinkedList<T>&& rhs) noexcept = default;
        nonvirtual Deque_DoublyLinkedList<T>& operator= (const Deque_DoublyLinkedList<T>& rhs)     = default;

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
