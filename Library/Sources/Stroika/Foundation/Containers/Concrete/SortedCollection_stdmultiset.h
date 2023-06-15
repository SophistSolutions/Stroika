/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include <set>

#include "../SortedCollection.h"

#ifndef _Stroika_Foundation_Containers_Concrete_SortedCollection_stdmultiset_h_
#define _Stroika_Foundation_Containers_Concrete_SortedCollection_stdmultiset_h_

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 *
 */

namespace Stroika::Foundation::Containers::Concrete {

    /**
     *  \brief SortedCollection_stdmultiset<T> is an stdmultiset-based concrete implementation of the SortedCollection<T> container pattern.
     *
     * \note Performance Notes:
     *      o   size () is O(log N)
     *      o   Uses Memory::UseBlockAllocationIfAppropriate
     *      o   Additions and Removals are O(log N)
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     */
    template <typename T>
    class SortedCollection_stdmultiset : public SortedCollection<T> {
    private:
        using inherited = SortedCollection<T>;

    public:
        template <typename POTENTIALLY_ADDABLE_T>
        static constexpr bool IsAddable_v = inherited::template IsAddable_v<POTENTIALLY_ADDABLE_T>;
        using value_type                  = typename inherited::value_type;
        using InOrderComparerType         = typename SortedCollection<T>::InOrderComparerType;

    public:
        /**
         */
        template <Common::IInOrderComparer<T> INORDER_COMPARER>
        using STDMULTISET =
            multiset<value_type, INORDER_COMPARER, Memory::BlockAllocatorOrStdAllocatorAsAppropriate<value_type, sizeof (value_type) <= 1024>>;

    public:
        /**
         *  \see docs on SortedCollection<> constructor
         */
        SortedCollection_stdmultiset ();
        template <Common::IInOrderComparer<T> INORDER_COMPARER>
        explicit SortedCollection_stdmultiset (INORDER_COMPARER&& inorderComparer);
        SortedCollection_stdmultiset (SortedCollection_stdmultiset&& src) noexcept      = default;
        SortedCollection_stdmultiset (const SortedCollection_stdmultiset& src) noexcept = default;
        SortedCollection_stdmultiset (const initializer_list<T>& src);
        template <Common::IInOrderComparer<T> INORDER_COMPARER>
        SortedCollection_stdmultiset (INORDER_COMPARER&& inOrderComparer, const initializer_list<T>& src);
        template <IIterableOfT<T> ITERABLE_OF_ADDABLE, enable_if_t<not is_base_of_v<SortedCollection_stdmultiset<T>, decay_t<ITERABLE_OF_ADDABLE>>>* = nullptr>
        explicit SortedCollection_stdmultiset (ITERABLE_OF_ADDABLE&& src);
        template <Common::IInOrderComparer<T> INORDER_COMPARER, IIterableOfT<T> ITERABLE_OF_ADDABLE>
        SortedCollection_stdmultiset (INORDER_COMPARER&& inOrderComparer, ITERABLE_OF_ADDABLE&& src);
        template <input_iterator ITERATOR_OF_ADDABLE>
        SortedCollection_stdmultiset (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end);
        template <Common::IInOrderComparer<T> INORDER_COMPARER, input_iterator ITERATOR_OF_ADDABLE>
        SortedCollection_stdmultiset (INORDER_COMPARER&& inOrderComparer, ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end);

    public:
        /**
         */
        nonvirtual SortedCollection_stdmultiset& operator= (SortedCollection_stdmultiset&& rhs) noexcept = default;
        nonvirtual SortedCollection_stdmultiset& operator= (const SortedCollection_stdmultiset& rhs)     = default;

    private:
        class IImplRepBase_;
        template <Common::IInOrderComparer<T> INORDER_COMPARER>
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
#include "SortedCollection_stdmultiset.inl"

#endif /*_Stroika_Foundation_Containers_Concrete_SortedCollection_stdmultiset_h_ */
