/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_Queue_DoublyLinkedList_h_
#define _Stroika_Foundation_Containers_Concrete_Queue_DoublyLinkedList_h_

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Containers/Queue.h"

/**
 *  \file
 *
 *  \note Code-Status:  <a href="Code-Status.md#Beta">Beta</a>
 */

namespace Stroika::Foundation::Containers::Concrete {

    /**
     *  \brief   Queue_DoublyLinkedList<T> is an Array-based concrete implementation of the Queue<T> container pattern.
     *
     * \note Runtime performance/complexity:
     *      o   size () is O(N)
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     */
    template <typename T>
    class Queue_DoublyLinkedList : public Queue<T> {
    private:
        using inherited = Queue<T>;

    public:
        using value_type = typename inherited::value_type;

    public:
        /**
         *  \see docs on Queue<T> constructor
         */
        Queue_DoublyLinkedList ();
        Queue_DoublyLinkedList (Queue_DoublyLinkedList&& src) noexcept      = default;
        Queue_DoublyLinkedList (const Queue_DoublyLinkedList& src) noexcept = default;
        Queue_DoublyLinkedList (const initializer_list<value_type>& src);
        template <IIterableOf<T> ITERABLE_OF_ADDABLE>
            requires (not derived_from<remove_cvref_t<ITERABLE_OF_ADDABLE>, Queue_DoublyLinkedList<T>>)
        explicit Queue_DoublyLinkedList (ITERABLE_OF_ADDABLE&& src)
#if qCompilerAndStdLib_RequiresNotMatchInlineOutOfLineForTemplateClassBeingDefined_Buggy
            : Queue_DoublyLinkedList{}
        {
            this->AddAllToTail (forward<ITERABLE_OF_ADDABLE> (src));
            AssertRepValidType_ ();
        }
#endif
        ;
        template <IInputIterator<T> ITERATOR_OF_ADDABLE>
        Queue_DoublyLinkedList (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end);

    public:
        nonvirtual Queue_DoublyLinkedList& operator= (Queue_DoublyLinkedList&& rhs) noexcept = default;
        nonvirtual Queue_DoublyLinkedList& operator= (const Queue_DoublyLinkedList& rhs)     = default;

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

#include "Queue_DoublyLinkedList.inl"

#endif /*_Stroika_Foundation_Containers_Concrete_Queue_DoublyLinkedList_h_ */
