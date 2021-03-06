/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include "../Sequence.h"

#ifndef _Stroika_Foundation_Containers_Concrete_Sequence_stdvector_h_
#define _Stroika_Foundation_Containers_Concrete_Sequence_stdvector_h_

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
 *
 *      @todo   Be sure can move-semantics into and out of Sequence_stdvector() - with vector<T> -
 *              so can go back and forth between reps efficiently. This COULD use used to avoid
 *              any performance overhead with Stroika Sequences.
 */

namespace Stroika::Foundation::Containers::Concrete {

    /**
     *  \brief   Sequence_stdvector<T> is an std::vector-based concrete implementation of the Sequence<T> container pattern.
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     *
     */
    template <typename T>
    class Sequence_stdvector : public Sequence<T> {
    private:
        using inherited = Sequence<T>;

    public:
        /**
         */
        Sequence_stdvector ();
        Sequence_stdvector (const Sequence_stdvector& src) = default;
        Sequence_stdvector (vector<T>&& src);
        template <typename CONTAINER_OF_ADDABLE, enable_if_t<Configuration::IsIterableOfT_v<CONTAINER_OF_ADDABLE, T> and not is_base_of_v<Sequence_stdvector<T>, Configuration::remove_cvref_t<CONTAINER_OF_ADDABLE>>>* = nullptr>
        Sequence_stdvector (CONTAINER_OF_ADDABLE&& src);
        template <typename COPY_FROM_ITERATOR_OF_T>
        explicit Sequence_stdvector (COPY_FROM_ITERATOR_OF_T start, COPY_FROM_ITERATOR_OF_T end);

    public:
        /**
         */
        nonvirtual Sequence_stdvector& operator= (const Sequence_stdvector& rhs);

    public:
        /**
         *  \brief  Reduce the space used to store the Sequence<T> contents.
         *
         *  This has no semantics, no observable behavior. But depending on the representation of
         *  the concrete sequence, calling this may save memory.
         */
        nonvirtual void Compact ();

    public:
        /**
         * This optional API allows pre-reserving space as an optimization.
         */
        nonvirtual size_t GetCapacity () const;
        nonvirtual void   SetCapacity (size_t slotsAlloced);

    public:
        /**
         *   STL-ish alias for GetCapacity ();
         */
        nonvirtual size_t capacity () const;

    public:
        /**
         *   STL-ish alias for SetCapacity ();
         */
        nonvirtual void reserve (size_t slotsAlloced);

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
