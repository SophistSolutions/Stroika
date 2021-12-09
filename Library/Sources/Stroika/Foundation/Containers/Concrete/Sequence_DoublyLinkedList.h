/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include "../Sequence.h"

#ifndef _Stroika_Foundation_Containers_Concrete_Sequence_DoublyLinkedList_h_
#define _Stroika_Foundation_Containers_Concrete_Sequence_DoublyLinkedList_h_

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 *
 */

namespace Stroika::Foundation::Containers::Concrete {

    /**
     *  \brief   Sequence_DoublyLinkedList<T> is an Array-based concrete implementation of the Sequence<T> container pattern.
     * 
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     *
     * \note Performance Notes:
     *      o   GetLength () is O(N)
     *      o   Append () is O(N)
     *      o   Prepend () is constant
     *      o   Indexing (GetAt/SetAt,operator[]) are O(N)
     */
    template <typename T>
    class Sequence_DoublyLinkedList : public Sequence<T> {
    private:
        using inherited = Sequence<T>;

    public:
        template <typename POTENTIALLY_ADDABLE_T>
        static constexpr bool IsAddable_v = inherited::template IsAddable_v<POTENTIALLY_ADDABLE_T>;
        using value_type                  = typename inherited::value_type;

    public:
        /**
         *  \see docs on Sequence<> constructor
         */
        Sequence_DoublyLinkedList ();
        Sequence_DoublyLinkedList (Sequence_DoublyLinkedList&& src) noexcept      = default;
        Sequence_DoublyLinkedList (const Sequence_DoublyLinkedList& src) noexcept = default;
        Sequence_DoublyLinkedList (const initializer_list<value_type>& src);
        template <typename ITERABLE_OF_ADDABLE, enable_if_t<Configuration::IsIterable_v<ITERABLE_OF_ADDABLE> and not is_base_of_v<Sequence_DoublyLinkedList<T>, decay_t<ITERABLE_OF_ADDABLE>>>* = nullptr>
        explicit Sequence_DoublyLinkedList (ITERABLE_OF_ADDABLE&& src);
        template <typename ITERATOR_OF_ADDABLE>
        Sequence_DoublyLinkedList (ITERATOR_OF_ADDABLE start, ITERATOR_OF_ADDABLE end);

    public:
        /**
         */
        nonvirtual Sequence_DoublyLinkedList& operator= (Sequence_DoublyLinkedList&& rhs) = default;
        nonvirtual Sequence_DoublyLinkedList& operator= (const Sequence_DoublyLinkedList& rhs) = default;

    protected:
        using _IterableRepSharedPtr = typename inherited::_IterableRepSharedPtr;
        using _SequenceRepSharedPtr = typename inherited::_IRepSharedPtr;

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

#include "Sequence_DoublyLinkedList.inl"

#endif /*_Stroika_Foundation_Containers_Concrete_Sequence_DoublyLinkedList_h_ */
