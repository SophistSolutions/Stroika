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
             ****************************** SortedMapping<Key,T> ****************************
             ********************************************************************************
             */
            template    <typename Key, typename T>
            SortedMapping<Key, T>::SortedMapping ()
                : inherited (static_cast<const inherited&> (Concrete::mkSortedMapping_Default<Key, T> ()))
            {
                EnsureMember (&inherited::_GetRep (), _IRep);
            }
#if 0
            template    <typename Key, typename T>
            inline  Mapping<Key, T>::Mapping (const Mapping<Key, T>& m)
                : inherited (m)
            {
            }
            template    <typename Key, typename T>
            template    <typename CONTAINER_OF_PAIR_KEY_T>
            inline  Mapping<Key, T>::Mapping (const CONTAINER_OF_PAIR_KEY_T& cp)
                : inherited (Concrete::Mapping_stdmap<Key, T> ())
            {
                AddAll (cp);
            }
            template    <typename Key, typename T>
            template    <typename COPY_FROM_ITERATOR_KEY_T>
            Mapping<Key, T>::Mapping (COPY_FROM_ITERATOR_KEY_T start, COPY_FROM_ITERATOR_KEY_T end)
                : inherited (Concrete::Mapping_stdmap<Key, T> ())
            {
                AddAll (start, end);
            }
#endif
            template    <typename Key, typename T>
            inline  SortedMapping<Key, T>::SortedMapping (const _SharedPtrIRep& rep)
                : inherited (typename inherited::_SharedPtrIRep (rep))
            {
                RequireNotNull (rep);
            }
            template    <typename Key, typename T>
            inline  SortedMapping<Key, T>& SortedMapping<Key, T>::operator= (const SortedMapping<Key, T>& src)
            {
                inherited::operator= (static_cast<const inherited&> (src));
                EnsureMember (&inherited::_GetRep (), _IRep);
                return *this;
            }


        }
    }
}

#endif /* _Stroika_Foundation_Containers_SortedMapping_inl_ */

//     See note at the end of Concrete/SortedMapping_Factory.h about why this is here
#include    "Concrete/SortedMapping_Factory.inl"
