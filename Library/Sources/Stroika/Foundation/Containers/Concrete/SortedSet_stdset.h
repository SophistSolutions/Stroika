/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include <set>

#include "../SortedSet.h"

#ifndef _Stroika_Foundation_Containers_Concrete_SortedSet_stdset_h_
#define _Stroika_Foundation_Containers_Concrete_SortedSet_stdset_h_

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 *
 *  TODO:
 *      @todo   Could optimize Equals() test for if both sorted, faster way to compare.
 */

namespace Stroika::Foundation::Containers::Concrete {

    using Common::IInOrderComparer;

    /**
     *  \brief   SortedSet_stdset<T> is an std::set-based concrete implementation of the SortedSet<T> container pattern.
     *
     * \note Performance Notes:
     *      Very good low overhead implementation
     *
     *      o   size () is constant complexity
     *      o   Uses Memory::BlockAllocatorOrStdAllocatorAsAppropriate
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     */
    template <typename T>
    class SortedSet_stdset : public SortedSet<T> {
    private:
        using inherited = SortedSet<T>;

    public:
        /**
         */
        template <typename POTENTIALLY_ADDABLE_T>
        static constexpr bool IsAddable_v = inherited::template IsAddable_v<POTENTIALLY_ADDABLE_T>;
        using ElementEqualityComparerType = typename Set<T>::ElementEqualityComparerType;
        using ElementInOrderComparerType  = typename inherited::ElementInOrderComparerType;
        using value_type                  = typename inherited::value_type;

    public:
        /**
         */
        template <IInOrderComparer<T> INORDER_COMPARER>
        using STDSET =
            set<value_type, INORDER_COMPARER, Memory::BlockAllocatorOrStdAllocatorAsAppropriate<value_type, sizeof (value_type) <= 1024>>;

    public:
        /**
         *  \see docs on SortedSet<> constructor
         *  \req IInOrderComparer<INORDER_COMPARER,T> ()
         */
        SortedSet_stdset ();
        template <IInOrderComparer<T> INORDER_COMPARER>
        explicit SortedSet_stdset (INORDER_COMPARER&& inorderComparer);
        SortedSet_stdset (SortedSet_stdset&& src) noexcept      = default;
        SortedSet_stdset (const SortedSet_stdset& src) noexcept = default;
        SortedSet_stdset (const initializer_list<T>& src);
        template <IInOrderComparer<T> INORDER_COMPARER>
        SortedSet_stdset (INORDER_COMPARER&& inOrderComparer, const initializer_list<T>& src);
        template <ranges::range ITERABLE_OF_ADDABLE, enable_if_t<not is_base_of_v<SortedSet_stdset<T>, decay_t<ITERABLE_OF_ADDABLE>>>* = nullptr>
        explicit SortedSet_stdset (ITERABLE_OF_ADDABLE&& src);
        template <IInOrderComparer<T> INORDER_COMPARER, ranges::range ITERABLE_OF_ADDABLE>
        SortedSet_stdset (INORDER_COMPARER&& inOrderComparer, ITERABLE_OF_ADDABLE&& src);
        template <input_iterator ITERATOR_OF_ADDABLE>
        SortedSet_stdset (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end);
        template <IInOrderComparer<T> INORDER_COMPARER, input_iterator ITERATOR_OF_ADDABLE>
        SortedSet_stdset (INORDER_COMPARER&& inOrderComparer, ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end);

    public:
        /**
         */
        nonvirtual SortedSet_stdset& operator= (SortedSet_stdset&& rhs) noexcept = default;
        nonvirtual SortedSet_stdset& operator= (const SortedSet_stdset& rhs)     = default;

    private:
        class IImplRepBase_;
        template <IInOrderComparer<T> INORDER_COMPARER>
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
#include "SortedSet_stdset.inl"

#endif /*_Stroika_Foundation_Containers_Concrete_SortedSet_stdset_h_ */
