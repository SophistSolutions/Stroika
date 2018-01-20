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
             ************************ SortedCollection<T, TRAITS> ***************************
             ********************************************************************************
             */
            template <typename T, typename TRAITS>
            inline SortedCollection<T, TRAITS>::SortedCollection ()
                : inherited (move (Factory::SortedCollection_Factory<T, TRAITS>::New ()))
            {
                _AssertRepValidType ();
            }
            template <typename T, typename TRAITS>
            inline SortedCollection<T, TRAITS>::SortedCollection (const SortedCollection<T, TRAITS>& src) noexcept
                : inherited (src)
            {
                _AssertRepValidType ();
            }
            template <typename T, typename TRAITS>
            inline SortedCollection<T, TRAITS>::SortedCollection (SortedCollection<T, TRAITS>&& src) noexcept
                : inherited (move (src))
            {
                _AssertRepValidType ();
            }
            template <typename T, typename TRAITS>
            inline SortedCollection<T, TRAITS>::SortedCollection (const _SortedCollectionRepSharedPtr& src) noexcept
                : inherited (src)
            {
                RequireNotNull (src);
                _AssertRepValidType ();
            }
            template <typename T, typename TRAITS>
            inline SortedCollection<T, TRAITS>::SortedCollection (_SortedCollectionRepSharedPtr&& src) noexcept
                : inherited ((RequireNotNull (src), move (src)))
            {
                _AssertRepValidType ();
            }
            template <typename T, typename TRAITS>
            inline SortedCollection<T, TRAITS>::SortedCollection (const initializer_list<T>& src)
                : SortedCollection ()
            {
                this->AddAll (src);
                _AssertRepValidType ();
            }
            template <typename T, typename TRAITS>
            template <typename CONTAINER_OF_T, typename ENABLE_IF>
            inline SortedCollection<T, TRAITS>::SortedCollection (const CONTAINER_OF_T& src)
                : SortedCollection ()
            {
                this->AddAll (src);
                _AssertRepValidType ();
            }
            template <typename T, typename TRAITS>
            inline void SortedCollection<T, TRAITS>::_AssertRepValidType () const
            {
#if qDebug
                _SafeReadRepAccessor<_IRep>{this};
#endif
            }
        }
    }
}

#endif /* _Stroika_Foundation_Containers_SortedCollection_inl_ */
