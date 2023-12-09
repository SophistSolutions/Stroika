/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_SortedMapping_inl_
#define _Stroika_Foundation_Containers_SortedMapping_inl_

#include "../Debug/Assertions.h"
#include "Factory/SortedMapping_Factory.h"

namespace Stroika::Foundation::Containers {

    /*
     ********************************************************************************
     **************** SortedMapping<KEY_TYPE, MAPPED_VALUE_TYPE> ********************
     ********************************************************************************
     */
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline SortedMapping<KEY_TYPE, MAPPED_VALUE_TYPE>::SortedMapping ()
        requires (IInOrderComparer<less<KEY_TYPE>, KEY_TYPE>)
        : SortedMapping{less<KEY_TYPE>{}}
    {
        _AssertRepValidType ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <IInOrderComparer<KEY_TYPE> KEY_INORDER_COMPARER>
    inline SortedMapping<KEY_TYPE, MAPPED_VALUE_TYPE>::SortedMapping (KEY_INORDER_COMPARER&& inorderComparer)
        : inherited{Factory::SortedMapping_Factory<KEY_TYPE, MAPPED_VALUE_TYPE, remove_cvref_t<KEY_INORDER_COMPARER>>::Default () (
              forward<KEY_INORDER_COMPARER> (inorderComparer))}
    {
        _AssertRepValidType ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline SortedMapping<KEY_TYPE, MAPPED_VALUE_TYPE>::SortedMapping (const initializer_list<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>& src)
        requires (IInOrderComparer<less<KEY_TYPE>, KEY_TYPE>)
        : SortedMapping{}
    {
        this->AddAll (src);
        _AssertRepValidType ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <IInOrderComparer<KEY_TYPE> KEY_INORDER_COMPARER>
    inline SortedMapping<KEY_TYPE, MAPPED_VALUE_TYPE>::SortedMapping (KEY_INORDER_COMPARER&& inorderComparer,
                                                                      const initializer_list<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>& src)
        : SortedMapping{forward<KEY_INORDER_COMPARER> (inorderComparer)}
    {
        this->AddAll (src);
        _AssertRepValidType ();
    }
#if !qCompilerAndStdLib_RequiresNotMatchInlineOutOfLineForTemplateClassBeingDefined_Buggy
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <IIterableOf<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>> ITERABLE_OF_ADDABLE>
    inline SortedMapping<KEY_TYPE, MAPPED_VALUE_TYPE>::SortedMapping (ITERABLE_OF_ADDABLE&& src)
        requires (IInOrderComparer<less<KEY_TYPE>, KEY_TYPE> and
                  not derived_from<remove_cvref_t<ITERABLE_OF_ADDABLE>, SortedMapping<KEY_TYPE, MAPPED_VALUE_TYPE>>)
        : SortedMapping{}
    {
        _AssertRepValidType ();
        this->AddAll (forward<ITERABLE_OF_ADDABLE> (src));
        _AssertRepValidType ();
    }
#endif
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <IInOrderComparer<KEY_TYPE> KEY_INORDER_COMPARER, IIterableOf<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>> ITERABLE_OF_ADDABLE>
    inline SortedMapping<KEY_TYPE, MAPPED_VALUE_TYPE>::SortedMapping (KEY_INORDER_COMPARER&& inorderComparer, ITERABLE_OF_ADDABLE&& src)
        : SortedMapping{forward<KEY_INORDER_COMPARER> (inorderComparer)}
    {
        _AssertRepValidType ();
        this->AddAll (forward<ITERABLE_OF_ADDABLE> (src));
        _AssertRepValidType ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <IInputIterator<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>> ITERATOR_OF_ADDABLE>
    SortedMapping<KEY_TYPE, MAPPED_VALUE_TYPE>::SortedMapping (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end)
        requires (IInOrderComparer<less<KEY_TYPE>, KEY_TYPE>)
        : SortedMapping{}
    {
        this->AddAll (forward<ITERATOR_OF_ADDABLE> (start), forward<ITERATOR_OF_ADDABLE> (end));
        _AssertRepValidType ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <IInOrderComparer<KEY_TYPE> KEY_INORDER_COMPARER, IInputIterator<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>> ITERATOR_OF_ADDABLE>
    SortedMapping<KEY_TYPE, MAPPED_VALUE_TYPE>::SortedMapping (KEY_INORDER_COMPARER&& inorderComparer, ITERATOR_OF_ADDABLE&& start,
                                                               ITERATOR_OF_ADDABLE&& end)
        : SortedMapping{forward<KEY_INORDER_COMPARER> (inorderComparer)}
    {
        this->AddAll (forward<ITERATOR_OF_ADDABLE> (start), forward<ITERATOR_OF_ADDABLE> (end));
        _AssertRepValidType ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline SortedMapping<KEY_TYPE, MAPPED_VALUE_TYPE>::SortedMapping (const shared_ptr<_IRep>& src) noexcept
        : inherited{(RequireExpression (src != nullptr), src)}
    {
        _AssertRepValidType ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline SortedMapping<KEY_TYPE, MAPPED_VALUE_TYPE>::SortedMapping (shared_ptr<_IRep>&& src) noexcept
        : inherited{(RequireExpression (src != nullptr), move (src))}
    {
        _AssertRepValidType ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline void SortedMapping<KEY_TYPE, MAPPED_VALUE_TYPE>::_AssertRepValidType () const
    {
        if constexpr (qDebug) {
            [[maybe_unused]] _SafeReadRepAccessor<_IRep> ignored{this};
        }
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline auto SortedMapping<KEY_TYPE, MAPPED_VALUE_TYPE>::GetInOrderKeyComparer () const -> KeyInOrderKeyComparerType
    {
        return _SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().GetInOrderKeyComparer ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <derived_from<Iterable<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>> RESULT_CONTAINER, typename INCLUDE_PREDICATE>
    inline RESULT_CONTAINER SortedMapping<KEY_TYPE, MAPPED_VALUE_TYPE>::Where (INCLUDE_PREDICATE&& includeIfTrue) const
        requires (predicate<INCLUDE_PREDICATE, KEY_TYPE> or predicate<INCLUDE_PREDICATE, KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>)
    {
        return inherited::template Where<RESULT_CONTAINER> (forward<INCLUDE_PREDICATE> (includeIfTrue));
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline strong_ordering SortedMapping<KEY_TYPE, MAPPED_VALUE_TYPE>::operator<=> (const SortedMapping& rhs) const
    {
        // Iterate over both mappings at the same time, only looking at the keys. If any two elements are !=,
        // then use the value as a tie-breaker
        using SeqCmp = typename Iterable<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>::template SequentialThreeWayComparer<compare_three_way>;
        return SeqCmp{}(*this, rhs);
    }

}

#endif /* _Stroika_Foundation_Containers_SortedMapping_inl_ */
