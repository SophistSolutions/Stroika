/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include <map>

#include "../Mapping.h"

#ifndef _Stroika_Foundation_Containers_Concrete_Mapping_stdmap_h_
#define _Stroika_Foundation_Containers_Concrete_Mapping_stdmap_h_

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 *
 *  TODO:
 */

namespace Stroika::Foundation::Containers::Concrete {

    /**
     *  \brief   Mapping_stdmap<KEY_TYPE, MAPPED_VALUE_TYPE, TRAITS> is an std::map-based concrete implementation of the Mapping<KEY_TYPE, MAPPED_VALUE_TYPE, typename TRAITS::MappingTraitsType> container pattern.
     *
     *  \note   \em Implementation Details
     *          This module is essentially identical to SortedMapping_stdmap, but making it dependent on SortedMapping<> creates
     *          problems with circular dependencies - especially give how the default Mapping CTOR calls the factory class
     *          which maps back to the _stdmap<> variant.
     *
     *          There maybe another (better) way, but this works.
     *
     * \note Performance Notes:
     *      o   GetLength () is constant
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     */
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    class Mapping_stdmap : public Mapping<KEY_TYPE, MAPPED_VALUE_TYPE> {
    private:
        using inherited = Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>;

    public:
        using KeyEqualsCompareFunctionType = typename inherited::KeyEqualsCompareFunctionType;
        using key_type                     = typename inherited::key_type;
        using value_type                   = typename inherited::value_type;
        using mapped_type                  = typename inherited::mapped_type;

    public:
        /**
         *  \brief STDMAP is std::map<> that can be used inside Mapping_stdmap
         */
        template <typename KEY_INORDER_COMPARER = less<key_type>>
        using STDMAP = map<KEY_TYPE, MAPPED_VALUE_TYPE, KEY_INORDER_COMPARER, Memory::BlockAllocatorOrStdAllocatorAsAppropriate<pair<const key_type, mapped_type>, sizeof (value_type) <= 1024>>;

    public:
        /**
         *  @todo - https://stroika.atlassian.net/browse/STK-652 - add COMPARER constructor overloads like the archtype base class
         */
        Mapping_stdmap ();
        Mapping_stdmap (STDMAP<>&& src);
        template <typename KEY_INORDER_COMPARER, enable_if_t<Common::IsPotentiallyComparerRelation_Old<KEY_TYPE, KEY_INORDER_COMPARER> ()>* = nullptr>
        explicit Mapping_stdmap (const KEY_INORDER_COMPARER& inorderComparer);
        Mapping_stdmap (const Mapping_stdmap& src) = default;
        Mapping_stdmap (Mapping_stdmap&& src)      = default;
        template <typename CONTAINER_OF_ADDABLE, enable_if_t<Configuration::IsIterable_v<CONTAINER_OF_ADDABLE> and not is_convertible_v<const CONTAINER_OF_ADDABLE*, const Mapping_stdmap<KEY_TYPE, MAPPED_VALUE_TYPE>*>>* = nullptr>
        explicit Mapping_stdmap (const CONTAINER_OF_ADDABLE& src);
        template <typename COPY_FROM_ITERATOR_KEYVALUE>
        explicit Mapping_stdmap (COPY_FROM_ITERATOR_KEYVALUE start, COPY_FROM_ITERATOR_KEYVALUE end);

    public:
        /**
         */
        nonvirtual Mapping_stdmap& operator= (const Mapping_stdmap& rhs) = default;

    protected:
        using _IterableRepSharedPtr = typename inherited::_IterableRepSharedPtr;
        using _MappingRepSharedPtr  = typename inherited::_IRepSharedPtr;

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
#include "Mapping_stdmap.inl"

#endif /*_Stroika_Foundation_Containers_Concrete_Mapping_stdmap_h_ */
