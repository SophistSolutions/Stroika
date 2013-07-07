/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_SortedBag_inl_
#define _Stroika_Foundation_Containers_SortedBag_inl_

#include    "../Debug/Assertions.h"

#include    "Concrete/SortedBag_Factory.h"


namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {


            /*
             ********************************************************************************
             ************************************ SortedBag<T, TRAITS> ******************************
             ********************************************************************************
             */
            template    <typename T, typename TRAITS>
            inline  SortedBag<T, TRAITS>::SortedBag ()
                : inherited (static_cast<const inherited&> (Concrete::mkSortedBag_Default<T, TRAITS> ()))
            {
            }
            template    <typename T, typename TRAITS>
            inline  SortedBag<T, TRAITS>::SortedBag (const SortedBag<T, TRAITS>& sb)
                : inherited (static_cast<const inherited&> (sb))
            {
            }
            template    <typename T, typename TRAITS>
            inline  SortedBag<T, TRAITS>::SortedBag (const _SharedPtrIRep& rep)
                : inherited (typename inherited::_SharedPtrIRep (rep))
            {
                RequireNotNull (rep);
                EnsureMember (&inherited::_GetRep (), _IRep);
            }
            template    <typename T, typename TRAITS>
            template    <typename CONTAINER_OF_T>
            inline  SortedBag<T, TRAITS>::SortedBag (const CONTAINER_OF_T& s)
                : inherited (static_cast<const inherited&> (Concrete::mkSortedBag_Default<T, TRAITS> ()))
            {
                AssertMember (&inherited::_GetRep (), _IRep);
                this->AddAll (s);
            }
            template    <typename T, typename TRAITS>
            inline  SortedBag<T, TRAITS>&   SortedBag<T, TRAITS>::operator= (const SortedBag<T, TRAITS>& rhs)
            {
                inherited::operator= (static_cast<const inherited&> (rhs));
                EnsureMember (&inherited::_GetRep (), _IRep);
                return *this;
            }


        }
    }
}



#endif /* _Stroika_Foundation_Containers_SortedBag_inl_ */
