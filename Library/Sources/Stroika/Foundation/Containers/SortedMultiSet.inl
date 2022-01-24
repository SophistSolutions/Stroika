/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_SortedMultiSet_inl_
#define _Stroika_Foundation_Containers_SortedMultiSet_inl_

#include "../Debug/Assertions.h"
#include "Factory/SortedMultiSet_Factory.h"

namespace Stroika::Foundation::Containers {

    /*
     ********************************************************************************
     ************************ SortedMultiSet<T, TRAITS> *****************************
     ********************************************************************************
     */
    template <typename T, typename TRAITS>
    inline SortedMultiSet<T, TRAITS>::SortedMultiSet ()
        : SortedMultiSet{less<T>{}}
    {
        _AssertRepValidType ();
    }
    template <typename T, typename TRAITS>
    template <typename INORDER_COMPARER, enable_if_t<Common::IsStrictInOrderComparer<INORDER_COMPARER, T> ()>*>
    inline SortedMultiSet<T, TRAITS>::SortedMultiSet (INORDER_COMPARER&& inorderComparer)
        : inherited{Factory::SortedMultiSet_Factory<T, TRAITS, INORDER_COMPARER> (forward<INORDER_COMPARER> (inorderComparer)) ()}
    {
        static_assert (Common::IsStrictInOrderComparer<INORDER_COMPARER> (), "SortedMultiSet constructor with INORDER_COMPARER - comparer not valid IsStrictInOrderComparer- see ComparisonRelationDeclaration<Common::ComparisonRelationType::eStrictInOrder, function<bool(T, T)>");
        _AssertRepValidType ();
    }
    template <typename T, typename TRAITS>
    inline SortedMultiSet<T, TRAITS>::SortedMultiSet (const _IRepSharedPtr& src) noexcept
        : inherited{(RequireNotNull (src), src)}
    {
        _AssertRepValidType ();
    }
    template <typename T, typename TRAITS>
    inline SortedMultiSet<T, TRAITS>::SortedMultiSet (_IRepSharedPtr&& src) noexcept
        : inherited{(RequireNotNull (src), move (src))}
    {
        _AssertRepValidType ();
    }
    template <typename T, typename TRAITS>
    SortedMultiSet<T, TRAITS>::SortedMultiSet (const initializer_list<T>& src)
        : SortedMultiSet{}
    {
        this->AddAll (src);
        _AssertRepValidType ();
    }
    template <typename T, typename TRAITS>
    template <typename INORDER_COMPARER, enable_if_t<Common::IsStrictInOrderComparer<INORDER_COMPARER, T> ()>*>
    SortedMultiSet<T, TRAITS>::SortedMultiSet (INORDER_COMPARER&& inorderComparer, const initializer_list<T>& src)
        : SortedMultiSet{forward<INORDER_COMPARER> (inorderComparer)}
    {
        this->AddAll (src);
        _AssertRepValidType ();
    }
    template <typename T, typename TRAITS>
    SortedMultiSet<T, TRAITS>::SortedMultiSet (const initializer_list<value_type>& src)
        : SortedMultiSet{}
    {
        this->AddAll (src);
        _AssertRepValidType ();
    }
    template <typename T, typename TRAITS>
    template <typename INORDER_COMPARER, enable_if_t<Common::IsStrictInOrderComparer<INORDER_COMPARER, T> ()>*>
    SortedMultiSet<T, TRAITS>::SortedMultiSet (INORDER_COMPARER&& inorderComparer, const initializer_list<value_type>& src)
        : SortedMultiSet{forward<INORDER_COMPARER> (inorderComparer)}
    {
        this->AddAll (src);
        _AssertRepValidType ();
    }
    template <typename T, typename TRAITS>
    template <typename ITERABLE_OF_ADDABLE, enable_if_t<Configuration::IsIterable_v<ITERABLE_OF_ADDABLE> and not is_base_of_v<SortedMultiSet<T, TRAITS>, decay_t<ITERABLE_OF_ADDABLE>>>*>
    inline SortedMultiSet<T, TRAITS>::SortedMultiSet (ITERABLE_OF_ADDABLE&& src)
        : SortedMultiSet{}
    {
        static_assert (IsAddable_v<ExtractValueType_t<ITERABLE_OF_ADDABLE>>);
        this->AddAll (forward<ITERABLE_OF_ADDABLE> (src));
        _AssertRepValidType ();
    }
    template <typename T, typename TRAITS>
    template <typename INORDER_COMPARER, typename ITERABLE_OF_ADDABLE, enable_if_t<Common::IsStrictInOrderComparer<INORDER_COMPARER, T> () and Configuration::IsIterable_v<ITERABLE_OF_ADDABLE>>*>
    inline SortedMultiSet<T, TRAITS>::SortedMultiSet (INORDER_COMPARER&& inorderComparer, ITERABLE_OF_ADDABLE&& src)
        : SortedMultiSet{forward<INORDER_COMPARER> (inorderComparer)}
    {
        static_assert (IsAddable_v<ExtractValueType_t<ITERABLE_OF_ADDABLE>>);
        this->AddAll (forward<ITERABLE_OF_ADDABLE> (src));
        _AssertRepValidType ();
    }
    template <typename T, typename TRAITS>
    template <typename ITERATOR_OF_ADDABLE, enable_if_t<Configuration::IsIterator_v<ITERATOR_OF_ADDABLE>>*>
    SortedMultiSet<T, TRAITS>::SortedMultiSet (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end)
        : SortedMultiSet{}
    {
        static_assert (IsAddable_v<ExtractValueType_t<ITERATOR_OF_ADDABLE>>);
        AddAll (forward<ITERATOR_OF_ADDABLE> (start), forward<ITERATOR_OF_ADDABLE> (end));
        _AssertRepValidType ();
    }
    template <typename T, typename TRAITS>
    template <typename INORDER_COMPARER, typename ITERATOR_OF_ADDABLE, enable_if_t<Common::IsStrictInOrderComparer<INORDER_COMPARER, T> () and Configuration::IsIterator_v<ITERATOR_OF_ADDABLE>>*>
    SortedMultiSet<T, TRAITS>::SortedMultiSet (INORDER_COMPARER&& inorderComparer, ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end)
        : SortedMultiSet{forward<INORDER_COMPARER> (inorderComparer)}
    {
        static_assert (IsAddable_v<ExtractValueType_t<ITERATOR_OF_ADDABLE>>);
        AddAll (forward<ITERATOR_OF_ADDABLE> (start), forward<ITERATOR_OF_ADDABLE> (end));
        _AssertRepValidType ();
    }
    template <typename T, typename TRAITS>
    inline void SortedMultiSet<T, TRAITS>::_AssertRepValidType () const
    {
#if qDebug
        _SafeReadRepAccessor<_IRep>{this};
#endif
    }
    template <typename T, typename TRAITS>
    inline auto SortedMultiSet<T, TRAITS>::GetElementInOrderComparer () const -> ElementInOrderComparerType
    {
        return _SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().GetElementInOrderComparer ();
    }
    template <typename T, typename TRAITS>
    inline strong_ordering SortedMultiSet<T, TRAITS>::operator<=> (const SortedMultiSet& rhs) const
    {
        return typename Iterable<CountedValue<T>>::SequentialThreeWayComparer{Common::ThreeWayComparerAdapter (GetElementInOrderComparer ())}(*this, rhs);
    }

}

#endif /* _Stroika_Foundation_Containers_SortedMultiSet_inl_ */
