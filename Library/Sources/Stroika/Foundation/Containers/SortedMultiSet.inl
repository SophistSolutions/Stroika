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
    template <Common::IInOrderComparer<T> INORDER_COMPARER>
    inline SortedMultiSet<T, TRAITS>::SortedMultiSet (INORDER_COMPARER&& inorderComparer)
        : inherited{Factory::SortedMultiSet_Factory<T, TRAITS, decay_t<INORDER_COMPARER>>::Default () (forward<INORDER_COMPARER> (inorderComparer))}
    {
        _AssertRepValidType ();
    }
    template <typename T, typename TRAITS>
    inline SortedMultiSet<T, TRAITS>::SortedMultiSet (const shared_ptr<_IRep>& src) noexcept
        : inherited{(RequireNotNull (src), src)}
    {
        _AssertRepValidType ();
    }
    template <typename T, typename TRAITS>
    inline SortedMultiSet<T, TRAITS>::SortedMultiSet (shared_ptr<_IRep>&& src) noexcept
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
    template <Common::IInOrderComparer<T> INORDER_COMPARER>
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
    template <Common::IInOrderComparer<T> INORDER_COMPARER>
    SortedMultiSet<T, TRAITS>::SortedMultiSet (INORDER_COMPARER&& inorderComparer, const initializer_list<value_type>& src)
        : SortedMultiSet{forward<INORDER_COMPARER> (inorderComparer)}
    {
        this->AddAll (src);
        _AssertRepValidType ();
    }
#if !qCompilerAndStdLib_RequiresNotMatchInlineOutOfLineForTemplateClassBeingDefined_Buggy
    template <typename T, typename TRAITS>
    template <IIterable<CountedValue<T>> ITERABLE_OF_ADDABLE>
    inline SortedMultiSet<T, TRAITS>::SortedMultiSet (ITERABLE_OF_ADDABLE&& src)
        requires (not is_base_of_v<SortedMultiSet<T, TRAITS>, decay_t<ITERABLE_OF_ADDABLE>>)
        : SortedMultiSet{}
    {
        this->AddAll (forward<ITERABLE_OF_ADDABLE> (src));
        _AssertRepValidType ();
    }
#endif
    template <typename T, typename TRAITS>
    template <Common::IInOrderComparer<T> INORDER_COMPARER, IIterable<CountedValue<T>> ITERABLE_OF_ADDABLE>
    inline SortedMultiSet<T, TRAITS>::SortedMultiSet (INORDER_COMPARER&& inorderComparer, ITERABLE_OF_ADDABLE&& src)
        : SortedMultiSet{forward<INORDER_COMPARER> (inorderComparer)}
    {
        this->AddAll (forward<ITERABLE_OF_ADDABLE> (src));
        _AssertRepValidType ();
    }
    template <typename T, typename TRAITS>
    template <IInputIterator<CountedValue<T>> ITERATOR_OF_ADDABLE>
    SortedMultiSet<T, TRAITS>::SortedMultiSet (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end)
        : SortedMultiSet{}
    {
        AddAll (forward<ITERATOR_OF_ADDABLE> (start), forward<ITERATOR_OF_ADDABLE> (end));
        _AssertRepValidType ();
    }
    template <typename T, typename TRAITS>
    template <IInputIterator<CountedValue<T>> INORDER_COMPARER, IInputIterator<CountedValue<T>> ITERATOR_OF_ADDABLE>
    SortedMultiSet<T, TRAITS>::SortedMultiSet (INORDER_COMPARER&& inorderComparer, ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end)
        : SortedMultiSet{forward<INORDER_COMPARER> (inorderComparer)}
    {
        AddAll (forward<ITERATOR_OF_ADDABLE> (start), forward<ITERATOR_OF_ADDABLE> (end));
        _AssertRepValidType ();
    }
    template <typename T, typename TRAITS>
    inline void SortedMultiSet<T, TRAITS>::_AssertRepValidType () const
    {
        if constexpr (qDebug) {
            _SafeReadRepAccessor<_IRep>{this};
        }
    }
    template <typename T, typename TRAITS>
    inline auto SortedMultiSet<T, TRAITS>::GetElementInOrderComparer () const -> ElementInOrderComparerType
    {
        return _SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().GetElementInOrderComparer ();
    }
    template <typename T, typename TRAITS>
    inline strong_ordering SortedMultiSet<T, TRAITS>::operator<=> (const SortedMultiSet& rhs) const
    {
        return typename Iterable<CountedValue<T>>::SequentialThreeWayComparer{Common::ThreeWayComparerAdapter{GetElementInOrderComparer ()}}(*this, rhs);
    }

}

#endif /* _Stroika_Foundation_Containers_SortedMultiSet_inl_ */
