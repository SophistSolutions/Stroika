/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_SortedSet_inl_
#define _Stroika_Foundation_Containers_SortedSet_inl_

#include "../Debug/Assertions.h"
#include "Factory/SortedSet_Factory.h"

namespace Stroika::Foundation::Containers {

    /*
     ********************************************************************************
     ********************************** SortedSet<T> ********************************
     ********************************************************************************
     */
    template <typename T>
    inline SortedSet<T>::SortedSet ()
        : SortedSet{less<T>{}}
    {
        _AssertRepValidType ();
    }
    template <typename T>
    template <typename INORDER_COMPARER, enable_if_t<Common::IsStrictInOrderComparer<INORDER_COMPARER, T> ()>*>
    inline SortedSet<T>::SortedSet (INORDER_COMPARER&& inorderComparer)
        : inherited{Factory::SortedSet_Factory<T, INORDER_COMPARER> (forward<INORDER_COMPARER> (inorderComparer)) ()}
    {
        static_assert (Common::IsStrictInOrderComparer<INORDER_COMPARER> (), "StrictInOrder comparer required with SortedSet");
        _AssertRepValidType ();
    }
    template <typename T>
    inline SortedSet<T>::SortedSet (const _IRepSharedPtr& src) noexcept
        : inherited{src}
    {
        RequireNotNull (src);
        _AssertRepValidType ();
    }
    template <typename T>
    inline SortedSet<T>::SortedSet (_IRepSharedPtr&& src) noexcept
        : inherited{(RequireNotNull (src), move (src))}
    {
        _AssertRepValidType ();
    }
    template <typename T>
    inline SortedSet<T>::SortedSet (const initializer_list<T>& src)
        : SortedSet{}
    {
        this->AddAll (src);
        _AssertRepValidType ();
    }
    template <typename T>
    template <typename INORDER_COMPARER, enable_if_t<Common::IsStrictInOrderComparer<INORDER_COMPARER, T> ()>*>
    inline SortedSet<T>::SortedSet (INORDER_COMPARER&& inOrderComparer, const initializer_list<T>& src)
        : SortedSet{forward<INORDER_COMPARER> (inOrderComparer)}
    {
        this->AddAll (src);
        _AssertRepValidType ();
    }
    template <typename T>
    template <typename ITERABLE_OF_ADDABLE, enable_if_t<Configuration::IsIterable_v<ITERABLE_OF_ADDABLE> and not is_base_of_v<SortedSet<T>, decay_t<ITERABLE_OF_ADDABLE>>>*>
    inline SortedSet<T>::SortedSet (const ITERABLE_OF_ADDABLE& src)
        : SortedSet{}
    {
        static_assert (IsAddable_v<ExtractValueType_t<ITERABLE_OF_ADDABLE>>);
        this->AddAll (src);
        _AssertRepValidType ();
    }
    template <typename T>
    template <typename INORDER_COMPARER, typename ITERABLE_OF_ADDABLE, enable_if_t<Common::IsStrictInOrderComparer<INORDER_COMPARER, T> () and Configuration::IsIterable_v<ITERABLE_OF_ADDABLE>>*>
    inline SortedSet<T>::SortedSet (INORDER_COMPARER&& inOrderComparer, const ITERABLE_OF_ADDABLE& src)
        : SortedSet (forward<INORDER_COMPARER> (inOrderComparer))
    {
        static_assert (IsAddable_v<ExtractValueType_t<ITERABLE_OF_ADDABLE>>);
        this->AddAll (src);
        _AssertRepValidType ();
    }
    template <typename T>
    template <typename ITERATOR_OF_ADDABLE, enable_if_t<Configuration::IsIterator_v<ITERATOR_OF_ADDABLE>>*>
    inline SortedSet<T>::SortedSet (ITERATOR_OF_ADDABLE start, ITERATOR_OF_ADDABLE end)
        : SortedSet{}
    {
        static_assert (IsAddable_v<ExtractValueType_t<ITERATOR_OF_ADDABLE>>);
        this->AddAll (start, end);
        _AssertRepValidType ();
    }
    template <typename T>
    template <typename INORDER_COMPARER, typename ITERATOR_OF_ADDABLE, enable_if_t<Common::IsStrictInOrderComparer<INORDER_COMPARER, T> () and Configuration::IsIterator_v<ITERATOR_OF_ADDABLE>>*>
    inline SortedSet<T>::SortedSet (INORDER_COMPARER&& inOrderComparer, ITERATOR_OF_ADDABLE start, ITERATOR_OF_ADDABLE end)
        : SortedSet (forward<INORDER_COMPARER> (inOrderComparer))
    {
        static_assert (IsAddable_v<ExtractValueType_t<ITERATOR_OF_ADDABLE>>);
        this->AddAll (start, end);
        _AssertRepValidType ();
    }
    template <typename T>
    inline void SortedSet<T>::_AssertRepValidType () const
    {
#if qDebug
        _SafeReadRepAccessor<_IRep>{this};
#endif
    }
    template <typename T>
    inline auto SortedSet<T>::GetInOrderComparer () const -> ElementInOrderComparerType
    {
        return _SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().GetInOrderComparer ();
    }
#if __cpp_impl_three_way_comparison >= 201907
    template <typename T>
    inline strong_ordering SortedSet<T>::operator<=> (const SortedSet& rhs) const
    {
        return typename Iterable<T>::SequentialThreeWayComparer{Common::ThreeWayComparerAdapter (GetInOrderComparer ())}(*this, rhs);
    }
#endif

}

#endif /* _Stroika_Foundation_Containers_SortedSet_inl_ */
