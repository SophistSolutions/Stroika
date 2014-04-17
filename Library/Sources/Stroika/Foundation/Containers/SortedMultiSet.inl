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
                _AssertRepValidType ();
            }
            template    <typename T, typename TRAITS>
            inline  SortedMultiSet<T, TRAITS>::SortedMultiSet (const SortedMultiSet<T, TRAITS>& src)
                : inherited (static_cast<const inherited&> (src))
            {
                _AssertRepValidType ();
            }
            template    <typename T, typename TRAITS>
            inline  SortedMultiSet<T, TRAITS>::SortedMultiSet (const _SharedPtrIRep& rep)
                : inherited (typename inherited::_SharedPtrIRep (rep))
            {
                _AssertRepValidType ();
                RequireNotNull (rep);
                _AssertRepValidType ();
            }
            template    <typename T, typename TRAITS>
            SortedMultiSet<T, TRAITS>::SortedMultiSet (const initializer_list<T>& s)
                : inherited (static_cast < const inherited&& > (Concrete::SortedMultiSet_Factory<T, TRAITS>::mk ()))
            {
                _AssertRepValidType ();
                this->AddAll (s);
                _AssertRepValidType ();
            }
            template    <typename T, typename TRAITS>
            SortedMultiSet<T, TRAITS>::SortedMultiSet (const initializer_list<MultiSetEntry<T>>& s)
                : inherited (static_cast < const inherited&& > (Concrete::SortedMultiSet_Factory<T, TRAITS>::mk ()))
            {
                _AssertRepValidType ();
                this->AddAll (s);
                _AssertRepValidType ();
            }
            template    <typename T, typename TRAITS>
            template    <typename CONTAINER_OF_T>
            inline  SortedMultiSet<T, TRAITS>::SortedMultiSet (const CONTAINER_OF_T& src)
                : inherited (static_cast<const inherited&> (Concrete::SortedMultiSet_Factory<T, TRAITS>::mk ()))
            {
                _AssertRepValidType ();
                this->AddAll (src);
                _AssertRepValidType ();
            }
            template    <typename T, typename TRAITS>
            inline  SortedMultiSet<T, TRAITS>&   SortedMultiSet<T, TRAITS>::operator= (const SortedMultiSet<T, TRAITS>& src)
            {
                _AssertRepValidType ();
                inherited::operator= (static_cast<const inherited&> (src));
                _AssertRepValidType ();
                return *this;
            }
            template    <typename T, typename TRAITS>
            inline  void    SortedMultiSet <T, TRAITS>::_AssertRepValidType () const
            {
#if     qDebug
                AssertMember (&inherited::_ConstGetRep (), _IRep);
#endif
            }


        }
    }
}
#endif /* _Stroika_Foundation_Containers_SortedMultiSet_inl_ */
