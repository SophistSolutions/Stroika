/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include <set>

#include "../Collection.h"

#ifndef _Stroika_Foundation_Containers_Concrete_Collection_stdmultiset_h_
#define _Stroika_Foundation_Containers_Concrete_Collection_stdmultiset_h_

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 *
 */

namespace Stroika::Foundation::Containers::Concrete {

    /**
     *  \brief Collection_stdmultiset<T> is an stdmultiset-based concrete implementation of the Collection<T> container pattern.
     *
     * \note Performance Notes:
     *      Collection_stdmultiset<T> is a good implementation of Collections, so long as you have an in-order comparison function to provide
     *
     *      o   size () is O(log N)
     *      o   Uses Memory::UseBlockAllocationIfAppropriate
     *      o   Additions and Removals are O(log N)
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     */
    template <typename T>
    class Collection_stdmultiset : public Collection<T> {
    private:
        using inherited = Collection<T>;

    public:
        template <typename POTENTIALLY_ADDABLE_T>
        static constexpr bool IsAddable_v = inherited::template IsAddable_v<POTENTIALLY_ADDABLE_T>;
        using value_type                  = typename inherited::value_type;

    public:
        /**
         */
        template <typename INORDER_COMPARER>
        using STDMULTISET = multiset<value_type, INORDER_COMPARER, Memory::BlockAllocatorOrStdAllocatorAsAppropriate<value_type, sizeof (value_type) <= 1024>>;

    public:
        /**
         *  \see docs on Collection<T> constructor
         */
        Collection_stdmultiset ();
        template <typename INORDER_COMPARER>
        explicit Collection_stdmultiset (INORDER_COMPARER&& inorderComparer);
        Collection_stdmultiset (Collection_stdmultiset&& src) noexcept      = default;
        Collection_stdmultiset (const Collection_stdmultiset& src) noexcept = default;
        Collection_stdmultiset (const initializer_list<value_type>& src);
        template <typename ITERABLE_OF_ADDABLE, enable_if_t<Configuration::IsIterable_v<ITERABLE_OF_ADDABLE> and not is_base_of_v<Collection_stdmultiset<T>, decay_t<ITERABLE_OF_ADDABLE>>>* = nullptr>
        Collection_stdmultiset (ITERABLE_OF_ADDABLE&& src);
        template <typename ITERATOR_OF_ADDABLE>
        Collection_stdmultiset (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end);

    public:
        /**
         */
        nonvirtual Collection_stdmultiset& operator= (Collection_stdmultiset&& rhs) noexcept = default;
        nonvirtual Collection_stdmultiset& operator= (const Collection_stdmultiset& rhs) = default;

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
#include "Collection_stdmultiset.inl"

#endif /*_Stroika_Foundation_Containers_Concrete_Collection_stdmultiset_h_ */
