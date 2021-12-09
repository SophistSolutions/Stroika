/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include "../Sequence.h"

#ifndef _Stroika_Foundation_Containers_Concrete_Sequence_Array_h_
#define _Stroika_Foundation_Containers_Concrete_Sequence_Array_h_

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 *
 */

namespace Stroika::Foundation::Containers::Concrete {

    /**
     *  \brief   Sequence_Array<T> is an Array-based concrete implementation of the Sequence<T> container pattern.
     *
     * \note Performance Notes:
     *      o   Append/Prepend should perform well (typically constant time, but occasionally O(N))
     *      o   it is FAST to array index a Sequence_Array.
     *      o   GetLength () is constant
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     */
    template <typename T>
    class Sequence_Array : public Sequence<T> {
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
        Sequence_Array ();
        Sequence_Array (Sequence_Array&& src) noexcept      = default;
        Sequence_Array (const Sequence_Array& src) noexcept = default;
        Sequence_Array (const initializer_list<value_type>& src);
        template <typename ITERABLE_OF_ADDABLE, enable_if_t<Configuration::IsIterable_v<ITERABLE_OF_ADDABLE> and not is_base_of_v<Sequence_Array<T>, decay_t<ITERABLE_OF_ADDABLE>>>* = nullptr>
        explicit Sequence_Array (ITERABLE_OF_ADDABLE&& src);
        template <typename ITERATOR_OF_ADDABLE>
        Sequence_Array (ITERATOR_OF_ADDABLE start, ITERATOR_OF_ADDABLE end);

    public:
        /**
         */
        nonvirtual Sequence_Array& operator= (Sequence_Array&& rhs) = default;
        nonvirtual Sequence_Array& operator= (const Sequence_Array& rhs) = default;

    public:
        /**
         *  \brief  Reduce the space used to store the Sequence<T> contents.
         *
         *  This has no semantics, no observable behavior. But depending on the representation of
         *  the concrete sequence, calling this may save memory.
         */
        nonvirtual void shrink_to_fit ();

    public:
        /**
         * Return the number of allocated vector/array elements.
         */
        nonvirtual size_t GetCapacity () const;

    public:
        /**
         * This optional API allows pre-reserving space as an optimization.
         */
        nonvirtual void SetCapacity (size_t slotsAlloced);

    public:
        /**
         *  STL-ish alias for GetCapacity ();
         */
        nonvirtual size_t capacity () const;

    public:
        /**
         *  STL-ish alias for SetCapacity ();
         */
        nonvirtual void reserve (size_t slotsAlloced);

    protected:
        using _IterableRepSharedPtr = typename inherited::_IterableRepSharedPtr;
        using _SequenceRepSharedPtr = typename inherited::_IRepSharedPtr;

    private:
        class IImplRep_;
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

#include "Sequence_Array.inl"

#endif /*_Stroika_Foundation_Containers_Concrete_Sequence_Array_h_ */
