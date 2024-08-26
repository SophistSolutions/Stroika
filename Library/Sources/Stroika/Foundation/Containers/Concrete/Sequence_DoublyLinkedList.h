/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_Sequence_DoublyLinkedList_h_
#define _Stroika_Foundation_Containers_Concrete_Sequence_DoublyLinkedList_h_

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Containers/Sequence.h"

/**
 *  \file
 *
 *  \note Code-Status:  <a href="Code-Status.md#Beta">Beta</a>
 *
 */

namespace Stroika::Foundation::Containers::Concrete {

    /**
     *  \brief   Sequence_DoublyLinkedList<T> is an Array-based concrete implementation of the Sequence<T> container pattern.
     * 
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     *
     * \note Runtime performance/complexity:
     *      o   size () is O(N), but empty () is constant
     *      o   Append () is O(N)
     *      o   Prepend () is constant complexity
     *      o   Indexing (GetAt/SetAt,operator[]) are O(N)
     */
    template <typename T>
    class Sequence_DoublyLinkedList : public Sequence<T> {
    private:
        using inherited = Sequence<T>;

    public:
        using value_type = typename inherited::value_type;

    public:
        /**
         *  \see docs on Sequence<> constructor
         */
        Sequence_DoublyLinkedList ();
        Sequence_DoublyLinkedList (Sequence_DoublyLinkedList&&) noexcept      = default;
        Sequence_DoublyLinkedList (const Sequence_DoublyLinkedList&) noexcept = default;
        Sequence_DoublyLinkedList (const initializer_list<value_type>& src);
        template <IIterableOf<T> ITERABLE_OF_ADDABLE>
            requires (not derived_from<remove_cvref_t<ITERABLE_OF_ADDABLE>, Sequence_DoublyLinkedList<T>>)
        explicit Sequence_DoublyLinkedList (ITERABLE_OF_ADDABLE&& src)
#if qCompilerAndStdLib_RequiresNotMatchInlineOutOfLineForTemplateClassBeingDefined_Buggy
            : Sequence_DoublyLinkedList{}
        {
            this->AppendAll (forward<ITERABLE_OF_ADDABLE> (src));
            AssertRepValidType_ ();
        }
#endif
        ;
        template <IInputIterator<T> ITERATOR_OF_ADDABLE>
        Sequence_DoublyLinkedList (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end);

    public:
        /**
         */
        nonvirtual Sequence_DoublyLinkedList& operator= (Sequence_DoublyLinkedList&&) noexcept = default;
        nonvirtual Sequence_DoublyLinkedList& operator= (const Sequence_DoublyLinkedList&)     = default;

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

#include "Sequence_DoublyLinkedList.inl"

#endif /*_Stroika_Foundation_Containers_Concrete_Sequence_DoublyLinkedList_h_ */
