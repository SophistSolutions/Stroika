/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include <map>

#include "../SortedMultiSet.h"

#ifndef _Stroika_Foundation_Containers_Concrete_SortedMultiSet_stdmap_h_
#define _Stroika_Foundation_Containers_Concrete_SortedMultiSet_stdmap_h_ 1

/**
  *  \file
  *
  *  \version    <a href="Code-Status.md#Beta">Beta</a>
  *
  */

namespace Stroika::Foundation::Containers::Concrete {

    /**
     *
     *
     * \note Performance Notes:
     *      Very good low overhead implementation
     *
     *      o   GetLength () is constant
     *      o   Uses Memory::BlockAllocatorOrStdAllocatorAsAppropriate
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     *
     */
    template <typename T, typename TRAITS = DefaultTraits::MultiSet<T>>
    class SortedMultiSet_stdmap : public SortedMultiSet<T, TRAITS> {
    private:
        using inherited = SortedMultiSet<T, TRAITS>;

    public:
        template <typename POTENTIALLY_ADDABLE_T>
        static constexpr bool IsAddable_v = inherited::template IsAddable_v<POTENTIALLY_ADDABLE_T>;
        using TraitsType                  = typename inherited::TraitsType;
        using CounterType                 = typename inherited::CounterType;
        using ElementEqualityComparerType = typename inherited::ElementEqualityComparerType;
        using ElementInOrderComparerType  = typename inherited::ElementInOrderComparerType;
        using value_type                  = typename inherited::value_type;

    public:
        /**
         *  \brief STDMAP is std::map<> that can be used inside MultiSet_stdmap
         */
        template <typename INORDER_COMPARER = less<T>>
        using STDMAP = map<T, CounterType, INORDER_COMPARER, Memory::BlockAllocatorOrStdAllocatorAsAppropriate<pair<const T, CounterType>, sizeof (value_type) <= 1024>>;

    public:
        /**
         */
        SortedMultiSet_stdmap ();
        template <typename INORDER_COMPARER, enable_if_t<Common::IsStrictInOrderComparer<INORDER_COMPARER, T> ()>* = nullptr>
        explicit SortedMultiSet_stdmap (INORDER_COMPARER&& inorderComparer);
        SortedMultiSet_stdmap (SortedMultiSet_stdmap&& src) noexcept      = default;
        SortedMultiSet_stdmap (const SortedMultiSet_stdmap& src) noexcept = default;
        SortedMultiSet_stdmap (const initializer_list<T>& src);
        template <typename INORDER_COMPARER, enable_if_t<Common::IsStrictInOrderComparer<INORDER_COMPARER, T> ()>* = nullptr>
        SortedMultiSet_stdmap (INORDER_COMPARER&& inorderComparer, const initializer_list<T>& src);
        SortedMultiSet_stdmap (const initializer_list<value_type>& src);
        template <typename INORDER_COMPARER, enable_if_t<Common::IsStrictInOrderComparer<INORDER_COMPARER, T> ()>* = nullptr>
        SortedMultiSet_stdmap (INORDER_COMPARER&& inorderComparer, const initializer_list<value_type>& src);
        template <typename ITERABLE_OF_ADDABLE, enable_if_t<Configuration::IsIterable_v<ITERABLE_OF_ADDABLE> and not is_base_of_v<SortedMultiSet_stdmap<T, TRAITS>, decay_t<ITERABLE_OF_ADDABLE>>>* = nullptr>
        explicit SortedMultiSet_stdmap (ITERABLE_OF_ADDABLE&& src);
        template <typename INORDER_COMPARER, typename ITERABLE_OF_ADDABLE, enable_if_t<Common::IsStrictInOrderComparer<INORDER_COMPARER, T> () and Configuration::IsIterable_v<ITERABLE_OF_ADDABLE>>* = nullptr>
        SortedMultiSet_stdmap (INORDER_COMPARER&& inorderComparer, ITERABLE_OF_ADDABLE&& src);
        template <typename ITERATOR_OF_ADDABLE, enable_if_t<Configuration::IsIterator_v<ITERATOR_OF_ADDABLE>>* = nullptr>
        SortedMultiSet_stdmap (ITERATOR_OF_ADDABLE start, ITERATOR_OF_ADDABLE end);
        template <typename INORDER_COMPARER, typename ITERATOR_OF_ADDABLE, enable_if_t<Common::IsStrictInOrderComparer<INORDER_COMPARER, T> () and Configuration::IsIterator_v<ITERATOR_OF_ADDABLE>>* = nullptr>
        SortedMultiSet_stdmap (INORDER_COMPARER&& inorderComparer, ITERATOR_OF_ADDABLE start, ITERATOR_OF_ADDABLE end);

    public:
        /**
         */
        nonvirtual SortedMultiSet_stdmap& operator= (SortedMultiSet_stdmap&& rhs) = default;
        nonvirtual SortedMultiSet_stdmap& operator= (const SortedMultiSet_stdmap& rhs) = default;

    protected:
        using _IterableRepSharedPtr = typename inherited::_IterableRepSharedPtr;
        using _MultiSetRepSharedPtr = typename MultiSet<T, TRAITS>::_IRepSharedPtr;

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
#include "SortedMultiSet_stdmap.inl"

#endif /*_Stroika_Foundation_Containers_Concrete_SortedMultiSet_stdmap_h_*/
