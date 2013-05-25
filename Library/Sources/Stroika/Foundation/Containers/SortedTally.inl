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
             ********************************** SortedTally<T> ******************************
             ********************************************************************************
             */
            template    <typename T>
            inline  SortedTally<T>::SortedTally ()
                : inherited (static_cast<const inherited&> (Concrete::mkSortedTally_Default<T> ()))
            {
            }
            template    <typename T>
            inline  SortedTally<T>::SortedTally (const SortedTally<T>& st)
                : inherited (static_cast<const inherited&> (st))
            {
            }
            template    <typename T>
            inline  SortedTally<T>::SortedTally (const _SharedPtrIRep& rep)
                : inherited (typename inherited::_SharedPtrIRep (rep))
            {
                RequireNotNull (rep);
                EnsureMember (&inherited::_GetRep (), _IRep);
            }
            template    <typename T>
            template    <typename CONTAINER_OF_T>
            inline  SortedTally<T>::SortedTally (const CONTAINER_OF_T& s)
                : inherited (static_cast<const inherited&> (Concrete::mkSortedTally_Default<T> ()))
            {
                AssertMember (&inherited::_GetRep (), _IRep);
                this->AddAll (s);
            }
            template    <typename T>
            inline  SortedTally<T>&   SortedTally<T>::operator= (const SortedTally<T>& src)
            {
                inherited::operator= (static_cast<const inherited&> (src));
                EnsureMember (&inherited::_GetRep (), _IRep);
                return *this;
            }


        }
    }
}
#endif /* _Stroika_Foundation_Containers_SortedTally_inl_ */
