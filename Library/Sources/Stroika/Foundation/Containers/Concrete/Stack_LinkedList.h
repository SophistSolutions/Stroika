/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include "../Stack.h"

#ifndef _Stroika_Foundation_Containers_Concrete_Stack_LinkedList_h_
#define _Stroika_Foundation_Containers_Concrete_Stack_LinkedList_h_

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 *
 *  TODO:
 *
 *      @todo   VERY INCOMPLETE/WRONG IMPLEMENTATION - ESPECIALLY COPYING WRONG!!!
 *
 */

namespace Stroika::Foundation::Containers::Concrete {

    /**
     *  \brief   Stack_LinkedList<T> is an LinkedList-based concrete implementation of the Stack<T> container pattern.
     *
     * \note Performance Notes:
     *      Very good low overhead implementation
     *
     *      o   push/pop and top () are all constant complexity
     *      o   size () is O(N)
     *      o   Uses Memory::UseBlockAllocationIfAppropriate
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     *
     */
    template <typename T>
    class Stack_LinkedList : public Stack<T> {
    private:
        using inherited = Stack<T>;

    public:
        /**
         */
        template <typename POTENTIALLY_ADDABLE_T>
        static constexpr bool IsAddable_v = inherited::template IsAddable_v<POTENTIALLY_ADDABLE_T>;
        using value_type                  = typename inherited::value_type;

    public:
        /**
         */
        Stack_LinkedList ();
        Stack_LinkedList (Stack_LinkedList&& src) noexcept      = default;
        Stack_LinkedList (const Stack_LinkedList& src) noexcept = default;
        template <IIterableOfT<T> ITERABLE_OF_ADDABLE, enable_if_t<not is_base_of_v<Stack_LinkedList<T>, decay_t<ITERABLE_OF_ADDABLE>>>* = nullptr>
        explicit Stack_LinkedList (ITERABLE_OF_ADDABLE&& src);
        template <IInputIteratorOfT<T> ITERATOR_OF_ADDABLE>
        Stack_LinkedList (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end);

    public:
        nonvirtual Stack_LinkedList& operator= (Stack_LinkedList&& rhs) noexcept = default;
        nonvirtual Stack_LinkedList& operator= (const Stack_LinkedList& rhs)     = default;

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

#include "Stack_LinkedList.inl"

#endif /*_Stroika_Foundation_Containers_Concrete_Stack_LinkedList_h_ */
