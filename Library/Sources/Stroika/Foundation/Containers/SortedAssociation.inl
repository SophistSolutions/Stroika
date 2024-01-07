/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_SortedAssociation_inl_
#define _Stroika_Foundation_Containers_SortedAssociation_inl_

#include "../Debug/Assertions.h"
#include "Factory/SortedAssociation_Factory.h"

namespace Stroika::Foundation::Containers {

    /*
     ********************************************************************************
     ************* SortedAssociation<KEY_TYPE, MAPPED_VALUE_TYPE> *******************
     ********************************************************************************
     */
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline SortedAssociation<KEY_TYPE, MAPPED_VALUE_TYPE>::SortedAssociation ()
        requires (IInOrderComparer<less<KEY_TYPE>, KEY_TYPE>)
        : SortedAssociation{less<KEY_TYPE>{}}
    {
        _AssertRepValidType ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <IInOrderComparer<KEY_TYPE> KEY_INORDER_COMPARER>
    inline SortedAssociation<KEY_TYPE, MAPPED_VALUE_TYPE>::SortedAssociation (KEY_INORDER_COMPARER&& inorderComparer)
        : inherited{Factory::SortedAssociation_Factory<KEY_TYPE, MAPPED_VALUE_TYPE, remove_cvref_t<KEY_INORDER_COMPARER>>::Default () (
              forward<KEY_INORDER_COMPARER> (inorderComparer))}
    {
        _AssertRepValidType ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline SortedAssociation<KEY_TYPE, MAPPED_VALUE_TYPE>::SortedAssociation (const initializer_list<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>& src)
        requires (IInOrderComparer<less<KEY_TYPE>, KEY_TYPE>)
        : SortedAssociation{}
    {
        this->AddAll (src);
        _AssertRepValidType ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <IInOrderComparer<KEY_TYPE> KEY_INORDER_COMPARER>
    inline SortedAssociation<KEY_TYPE, MAPPED_VALUE_TYPE>::SortedAssociation (KEY_INORDER_COMPARER&& inorderComparer,
                                                                              const initializer_list<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>& src)
        : SortedAssociation{forward<KEY_INORDER_COMPARER> (inorderComparer)}
    {
        this->AddAll (src);
        _AssertRepValidType ();
    }
#if !qCompilerAndStdLib_RequiresNotMatchInlineOutOfLineForTemplateClassBeingDefined_Buggy
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <IIterableOf<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>> ITERABLE_OF_ADDABLE>
    inline SortedAssociation<KEY_TYPE, MAPPED_VALUE_TYPE>::SortedAssociation (ITERABLE_OF_ADDABLE&& src)
        requires (IInOrderComparer<less<KEY_TYPE>, KEY_TYPE> and
                  not derived_from<remove_cvref_t<ITERABLE_OF_ADDABLE>, SortedAssociation<KEY_TYPE, MAPPED_VALUE_TYPE>>)
        : SortedAssociation{}
    {
        _AssertRepValidType ();
        this->AddAll (forward<ITERABLE_OF_ADDABLE> (src));
        _AssertRepValidType ();
    }
#endif
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <IInOrderComparer<KEY_TYPE> KEY_INORDER_COMPARER, IIterableOf<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>> ITERABLE_OF_ADDABLE>
    inline SortedAssociation<KEY_TYPE, MAPPED_VALUE_TYPE>::SortedAssociation (KEY_INORDER_COMPARER&& inorderComparer, ITERABLE_OF_ADDABLE&& src)
        : SortedAssociation{forward<KEY_INORDER_COMPARER> (inorderComparer)}
    {
        _AssertRepValidType ();
        this->AddAll (forward<ITERABLE_OF_ADDABLE> (src));
        _AssertRepValidType ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <IInputIterator<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>> ITERATOR_OF_ADDABLE>
    SortedAssociation<KEY_TYPE, MAPPED_VALUE_TYPE>::SortedAssociation (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end)
        requires (IInOrderComparer<less<KEY_TYPE>, KEY_TYPE>)
        : SortedAssociation{}
    {
        this->AddAll (forward<ITERATOR_OF_ADDABLE> (start), forward<ITERATOR_OF_ADDABLE> (end));
        _AssertRepValidType ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <IInOrderComparer<KEY_TYPE> KEY_INORDER_COMPARER, IInputIterator<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>> ITERATOR_OF_ADDABLE>
    SortedAssociation<KEY_TYPE, MAPPED_VALUE_TYPE>::SortedAssociation (KEY_INORDER_COMPARER&& inorderComparer, ITERATOR_OF_ADDABLE&& start,
                                                                       ITERATOR_OF_ADDABLE&& end)
        : SortedAssociation{forward<KEY_INORDER_COMPARER> (inorderComparer)}
    {
        this->AddAll (forward<ITERATOR_OF_ADDABLE> (start), forward<ITERATOR_OF_ADDABLE> (end));
        _AssertRepValidType ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline SortedAssociation<KEY_TYPE, MAPPED_VALUE_TYPE>::SortedAssociation (const shared_ptr<_IRep>& src) noexcept
        : inherited{(RequireExpression (src != nullptr), src)}
    {
        _AssertRepValidType ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline SortedAssociation<KEY_TYPE, MAPPED_VALUE_TYPE>::SortedAssociation (shared_ptr<_IRep>&& src) noexcept
        : inherited{(RequireExpression (src != nullptr), move (src))}
    {
        _AssertRepValidType ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline void SortedAssociation<KEY_TYPE, MAPPED_VALUE_TYPE>::_AssertRepValidType () const
    {
        if constexpr (qDebug) {
            [[maybe_unused]] _SafeReadRepAccessor<_IRep> ignored{this};
        }
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline auto SortedAssociation<KEY_TYPE, MAPPED_VALUE_TYPE>::GetInOrderKeyComparer () const -> KeyInOrderKeyComparerType
    {
        return _SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().GetInOrderKeyComparer ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <typename RESULT_CONTAINER, invocable<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>> ELEMENT_MAPPER>
    inline RESULT_CONTAINER SortedAssociation<KEY_TYPE, MAPPED_VALUE_TYPE>::Map (ELEMENT_MAPPER&& elementMapper) const
        requires (convertible_to<invoke_result_t<ELEMENT_MAPPER, KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>, typename RESULT_CONTAINER::value_type> or
                  convertible_to<invoke_result_t<ELEMENT_MAPPER, KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>, optional<typename RESULT_CONTAINER::value_type>>)
    {
        if constexpr (same_as<RESULT_CONTAINER, SortedAssociation>) {
            // clone the rep so we retain any ordering function/etc, rep type
            return inherited::template Where<RESULT_CONTAINER> (
                forward<ELEMENT_MAPPER> (elementMapper), RESULT_CONTAINER{_SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().CloneEmpty ()});
        }
        else {
            return inherited::template Where<RESULT_CONTAINER> (forward<ELEMENT_MAPPER> (elementMapper)); // default Iterable<> implementation then...
        }
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <derived_from<Iterable<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>> RESULT_CONTAINER, typename INCLUDE_PREDICATE>
    inline RESULT_CONTAINER SortedAssociation<KEY_TYPE, MAPPED_VALUE_TYPE>::Where (INCLUDE_PREDICATE&& includeIfTrue) const
        requires (predicate<INCLUDE_PREDICATE, KEY_TYPE> or predicate<INCLUDE_PREDICATE, KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>)
    {
        if constexpr (same_as<RESULT_CONTAINER, SortedAssociation>) {
            // clone the rep so we retain any ordering function/etc, rep type
            return inherited::template Where<RESULT_CONTAINER> (
                forward<INCLUDE_PREDICATE> (includeIfTrue), RESULT_CONTAINER{_SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().CloneEmpty ()});
        }
        else {
            return inherited::template Where<RESULT_CONTAINER> (forward<INCLUDE_PREDICATE> (includeIfTrue)); // default Iterable<> implementation then...
        }
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline strong_ordering SortedAssociation<KEY_TYPE, MAPPED_VALUE_TYPE>::operator<=> (const SortedAssociation& rhs) const
    {
        // nb: no need to take into account comparison on values, because total ordering on keys sequences these elements
        return typename Iterable<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>::SequentialThreeWayComparer{
            Common::ThreeWayComparerAdapter{GetInOrderKeyComparer ()}}(*this, rhs);
    }

}

#endif /* _Stroika_Foundation_Containers_SortedAssociation_inl_ */
