/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
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
        : SortedAssociation{less<KEY_TYPE>{}}
    {
        _AssertRepValidType ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <IInOrderComparer<KEY_TYPE> KEY_INORDER_COMPARER>
    inline SortedAssociation<KEY_TYPE, MAPPED_VALUE_TYPE>::SortedAssociation (KEY_INORDER_COMPARER&& inorderComparer)
        : inherited{Factory::SortedAssociation_Factory<KEY_TYPE, MAPPED_VALUE_TYPE, decay_t<KEY_INORDER_COMPARER>>::Default () (
              forward<KEY_INORDER_COMPARER> (inorderComparer))}
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
    template <ranges::range ITERABLE_OF_ADDABLE>
    inline SortedAssociation<KEY_TYPE, MAPPED_VALUE_TYPE>::SortedAssociation (ITERABLE_OF_ADDABLE&& src)
        requires (not derived_from<decay_t<ITERABLE_OF_ADDABLE>, SortedAssociation<KEY_TYPE, MAPPED_VALUE_TYPE>>)
        : SortedAssociation{}
    {
        _AssertRepValidType ();
        this->AddAll (forward<ITERABLE_OF_ADDABLE> (src));
        _AssertRepValidType ();
    }
#endif
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <IInOrderComparer<KEY_TYPE> KEY_INORDER_COMPARER, ranges::range ITERABLE_OF_ADDABLE>
    inline SortedAssociation<KEY_TYPE, MAPPED_VALUE_TYPE>::SortedAssociation (KEY_INORDER_COMPARER&& inorderComparer, ITERABLE_OF_ADDABLE&& src)
        : SortedAssociation{forward<KEY_INORDER_COMPARER> (inorderComparer)}
    {
        _AssertRepValidType ();
        this->AddAll (forward<ITERABLE_OF_ADDABLE> (src));
        _AssertRepValidType ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <IInputIteratorOfT<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>> ITERATOR_OF_ADDABLE>
    SortedAssociation<KEY_TYPE, MAPPED_VALUE_TYPE>::SortedAssociation (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end)
        : SortedAssociation{}
    {
        this->AddAll (forward<ITERATOR_OF_ADDABLE> (start), forward<ITERATOR_OF_ADDABLE> (end));
        _AssertRepValidType ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <IInOrderComparer<KEY_TYPE> KEY_INORDER_COMPARER, IInputIteratorOfT<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>> ITERATOR_OF_ADDABLE>
    SortedAssociation<KEY_TYPE, MAPPED_VALUE_TYPE>::SortedAssociation (KEY_INORDER_COMPARER&& inorderComparer, ITERATOR_OF_ADDABLE&& start,
                                                                       ITERATOR_OF_ADDABLE&& end)
        : SortedAssociation{forward<KEY_INORDER_COMPARER> (inorderComparer)}
    {
        this->AddAll (forward<ITERATOR_OF_ADDABLE> (start), forward<ITERATOR_OF_ADDABLE> (end));
        _AssertRepValidType ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline SortedAssociation<KEY_TYPE, MAPPED_VALUE_TYPE>::SortedAssociation (const shared_ptr<_IRep>& src) noexcept
        : inherited{(RequireNotNull (src), src)}
    {
        _AssertRepValidType ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline SortedAssociation<KEY_TYPE, MAPPED_VALUE_TYPE>::SortedAssociation (shared_ptr<_IRep>&& src) noexcept
        : inherited{(RequireNotNull (src), move (src))}
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
    inline strong_ordering SortedAssociation<KEY_TYPE, MAPPED_VALUE_TYPE>::operator<=> (const SortedAssociation& rhs) const
    {
        // nb: no need to take into account comparison on values, because total ordering on keys sequences these elements
        return typename Iterable<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>::SequentialThreeWayComparer{
            Common::ThreeWayComparerAdapter{GetInOrderKeyComparer ()}}(*this, rhs);
    }

}

#endif /* _Stroika_Foundation_Containers_SortedAssociation_inl_ */
