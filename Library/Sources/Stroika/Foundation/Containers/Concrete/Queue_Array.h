/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include "../Queue.h"

#ifndef _Stroika_Foundation_Containers_Concrete_Queue_Array_h_
#define _Stroika_Foundation_Containers_Concrete_Queue_Array_h_

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 *
 *  TODO:
 *      @todo   Add "MAX ENTRIES" feature - so throws/crashes when exceeded. Useful for a variety
 *              of queues, but I'm thinking mostly of Queue of incoming singals and need to avoid
 *              malloc call.
 *
 *      @todo   Redo using circular array strategy (we have this code in OLD stroika!!!)
 */

namespace Stroika::Foundation::Containers::Concrete {

    /**
     *  \brief   Queue_Array<T> is an Array-based concrete implementation of the Queue<T> container pattern.
     *
     * \note Performance Notes:
     *      o   GetLength () is constant
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     */
    template <typename T>
    class Queue_Array : public Queue<T> {
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
        Queue_Array ();
        Queue_Array (Queue_Array&& src) noexcept      = default;
        Queue_Array (const Queue_Array& src) noexcept = default;
        Queue_Array (const initializer_list<value_type>& src);
        template <typename ITERABLE_OF_ADDABLE, enable_if_t<Configuration::IsIterable_v<ITERABLE_OF_ADDABLE> and not is_base_of_v<Queue_Array<T>, decay_t<ITERABLE_OF_ADDABLE>>>* = nullptr>
        explicit Queue_Array (ITERABLE_OF_ADDABLE&& src);
        template <typename ITERATOR_OF_ADDABLE>
        Queue_Array (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end);

    public:
        /**
         */
        nonvirtual Queue_Array& operator= (Queue_Array&& rhs) noexcept = default;
        nonvirtual Queue_Array& operator= (const Queue_Array& rhs) = default;

    public:
        /**
         *  \brief  Reduce the space used to store the Queue<T> contents.
         *
         *  This has no semantics, no observable behavior. But depending on the representation of
         *  the concrete sequence, calling this may save memory.
         */
        nonvirtual void shrink_to_fit ();

    public:
        /**
         *  This optional API allows pre-reserving space as an optimization. Note that this does not affect
         *  the semantics of Queue().
         */
        nonvirtual size_t GetCapacity () const;
        nonvirtual void   SetCapacity (size_t slotsAlloced);

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

#include "Queue_Array.inl"

#endif /*_Stroika_Foundation_Containers_Concrete_Queue_Array_h_ */
