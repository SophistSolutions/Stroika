/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_SortedCollection_LinkedList_h_
#define _Stroika_Foundation_Containers_Concrete_SortedCollection_LinkedList_h_

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Containers/SortedCollection.h"

/**
 *  \file
 *
 *  \note Code-Status:  <a href="Code-Status.md#Beta">Beta</a>
 *
 */

namespace Stroika::Foundation::Containers::Concrete {

    /**
     *  \brief SortedCollection_LinkedList<T> is an LinkedList-based concrete implementation of the SortedCollection<T> container pattern.
     *
     * \note Runtime performance/complexity:
     *      SortedCollection_LinkedList<T> is a compact, and reasonable implementation of Collections, so long as the Collection remains quite small
     *      (empty or just a few entires).
     *
     *      o   size () is O(N), but empty () is constant
     *      o   Uses Memory::UseBlockAllocationIfAppropriate
     *      o   Additions and Removals are O(N)
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     */
    template <typename T>
    class SortedCollection_LinkedList : public SortedCollection<T> {
    private:
        using inherited = SortedCollection<T>;

    public:
        using value_type          = typename inherited::value_type;
        using InOrderComparerType = typename SortedCollection<T>::InOrderComparerType;

    public:
        /**
         *  \see docs on SortedCollection<> constructor
         */
        SortedCollection_LinkedList ();
        template <Common::IInOrderComparer<T> INORDER_COMPARER>
        explicit SortedCollection_LinkedList (INORDER_COMPARER&& inorderComparer);
        SortedCollection_LinkedList (SortedCollection_LinkedList&&) noexcept      = default;
        SortedCollection_LinkedList (const SortedCollection_LinkedList&) noexcept = default;
        SortedCollection_LinkedList (const initializer_list<T>& src);
        template <Common::IInOrderComparer<T> INORDER_COMPARER>
        SortedCollection_LinkedList (INORDER_COMPARER&& inOrderComparer, const initializer_list<T>& src);
        template <IIterableOf<T> ITERABLE_OF_ADDABLE>
            requires (not derived_from<remove_cvref_t<ITERABLE_OF_ADDABLE>, SortedCollection_LinkedList<T>>)
        explicit SortedCollection_LinkedList (ITERABLE_OF_ADDABLE&& src)
#if qCompilerAndStdLib_RequiresNotMatchInlineOutOfLineForTemplateClassBeingDefined_Buggy
            : SortedCollection_LinkedList{}
        {
            this->AddAll (forward<ITERABLE_OF_ADDABLE> (src));
            AssertRepValidType_ ();
        }
#endif
        ;
        template <Common::IInOrderComparer<T> INORDER_COMPARER, IIterableOf<T> ITERABLE_OF_ADDABLE>
        SortedCollection_LinkedList (INORDER_COMPARER&& inOrderComparer, ITERABLE_OF_ADDABLE&& src);
        template <IInputIterator<T> ITERATOR_OF_ADDABLE>
        SortedCollection_LinkedList (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end);
        template <Common::IInOrderComparer<T> INORDER_COMPARER, IInputIterator<T> ITERATOR_OF_ADDABLE>
        SortedCollection_LinkedList (INORDER_COMPARER&& inOrderComparer, ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end);

    public:
        /**
         */
        nonvirtual SortedCollection_LinkedList& operator= (SortedCollection_LinkedList&&) noexcept = default;
        nonvirtual SortedCollection_LinkedList& operator= (const SortedCollection_LinkedList&)     = default;

    private:
        using IImplRepBase_ = typename SortedCollection<T>::_IRep;
        template <qCompilerAndStdLib_ConstraintDiffersInTemplateRedeclaration_BWA (Common::IInOrderComparer<T>) INORDER_COMPARER>
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
#include "SortedCollection_LinkedList.inl"

#endif /*_Stroika_Foundation_Containers_Concrete_SortedCollection_LinkedList_h_ */
