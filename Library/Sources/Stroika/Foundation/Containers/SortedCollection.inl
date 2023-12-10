/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_SortedCollection_inl_
#define _Stroika_Foundation_Containers_SortedCollection_inl_

#include "../Debug/Assertions.h"
#include "Factory/SortedCollection_Factory.h"

namespace Stroika::Foundation::Containers {

    /*
     ********************************************************************************
     ***************************** SortedCollection<T> ******************************
     ********************************************************************************
     */
    template <typename T>
    inline SortedCollection<T>::SortedCollection ()
        requires (IInOrderComparer<less<T>, T>)
        : SortedCollection{less<T>{}}
    {
        _AssertRepValidType ();
    }
    template <typename T>
    template <IInOrderComparer<T> INORDER_COMPARER>
    inline SortedCollection<T>::SortedCollection (INORDER_COMPARER&& inorderComparer)
        : inherited{Factory::SortedCollection_Factory<T, remove_cvref_t<INORDER_COMPARER>> () (forward<INORDER_COMPARER> (inorderComparer))}
    {
        _AssertRepValidType ();
    }
    template <typename T>
    inline SortedCollection<T>::SortedCollection (const shared_ptr<_IRep>& src) noexcept
        : inherited{src}
    {
        RequireNotNull (src);
        _AssertRepValidType ();
    }
    template <typename T>
    inline SortedCollection<T>::SortedCollection (shared_ptr<_IRep>&& src) noexcept
        : inherited{(RequireExpression (src != nullptr), move (src))}
    {
        _AssertRepValidType ();
    }
    template <typename T>
    inline SortedCollection<T>::SortedCollection (const initializer_list<T>& src)
        requires (IInOrderComparer<less<T>, T>)
        : SortedCollection{}
    {
        this->AddAll (src);
        _AssertRepValidType ();
    }
    template <typename T>
    template <IInOrderComparer<T> INORDER_COMPARER>
    inline SortedCollection<T>::SortedCollection (INORDER_COMPARER&& inOrderComparer, const initializer_list<T>& src)
        : SortedCollection{forward<INORDER_COMPARER> (inOrderComparer)}
    {
        this->AddAll (src);
        _AssertRepValidType ();
    }
#if !qCompilerAndStdLib_RequiresNotMatchInlineOutOfLineForTemplateClassBeingDefined_Buggy
    template <typename T>
    template <IIterableOf<T> ITERABLE_OF_ADDABLE>
    inline SortedCollection<T>::SortedCollection (ITERABLE_OF_ADDABLE&& src)
        requires (IInOrderComparer<less<T>, T> and not derived_from<remove_cvref_t<ITERABLE_OF_ADDABLE>, SortedCollection<T>>)
        : SortedCollection{}
    {
        this->AddAll (forward<ITERABLE_OF_ADDABLE> (src));
        _AssertRepValidType ();
    }
#endif
    template <typename T>
    template <IInOrderComparer<T> INORDER_COMPARER, IIterableOf<T> ITERABLE_OF_ADDABLE>
    inline SortedCollection<T>::SortedCollection (INORDER_COMPARER&& inOrderComparer, ITERABLE_OF_ADDABLE&& src)
        : SortedCollection{forward<INORDER_COMPARER> (inOrderComparer)}
    {
        this->AddAll (forward<ITERABLE_OF_ADDABLE> (src));
        _AssertRepValidType ();
    }
    template <typename T>
    template <IInputIterator<T> ITERATOR_OF_ADDABLE>
    inline SortedCollection<T>::SortedCollection (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end)
        requires (IInOrderComparer<less<T>, T>)
        : SortedCollection{}
    {
        this->AddAll (forward<ITERATOR_OF_ADDABLE> (start), forward<ITERATOR_OF_ADDABLE> (end));
        _AssertRepValidType ();
    }
    template <typename T>
    template <IInOrderComparer<T> INORDER_COMPARER, IInputIterator<T> ITERATOR_OF_ADDABLE>
    inline SortedCollection<T>::SortedCollection (INORDER_COMPARER&& inOrderComparer, ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end)
        : SortedCollection{forward<INORDER_COMPARER> (inOrderComparer)}
    {
        this->AddAll (forward<ITERATOR_OF_ADDABLE> (start), forward<ITERATOR_OF_ADDABLE> (end));
        _AssertRepValidType ();
    }
    template <typename T>
    inline void SortedCollection<T>::_AssertRepValidType () const
    {
        if constexpr (qDebug) {
            _SafeReadRepAccessor<_IRep>{this};
        }
    }
    template <typename T>
    inline auto SortedCollection<T>::GetInOrderComparer () const -> InOrderComparerType
    {
        return _SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().GetInOrderComparer ();
    }
    template <typename T>
    inline bool SortedCollection<T>::Contains (ArgByValueType<T> item) const
    {
        return _SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().Contains (item);
    }
    template <typename T>
    inline void SortedCollection<T>::Remove (ArgByValueType<T> item)
    {
        _SafeReadRepAccessor<_IRep>{this}._GetRep ().Remove (item);
    }

    template <typename T>
    template <typename RESULT_CONTAINER, invocable<T> ELEMENT_MAPPER>
    inline RESULT_CONTAINER SortedCollection<T>::Map (ELEMENT_MAPPER&& elementMapper) const
        requires (convertible_to<invoke_result_t<ELEMENT_MAPPER, T>, typename RESULT_CONTAINER::value_type> or
                  convertible_to<invoke_result_t<ELEMENT_MAPPER, T>, optional<typename RESULT_CONTAINER::value_type>>)
    {
        if constexpr (same_as<RESULT_CONTAINER, SortedCollection>) {
            // clone the rep so we retain the ordering function etc
            return inherited::template Map<RESULT_CONTAINER> (forward<ELEMENT_MAPPER> (elementMapper),
                                                              RESULT_CONTAINER{_SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().CloneEmpty ()});
        }
        else {
            return inherited::template Map<RESULT_CONTAINER> (forward<ELEMENT_MAPPER> (elementMapper)); // default inherited interpretation
        }
    }
    template <typename T>
    template <derived_from<Iterable<T>> RESULT_CONTAINER, predicate<T> INCLUDE_PREDICATE>
    inline RESULT_CONTAINER SortedCollection<T>::Where (INCLUDE_PREDICATE&& includeIfTrue) const
    {
        if constexpr (same_as<RESULT_CONTAINER, SortedCollection>) {
            // clone the rep so we retain the ordering function etc
            return inherited::template Where<RESULT_CONTAINER> (
                forward<INCLUDE_PREDICATE> (includeIfTrue), RESULT_CONTAINER{_SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().CloneEmpty ()});
        }
        else {
            return inherited::template Where<RESULT_CONTAINER> (forward<INCLUDE_PREDICATE> (includeIfTrue)); // default inherited interpretation
        }
    }
    template <typename T>
    inline bool SortedCollection<T>::operator== (const SortedCollection& rhs) const
    {
        auto elementEqualsComparer = Common::EqualsComparerAdapter{this->GetInOrderComparer ()};
        // @todo understand/fix why decltype(elementEqualsComparer>) needed, and not deduced
        return typename Iterable<T>::template SequentialEqualsComparer<decltype (elementEqualsComparer)>{elementEqualsComparer}(*this, rhs);
    }
    template <typename T>
    inline strong_ordering SortedCollection<T>::operator<=> (const SortedCollection& rhs) const
    {
        auto elementThreeWayComparer = Common::ThreeWayComparerAdapter{this->GetInOrderComparer ()};
        // @todo understand/fix why decltype(elementEqualsComparer>) needed, and not deduced
        return typename Iterable<T>::template SequentialThreeWayComparer<decltype (elementThreeWayComparer)>{elementThreeWayComparer}(*this, rhs);
    }

}

#endif /* _Stroika_Foundation_Containers_SortedCollection_inl_ */
