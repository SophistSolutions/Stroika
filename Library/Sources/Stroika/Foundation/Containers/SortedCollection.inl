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
             ************************ SortedCollection<T, TRAITS> ***************************
             ********************************************************************************
             */
            template    <typename T, typename TRAITS>
            inline  SortedCollection<T, TRAITS>::SortedCollection ()
                : inherited (static_cast < const inherited&& > (Concrete::SortedCollection_Factory<T, TRAITS>::mk ()))
            {
                _AssertRepValidType ();
            }
            template    <typename T, typename TRAITS>
            inline  SortedCollection<T, TRAITS>::SortedCollection (const SortedCollection<T, TRAITS>& src)
                : inherited (static_cast < const inherited&& > (src))
            {
                _AssertRepValidType ();
            }
            template    <typename T, typename TRAITS>
            inline  SortedCollection<T, TRAITS>::SortedCollection (const _SharedPtrIRep& rep)
                : inherited (typename inherited::_SharedPtrIRep (rep))
            {
                RequireNotNull (rep);
                _AssertRepValidType ();
            }
            template    <typename T, typename TRAITS>
            inline  SortedCollection<T, TRAITS>::SortedCollection (const std::initializer_list<T>& src)
                : inherited (static_cast<const inherited&> (Concrete::SortedCollection_Factory<T, TRAITS>::mk ()))
            {
                _AssertRepValidType ();
                this->AddAll (src);
                _AssertRepValidType ();
            }
            template    <typename T, typename TRAITS>
            template    <typename CONTAINER_OF_T>
            inline  SortedCollection<T, TRAITS>::SortedCollection (const CONTAINER_OF_T& src)
                : inherited (static_cast<const inherited&> (Concrete::SortedCollection_Factory<T, TRAITS>::mk ()))
            {
                _AssertRepValidType ();
                AssertMember (&inherited::_ConstGetRep (), _IRep);
                _AssertRepValidType ();
            }
            template    <typename T, typename TRAITS>
            inline  SortedCollection<T, TRAITS>&   SortedCollection<T, TRAITS>::operator= (const SortedCollection<T, TRAITS>& rhs)
            {
                _AssertRepValidType ();
                inherited::operator= (static_cast<const inherited&> (rhs));
                _AssertRepValidType ();
                return *this;
            }
            template    <typename T, typename TRAITS>
            inline  void    SortedCollection <T, TRAITS>::_AssertRepValidType () const
            {
#if     qDebug
                AssertMember (&inherited::_ConstGetRep (), _IRep);
#endif
            }


        }
    }
}



#endif /* _Stroika_Foundation_Containers_SortedCollection_inl_ */
