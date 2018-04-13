/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_SortedMultiSet_inl_
#define _Stroika_Foundation_Containers_SortedMultiSet_inl_

#include "../Debug/Assertions.h"
#include "Factory/SortedMultiSet_Factory.h"

namespace Stroika {
    namespace Foundation {
        namespace Containers {

            /*
             ********************************************************************************
             ************************ SortedMultiSet<T, TRAITS> *****************************
             ********************************************************************************
             */
            template <typename T, typename TRAITS>
            inline SortedMultiSet<T, TRAITS>::SortedMultiSet ()
                : SortedMultiSet (std::less<T>{})
            {
                _AssertRepValidType ();
            }
            template <typename T, typename TRAITS>
            template <typename INORDER_COMPARER, typename ENABLE_IF>
            inline SortedMultiSet<T, TRAITS>::SortedMultiSet (INORDER_COMPARER&& inorderComparer, ENABLE_IF*)
                : inherited (move (Factory::SortedMultiSet_Factory<T, TRAITS, INORDER_COMPARER> (std::forward<INORDER_COMPARER> (inorderComparer)) ()))
            {
                static_assert (Common::IsStrictInOrderComparer<INORDER_COMPARER> (), "SortedMultiSet constructor with INORDER_COMPARER - comparer not valid IsStrictInOrderComparer- see ComparisonRelationDeclaration<Common::ComparisonRelationType::eStrictInOrder, function<bool(T, T)>");
                _AssertRepValidType ();
            }
            template <typename T, typename TRAITS>
            inline SortedMultiSet<T, TRAITS>::SortedMultiSet (const _SortedMultiSetRepSharedPtr& src) noexcept
                : inherited ((RequireNotNull (src), src))
            {
                _AssertRepValidType ();
            }
            template <typename T, typename TRAITS>
            inline SortedMultiSet<T, TRAITS>::SortedMultiSet (_SortedMultiSetRepSharedPtr&& src) noexcept
                : inherited ((RequireNotNull (src), move (src)))
            {
                _AssertRepValidType ();
            }

            template <typename T, typename TRAITS>
            SortedMultiSet<T, TRAITS>::SortedMultiSet (const initializer_list<T>& src)
                : SortedMultiSet ()
            {
                this->AddAll (src);
                _AssertRepValidType ();
            }
            template <typename T, typename TRAITS>
            template <typename INORDER_COMPARER, typename ENABLE_IF>
            SortedMultiSet<T, TRAITS>::SortedMultiSet (INORDER_COMPARER&& inorderComparer, const initializer_list<T>& src)
                : SortedMultiSet (std::forward<INORDER_COMPARER> (inorderComparer))
            {
                this->AddAll (src);
                _AssertRepValidType ();
            }
            template <typename T, typename TRAITS>
            SortedMultiSet<T, TRAITS>::SortedMultiSet (const initializer_list<CountedValue<T>>& src)
                : SortedMultiSet ()
            {
                this->AddAll (src);
                _AssertRepValidType ();
            }
            template <typename T, typename TRAITS>
            template <typename INORDER_COMPARER, typename ENABLE_IF>
            SortedMultiSet<T, TRAITS>::SortedMultiSet (INORDER_COMPARER&& inorderComparer, const initializer_list<CountedValue<T>>& src)
                : SortedMultiSet (std::forward<INORDER_COMPARER> (inorderComparer))
            {
                this->AddAll (src);
                _AssertRepValidType ();
            }
            template <typename T, typename TRAITS>
            template <typename CONTAINER_OF_ADDABLE, typename ENABLE_IF>
            inline SortedMultiSet<T, TRAITS>::SortedMultiSet (const CONTAINER_OF_ADDABLE& src)
                : SortedMultiSet ()
            {
                this->AddAll (src);
                _AssertRepValidType ();
            }
            template <typename T, typename TRAITS>
            template <typename INORDER_COMPARER, typename CONTAINER_OF_ADDABLE, typename ENABLE_IF>
            inline SortedMultiSet<T, TRAITS>::SortedMultiSet (INORDER_COMPARER&& inorderComparer, const CONTAINER_OF_ADDABLE& src)
                : SortedMultiSet (std::forward<INORDER_COMPARER> (inorderComparer))
            {
                this->AddAll (src);
                _AssertRepValidType ();
            }
            template <typename T, typename TRAITS>
            template <typename COPY_FROM_ITERATOR_OF_ADDABLE, typename ENABLE_IF>
            SortedMultiSet<T, TRAITS>::SortedMultiSet (COPY_FROM_ITERATOR_OF_ADDABLE start, COPY_FROM_ITERATOR_OF_ADDABLE end)
                : SortedMultiSet ()
            {
                AddAll (start, end);
                _AssertRepValidType ();
            }
            template <typename T, typename TRAITS>
            template <ttypename INORDER_COMPARER, ypename COPY_FROM_ITERATOR_OF_ADDABLE, typename ENABLE_IF>
            SortedMultiSet<T, TRAITS>::SortedMultiSet (INORDER_COMPARER&& inorderComparer, COPY_FROM_ITERATOR_OF_ADDABLE start, COPY_FROM_ITERATOR_OF_ADDABLE end)
                : SortedMultiSet (std::forward<INORDER_COMPARER> (inorderComparer))
            {
                AddAll (start, end);
                _AssertRepValidType ();
            }
            template <typename T, typename TRAITS>
            inline void SortedMultiSet<T, TRAITS>::_AssertRepValidType () const
            {
#if qDebug
                _SafeReadRepAccessor<_IRep>{this};
#endif
            }
        }
    }
}
#endif /* _Stroika_Foundation_Containers_SortedMultiSet_inl_ */
