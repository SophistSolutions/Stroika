/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_SortedTally_inl_
#define _Stroika_Foundation_Containers_SortedTally_inl_

#include    "../Debug/Assertions.h"

#include    "Concrete/SortedTally_Factory.h"


namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {


            /*
             ********************************************************************************
             *************************** SortedTally<T, TRAITS> *****************************
             ********************************************************************************
             */
            template    <typename T, typename TRAITS>
            inline  SortedTally<T, TRAITS>::SortedTally ()
                : inherited (static_cast<const inherited&> (Concrete::mkSortedTally_Default<T, TRAITS> ()))
            {
            }
            template    <typename T, typename TRAITS>
            inline  SortedTally<T, TRAITS>::SortedTally (const SortedTally<T, TRAITS>& src)
                : inherited (static_cast<const inherited&> (src))
            {
            }
            template    <typename T, typename TRAITS>
            inline  SortedTally<T, TRAITS>::SortedTally (const _SharedPtrIRep& rep)
                : inherited (typename inherited::_SharedPtrIRep (rep))
            {
                RequireNotNull (rep);
                EnsureMember (&inherited::_GetRep (), _IRep);
            }
            template    <typename T, typename TRAITS>
            template    <typename CONTAINER_OF_T>
            inline  SortedTally<T, TRAITS>::SortedTally (const CONTAINER_OF_T& src)
                : inherited (static_cast<const inherited&> (Concrete::mkSortedTally_Default<T, TRAITS> ()))
            {
                AssertMember (&inherited::_GetRep (), _IRep);
                this->AddAll (src);
            }
            template    <typename T, typename TRAITS>
            inline  SortedTally<T, TRAITS>&   SortedTally<T, TRAITS>::operator= (const SortedTally<T, TRAITS>& src)
            {
                inherited::operator= (static_cast<const inherited&> (src));
                EnsureMember (&inherited::_GetRep (), _IRep);
                return *this;
            }


        }
    }
}
#endif /* _Stroika_Foundation_Containers_SortedTally_inl_ */
