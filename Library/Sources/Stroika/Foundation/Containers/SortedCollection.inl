/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_SortedCollection_inl_
#define _Stroika_Foundation_Containers_SortedCollection_inl_

#include    "../Debug/Assertions.h"

#include    "Concrete/SortedCollection_Factory.h"


namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {


            /*
             ********************************************************************************
             **************************** SortedCollection<T, TRAITS> ******************************
             ********************************************************************************
             */
            template    <typename T, typename TRAITS>
            inline  SortedCollection<T, TRAITS>::SortedCollection ()
                : inherited (static_cast<const inherited&> (Concrete::SortedCollection_Factory<T, TRAITS>::mk ()))
            {
            }
            template    <typename T, typename TRAITS>
            inline  SortedCollection<T, TRAITS>::SortedCollection (const SortedCollection<T, TRAITS>& sb)
                : inherited (static_cast<const inherited&> (sb))
            {
            }
            template    <typename T, typename TRAITS>
            inline  SortedCollection<T, TRAITS>::SortedCollection (const _SharedPtrIRep& rep)
                : inherited (typename inherited::_SharedPtrIRep (rep))
            {
                RequireNotNull (rep);
                EnsureMember (&inherited::_GetRep (), _IRep);
            }
            template    <typename T, typename TRAITS>
            inline  SortedCollection<T, TRAITS>::SortedCollection (const std::initializer_list<T>& sb)
                : inherited (static_cast<const inherited&> (Concrete::SortedCollection_Factory<T, TRAITS>::mk ()))
            {
                AssertMember (&inherited::_GetRep (), _IRep);
                this->AddAll (sb);
            }
            template    <typename T, typename TRAITS>
            template    <typename CONTAINER_OF_T>
            inline  SortedCollection<T, TRAITS>::SortedCollection (const CONTAINER_OF_T& s)
                : inherited (static_cast<const inherited&> (Concrete::SortedCollection_Factory<T, TRAITS>::mk ()))
            {
                AssertMember (&inherited::_GetRep (), _IRep);
                this->AddAll (s);
            }
            template    <typename T, typename TRAITS>
            inline  SortedCollection<T, TRAITS>&   SortedCollection<T, TRAITS>::operator= (const SortedCollection<T, TRAITS>& rhs)
            {
                inherited::operator= (static_cast<const inherited&> (rhs));
                EnsureMember (&inherited::_GetRep (), _IRep);
                return *this;
            }


        }
    }
}



#endif /* _Stroika_Foundation_Containers_SortedCollection_inl_ */
