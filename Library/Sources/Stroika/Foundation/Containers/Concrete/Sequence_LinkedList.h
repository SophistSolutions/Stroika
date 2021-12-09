/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include "../Sequence.h"

#ifndef _Stroika_Foundation_Containers_Concrete_Sequence_LinkedList_h_
#define _Stroika_Foundation_Containers_Concrete_Sequence_LinkedList_h_

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 *
 */

namespace Stroika::Foundation::Containers::Concrete {

    /**
     *  \brief   Sequence_LinkedList<T> is a LinkedList-based concrete implementation of the Sequence<T> container pattern.
     * 
     * \note Performance Notes:
     *       
     *      Suitable for short lists, low memory overhead, where you can new entries, and remove from the start
     *      (or if array indexing is rare). To keep the order, but not use it much.
     * 
     *      o   GetLength () is constant
     *      o   Append () is O(N)
     *      o   Prepend () is constant
     *      o   Indexing (GetAt/SetAt,operator[]) are O(N)
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     */
    template <typename T>
    class Sequence_LinkedList : public Sequence<T> {
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
        Sequence_LinkedList ();
        Sequence_LinkedList (Sequence_LinkedList&& src) noexcept      = default;
        Sequence_LinkedList (const Sequence_LinkedList& src) noexcept = default;
        Sequence_LinkedList (const initializer_list<value_type>& src);
        template <typename ITERABLE_OF_ADDABLE, enable_if_t<Configuration::IsIterable_v<ITERABLE_OF_ADDABLE> and not is_base_of_v<Sequence_LinkedList<T>, decay_t<ITERABLE_OF_ADDABLE>>>* = nullptr>
        explicit Sequence_LinkedList (ITERABLE_OF_ADDABLE&& src);
        template <typename ITERATOR_OF_ADDABLE>
        Sequence_LinkedList (ITERATOR_OF_ADDABLE start, ITERATOR_OF_ADDABLE end);

    public:
        /**
         */
        nonvirtual Sequence_LinkedList& operator= (Sequence_LinkedList&& rhs) = default;
        nonvirtual Sequence_LinkedList& operator= (const Sequence_LinkedList& rhs) = default;

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

#include "Sequence_LinkedList.inl"

#endif /*_Stroika_Foundation_Containers_Concrete_Sequence_LinkedList_h_ */
