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
            template <typename T, typename TRAITS>
            inline SortedSet<T, TRAITS>::SortedSet ()
                : inherited (move (Factory::SortedSet_Factory<T, TRAITS> () ()))
            {
                _AssertRepValidType ();
            }
            template <typename T, typename TRAITS>
            inline SortedSet<T, TRAITS>::SortedSet (const _SortedSetRepSharedPtr& src) noexcept
                : inherited (src)
            {
                RequireNotNull (src);
                _AssertRepValidType ();
            }
            template <typename T, typename TRAITS>
            inline SortedSet<T, TRAITS>::SortedSet (_SortedSetRepSharedPtr&& src) noexcept
                : inherited ((RequireNotNull (src), move (src)))
            {
                _AssertRepValidType ();
            }
            template <typename T, typename TRAITS>
            inline SortedSet<T, TRAITS>::SortedSet (const initializer_list<T>& src)
                : SortedSet ()
            {
                this->AddAll (src);
                _AssertRepValidType ();
            }
            template <typename T, typename TRAITS>
            template <typename CONTAINER_OF_T, typename ENABLE_IF>
            inline SortedSet<T, TRAITS>::SortedSet (const CONTAINER_OF_T& src)
                : SortedSet ()
            {
                this->AddAll (src);
                _AssertRepValidType ();
            }
            template <typename T, typename TRAITS>
            template <typename COPY_FROM_ITERATOR_OF_T>
            inline SortedSet<T, TRAITS>::SortedSet (COPY_FROM_ITERATOR_OF_T start, COPY_FROM_ITERATOR_OF_T end)
                : SortedSet ()
            {
                this->AddAll (start, end);
                _AssertRepValidType ();
            }
            template <typename T, typename TRAITS>
            inline void SortedSet<T, TRAITS>::_AssertRepValidType () const
            {
#if qDebug
                _SafeReadRepAccessor<_IRep>{this};
#endif
            }
        }
    }
}

#endif /* _Stroika_Foundation_Containers_SortedSet_inl_ */
