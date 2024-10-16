/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_Sequence_LinkedList_h_
#define _Stroika_Foundation_Containers_Concrete_Sequence_LinkedList_h_

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
     *  \brief   Sequence_LinkedList<T> is a LinkedList-based concrete implementation of the Sequence<T> container pattern.
     * 
     * \note Runtime performance/complexity:
     *       
     *      Suitable for short lists, low memory overhead, where you can new entries, and remove from the start
     *      (or if array indexing is rare). To keep the order, but not use it much.
     * 
     *      o   size () is O(N), but empty () is constant
     *      o   Append () is O(N)
     *      o   Prepend () is constant complexity
     *      o   Indexing (GetAt/SetAt,operator[]) are O(N)
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     */
    template <typename T>
    class Sequence_LinkedList : public Sequence<T> {
    private:
        using inherited = Sequence<T>;

    public:
        using value_type = typename inherited::value_type;

    public:
        /**
         *  \see docs on Sequence<> constructor
         */
        Sequence_LinkedList ();
        Sequence_LinkedList (Sequence_LinkedList&&) noexcept      = default;
        Sequence_LinkedList (const Sequence_LinkedList&) noexcept = default;
        Sequence_LinkedList (const initializer_list<value_type>& src);
        template <IIterableOf<T> ITERABLE_OF_ADDABLE>
            requires (not derived_from<remove_cvref_t<ITERABLE_OF_ADDABLE>, Sequence_LinkedList<T>>)
        explicit Sequence_LinkedList (ITERABLE_OF_ADDABLE&& src)
#if qCompilerAndStdLib_RequiresNotMatchInlineOutOfLineForTemplateClassBeingDefined_Buggy
            : Sequence_LinkedList{}
        {
            this->AppendAll (forward<ITERABLE_OF_ADDABLE> (src));
            AssertRepValidType_ ();
        }
#endif
        ;
        template <IInputIterator<T> ITERATOR_OF_ADDABLE>
        Sequence_LinkedList (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end);

    public:
        /**
         */
        nonvirtual Sequence_LinkedList& operator= (Sequence_LinkedList&&) noexcept = default;
        nonvirtual Sequence_LinkedList& operator= (const Sequence_LinkedList&)     = default;

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

#include "Sequence_LinkedList.inl"

#endif /*_Stroika_Foundation_Containers_Concrete_Sequence_LinkedList_h_ */
