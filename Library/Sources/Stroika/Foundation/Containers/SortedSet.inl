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
             ********************************** SortedSet<T> ********************************
             ********************************************************************************
             */
            template    <typename T, typename TRAITS>
            inline  SortedSet<T, TRAITS>::SortedSet ()
                : inherited (static_cast<const inherited&> (Concrete::SortedSet_Factory<T, TRAITS>::mk ()))
            {
                AssertMember (&inherited::_GetRep (), _IRep);
            }
            template    <typename T, typename TRAITS>
            inline  SortedSet<T, TRAITS>::SortedSet (const SortedSet<T, TRAITS>& ss)
                : inherited (static_cast<const inherited&> (ss))
            {
            }
            template    <typename T, typename TRAITS>
            inline  SortedSet<T, TRAITS>::SortedSet (const _SharedPtrIRep& rep)
                : inherited (typename inherited::_SharedPtrIRep (rep))
            {
                RequireNotNull (rep);
                EnsureMember (&inherited::_GetRep (), _IRep);
            }
            template    <typename T, typename TRAITS>
            inline  SortedSet<T, TRAITS>::SortedSet (const std::initializer_list<T>& s)
                : inherited (static_cast<const inherited&> (Concrete::SortedSet_Factory<T, TRAITS>::mk ()))
            {
                AssertMember (&inherited::_GetRep (), _IRep);
                this->AddAll (s);
            }
            template    <typename T, typename TRAITS>
            template    <typename CONTAINER_OF_T>
            inline  SortedSet<T, TRAITS>::SortedSet (const CONTAINER_OF_T& s)
                : inherited (static_cast<const inherited&> (Concrete::SortedSet_Factory<T, TRAITS>::mk ()))
            {
                AssertMember (&inherited::_GetRep (), _IRep);
                this->AddAll (s);
            }
            template    <typename T, typename TRAITS>
            inline  SortedSet<T, TRAITS>&   SortedSet<T, TRAITS>::operator= (const SortedSet<T, TRAITS>& src)
            {
                inherited::operator= (static_cast<const inherited&> (src));
                EnsureMember (&inherited::_GetRep (), _IRep);
                return *this;
            }


        }
    }
}

#endif /* _Stroika_Foundation_Containers_SortedSet_inl_ */
