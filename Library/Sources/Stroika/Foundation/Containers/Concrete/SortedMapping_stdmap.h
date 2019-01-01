/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include "../SortedMapping.h"

#ifndef _Stroika_Foundation_Containers_Concrete_SortedMapping_stdmap_h_
#define _Stroika_Foundation_Containers_Concrete_SortedMapping_stdmap_h_

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Late</a>
 *
 *  TODO:
 *
 *      @todo   Cleanup IteratorRep_ code.
 *
 *      @todo   Finish using CONTAINER_LOCK_HELPER_START() - synchronization support
 *              THEN - MAYBE - try todo better, but at least do this as starter
 */
namespace Stroika::Foundation::Containers::Concrete {

    /**
     *  \brief   SortedMapping_stdmap<KEY_TYPE,MAPPED_VALUE_TYPE> is an std::map-based concrete implementation of the SortedMapping<KEY_TYPE,MAPPED_VALUE_TYPE> container pattern.
     *
     *  \note   \em Thread-Safety   <a href="thread_safety.html#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     *
     */
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    class SortedMapping_stdmap : public SortedMapping<KEY_TYPE, MAPPED_VALUE_TYPE> {
    private:
        using inherited = SortedMapping<KEY_TYPE, MAPPED_VALUE_TYPE>;

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
