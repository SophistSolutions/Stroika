/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_SortedMapping_inl_
#define _Stroika_Foundation_Containers_SortedMapping_inl_

#include    "../Debug/Assertions.h"

#include    "Concrete/SortedMapping_Factory.h"


namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {


            /*
             ********************************************************************************
             ************** SortedMapping<KEY_TYPE, VALUE_TYPE, TRAITS> *********************
             ********************************************************************************
             */
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            SortedMapping<KEY_TYPE, VALUE_TYPE, TRAITS>::SortedMapping ()
                : inherited (static_cast < const inherited&& > (Concrete::SortedMapping_Factory<KEY_TYPE, VALUE_TYPE, TRAITS>::mk ()))
            {
                EnsureMember (&inherited::_ConstGetRep (), _IRep);
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            inline  SortedMapping<KEY_TYPE, VALUE_TYPE, TRAITS>::SortedMapping (const SortedMapping<KEY_TYPE, VALUE_TYPE, TRAITS>& src)
                : inherited (static_cast<const inherited&> (src))
            {
                EnsureMember (&inherited::_ConstGetRep (), _IRep);
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            inline  SortedMapping<KEY_TYPE, VALUE_TYPE, TRAITS>::SortedMapping (const std::initializer_list<KeyValuePair<KEY_TYPE, VALUE_TYPE>>& src)
                : inherited (static_cast < const inherited&& > (Concrete::SortedMapping_Factory<KEY_TYPE, VALUE_TYPE, TRAITS>::mk ()))
            {
                AssertMember (&inherited::_ConstGetRep (), _IRep);
                this->AddAll (src);
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            inline  SortedMapping<KEY_TYPE, VALUE_TYPE, TRAITS>::SortedMapping (const std::initializer_list<pair<KEY_TYPE, VALUE_TYPE>>& src)
                : inherited (static_cast < const inherited&& > (Concrete::SortedMapping_Factory<KEY_TYPE, VALUE_TYPE, TRAITS>::mk ()))
            {
                AssertMember (&inherited::_ConstGetRep (), _IRep);
                this->AddAll (src);
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            template    <typename CONTAINER_OF_PAIR_KEY_T>
            inline  SortedMapping<KEY_TYPE, VALUE_TYPE, TRAITS>::SortedMapping (const CONTAINER_OF_PAIR_KEY_T& src)
                : inherited (static_cast<const inherited&> (Concrete::SortedMapping_Factory<KEY_TYPE, VALUE_TYPE, TRAITS>::mk ()))
            {
                AssertMember (&inherited::_ConstGetRep (), _IRep);
                this->AddAll (src);
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            template    <typename COPY_FROM_ITERATOR_KEY_T>
            SortedMapping<KEY_TYPE, VALUE_TYPE, TRAITS>::SortedMapping (COPY_FROM_ITERATOR_KEY_T start, COPY_FROM_ITERATOR_KEY_T end)
                : inherited (static_cast<const inherited&> (Concrete::SortedMapping_Factory<KEY_TYPE, VALUE_TYPE, TRAITS>::mk ()))
            {
                AssertMember (&inherited::_ConstGetRep (), _IRep);
                this->AddAll (start, end);
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            inline  SortedMapping<KEY_TYPE, VALUE_TYPE, TRAITS>::SortedMapping (const _SharedPtrIRep& rep)
                : inherited (typename inherited::_SharedPtrIRep (rep))
            {
                RequireNotNull (rep);
                EnsureMember (&inherited::_ConstGetRep (), _IRep);
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            inline  SortedMapping<KEY_TYPE, VALUE_TYPE, TRAITS>& SortedMapping<KEY_TYPE, VALUE_TYPE, TRAITS>::operator= (const SortedMapping<KEY_TYPE, VALUE_TYPE, TRAITS>& src)
            {
                inherited::operator= (static_cast<const inherited&> (src));
                EnsureMember (&inherited::_ConstGetRep (), _IRep);
                return *this;
            }


        }
    }
}

#endif /* _Stroika_Foundation_Containers_SortedMapping_inl_ */
