/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include "../Sequence.h"

#ifndef _Stroika_Foundation_Containers_Concrete_Sequence_stdvector_h_
#define _Stroika_Foundation_Containers_Concrete_Sequence_stdvector_h_

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 *
 *  TODO:
 *      @todo   Be sure can move-semantics into and out of Sequence_stdvector() - with vector<T> -
 *              so can go back and forth between reps efficiently. This COULD use used to avoid
 *              any performance overhead with Stroika Sequences.
 */

namespace Stroika::Foundation::Containers::Concrete {

    /**
     *  \brief   Sequence_stdvector<T> is an std::vector-based concrete implementation of the Sequence<T> container pattern.
     *
     *  \note Performance notes
     * 
     *      A good default implementation, except that empirically, this appears slower than Sequence_Array<>.
     *
     *      o   Append/Prepend should perform well (typically constant time, but occasionally O(N))
     *      o   it is FAST to array index a Sequence_stdvector.
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     *
     */
    template <typename T>
    class Sequence_stdvector : public Sequence<T> {
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
        Sequence_stdvector ();
        Sequence_stdvector (Sequence_stdvector&& src) noexcept      = default;
        Sequence_stdvector (const Sequence_stdvector& src) noexcept = default;
        Sequence_stdvector (const initializer_list<value_type>& src);
        template <typename ITERABLE_OF_ADDABLE, enable_if_t<Configuration::IsIterable_v<ITERABLE_OF_ADDABLE> and not is_base_of_v<Sequence_stdvector<T>, decay_t<ITERABLE_OF_ADDABLE>>>* = nullptr>
        explicit Sequence_stdvector (ITERABLE_OF_ADDABLE&& src);
        template <typename ITERATOR_OF_ADDABLE>
        Sequence_stdvector (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end);

    public:
        /**
         */
        nonvirtual Sequence_stdvector& operator= (Sequence_stdvector&& rhs) noexcept = default;
        nonvirtual Sequence_stdvector& operator= (const Sequence_stdvector& rhs) = default;

    public:
        /*
         *  \brief Return the number of allocated vector/array elements.
         * 
         * This optional API allows pre-reserving space as an optimization.
         * 
         *  \note alias GetCapacity ();
         */
        nonvirtual size_t capacity () const;

    public:
        /**
         * This optional API allows pre-reserving space as an optimization.
         * 
         *  \note Alias SetCapacity ();
         * 
         *  \note Note that this does not affect the semantics of the Sequence.
         * 
         *  \req slotsAllocated >= size ()
         */
        nonvirtual void reserve (size_t slotsAlloced);

    public:
        /**
         *  \brief  Reduce the space used to store the Sequence<T> contents.
         *
         *  This has no semantics, no observable behavior. But depending on the representation of
         *  the concrete sequence, calling this may save memory.
         */
        nonvirtual void shrink_to_fit ();

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
#include "Sequence_stdvector.inl"

#endif /*_Stroika_Foundation_Containers_Concrete_Sequence_stdvector_h_ */
