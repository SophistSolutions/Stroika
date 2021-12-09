/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include "../SortedCollection.h"

#ifndef _Stroika_Foundation_Containers_Concrete_SortedCollection_LinkedList_h_
#define _Stroika_Foundation_Containers_Concrete_SortedCollection_LinkedList_h_

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 *
 */

namespace Stroika::Foundation::Containers::Concrete {

    /**
     *  \brief SortedCollection_LinkedList<T> is an LinkedList-based concrete implementation of the SortedCollection<T> container pattern.
     *
     * \note Performance Notes:
     *      SortedCollection_LinkedList<T> is a compact, and reasonable implementation of Collections, so long as the Collection remains quite small
     *      (empty or just a few entires).
     *
     *      o   GetLength () is O(N)
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
        template <typename POTENTIALLY_ADDABLE_T>
        static constexpr bool IsAddable_v = inherited::template IsAddable_v<POTENTIALLY_ADDABLE_T>;
        using value_type          = typename inherited::value_type;
        using InOrderComparerType = typename SortedCollection<T>::InOrderComparerType;

    public:
        /**
         *  \see docs on SortedCollection<> constructor
         */
        SortedCollection_LinkedList ();
        template <typename INORDER_COMPARER, enable_if_t<Common::IsStrictInOrderComparer<INORDER_COMPARER, T> ()>* = nullptr>
        explicit SortedCollection_LinkedList (INORDER_COMPARER&& inorderComparer);
        SortedCollection_LinkedList (SortedCollection_LinkedList&& src) noexcept      = default;
        SortedCollection_LinkedList (const SortedCollection_LinkedList& src) noexcept = default;
        SortedCollection_LinkedList (const initializer_list<T>& src);
        template <typename INORDER_COMPARER, enable_if_t<Common::IsStrictInOrderComparer<INORDER_COMPARER, T> ()>* = nullptr>
        SortedCollection_LinkedList (INORDER_COMPARER&& inOrderComparer, const initializer_list<T>& src);
        template <typename ITERABLE_OF_ADDABLE, enable_if_t<Configuration::IsIterable_v<ITERABLE_OF_ADDABLE> and not is_base_of_v<SortedCollection_LinkedList<T>, decay_t<ITERABLE_OF_ADDABLE>>>* = nullptr>
        explicit SortedCollection_LinkedList (ITERABLE_OF_ADDABLE&& src);
        template <typename INORDER_COMPARER, typename ITERABLE_OF_ADDABLE, enable_if_t<Common::IsStrictInOrderComparer<INORDER_COMPARER, T> () and Configuration::IsIterable_v<ITERABLE_OF_ADDABLE>>* = nullptr>
        SortedCollection_LinkedList (INORDER_COMPARER&& inOrderComparer, ITERABLE_OF_ADDABLE&& src);
        template <typename ITERATOR_OF_ADDABLE>
        SortedCollection_LinkedList (ITERATOR_OF_ADDABLE start, ITERATOR_OF_ADDABLE end);
        template <typename INORDER_COMPARER, typename ITERATOR_OF_ADDABLE, enable_if_t<Common::IsStrictInOrderComparer<INORDER_COMPARER, T> ()>* = nullptr>
        SortedCollection_LinkedList (INORDER_COMPARER&& inOrderComparer, ITERATOR_OF_ADDABLE start, ITERATOR_OF_ADDABLE end);

    public:
        /**
         */
        nonvirtual SortedCollection_LinkedList& operator= (SortedCollection_LinkedList&& rhs) = default;
        nonvirtual SortedCollection_LinkedList& operator= (const SortedCollection_LinkedList& rhs) = default;

    protected:
        using _IterableRepSharedPtr   = typename Iterable<T>::_IterableRepSharedPtr;
        using _CollectionRepSharedPtr = typename Collection<T>::_IRepSharedPtr;

    private:
        class IImplRepBase_;
        template <typename INORDER_COMPARER>
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
