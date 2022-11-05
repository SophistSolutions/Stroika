/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_SortedKeyedCollection_inl_
#define _Stroika_Foundation_Containers_SortedKeyedCollection_inl_

#include "../Debug/Assertions.h"

#include "Factory/SortedKeyedCollection_Factory.h"

namespace Stroika::Foundation::Containers {

    /*
     ********************************************************************************
     **************** SortedKeyedCollection<T, KEY_TYPE, TRAITS> ********************
     ********************************************************************************
     */
    template <typename T, typename KEY_TYPE, typename TRAITS>
    template <typename KEY_INORDER_COMPARER,
              typename KEY_EXTRACTOR,
              enable_if_t<
                  Common::IsStrictInOrderComparer<KEY_INORDER_COMPARER, KEY_TYPE> () and KeyedCollection_IsKeyExctractor<T, KEY_TYPE, KEY_EXTRACTOR> ()>*>
    inline SortedKeyedCollection<T, KEY_TYPE, TRAITS>::SortedKeyedCollection (KEY_INORDER_COMPARER&& keyComparer)
        : SortedKeyedCollection{KEY_EXTRACTOR{}, forward<KEY_INORDER_COMPARER> (keyComparer)}
    {
        _AssertRepValidType ();
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    template <typename KEY_EXTRACTOR,
              typename KEY_INORDER_COMPARER,
              enable_if_t<
                  Common::IsStrictInOrderComparer<KEY_INORDER_COMPARER, KEY_TYPE> () and KeyedCollection_IsKeyExctractor<T, KEY_TYPE, KEY_EXTRACTOR> ()>*>
    SortedKeyedCollection<T, KEY_TYPE, TRAITS>::SortedKeyedCollection (KEY_EXTRACTOR&& keyExtractor, KEY_INORDER_COMPARER&& keyComparer)
        // @todo see https://stroika.atlassian.net/browse/STK-933 for why this decay_t is needed - unclear why!
        : inherited (Factory::SortedKeyedCollection_Factory<T, KEY_TYPE, TRAITS, KEY_EXTRACTOR, decay_t<KEY_INORDER_COMPARER>>{forward<KEY_EXTRACTOR> (keyExtractor), forward<KEY_INORDER_COMPARER> (keyComparer)}())
    {
        _AssertRepValidType ();
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    template <typename ITERABLE_OF_ADDABLE,
              typename KEY_EXTRACTOR,
              typename KEY_INORDER_COMPARER,
              enable_if_t<
                  Configuration::IsIterable_v<ITERABLE_OF_ADDABLE> and not is_base_of_v<SortedKeyedCollection<T, KEY_TYPE, TRAITS>, decay_t<ITERABLE_OF_ADDABLE>> and Common::IsStrictInOrderComparer<KEY_INORDER_COMPARER, KEY_TYPE> () and KeyedCollection_IsKeyExctractor<T, KEY_TYPE, KEY_EXTRACTOR> ()>*>
    inline SortedKeyedCollection<T, KEY_TYPE, TRAITS>::SortedKeyedCollection (ITERABLE_OF_ADDABLE&& src)
        : SortedKeyedCollection{KEY_EXTRACTOR{}, KEY_INORDER_COMPARER{}}
    {
        static_assert (IsAddable_v<ExtractValueType_t<ITERABLE_OF_ADDABLE>>);
        this->AddAll (src);
        _AssertRepValidType ();
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    template <typename ITERABLE_OF_ADDABLE,
              typename KEY_EXTRACTOR,
              typename KEY_INORDER_COMPARER,
              enable_if_t<
                  Configuration::IsIterable_v<ITERABLE_OF_ADDABLE> and not is_base_of_v<SortedKeyedCollection<T, KEY_TYPE, TRAITS>, decay_t<ITERABLE_OF_ADDABLE>> and Common::IsStrictInOrderComparer<KEY_INORDER_COMPARER, KEY_TYPE> () and KeyedCollection_IsKeyExctractor<T, KEY_TYPE, KEY_EXTRACTOR> ()>*>
    inline SortedKeyedCollection<T, KEY_TYPE, TRAITS>::SortedKeyedCollection (KEY_INORDER_COMPARER&& keyComparer, ITERABLE_OF_ADDABLE&& src)
    {
        static_assert (IsAddable_v<ExtractValueType_t<ITERABLE_OF_ADDABLE>>);
        this->AddAll (src);
        _AssertRepValidType ();
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    template <typename KEY_EXTRACTOR, typename KEY_INORDER_COMPARER, typename ITERABLE_OF_ADDABLE,
              enable_if_t<
                  KeyedCollection_IsKeyExctractor<T, KEY_TYPE, KEY_EXTRACTOR> () and Common::IsStrictInOrderComparer<KEY_INORDER_COMPARER, KEY_TYPE> () and Configuration::IsIterable_v<ITERABLE_OF_ADDABLE>>*>
    inline SortedKeyedCollection<T, KEY_TYPE, TRAITS>::SortedKeyedCollection (KEY_EXTRACTOR&& keyExtractor, KEY_INORDER_COMPARER&& keyComparer, ITERABLE_OF_ADDABLE&& src)
        : SortedKeyedCollection{forward<KEY_EXTRACTOR> (keyExtractor), forward<KEY_INORDER_COMPARER> (keyComparer)}
    {
        static_assert (IsAddable_v<ExtractValueType_t<ITERABLE_OF_ADDABLE>>);
        this->AddAll (src);
        _AssertRepValidType ();
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    template <typename ITERATOR_OF_ADDABLE,
              typename KEY_EXTRACTOR,
              typename KEY_INORDER_COMPARER,
              enable_if_t<
                  Configuration::IsIterator_v<ITERATOR_OF_ADDABLE> and KeyedCollection_IsKeyExctractor<T, KEY_TYPE, KEY_EXTRACTOR> () and Common::IsStrictInOrderComparer<KEY_INORDER_COMPARER, KEY_TYPE> ()>*>
    inline SortedKeyedCollection<T, KEY_TYPE, TRAITS>::SortedKeyedCollection (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end)
        : SortedKeyedCollection{KEY_EXTRACTOR{}, KEY_INORDER_COMPARER{}}
    {
        static_assert (IsAddable_v<ExtractValueType_t<ITERATOR_OF_ADDABLE>>);
        this->AddAll (forward<ITERATOR_OF_ADDABLE> (start), forward<ITERATOR_OF_ADDABLE> (end));
        _AssertRepValidType ();
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    template <typename ITERATOR_OF_ADDABLE,
              typename KEY_EXTRACTOR,
              typename KEY_INORDER_COMPARER,
              enable_if_t<
                  Configuration::IsIterator_v<ITERATOR_OF_ADDABLE> and KeyedCollection_IsKeyExctractor<T, KEY_TYPE, KEY_EXTRACTOR> () and Common::IsStrictInOrderComparer<KEY_INORDER_COMPARER, KEY_TYPE> ()>*>
    inline SortedKeyedCollection<T, KEY_TYPE, TRAITS>::SortedKeyedCollection (KEY_INORDER_COMPARER&& keyComparer, ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end)
        : SortedKeyedCollection{KEY_EXTRACTOR{}, forward<KEY_INORDER_COMPARER> (keyComparer)}
    {
        static_assert (IsAddable_v<ExtractValueType_t<ITERATOR_OF_ADDABLE>>);
        this->AddAll (forward<ITERATOR_OF_ADDABLE> (start), forward<ITERATOR_OF_ADDABLE> (end));
        _AssertRepValidType ();
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    template <typename KEY_EXTRACTOR, typename KEY_INORDER_COMPARER, typename ITERATOR_OF_ADDABLE,
              enable_if_t<
                  KeyedCollection_IsKeyExctractor<T, KEY_TYPE, KEY_EXTRACTOR> () and Common::IsStrictInOrderComparer<KEY_INORDER_COMPARER, KEY_TYPE> () and Configuration::IsIterator_v<ITERATOR_OF_ADDABLE>>*>
    inline SortedKeyedCollection<T, KEY_TYPE, TRAITS>::SortedKeyedCollection (KEY_EXTRACTOR&& keyExtractor, KEY_INORDER_COMPARER&& keyComparer, ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end)
        : SortedKeyedCollection{forward<KEY_EXTRACTOR> (keyExtractor), forward<KEY_INORDER_COMPARER> (keyComparer)}
    {
        static_assert (IsAddable_v<ExtractValueType_t<ITERATOR_OF_ADDABLE>>);
        this->AddAll (forward<ITERATOR_OF_ADDABLE> (start), forward<ITERATOR_OF_ADDABLE> (end));
        _AssertRepValidType ();
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    inline SortedKeyedCollection<T, KEY_TYPE, TRAITS>::SortedKeyedCollection (const _IRepSharedPtr& src) noexcept
        : inherited{src}
    {
        RequireNotNull (src);
        _AssertRepValidType ();
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    inline SortedKeyedCollection<T, KEY_TYPE, TRAITS>::SortedKeyedCollection (_IRepSharedPtr&& src) noexcept
        : inherited{(RequireNotNull (src), move (src))}
    {
        _AssertRepValidType ();
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    inline void SortedKeyedCollection<T, KEY_TYPE, TRAITS>::_AssertRepValidType () const
    {
        if constexpr (qDebug) {
            _SafeReadRepAccessor<_IRep>{this};
        }
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    inline auto SortedKeyedCollection<T, KEY_TYPE, TRAITS>::GetInOrderKeyComparer () const -> KeyInOrderKeyComparerType
    {
        return _SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().GetInOrderKeyComparer ();
    }

}

#endif /* _Stroika_Foundation_Containers_SortedKeyedCollection_inl_ */
