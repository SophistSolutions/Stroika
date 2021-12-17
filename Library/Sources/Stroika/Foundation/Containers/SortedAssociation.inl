/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_SortedAssociation_inl_
#define _Stroika_Foundation_Containers_SortedAssociation_inl_

#include "../Debug/Assertions.h"
#include "Factory/SortedAssociation_Factory.h"

namespace Stroika::Foundation::Containers {

    /*
     ********************************************************************************
     ************* SortedAssociation<KEY_TYPE, MAPPED_VALUE_TYPE> ***********************
     ********************************************************************************
     */
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline SortedAssociation<KEY_TYPE, MAPPED_VALUE_TYPE>::SortedAssociation ()
        : SortedAssociation{less<KEY_TYPE>{}}
    {
        _AssertRepValidType ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <typename KEY_INORDER_COMPARER, enable_if_t<Common::IsStrictInOrderComparer<KEY_INORDER_COMPARER, KEY_TYPE> ()>*>
    inline SortedAssociation<KEY_TYPE, MAPPED_VALUE_TYPE>::SortedAssociation (KEY_INORDER_COMPARER&& inorderComparer)
        : inherited{Factory::SortedAssociation_Factory<KEY_TYPE, MAPPED_VALUE_TYPE, KEY_INORDER_COMPARER> (forward<KEY_INORDER_COMPARER> (inorderComparer)) ()}
    {
        _AssertRepValidType ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline SortedAssociation<KEY_TYPE, MAPPED_VALUE_TYPE>::SortedAssociation (const initializer_list<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>& src)
        : SortedAssociation{}
    {
        this->AddAll (src);
        _AssertRepValidType ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <typename KEY_INORDER_COMPARER, enable_if_t<Common::IsStrictInOrderComparer<KEY_INORDER_COMPARER, KEY_TYPE> ()>*>
    inline SortedAssociation<KEY_TYPE, MAPPED_VALUE_TYPE>::SortedAssociation (KEY_INORDER_COMPARER&& inorderComparer, const initializer_list<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>& src)
        : SortedAssociation{forward<KEY_INORDER_COMPARER> (inorderComparer)}
    {
        this->AddAll (src);
        _AssertRepValidType ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <typename ITERABLE_OF_ADDABLE, enable_if_t<Configuration::IsIterable_v<ITERABLE_OF_ADDABLE> and not is_base_of_v<SortedAssociation<KEY_TYPE, MAPPED_VALUE_TYPE>, decay_t<ITERABLE_OF_ADDABLE>>>*>
    inline SortedAssociation<KEY_TYPE, MAPPED_VALUE_TYPE>::SortedAssociation (ITERABLE_OF_ADDABLE&& src)
        : SortedAssociation{}
    {
        static_assert (IsAddable_v<ExtractValueType_t<ITERABLE_OF_ADDABLE>>);
        _AssertRepValidType ();
        this->AddAll (forward<ITERABLE_OF_ADDABLE> (src));
        _AssertRepValidType ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <typename KEY_INORDER_COMPARER, typename ITERABLE_OF_ADDABLE, enable_if_t<Common::IsStrictInOrderComparer<KEY_INORDER_COMPARER, KEY_TYPE> () and Configuration::IsIterable_v<ITERABLE_OF_ADDABLE>>*>
    inline SortedAssociation<KEY_TYPE, MAPPED_VALUE_TYPE>::SortedAssociation (KEY_INORDER_COMPARER&& inorderComparer, ITERABLE_OF_ADDABLE&& src)
        : SortedAssociation{forward<KEY_INORDER_COMPARER> (inorderComparer)}
    {
        static_assert (IsAddable_v<ExtractValueType_t<ITERABLE_OF_ADDABLE>>);
        _AssertRepValidType ();
        this->AddAll (forward<ITERABLE_OF_ADDABLE> (src));
        _AssertRepValidType ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <typename ITERATOR_OF_ADDABLE, enable_if_t<Configuration::IsIterator_v<ITERATOR_OF_ADDABLE>>*>
    SortedAssociation<KEY_TYPE, MAPPED_VALUE_TYPE>::SortedAssociation (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end)
        : SortedAssociation{}
    {
        static_assert (IsAddable_v<ExtractValueType_t<ITERATOR_OF_ADDABLE>>);
        this->AddAll (forward<ITERATOR_OF_ADDABLE> (start), forward<ITERATOR_OF_ADDABLE> (end));
        _AssertRepValidType ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <typename KEY_INORDER_COMPARER, typename ITERATOR_OF_ADDABLE, enable_if_t<Common::IsStrictInOrderComparer<KEY_INORDER_COMPARER, KEY_TYPE> () and Configuration::IsIterator_v<ITERATOR_OF_ADDABLE>>*>
    SortedAssociation<KEY_TYPE, MAPPED_VALUE_TYPE>::SortedAssociation (KEY_INORDER_COMPARER&& inorderComparer, ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end)
        : SortedAssociation{forward<KEY_INORDER_COMPARER> (inorderComparer)}
    {
        static_assert (IsAddable_v<ExtractValueType_t<ITERATOR_OF_ADDABLE>>);
        this->AddAll (forward<ITERATOR_OF_ADDABLE> (start), forward<ITERATOR_OF_ADDABLE> (end));
        _AssertRepValidType ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline SortedAssociation<KEY_TYPE, MAPPED_VALUE_TYPE>::SortedAssociation (const _IRepSharedPtr& src) noexcept
        : inherited{(RequireNotNull (src), src)}
    {
        _AssertRepValidType ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline SortedAssociation<KEY_TYPE, MAPPED_VALUE_TYPE>::SortedAssociation (_IRepSharedPtr&& src) noexcept
        : inherited{(RequireNotNull (src), move (src))}
    {
        _AssertRepValidType ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline void SortedAssociation<KEY_TYPE, MAPPED_VALUE_TYPE>::_AssertRepValidType () const
    {
#if qDebug
        [[maybe_unused]] _SafeReadRepAccessor<_IRep> ignored{this};
#endif
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline auto SortedAssociation<KEY_TYPE, MAPPED_VALUE_TYPE>::GetInOrderKeyComparer () const -> KeyInOrderKeyComparerType
    {
        return _SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().GetInOrderKeyComparer ();
    }
#if __cpp_impl_three_way_comparison >= 201907
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline strong_ordering SortedAssociation<KEY_TYPE, MAPPED_VALUE_TYPE>::operator<=> (const SortedAssociation& rhs) const
    {
        // nb: no need to take into account comparison on values, because total ordering on keys sequences these elements
        return typename Iterable<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>::SequentialThreeWayComparer{Common::ThreeWayComparerAdapter (GetInOrderKeyComparer ())}(*this, rhs);
    }
#endif

}

#endif /* _Stroika_Foundation_Containers_SortedAssociation_inl_ */
