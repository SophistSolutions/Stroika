/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_SortedAssociation_inl_
#define _Stroika_Foundation_Containers_SortedAssociation_inl_

#include "../Debug/Assertions.h"
#include "Factory/SortedAssociation_Factory.h"

namespace Stroika {
    namespace Foundation {
        namespace Containers {

            /*
             ********************************************************************************
             ************ SortedAssociation<KEY_TYPE, VALUE_TYPE, TRAITS> *******************
             ********************************************************************************
             */
            template <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            SortedAssociation<KEY_TYPE, VALUE_TYPE, TRAITS>::SortedAssociation ()
                : inherited (move (Factory::SortedAssociation_Factory<KEY_TYPE, VALUE_TYPE, TRAITS> () ()))
            {
                EnsureMember (&inherited::_ConstGetRep (), _IRep);
            }
            template <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            inline SortedAssociation<KEY_TYPE, VALUE_TYPE, TRAITS>::SortedAssociation (const std::initializer_list<KeyValuePair<KEY_TYPE, VALUE_TYPE>>& src)
                : SortedAssociation ()
            {
                AssertMember (&inherited::_ConstGetRep (), _IRep);
                this->AddAll (src);
            }
            template <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            inline SortedAssociation<KEY_TYPE, VALUE_TYPE, TRAITS>::SortedAssociation (const std::initializer_list<pair<KEY_TYPE, VALUE_TYPE>>& src)
                : SortedAssociation ()
            {
                AssertMember (&inherited::_ConstGetRep (), _IRep);
                this->AddAll (src);
            }
            template <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            template <typename CONTAINER_OF_PAIR_KEY_T, typename ENABLE_IF>
            inline SortedAssociation<KEY_TYPE, VALUE_TYPE, TRAITS>::SortedAssociation (const CONTAINER_OF_PAIR_KEY_T& src)
                : SortedAssociation ()
            {
                AssertMember (&inherited::_ConstGetRep (), _IRep);
                this->AddAll (src);
            }
            template <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            template <typename COPY_FROM_ITERATOR_KEY_T>
            SortedAssociation<KEY_TYPE, VALUE_TYPE, TRAITS>::SortedAssociation (COPY_FROM_ITERATOR_KEY_T start, COPY_FROM_ITERATOR_KEY_T end)
                : SortedAssociation ()
            {
                AssertMember (&inherited::_ConstGetRep (), _IRep);
                this->AddAll (start, end);
            }
            template <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            inline SortedAssociation<KEY_TYPE, VALUE_TYPE, TRAITS>::SortedAssociation (const _SharedPtrIRep& src) noexcept
                : inherited (static_cast<const typename inherited::_SharedPtrIRep&> (src))
            {
                RequireNotNull (rep);
                EnsureMember (&inherited::_ConstGetRep (), _IRep);
            }
            template <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            inline SortedAssociation<KEY_TYPE, VALUE_TYPE, TRAITS>::SortedAssociation (_SharedPtrIRep&& src) noexcept
                : inherited ((RequireNotNull (src), move<typename inherited::_SharedPtrIRep> (src)))
            {
                EnsureMember (&inherited::_ConstGetRep (), _IRep);
            }
        }
    }
}

#endif /* _Stroika_Foundation_Containers_SortedAssociation_inl_ */
