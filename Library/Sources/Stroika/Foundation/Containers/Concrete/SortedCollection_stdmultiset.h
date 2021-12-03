/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
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
     *  \brief SortedCollection_stdmultiset<T> is an stdset-based concrete implementation of the SortedCollection<T> container pattern.
     *
     * \note Performance Notes:
     *      o   GetLength () is O(log N)
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
        using value_type          = typename inherited::value_type;
        using InOrderComparerType = typename SortedCollection<T>::InOrderComparerType;

    public:
        /**
         */
        template <typename INORDER_COMPARER>
        using STDSET = multiset<value_type, INORDER_COMPARER, Memory::BlockAllocatorOrStdAllocatorAsAppropriate<value_type, sizeof (value_type) <= 1024>>;

    public:
        /**
         *  @todo - https://stroika.atlassian.net/browse/STK-652 - add COMPARER constructor overloads like the archtype base class
         */
        SortedCollection_stdmultiset ();
        template <typename INORDER_COMPARER>
        explicit SortedCollection_stdmultiset (const INORDER_COMPARER& inorderComparer);
        SortedCollection_stdmultiset (const SortedCollection_stdmultiset& src) noexcept = default;
        SortedCollection_stdmultiset (SortedCollection_stdmultiset&& src) noexcept      = default;
        SortedCollection_stdmultiset (const initializer_list<value_type>& src);
        SortedCollection_stdmultiset (const InOrderComparerType& inOrderComparer, const initializer_list<value_type>& src);
        template <typename CONTAINER_OF_ADDABLE, enable_if_t<Configuration::IsIterableOfT_v<CONTAINER_OF_ADDABLE, T> and not is_base_of_v<Collection<T>, decay_t<CONTAINER_OF_ADDABLE>>>* = nullptr>
        SortedCollection_stdmultiset (CONTAINER_OF_ADDABLE&& src);
        template <typename CONTAINER_OF_ADDABLE, enable_if_t<Configuration::IsIterableOfT_v<CONTAINER_OF_ADDABLE, T>>* = nullptr>
        SortedCollection_stdmultiset (const InOrderComparerType& inOrderComparer, CONTAINER_OF_ADDABLE&& src);
        template <typename COPY_FROM_ITERATOR_OF_T>
        SortedCollection_stdmultiset (COPY_FROM_ITERATOR_OF_T start, COPY_FROM_ITERATOR_OF_T end);
        template <typename COPY_FROM_ITERATOR_OF_T>
        SortedCollection_stdmultiset (const InOrderComparerType& inOrderComparer, COPY_FROM_ITERATOR_OF_T start, COPY_FROM_ITERATOR_OF_T end);

    public:
        /**
         */
        nonvirtual SortedCollection_stdmultiset& operator= (const SortedCollection_stdmultiset& rhs) = default;

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
#include "SortedCollection_stdmultiset.inl"

#endif /*_Stroika_Foundation_Containers_Concrete_SortedCollection_stdmultiset_h_ */
