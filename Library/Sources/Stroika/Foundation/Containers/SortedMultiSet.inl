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
            template <typename INORDER_COMPARER, typename ENABLE_IF_IS_COMPARER>
            inline SortedMultiSet<T, TRAITS>::SortedMultiSet (const INORDER_COMPARER& inorderComparer, ENABLE_IF_IS_COMPARER*)
                : inherited (move (Factory::SortedMultiSet_Factory<T, TRAITS, INORDER_COMPARER> (inorderComparer) ()))
            {
                static_assert (Common::IsInOrderComparer<INORDER_COMPARER> (), "SortedMultiSet constructor with INORDER_COMPARER - comparer not valid IsInOrderComparer- see FunctionComparerAdapter<Common::OrderingRelationType::eStrictInOrder, function<bool(T, T)>");
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
            SortedMultiSet<T, TRAITS>::SortedMultiSet (const initializer_list<CountedValue<T>>& src)
                : SortedMultiSet ()
            {
                this->AddAll (src);
                _AssertRepValidType ();
            }
            template <typename T, typename TRAITS>
            template <typename CONTAINER_OF_T, typename ENABLE_IF>
            inline SortedMultiSet<T, TRAITS>::SortedMultiSet (const CONTAINER_OF_T& src)
                : SortedMultiSet ()
            {
                this->AddAll (src);
                _AssertRepValidType ();
            }
            template <typename T, typename TRAITS>
            template <typename COPY_FROM_ITERATOR>
            SortedMultiSet<T, TRAITS>::SortedMultiSet (COPY_FROM_ITERATOR start, COPY_FROM_ITERATOR end)
                : SortedMultiSet ()
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
