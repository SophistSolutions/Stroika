/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_SortedSet_inl_
#define _Stroika_Foundation_Containers_SortedSet_inl_

#include    "../Debug/Assertions.h"

#include    "Concrete/SortedSet_Factory.h"


namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {


            /*
             ********************************************************************************
             ************************************ SortedSet<T> ******************************
             ********************************************************************************
             */
            template    <typename T>
            inline  SortedSet<T>::SortedSet ()
                : inherited (Concrete::mkSortedSet_Default<T> ())
            {
            }
            template    <typename T>
            inline  SortedSet<T>::SortedSet (const SortedSet<T>& ss)
                : inherited (static_cast<const inherited&> (ss))
            {
            }
            template    <typename T>
            inline  SortedSet<T>::SortedSet (const _SharedPtrIRep& rep)
                : inherited (typename inherited::_SharedPtrIRep (rep))
            {
                RequireNotNull (rep);
                EnsureMember (&inherited::_GetRep (), _IRep);
            }
            template    <typename T>
            template    <typename CONTAINER_OF_T>
            inline  SortedSet<T>::SortedSet (const CONTAINER_OF_T& s)
                : inherited (Concrete::mkSortedSet_Default<T> ())
            {
                AssertMember (&inherited::_GetRep (), _IRep);
                this->AddAll (s);
            }
            template    <typename T>
            inline  SortedSet<T>&   SortedSet<T>::operator= (const SortedSet<T>& src)
            {
                inherited::operator= (static_cast<const inherited&> (src));
                EnsureMember (&inherited::_GetRep (), _IRep);
                return *this;
            }


        }
    }
}

#endif /* _Stroika_Foundation_Containers_SortedSet_inl_ */
