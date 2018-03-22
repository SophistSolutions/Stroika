/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_SortedSet_inl_
#define _Stroika_Foundation_Containers_SortedSet_inl_

#include "../Debug/Assertions.h"
#include "Factory/SortedSet_Factory.h"

namespace Stroika {
    namespace Foundation {
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
            template <typename LESS_COMPARER, typename ENABLE_IF_IS_COMPARER>
            inline SortedSet<T>::SortedSet (const LESS_COMPARER& lessComparer, ENABLE_IF_IS_COMPARER*)
                : inherited (move (Factory::SortedSet_Factory<T, LESS_COMPARER> (lessComparer) ()))
            {
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
            inline SortedSet<T>::SortedSet (const EqualityComparerType& equalityComparer, const initializer_list<T>& src)
                : SortedSet (equalityComparer)
            {
                this->AddAll (src);
                _AssertRepValidType ();
            }
            template <typename T>
            template <typename CONTAINER_OF_T, typename ENABLE_IF>
            inline SortedSet<T>::SortedSet (const CONTAINER_OF_T& src)
                : SortedSet ()
            {
                this->AddAll (src);
                _AssertRepValidType ();
            }
            template <typename T>
            template <typename CONTAINER_OF_T, typename ENABLE_IF>
            inline SortedSet<T>::SortedSet (const EqualityComparerType& equalityComparer, const CONTAINER_OF_T& src)
                : SortedSet (equalityComparer)
            {
                this->AddAll (src);
                _AssertRepValidType ();
            }
            template <typename T>
            template <typename COPY_FROM_ITERATOR_OF_T, typename ENABLE_IF>
            inline SortedSet<T>::SortedSet (COPY_FROM_ITERATOR_OF_T start, COPY_FROM_ITERATOR_OF_T end)
                : SortedSet ()
            {
                this->AddAll (start, end);
                _AssertRepValidType ();
            }
            template <typename T>
            template <typename COPY_FROM_ITERATOR_OF_T, typename ENABLE_IF>
            inline SortedSet<T>::SortedSet (const EqualityComparerType& equalityComparer, COPY_FROM_ITERATOR_OF_T start, COPY_FROM_ITERATOR_OF_T end)
                : SortedSet (equalityComparer)
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
        }
    }
}

#endif /* _Stroika_Foundation_Containers_SortedSet_inl_ */
