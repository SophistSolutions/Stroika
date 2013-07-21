/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
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
                : inherited (static_cast<const inherited&> (Concrete::mkSortedMapping_Default<KEY_TYPE, VALUE_TYPE, TRAITS> ()))
            {
                EnsureMember (&inherited::_GetRep (), _IRep);
            }
#if 0
            template    <typename Key, typename T>
            inline  Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::Mapping (const Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>& m)
                : inherited (m)
            {
            }
            template    <typename Key, typename T>
            template    <typename CONTAINER_OF_PAIR_KEY_T>
            inline  Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::Mapping (const CONTAINER_OF_PAIR_KEY_T& cp)
                : inherited (Concrete::Mapping_stdmap<KEY_TYPE, VALUE_TYPE, TRAITS> ())
            {
                AddAll (cp);
            }
            template    <typename Key, typename T>
            template    <typename COPY_FROM_ITERATOR_KEY_T>
            Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::Mapping (COPY_FROM_ITERATOR_KEY_T start, COPY_FROM_ITERATOR_KEY_T end)
                : inherited (Concrete::Mapping_stdmap<KEY_TYPE, VALUE_TYPE, TRAITS> ())
            {
                AddAll (start, end);
            }
#endif
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            inline  SortedMapping<KEY_TYPE, VALUE_TYPE, TRAITS>::SortedMapping (const _SharedPtrIRep& rep)
                : inherited (typename inherited::_SharedPtrIRep (rep))
            {
                RequireNotNull (rep);
                EnsureMember (&inherited::_GetRep (), _IRep);
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            inline  SortedMapping<KEY_TYPE, VALUE_TYPE, TRAITS>& SortedMapping<KEY_TYPE, VALUE_TYPE, TRAITS>::operator= (const SortedMapping<KEY_TYPE, VALUE_TYPE, TRAITS>& src)
            {
                inherited::operator= (static_cast<const inherited&> (src));
                EnsureMember (&inherited::_GetRep (), _IRep);
                return *this;
            }


        }
    }
}

#endif /* _Stroika_Foundation_Containers_SortedMapping_inl_ */
