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
        template <typename KEY_INORDER_COMPARER = less<KEY_TYPE>,
                  typename KEY_EXTRACTOR        = typename TraitsType::DefaultKeyExtractor,
                  enable_if_t<
                      Common::IsPotentiallyComparerRelation<KEY_TYPE, KEY_INORDER_COMPARER> () and KeyedCollection_IsKeyExctractor<T, KEY_TYPE, KEY_EXTRACTOR> ()>* = nullptr>
        SortedKeyedCollection_stdset (KEY_INORDER_COMPARER&& keyComparer = KEY_INORDER_COMPARER{});
        template <typename KEY_EXTRACTOR,
                  typename KEY_INORDER_COMPARER = less<KEY_TYPE>,
                  enable_if_t<
                      Common::IsPotentiallyComparerRelation<KEY_TYPE, KEY_INORDER_COMPARER> () and KeyedCollection_IsKeyExctractor<T, KEY_TYPE, KEY_EXTRACTOR> ()>* = nullptr>
        SortedKeyedCollection_stdset (KEY_EXTRACTOR&& keyExtractor, KEY_INORDER_COMPARER&& keyComparer = KEY_INORDER_COMPARER{});
        SortedKeyedCollection_stdset (const SortedKeyedCollection_stdset& src) noexcept = default;
        template <typename CONTAINER_OF_ADDABLE,
                  typename KEY_EXTRACTOR        = typename TraitsType::DefaultKeyExtractor,
                  typename KEY_INORDER_COMPARER = equal_to<KEY_TYPE>,
                  enable_if_t<
                      Configuration::IsIterableOfT_v<CONTAINER_OF_ADDABLE, T> and not is_base_of_v<SortedKeyedCollection_stdset<T, KEY_TYPE, TRAITS>, Configuration::remove_cvref_t<CONTAINER_OF_ADDABLE>> and Common::IsPotentiallyComparerRelation<KEY_TYPE, KEY_INORDER_COMPARER> () and KeyedCollection_IsKeyExctractor<T, KEY_TYPE, KEY_EXTRACTOR> ()>* = nullptr>
        SortedKeyedCollection_stdset (CONTAINER_OF_ADDABLE&& src);
        template <typename CONTAINER_OF_ADDABLE,
                  typename KEY_EXTRACTOR        = typename TraitsType::DefaultKeyExtractor,
                  typename KEY_INORDER_COMPARER = less<KEY_TYPE>,
                  enable_if_t<
                      Configuration::IsIterableOfT_v<CONTAINER_OF_ADDABLE, T> and not is_base_of_v<SortedKeyedCollection_stdset<T, KEY_TYPE, TRAITS>, Configuration::remove_cvref_t<CONTAINER_OF_ADDABLE>> and Common::IsPotentiallyComparerRelation<KEY_TYPE, KEY_INORDER_COMPARER> () and KeyedCollection_IsKeyExctractor<T, KEY_TYPE, KEY_EXTRACTOR> ()>* = nullptr>
        SortedKeyedCollection_stdset (KEY_INORDER_COMPARER&& keyComparer, CONTAINER_OF_ADDABLE&& src);
        template <typename KEY_EXTRACTOR, typename KEY_INORDER_COMPARER, typename CONTAINER_OF_ADDABLE,
                  enable_if_t<
                      KeyedCollection_IsKeyExctractor<T, KEY_TYPE, KEY_EXTRACTOR> () and Common::IsPotentiallyComparerRelation<KEY_TYPE, KEY_INORDER_COMPARER> () and Configuration::IsIterableOfT_v<CONTAINER_OF_ADDABLE, T>>* = nullptr>
        SortedKeyedCollection_stdset (KEY_EXTRACTOR&& keyExtractor, KEY_INORDER_COMPARER&& keyComparer, CONTAINER_OF_ADDABLE&& src);
        template <typename COPY_FROM_ITERATOR_OF_ADDABLE,
                  typename KEY_EXTRACTOR        = typename TraitsType::DefaultKeyExtractor,
                  typename KEY_INORDER_COMPARER = less<KEY_TYPE>,
                  enable_if_t<
                      Configuration::is_iterator_v<COPY_FROM_ITERATOR_OF_ADDABLE> and KeyedCollection_IsKeyExctractor<T, KEY_TYPE, KEY_EXTRACTOR> () and Common::IsPotentiallyComparerRelation<KEY_TYPE, KEY_INORDER_COMPARER> ()>* = nullptr>
        SortedKeyedCollection_stdset (COPY_FROM_ITERATOR_OF_ADDABLE start, COPY_FROM_ITERATOR_OF_ADDABLE end);
        template <typename COPY_FROM_ITERATOR_OF_ADDABLE,
                  typename KEY_EXTRACTOR        = typename TraitsType::DefaultKeyExtractor,
                  typename KEY_INORDER_COMPARER = less<KEY_TYPE>,
                  enable_if_t<
                      Configuration::is_iterator_v<COPY_FROM_ITERATOR_OF_ADDABLE> and KeyedCollection_IsKeyExctractor<T, KEY_TYPE, KEY_EXTRACTOR> () and Common::IsPotentiallyComparerRelation<KEY_TYPE, KEY_INORDER_COMPARER> ()>* = nullptr>
        SortedKeyedCollection_stdset (KEY_INORDER_COMPARER&& keyComparer, COPY_FROM_ITERATOR_OF_ADDABLE start, COPY_FROM_ITERATOR_OF_ADDABLE end);
        template <typename KEY_EXTRACTOR, typename KEY_INORDER_COMPARER, typename COPY_FROM_ITERATOR_OF_ADDABLE,
                  enable_if_t<
                      KeyedCollection_IsKeyExctractor<T, KEY_TYPE, KEY_EXTRACTOR> () and Common::IsPotentiallyComparerRelation<KEY_TYPE, KEY_INORDER_COMPARER> () and Configuration::is_iterator_v<COPY_FROM_ITERATOR_OF_ADDABLE>>* = nullptr>
        SortedKeyedCollection_stdset (KEY_EXTRACTOR&& keyExtractor, KEY_INORDER_COMPARER&& keyComparer, COPY_FROM_ITERATOR_OF_ADDABLE start, COPY_FROM_ITERATOR_OF_ADDABLE end);
#if 0
        template <typename KEY_INORDER_COMPARER  = less<KEY_TYPE>,
                  typename DEFAULT_KEY_EXTRACTOR = typename TRAITS::DefaultKeyExtractor,
                  enable_if_t<
                      Common::IsPotentiallyComparerRelation<KEY_TYPE, KEY_INORDER_COMPARER> () and KeyedCollection_IsKeyExctractor<T, KEY_TYPE, DEFAULT_KEY_EXTRACTOR> ()>* = nullptr>
        SortedKeyedCollection_stdset (KEY_INORDER_COMPARER&& keyComparer = KEY_INORDER_COMPARER{});
        template <typename KEY_EXTRACTOR,
                  typename KEY_INORDER_COMPARER = less<KEY_TYPE>,
                  enable_if_t<
                      Common::IsPotentiallyComparerRelation<KEY_TYPE, KEY_INORDER_COMPARER> () and KeyedCollection_IsKeyExctractor<T, KEY_TYPE, KEY_EXTRACTOR> ()>* = nullptr>
        SortedKeyedCollection_stdset (KEY_EXTRACTOR&& keyExtractor, KEY_INORDER_COMPARER&& keyComparer = KEY_INORDER_COMPARER{});
        SortedKeyedCollection_stdset (const SortedKeyedCollection_stdset& src) noexcept = default;
#endif
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
