/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_SortedSet_inl_
#define _Stroika_Foundation_Containers_SortedSet_inl_

#include "../Debug/Assertions.h"
#include "Factory/SortedSet_Factory.h"

namespace Stroika::Foundation {
    namespace Containers {

        /*
         ********************************************************************************
         ********************************** SortedSet<T> ********************************
         ********************************************************************************
         */
        template <typename T>
        inline SortedSet<T>::SortedSet ()
            : SortedSet (std::less<T>{})
        {
            _AssertRepValidType ();
        }
        template <typename T>
        template <typename INORDER_COMPARER, typename ENABLE_IF>
        inline SortedSet<T>::SortedSet (INORDER_COMPARER&& inorderComparer, ENABLE_IF*)
            : inherited (move (Factory::SortedSet_Factory<T, INORDER_COMPARER> (std::forward<INORDER_COMPARER> (inorderComparer)) ()))
        {
            static_assert (Common::IsStrictInOrderComparer<INORDER_COMPARER> (), "StrictInOrder comparer required with SortedSet");
            _AssertRepValidType ();
        }
        template <typename T>
        inline SortedSet<T>::SortedSet (const _SortedSetRepSharedPtr& src) noexcept
            : inherited (src)
        {
            RequireNotNull (src);
            _AssertRepValidType ();
        }
        template <typename T>
        inline SortedSet<T>::SortedSet (_SortedSetRepSharedPtr&& src) noexcept
            : inherited ((RequireNotNull (src), move (src)))
        {
            _AssertRepValidType ();
        }
        template <typename T>
        inline SortedSet<T>::SortedSet (const initializer_list<T>& src)
            : SortedSet ()
        {
            this->AddAll (src);
            _AssertRepValidType ();
        }
        template <typename T>
        template <typename INORDER_COMPARER, typename ENABLE_IF>
        inline SortedSet<T>::SortedSet (INORDER_COMPARER&& inOrderComparer, const initializer_list<T>& src)
            : SortedSet (std::forward<INORDER_COMPARER> (inOrderComparer))
        {
            this->AddAll (src);
            _AssertRepValidType ();
        }
        template <typename T>
        template <typename CONTAINER_OF_ADDABLE, typename ENABLE_IF>
        inline SortedSet<T>::SortedSet (const CONTAINER_OF_ADDABLE& src)
            : SortedSet ()
        {
            this->AddAll (src);
            _AssertRepValidType ();
        }
        template <typename T>
        template <typename INORDER_COMPARER, typename CONTAINER_OF_ADDABLE, typename ENABLE_IF>
        inline SortedSet<T>::SortedSet (INORDER_COMPARER&& inOrderComparer, const CONTAINER_OF_ADDABLE& src)
            : SortedSet (std::forward<INORDER_COMPARER> (inOrderComparer))
        {
            this->AddAll (src);
            _AssertRepValidType ();
        }
        template <typename T>
        template <typename COPY_FROM_ITERATOR_OF_ADDABLE, typename ENABLE_IF>
        inline SortedSet<T>::SortedSet (COPY_FROM_ITERATOR_OF_ADDABLE start, COPY_FROM_ITERATOR_OF_ADDABLE end)
            : SortedSet ()
        {
            this->AddAll (start, end);
            _AssertRepValidType ();
        }
        template <typename T>
        template <typename INORDER_COMPARER, typename COPY_FROM_ITERATOR_OF_ADDABLE, typename ENABLE_IF>
        inline SortedSet<T>::SortedSet (INORDER_COMPARER&& inOrderComparer, COPY_FROM_ITERATOR_OF_ADDABLE start, COPY_FROM_ITERATOR_OF_ADDABLE end)
            : SortedSet (std::forward<INORDER_COMPARER> (inOrderComparer))
        {
            this->AddAll (start, end);
            _AssertRepValidType ();
        }
        template <typename T>
        inline void SortedSet<T>::_AssertRepValidType () const
        {
#if qDebug
            _SafeReadRepAccessor<_IRep>{this};
#endif
        }
        template <typename T>
        inline auto SortedSet<T>::GetInOrderComparer () const -> InOrderComparerType
        {
            return _SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().GetInOrderComparer ();
        }
    }
}

#endif /* _Stroika_Foundation_Containers_SortedSet_inl_ */
