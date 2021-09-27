/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include "../SortedKeyedCollection.h"

#ifndef _Stroika_Foundation_Containers_Concrete_SortedKeyedCollection_stdset_h_
#define _Stroika_Foundation_Containers_Concrete_SortedKeyedCollection_stdset_h_

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Late</a>
 *
 *  TODO:
 *
 */
namespace Stroika::Foundation::Containers::Concrete {

    /**
     *  \brief   SortedKeyedCollection_stdset<KEY_TYPE,MAPPED_VALUE_TYPE> is an std::map-based concrete implementation of the SortedKeyedCollection<KEY_TYPE,MAPPED_VALUE_TYPE> container pattern.
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     *
     */
    template <typename T, typename KEY_TYPE, typename TRAITS = KeyedCollection_DefaultTraits<T, KEY_TYPE>>
    class SortedKeyedCollection_stdset : public SortedKeyedCollection<T, KEY_TYPE, TRAITS> {
    private:
        using inherited = SortedKeyedCollection<T, KEY_TYPE, TRAITS>;

    public:
        template <typename KEY_INORDER_COMPARER  = less<KEY_TYPE>,
                  typename DEFAULT_KEY_EXTRACTOR = typename TraitsType::DefaultKeyExtractor,
                  enable_if_t<
                      Common::IsPotentiallyComparerRelation<KEY_TYPE, KEY_INORDER_COMPARER> () and KeyedCollection_IsKeyExctractor<T, KEY_TYPE, DEFAULT_KEY_EXTRACTOR> ()>* = nullptr>
        SortedKeyedCollection_stdset (KEY_INORDER_COMPARER&& keyComparer = KEY_INORDER_COMPARER{});
        template <typename KEY_EXTRACTOR,
                  typename KEY_INORDER_COMPARER = less<KEY_TYPE>,
                  enable_if_t<
                      Common::IsPotentiallyComparerRelation<KEY_TYPE, KEY_INORDER_COMPARER> () and KeyedCollection_IsKeyExctractor<T, KEY_TYPE, KEY_EXTRACTOR> ()>* = nullptr>
        SortedKeyedCollection_stdset (KEY_EXTRACTOR&& keyExtractor, KEY_INORDER_COMPARER&& keyComparer = KEY_INORDER_COMPARER{});
        SortedKeyedCollection_stdset (const SortedKeyedCollection_stdset& src) noexcept = default;

    public:
        /**
         */
        nonvirtual SortedKeyedCollection_stdset& operator= (const SortedKeyedCollection_stdset& rhs) = default;

    private:
        class IImplRepBase_;
        template <typename KEY_EXTRACTOR, typename KEY_INORDER_COMPARER>
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
#include "SortedKeyedCollection_stdset.inl"

#endif /*_Stroika_Foundation_Containers_Concrete_SortedKeyedCollection_stdset_h_ */
