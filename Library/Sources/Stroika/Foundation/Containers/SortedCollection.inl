/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_SortedCollection_inl_
#define _Stroika_Foundation_Containers_SortedCollection_inl_

#include "../Debug/Assertions.h"
#include "Factory/SortedCollection_Factory.h"

namespace Stroika {
    namespace Foundation {
        namespace Containers {

            /*
             ********************************************************************************
             ***************************** SortedCollection<T> ******************************
             ********************************************************************************
             */
            template <typename T>
            inline SortedCollection<T>::SortedCollection ()
                : SortedCollection (std::less<T>{})
            {
                _AssertRepValidType ();
            }
            template <typename T>
            template <typename INORDER_COMPARER, typename ENABLE_IF_IS_COMPARER>
            inline SortedCollection<T>::SortedCollection (const INORDER_COMPARER& inorderComparer, ENABLE_IF_IS_COMPARER*)
                : inherited (move (Factory::SortedCollection_Factory<T, INORDER_COMPARER> (inorderComparer) ()))
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
            template <typename INORDER_COMPARER, typename ENABLE_IF_IS_COMPARER>
            inline SortedCollection<T>::SortedCollection (INORDER_COMPARER&& inOrderComparer, const initializer_list<T>& src)
                : SortedCollection (std::forward<INORDER_COMPARER> (inOrderComparer))
            {
                this->AddAll (src);
                _AssertRepValidType ();
            }
            template <typename T>
            template <typename CONTAINER_OF_T, typename ENABLE_IF>
            inline SortedCollection<T>::SortedCollection (const CONTAINER_OF_T& src)
                : SortedCollection ()
            {
                this->AddAll (src);
                _AssertRepValidType ();
            }
            template <typename T>
            template <typename INORDER_COMPARER, typename CONTAINER_OF_T, typename ENABLE_IF>
            inline SortedCollection<T>::SortedCollection (INORDER_COMPARER&& inOrderComparer, const CONTAINER_OF_T& src)
                : SortedCollection (std::foward<INORDER_COMPARER> (inOrderComparer))
            {
                this->AddAll (src);
                _AssertRepValidType ();
            }
            template <typename T>
            template <typename COPY_FROM_ITERATOR_OF_T>
            inline SortedCollection<T>::SortedCollection (COPY_FROM_ITERATOR_OF_T start, COPY_FROM_ITERATOR_OF_T end)
                : SortedCollection ()
            {
                this->AddAll (start, end);
                _AssertRepValidType ();
            }
            template <typename T>
            template <typename INORDER_COMPARER, typename COPY_FROM_ITERATOR_OF_T, typename ENABLE_IF>
            inline SortedCollection<T>::SortedCollection (INORDER_COMPARER&& inOrderComparer, COPY_FROM_ITERATOR_OF_T start, COPY_FROM_ITERATOR_OF_T end)
                : SortedCollection (std::foward<INORDER_COMPARER> (inOrderComparer))
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
        }
    }
}

#endif /* _Stroika_Foundation_Containers_SortedCollection_inl_ */
