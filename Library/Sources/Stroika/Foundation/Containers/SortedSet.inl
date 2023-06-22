/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
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
    template <IInOrderComparer<T> INORDER_COMPARER>
    inline SortedSet<T>::SortedSet (INORDER_COMPARER&& inorderComparer)
        : inherited{Factory::SortedSet_Factory<T, remove_cvref_t<INORDER_COMPARER>>::Default () (forward<INORDER_COMPARER> (inorderComparer))}
    {
        _AssertRepValidType ();
    }
    template <typename T>
    inline SortedSet<T>::SortedSet (const _IRepSharedPtr& src) noexcept
        : inherited{(RequireNotNull (src), src)}
    {
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
    template <IInOrderComparer<T> INORDER_COMPARER>
    inline SortedSet<T>::SortedSet (INORDER_COMPARER&& inOrderComparer, const initializer_list<T>& src)
        : SortedSet{forward<INORDER_COMPARER> (inOrderComparer)}
    {
        this->AddAll (src);
        _AssertRepValidType ();
    }
#if !qCompilerAndStdLib_RequiresNotMatchInlineOutOfLineForTemplateClassBeingDefined_Buggy
    template <typename T>
    template <IIterableOf<T> ITERABLE_OF_ADDABLE>
    inline SortedSet<T>::SortedSet (ITERABLE_OF_ADDABLE&& src)
        requires (not derived_from<remove_cvref_t<ITERABLE_OF_ADDABLE>, SortedSet<T>>)
        : SortedSet{}
    {
        this->AddAll (forward<ITERABLE_OF_ADDABLE> (src));
        _AssertRepValidType ();
    }
#endif
    template <typename T>
    template <IInOrderComparer<T> INORDER_COMPARER, IIterableOf<T> ITERABLE_OF_ADDABLE>
    inline SortedSet<T>::SortedSet (INORDER_COMPARER&& inOrderComparer, ITERABLE_OF_ADDABLE&& src)
        : SortedSet (forward<INORDER_COMPARER> (inOrderComparer))
    {
        this->AddAll (forward<ITERABLE_OF_ADDABLE> (src));
        _AssertRepValidType ();
    }
    template <typename T>
    template <IInputIterator<T> ITERATOR_OF_ADDABLE>
    inline SortedSet<T>::SortedSet (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end)
        : SortedSet{}
    {
        this->AddAll (forward<ITERATOR_OF_ADDABLE> (start), forward<ITERATOR_OF_ADDABLE> (end));
        _AssertRepValidType ();
    }
    template <typename T>
    template <IInOrderComparer<T> INORDER_COMPARER, IInputIterator<T> ITERATOR_OF_ADDABLE>
    inline SortedSet<T>::SortedSet (INORDER_COMPARER&& inOrderComparer, ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end)
        : SortedSet{forward<INORDER_COMPARER> (inOrderComparer)}
    {
        this->AddAll (forward<ITERATOR_OF_ADDABLE> (start), forward<ITERATOR_OF_ADDABLE> (end));
        _AssertRepValidType ();
    }
    template <typename T>
    inline void SortedSet<T>::_AssertRepValidType () const
    {
        if constexpr (qDebug) {
            _SafeReadRepAccessor<_IRep>{this};
        }
    }
    template <typename T>
    inline auto SortedSet<T>::GetInOrderComparer () const -> ElementInOrderComparerType
    {
        return _SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().GetInOrderComparer ();
    }
    template <typename T>
    inline strong_ordering SortedSet<T>::operator<=> (const SortedSet& rhs) const
    {
        return typename Iterable<T>::SequentialThreeWayComparer{Common::ThreeWayComparerAdapter{GetInOrderComparer ()}}(*this, rhs);
    }

}

#endif /* _Stroika_Foundation_Containers_SortedSet_inl_ */
