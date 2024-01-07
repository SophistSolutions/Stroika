/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
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
     *      o   size () is constant complexity
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     */
    template <typename T>
    class Queue_Array : public Queue<T> {
    private:
        using inherited = Queue<T>;

    public:
        using value_type = typename inherited::value_type;

    public:
        /**
         *  \see docs on Queue<T> constructor
         */
        Queue_Array ();
        Queue_Array (Queue_Array&& src) noexcept      = default;
        Queue_Array (const Queue_Array& src) noexcept = default;
        Queue_Array (const initializer_list<value_type>& src);
        template <IIterableOf<T> ITERABLE_OF_ADDABLE>
            requires (not derived_from<remove_cvref_t<ITERABLE_OF_ADDABLE>, Queue_Array<T>>)
        explicit Queue_Array (ITERABLE_OF_ADDABLE&& src)
#if qCompilerAndStdLib_RequiresNotMatchInlineOutOfLineForTemplateClassBeingDefined_Buggy
            : Queue_Array{}
        {
            if constexpr (Configuration::IHasSizeMethod<ITERABLE_OF_ADDABLE>) {
                reserve (src.size ());
            }
            AddAllToTail (forward<ITERABLE_OF_ADDABLE> (src));
            AssertRepValidType_ ();
        }
#endif
        ;
        template <IInputIterator<T> ITERATOR_OF_ADDABLE>
        Queue_Array (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end);

    public:
        /**
         */
        nonvirtual Queue_Array& operator= (Queue_Array&& rhs) noexcept = default;
        nonvirtual Queue_Array& operator= (const Queue_Array& rhs)     = default;

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
         *  \note Note that this does not affect the semantics of the Queue.
         * 
         *  \req slotsAllocated >= size ()
         */
        nonvirtual void reserve (size_t slotsAlloced);

    public:
        /**
         *  \brief  Reduce the space used to store the Queue<T> contents.
         *
         *  This has no semantics, no observable behavior. But depending on the representation of
         *  the concrete sequence, calling this may save memory.
         */
        nonvirtual void shrink_to_fit ();

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
