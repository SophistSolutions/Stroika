/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
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
        : SortedCollection (less<T>{})
    {
        _AssertRepValidType ();
    }
    template <typename T>
    template <typename INORDER_COMPARER, enable_if_t<Common::IsPotentiallyComparerRelation<T, INORDER_COMPARER> ()>*>
    inline SortedCollection<T>::SortedCollection (INORDER_COMPARER&& inorderComparer)
        : inherited (Factory::SortedCollection_Factory<T, INORDER_COMPARER> (forward<INORDER_COMPARER> (inorderComparer)) ())
    {
        static_assert (Common::IsStrictInOrderComparer<INORDER_COMPARER> (), "StrictInOrder comparer required with SortedCollection");
        _AssertRepValidType ();
    }
    template <typename T>
    inline SortedCollection<T>::SortedCollection (const _SortedCollectionRepSharedPtr& src) noexcept
        : inherited (src)
    {
        RequireNotNull (src);
        _AssertRepValidType ();
    }
    template <typename T>
    inline SortedCollection<T>::SortedCollection (_SortedCollectionRepSharedPtr&& src) noexcept
        : inherited ((RequireNotNull (src), move (src)))
    {
        _AssertRepValidType ();
    }
    template <typename T>
    inline SortedCollection<T>::SortedCollection (const initializer_list<T>& src)
        : SortedCollection ()
    {
        this->AddAll (src);
        _AssertRepValidType ();
    }
    template <typename T>
    template <typename INORDER_COMPARER, enable_if_t<Common::IsPotentiallyComparerRelation<T, INORDER_COMPARER> ()>*>
    inline SortedCollection<T>::SortedCollection (INORDER_COMPARER&& inOrderComparer, const initializer_list<T>& src)
        : SortedCollection (forward<INORDER_COMPARER> (inOrderComparer))
    {
        this->AddAll (src);
        _AssertRepValidType ();
    }
    template <typename T>
    template <typename CONTAINER_OF_ADDABLE, enable_if_t<Configuration::IsIterableOfT_v<CONTAINER_OF_ADDABLE, T> and not is_base_of_v<SortedCollection<T>, Configuration::remove_cvref_t<CONTAINER_OF_ADDABLE>>>*>
    inline SortedCollection<T>::SortedCollection (CONTAINER_OF_ADDABLE&& src)
        : SortedCollection ()
    {
        this->AddAll (forward<CONTAINER_OF_ADDABLE> (src));
        _AssertRepValidType ();
    }
    template <typename T>
    template <typename INORDER_COMPARER, typename CONTAINER_OF_ADDABLE, enable_if_t<Common::IsPotentiallyComparerRelation<T, INORDER_COMPARER> () and Configuration::IsIterableOfT_v<CONTAINER_OF_ADDABLE, T> and not is_base_of_v<SortedCollection<T>, Configuration::remove_cvref_t<CONTAINER_OF_ADDABLE>>>*>
    inline SortedCollection<T>::SortedCollection (INORDER_COMPARER&& inOrderComparer, CONTAINER_OF_ADDABLE&& src)
        : SortedCollection (forward<INORDER_COMPARER> (inOrderComparer))
    {
        this->AddAll (forward<CONTAINER_OF_ADDABLE> (src));
        _AssertRepValidType ();
    }
    template <typename T>
    template <typename COPY_FROM_ITERATOR_OF_ADDABLE>
    inline SortedCollection<T>::SortedCollection (COPY_FROM_ITERATOR_OF_ADDABLE start, COPY_FROM_ITERATOR_OF_ADDABLE end)
        : SortedCollection ()
    {
        this->AddAll (start, end);
        _AssertRepValidType ();
    }
    template <typename T>
    template <typename INORDER_COMPARER, typename COPY_FROM_ITERATOR_OF_ADDABLE, enable_if_t<Common::IsPotentiallyComparerRelation<T, INORDER_COMPARER> ()>*>
    inline SortedCollection<T>::SortedCollection (INORDER_COMPARER&& inOrderComparer, COPY_FROM_ITERATOR_OF_ADDABLE start, COPY_FROM_ITERATOR_OF_ADDABLE end)
        : SortedCollection (forward<INORDER_COMPARER> (inOrderComparer))
    {
        this->AddAll (start, end);
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

}

#endif /* _Stroika_Foundation_Containers_SortedCollection_inl_ */
