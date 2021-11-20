/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
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
     *      o   GetLength () is O(N)
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
        using value_type = typename inherited::value_type;

    public:
        /**
         */
        Stack_LinkedList ();
        Stack_LinkedList (const Stack_LinkedList& src) = default;

    public:
        nonvirtual Stack_LinkedList& operator= (const Stack_LinkedList& rhs) = default;

    protected:
        using _IterableRepSharedPtr = typename inherited::_IterableRepSharedPtr;
        using _StackRepSharedPtr    = typename inherited::_IRepSharedPtr;

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
