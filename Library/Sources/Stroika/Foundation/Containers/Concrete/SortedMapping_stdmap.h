/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include <map>

#include "../SortedMapping.h"

#ifndef _Stroika_Foundation_Containers_Concrete_SortedMapping_stdmap_h_
#define _Stroika_Foundation_Containers_Concrete_SortedMapping_stdmap_h_

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 *
 */
namespace Stroika::Foundation::Containers::Concrete {

    /**
     *  \brief   SortedMapping_stdmap<KEY_TYPE,MAPPED_VALUE_TYPE> is an std::map-based concrete implementation of the SortedMapping<KEY_TYPE,MAPPED_VALUE_TYPE> container pattern.
     *
     * \note Performance Notes:
     *      Very good low overhead implementation
     *
     *      o   GetLength () is constant
     *      o   Uses Memory::BlockAllocatorOrStdAllocatorAsAppropriate
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     */
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    class SortedMapping_stdmap : public SortedMapping<KEY_TYPE, MAPPED_VALUE_TYPE> {
    private:
        using inherited = SortedMapping<KEY_TYPE, MAPPED_VALUE_TYPE>;

    public:
        using KeyEqualsCompareFunctionType = typename inherited::KeyEqualsCompareFunctionType;
        using KeyInOrderKeyComparerType    = typename inherited::KeyInOrderKeyComparerType;
        using value_type                   = typename inherited::value_type;
        using key_type                     = typename inherited::key_type;
        using mapped_type                  = typename inherited::mapped_type;

    public:
        /**
         *  \brief STDMAP is std::map<> that can be used inside SortedMapping_stdmap
         */
        template <typename KEY_INORDER_COMPARER = less<key_type>>
        using STDMAP = map<KEY_TYPE, MAPPED_VALUE_TYPE, KEY_INORDER_COMPARER, Memory::BlockAllocatorOrStdAllocatorAsAppropriate<pair<const key_type, mapped_type>, sizeof (value_type) <= 1024>>;

    public:
        /**
         *  @todo - https://stroika.atlassian.net/browse/STK-652 - add COMPARER constructor overloads like the archtype base class
         */
        SortedMapping_stdmap ();
        template <typename KEY_INORDER_COMPARER, enable_if_t<Common::IsPotentiallyComparerRelation<KEY_TYPE, KEY_INORDER_COMPARER> ()>* = nullptr>
        explicit SortedMapping_stdmap (const KEY_INORDER_COMPARER& inorderComparer);
        SortedMapping_stdmap (const SortedMapping_stdmap& src) = default;
        template <typename CONTAINER_OF_ADDABLE, enable_if_t<Configuration::IsIterable_v<CONTAINER_OF_ADDABLE> and not is_convertible_v<const CONTAINER_OF_ADDABLE*, const SortedMapping_stdmap<KEY_TYPE, MAPPED_VALUE_TYPE>*>>* = nullptr>
        SortedMapping_stdmap (const CONTAINER_OF_ADDABLE& src);
        template <typename COPY_FROM_ITERATOR_KEYVALUE>
        SortedMapping_stdmap (COPY_FROM_ITERATOR_KEYVALUE start, COPY_FROM_ITERATOR_KEYVALUE end);

    public:
        /**
         */
        nonvirtual SortedMapping_stdmap& operator= (const SortedMapping_stdmap& rhs) = default;

    protected:
        using _IterableRepSharedPtr = typename inherited::_IterableRepSharedPtr;
        using _MappingRepSharedPtr  = typename Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>::_IRepSharedPtr;

    private:
        class IImplRepBase_;
        template <typename KEY_INORDER_COMPARER>
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
#include "SortedMapping_stdmap.inl"

#endif /*_Stroika_Foundation_Containers_Concrete_SortedMapping_stdmap_h_ */
