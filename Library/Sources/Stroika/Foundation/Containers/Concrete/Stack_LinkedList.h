/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_Stack_LinkedList_h_
#define _Stroika_Foundation_Containers_Concrete_Stack_LinkedList_h_

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Containers/Stack.h"

/**
 *  \file
 *
 *  \note Code-Status:  <a href="Code-Status.md#Beta">Beta</a>
 *
 *  TODO:
 *      @todo   Improve performance of CTOR's using reverse-iterator without copy - if constexpr check IT type
 *
 */

namespace Stroika::Foundation::Containers::Concrete {

    /**
     *  \brief   Stack_LinkedList<T> is an LinkedList-based concrete implementation of the Stack<T> container pattern.
     *
     * \note Runtime performance/complexity:
     *      Very good low overhead implementation
     *
     *      o   push/pop and top () are all constant complexity
     *      o   size () is O(N), but empty () is constant
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
        Stack_LinkedList (Stack_LinkedList&&) noexcept      = default;
        Stack_LinkedList (const Stack_LinkedList&) noexcept = default;
        template <IIterableOf<T> ITERABLE_OF_ADDABLE>
            requires (not derived_from<remove_cvref_t<ITERABLE_OF_ADDABLE>, Stack_LinkedList<T>>)
        explicit Stack_LinkedList (ITERABLE_OF_ADDABLE&& src)
#if qCompilerAndStdLib_RequiresNotMatchInlineOutOfLineForTemplateClassBeingDefined_Buggy
            : Stack_LinkedList{src.begin (), src.end ()} {}
#endif
            ;
        template <IInputIterator<T> ITERATOR_OF_ADDABLE>
        Stack_LinkedList (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end);

    public:
        nonvirtual Stack_LinkedList& operator= (Stack_LinkedList&&) noexcept = default;
        nonvirtual Stack_LinkedList& operator= (const Stack_LinkedList&)     = default;

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
