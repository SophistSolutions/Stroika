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
             ************************************ SortedBag<T> ******************************
             ********************************************************************************
             */
            template    <typename T>
            inline  SortedBag<T>::SortedBag ()
                : inherited (Concrete::mkSortedBag_Default<T> ())
            {
            }
            template    <typename T>
            inline  SortedBag<T>::SortedBag (const SortedBag<T>& sb)
                : inherited (static_cast<const inherited&> (sb))
            {
            }
            template    <typename T>
            inline  SortedBag<T>::SortedBag (const _SharedPtrIRep& rep)
                : inherited (typename inherited::_SharedPtrIRep (rep))
            {
                RequireNotNull (rep);
                EnsureMember (&inherited::_GetRep (), _IRep);
            }
            template    <typename T>
            template    <typename CONTAINER_OF_T>
            inline  SortedBag<T>::SortedBag (const CONTAINER_OF_T& s)
                : inherited (Concrete::mkSortedBag_Default<T> ())
            {
                AssertMember (&inherited::_GetRep (), _IRep);
                this->AddAll (s);
            }
            template    <typename T>
            inline  SortedBag<T>&   SortedBag<T>::operator= (const SortedBag<T>& rhs)
            {
                inherited::operator= (static_cast<const inherited&> (rhs));
                EnsureMember (&inherited::_GetRep (), _IRep);
                return *this;
            }


        }
    }
}



#endif /* _Stroika_Foundation_Containers_SortedBag_inl_ */
