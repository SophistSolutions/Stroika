/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_SortedMultiSet_inl_
#define _Stroika_Foundation_Containers_SortedMultiSet_inl_

#include    "../Debug/Assertions.h"

#include    "Concrete/SortedMultiSet_Factory.h"


namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {


            /*
             ********************************************************************************
             ************************ SortedMultiSet<T, TRAITS> *****************************
             ********************************************************************************
             */
            template    <typename T, typename TRAITS>
            inline  SortedMultiSet<T, TRAITS>::SortedMultiSet ()
                : inherited (static_cast < const inherited&& > (Concrete::SortedMultiSet_Factory<T, TRAITS>::mk ()))
            {
                AssertMember (&inherited::_ConstGetRep (), _IRep);
            }
            template    <typename T, typename TRAITS>
            inline  SortedMultiSet<T, TRAITS>::SortedMultiSet (const SortedMultiSet<T, TRAITS>& src)
                : inherited (static_cast<const inherited&> (src))
            {
                AssertMember (&inherited::_ConstGetRep (), _IRep);
            }
            template    <typename T, typename TRAITS>
            inline  SortedMultiSet<T, TRAITS>::SortedMultiSet (const _SharedPtrIRep& rep)
                : inherited (typename inherited::_SharedPtrIRep (rep))
            {
                RequireNotNull (rep);
                EnsureMember (&inherited::_ConstGetRep (), _IRep);
            }
            template    <typename T, typename TRAITS>
            SortedMultiSet<T, TRAITS>::SortedMultiSet (const std::initializer_list<T>& s)
                : inherited (static_cast < const inherited&& > (Concrete::SortedMultiSet_Factory<T, TRAITS>::mk ()))
            {
                AssertMember (&inherited::_ConstGetRep (), _IRep);
                this->AddAll (s);
            }
            template    <typename T, typename TRAITS>
            SortedMultiSet<T, TRAITS>::SortedMultiSet (const std::initializer_list<MultiSetEntry<T>>& s)
                : inherited (static_cast < const inherited&& > (Concrete::SortedMultiSet_Factory<T, TRAITS>::mk ()))
            {
                AssertMember (&inherited::_ConstGetRep (), _IRep);
                this->AddAll (s);
            }
            template    <typename T, typename TRAITS>
            template    <typename CONTAINER_OF_T>
            inline  SortedMultiSet<T, TRAITS>::SortedMultiSet (const CONTAINER_OF_T& src)
                : inherited (static_cast<const inherited&> (Concrete::SortedMultiSet_Factory<T, TRAITS>::mk ()))
            {
                AssertMember (&inherited::_ConstGetRep (), _IRep);
                this->AddAll (src);
            }
            template    <typename T, typename TRAITS>
            inline  SortedMultiSet<T, TRAITS>&   SortedMultiSet<T, TRAITS>::operator= (const SortedMultiSet<T, TRAITS>& src)
            {
                inherited::operator= (static_cast<const inherited&> (src));
                EnsureMember (&inherited::_ConstGetRep (), _IRep);
                return *this;
            }


        }
    }
}
#endif /* _Stroika_Foundation_Containers_SortedMultiSet_inl_ */
