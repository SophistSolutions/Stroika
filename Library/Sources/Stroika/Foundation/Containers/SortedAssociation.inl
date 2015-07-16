/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_SortedAssociation_inl_
#define _Stroika_Foundation_Containers_SortedAssociation_inl_

#include    "../Debug/Assertions.h"

#include    "Concrete/SortedAssociation_Factory.h"


namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {


            /*
             ********************************************************************************
             ************ SortedAssociation<KEY_TYPE, VALUE_TYPE, TRAITS> *******************
             ********************************************************************************
             */
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            SortedAssociation<KEY_TYPE, VALUE_TYPE, TRAITS>::SortedAssociation ()
                : inherited (move (Concrete::SortedAssociation_Factory<KEY_TYPE, VALUE_TYPE, TRAITS>::mk ()))
            {
                EnsureMember (&inherited::_ConstGetRep (), _IRep);
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            inline  SortedAssociation<KEY_TYPE, VALUE_TYPE, TRAITS>::SortedAssociation (const SortedAssociation<KEY_TYPE, VALUE_TYPE, TRAITS>& src)
                : inherited (static_cast<const inherited&> (src))
            {
                EnsureMember (&inherited::_ConstGetRep (), _IRep);
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            inline  SortedAssociation<KEY_TYPE, VALUE_TYPE, TRAITS>::SortedAssociation (const std::initializer_list<KeyValuePair<KEY_TYPE, VALUE_TYPE>>& src)
                : inherited (move (Concrete::SortedAssociation_Factory<KEY_TYPE, VALUE_TYPE, TRAITS>::mk ()))
            {
                AssertMember (&inherited::_ConstGetRep (), _IRep);
                this->AddAll (src);
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            inline  SortedAssociation<KEY_TYPE, VALUE_TYPE, TRAITS>::SortedAssociation (const std::initializer_list<pair<KEY_TYPE, VALUE_TYPE>>& src)
                : inherited (move (Concrete::SortedAssociation_Factory<KEY_TYPE, VALUE_TYPE, TRAITS>::mk ()))
            {
                AssertMember (&inherited::_ConstGetRep (), _IRep);
                this->AddAll (src);
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            template    <typename CONTAINER_OF_PAIR_KEY_T, typename ENABLE_IF>
            inline  SortedAssociation<KEY_TYPE, VALUE_TYPE, TRAITS>::SortedAssociation (const CONTAINER_OF_PAIR_KEY_T& src)
                : inherited (move (Concrete::SortedAssociation_Factory<KEY_TYPE, VALUE_TYPE, TRAITS>::mk ()))
            {
                AssertMember (&inherited::_ConstGetRep (), _IRep);
                this->AddAll (src);
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            template    <typename COPY_FROM_ITERATOR_KEY_T>
            SortedAssociation<KEY_TYPE, VALUE_TYPE, TRAITS>::SortedAssociation (COPY_FROM_ITERATOR_KEY_T start, COPY_FROM_ITERATOR_KEY_T end)
                : inherited (move (Concrete::SortedAssociation_Factory<KEY_TYPE, VALUE_TYPE, TRAITS>::mk ()))
            {
                AssertMember (&inherited::_ConstGetRep (), _IRep);
                this->AddAll (start, end);
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            inline  SortedAssociation<KEY_TYPE, VALUE_TYPE, TRAITS>::SortedAssociation (const _SharedPtrIRep& src)
                : inherited (static_cast<const typename inherited::_SharedPtrIRep&> (src))
            {
                RequireNotNull (rep);
                EnsureMember (&inherited::_ConstGetRep (), _IRep);
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            inline  SortedAssociation<KEY_TYPE, VALUE_TYPE, TRAITS>::SortedAssociation (_SharedPtrIRep&& src)
                : inherited (move<typename inherited::_SharedPtrIRep> (src))
            {
                //RequireNotNull (src); -- logically required, but we cannot test here, must test before mem-initializers
                EnsureMember (&inherited::_ConstGetRep (), _IRep);
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            inline  SortedAssociation<KEY_TYPE, VALUE_TYPE, TRAITS>& SortedAssociation<KEY_TYPE, VALUE_TYPE, TRAITS>::operator= (const SortedAssociation<KEY_TYPE, VALUE_TYPE, TRAITS>& src)
            {
                inherited::operator= (static_cast<const inherited&> (src));
                EnsureMember (&inherited::_ConstGetRep (), _IRep);
                return *this;
            }


        }
    }
}

#endif /* _Stroika_Foundation_Containers_SortedAssociation_inl_ */
