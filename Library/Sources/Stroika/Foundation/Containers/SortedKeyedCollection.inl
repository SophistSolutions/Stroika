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
     ************* SortedKeyedCollection<T, KEY_TYPE, TRAITS> ***********************
     ********************************************************************************
     */
    template <typename T, typename KEY_TYPE, typename TRAITS>
    template <typename KEY_INORDER_COMPARER,
              typename DEFAULT_KEY_EXTRACTOR,
              enable_if_t<
                  Common::IsPotentiallyComparerRelation<KEY_TYPE, KEY_INORDER_COMPARER> () and KeyedCollection_IsKeyExctractor<T, KEY_TYPE, DEFAULT_KEY_EXTRACTOR> ()>*>
    inline SortedKeyedCollection<T, KEY_TYPE, TRAITS>::SortedKeyedCollection (KEY_INORDER_COMPARER&& keyComparer)
        : SortedKeyedCollection{DEFAULT_KEY_EXTRACTOR{}, keyComparer}
    {
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    template <typename KEY_EXTRACTOR,
              typename KEY_INORDER_COMPARER,
              enable_if_t<
                  Common::IsPotentiallyComparerRelation<KEY_TYPE, KEY_INORDER_COMPARER> () and KeyedCollection_IsKeyExctractor<T, KEY_TYPE, KEY_EXTRACTOR> ()>*>
    SortedKeyedCollection<T, KEY_TYPE, TRAITS>::SortedKeyedCollection (KEY_EXTRACTOR&& keyExtractor, KEY_INORDER_COMPARER&& keyComparer)
        : inherited (Factory::SortedKeyedCollection_Factory<T, KEY_TYPE, TRAITS, KEY_EXTRACTOR, KEY_INORDER_COMPARER>{forward<KEY_EXTRACTOR> (keyExtractor), forward<KEY_INORDER_COMPARER> (keyComparer)}())
    {
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
#if 0
    template <typename T, typename KEY_TYPE, typename TRAITS>
    inline SortedKeyedCollection<T, KEY_TYPE, TRAITS>::SortedKeyedCollection (const initializer_list<T>& src)
        : SortedKeyedCollection{}
    {
        this->AddAll (src);
        _AssertRepValidType ();
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    template <typename INORDER_COMPARER, enable_if_t<Common::IsPotentiallyComparerRelation<T, INORDER_COMPARER> ()>*>
    inline SortedKeyedCollection<T, KEY_TYPE, TRAITS>::SortedKeyedCollection (INORDER_COMPARER&& inOrderComparer, const initializer_list<T>& src)
        : SortedKeyedCollection{forward<INORDER_COMPARER> (inOrderComparer)}
    {
        this->AddAll (src);
        _AssertRepValidType ();
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    template <typename CONTAINER_OF_ADDABLE, enable_if_t<Configuration::IsIterableOfT_v<CONTAINER_OF_ADDABLE, T> and not is_base_of_v<SortedKeyedCollection<T, KEY_TYPE, TRAITS>, Configuration::remove_cvref_t<CONTAINER_OF_ADDABLE>>>*>
    inline SortedKeyedCollection<T, KEY_TYPE, TRAITS>::SortedKeyedCollection (CONTAINER_OF_ADDABLE&& src)
        : SortedKeyedCollection{}
    {
        this->AddAll (forward<CONTAINER_OF_ADDABLE> (src));
        _AssertRepValidType ();
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    template <typename INORDER_COMPARER, typename CONTAINER_OF_ADDABLE, enable_if_t<Common::IsPotentiallyComparerRelation<T, INORDER_COMPARER> () and Configuration::IsIterableOfT_v<CONTAINER_OF_ADDABLE, T>>*>
    inline SortedKeyedCollection<T, KEY_TYPE, TRAITS>::SortedKeyedCollection (INORDER_COMPARER&& inOrderComparer, CONTAINER_OF_ADDABLE&& src)
        : SortedKeyedCollection{forward<INORDER_COMPARER> (inOrderComparer)}
    {
        this->AddAll (forward<CONTAINER_OF_ADDABLE> (src));
        _AssertRepValidType ();
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    template <typename COPY_FROM_ITERATOR_OF_ADDABLE>
    inline SortedKeyedCollection<T, KEY_TYPE, TRAITS>::SortedKeyedCollection (COPY_FROM_ITERATOR_OF_ADDABLE start, COPY_FROM_ITERATOR_OF_ADDABLE end)
        : SortedKeyedCollection{}
    {
        this->AddAll (start, end);
        _AssertRepValidType ();
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    template <typename INORDER_COMPARER, typename COPY_FROM_ITERATOR_OF_ADDABLE, enable_if_t<Common::IsPotentiallyComparerRelation<T, INORDER_COMPARER> ()>*>
    inline SortedKeyedCollection<T, KEY_TYPE, TRAITS>::SortedKeyedCollection (INORDER_COMPARER&& inOrderComparer, COPY_FROM_ITERATOR_OF_ADDABLE start, COPY_FROM_ITERATOR_OF_ADDABLE end)
        : SortedKeyedCollection{forward<INORDER_COMPARER> (inOrderComparer)}
    {
        this->AddAll (start, end);
        _AssertRepValidType ();
    }
#endif
    template <typename T, typename KEY_TYPE, typename TRAITS>
    inline void SortedKeyedCollection<T, KEY_TYPE, TRAITS>::_AssertRepValidType () const
    {
#if qDebug
        _SafeReadRepAccessor<_IRep>{this};
#endif
    }
#if 0
    template <typename T>
    inline auto SortedKeyedCollection<T, KEY_TYPE, TRAITS>::GetInOrderComparer () const -> InOrderComparerType
    {
        return _SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().GetInOrderComparer ();
    }
#if __cpp_impl_three_way_comparison >= 201907
    template <typename T>
    inline bool SortedKeyedCollection<T, KEY_TYPE, TRAITS>::operator== (const SortedKeyedCollection& rhs) const
    {
        return typename Iterable<T>::SequentialEqualsComparer{Common::EqualsComparerAdapter (GetInOrderComparer ())}(*this, rhs);
    }
    template <typename T>
    inline strong_ordering SortedKeyedCollection<T, KEY_TYPE, TRAITS>::operator<=> (const SortedKeyedCollection& rhs) const
    {
        return typename Iterable<T>::SequentialThreeWayComparer{Common::ThreeWayComparerAdapter (GetInOrderComparer ())}(*this, rhs);
    }
#endif
#endif

}

#endif /* _Stroika_Foundation_Containers_SortedKeyedCollection_inl_ */
