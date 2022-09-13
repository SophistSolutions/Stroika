/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
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
        : SortedCollection{less<T>{}}
    {
        _AssertRepValidType ();
    }
    template <typename T>
    template <typename INORDER_COMPARER, enable_if_t<Common::IsStrictInOrderComparer<INORDER_COMPARER, T> ()>*>
    inline SortedCollection<T>::SortedCollection (const INORDER_COMPARER& inorderComparer)
        : inherited{Factory::SortedCollection_Factory<T, INORDER_COMPARER>{inorderComparer}()}
    {
        static_assert (Common::IsStrictInOrderComparer<INORDER_COMPARER> (), "StrictInOrder comparer required with SortedCollection");
        _AssertRepValidType ();
    }
    template <typename T>
    template <typename INORDER_COMPARER, enable_if_t<Common::IsStrictInOrderComparer<INORDER_COMPARER, T> ()>*>
    inline SortedCollection<T>::SortedCollection (INORDER_COMPARER&& inorderComparer)
        : inherited{Factory::SortedCollection_Factory<T, INORDER_COMPARER>{forward<INORDER_COMPARER> (inorderComparer)}()}
    {
        static_assert (Common::IsStrictInOrderComparer<INORDER_COMPARER> (), "StrictInOrder comparer required with SortedCollection");
        _AssertRepValidType ();
    }
    template <typename T>
    inline SortedCollection<T>::SortedCollection (const _IRepSharedPtr& src) noexcept
        : inherited{src}
    {
        RequireNotNull (src);
        _AssertRepValidType ();
    }
    template <typename T>
    inline SortedCollection<T>::SortedCollection (_IRepSharedPtr&& src) noexcept
        : inherited{(RequireNotNull (src), move (src))}
    {
        _AssertRepValidType ();
    }
    template <typename T>
    inline SortedCollection<T>::SortedCollection (const initializer_list<T>& src)
        : SortedCollection{}
    {
        this->AddAll (src);
        _AssertRepValidType ();
    }
    template <typename T>
    template <typename INORDER_COMPARER, enable_if_t<Common::IsStrictInOrderComparer<INORDER_COMPARER, T> ()>*>
    inline SortedCollection<T>::SortedCollection (INORDER_COMPARER&& inOrderComparer, const initializer_list<T>& src)
        : SortedCollection{forward<INORDER_COMPARER> (inOrderComparer)}
    {
        this->AddAll (src);
        _AssertRepValidType ();
    }
    template <typename T>
    template <typename ITERABLE_OF_ADDABLE, enable_if_t<Configuration::IsIterable_v<ITERABLE_OF_ADDABLE> and not is_base_of_v<SortedCollection<T>, decay_t<ITERABLE_OF_ADDABLE>>>*>
    inline SortedCollection<T>::SortedCollection (ITERABLE_OF_ADDABLE&& src)
        : SortedCollection{}
    {
        static_assert (IsAddable_v<ExtractValueType_t<ITERABLE_OF_ADDABLE>>);
        this->AddAll (forward<ITERABLE_OF_ADDABLE> (src));
        _AssertRepValidType ();
    }
    template <typename T>
    template <typename INORDER_COMPARER, typename ITERABLE_OF_ADDABLE, enable_if_t<Common::IsStrictInOrderComparer<INORDER_COMPARER, T> () and Configuration::IsIterable_v<ITERABLE_OF_ADDABLE>>*>
    inline SortedCollection<T>::SortedCollection (INORDER_COMPARER&& inOrderComparer, ITERABLE_OF_ADDABLE&& src)
        : SortedCollection{forward<INORDER_COMPARER> (inOrderComparer)}
    {
        static_assert (IsAddable_v<ExtractValueType_t<ITERABLE_OF_ADDABLE>>);
        this->AddAll (forward<ITERABLE_OF_ADDABLE> (src));
        _AssertRepValidType ();
    }
    template <typename T>
    template <typename ITERATOR_OF_ADDABLE>
    inline SortedCollection<T>::SortedCollection (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end)
        : SortedCollection{}
    {
        static_assert (IsAddable_v<ExtractValueType_t<ITERATOR_OF_ADDABLE>>);
        this->AddAll (forward<ITERATOR_OF_ADDABLE> (start), forward<ITERATOR_OF_ADDABLE> (end));
        _AssertRepValidType ();
    }
    template <typename T>
    template <typename INORDER_COMPARER, typename ITERATOR_OF_ADDABLE, enable_if_t<Common::IsStrictInOrderComparer<INORDER_COMPARER, T> ()>*>
    inline SortedCollection<T>::SortedCollection (INORDER_COMPARER&& inOrderComparer, ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end)
        : SortedCollection{forward<INORDER_COMPARER> (inOrderComparer)}
    {
        static_assert (IsAddable_v<ExtractValueType_t<ITERATOR_OF_ADDABLE>>);
        this->AddAll (forward<ITERATOR_OF_ADDABLE> (start), forward<ITERATOR_OF_ADDABLE> (end));
        _AssertRepValidType ();
    }
    template <typename T>
    inline void SortedCollection<T>::_AssertRepValidType () const
    {
#if qDebug
        _SafeReadRepAccessor<_IRep>{this};
#endif
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
#if __cpp_impl_three_way_comparison >= 201907
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
#endif

#if __cpp_impl_three_way_comparison < 201907
    /*
     ********************************************************************************
     ********************* SortedCollection<T> comparison operators *****************
     ********************************************************************************
     */
    template <typename T>
    inline bool operator== (const SortedCollection<T>& lhs, const SortedCollection<T>& rhs)
    {
        auto elementEqualsComparer = Common::EqualsComparerAdapter{lhs.GetInOrderComparer ()};
        return typename Iterable<T>::template SequentialEqualsComparer<decltype (elementEqualsComparer)>{elementEqualsComparer}(lhs, rhs);
    }
    template <typename T>
    inline bool operator!= (const SortedCollection<T>& lhs, const SortedCollection<T>& rhs)
    {
        return not(lhs == rhs);
    }
#endif

}

#endif /* _Stroika_Foundation_Containers_SortedCollection_inl_ */
