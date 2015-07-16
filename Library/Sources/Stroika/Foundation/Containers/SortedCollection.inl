/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
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
                : inherited (move<inherited> (Concrete::SortedCollection_Factory<T, TRAITS>::mk ()))
            {
                _AssertRepValidType ();
            }
            template    <typename T, typename TRAITS>
            inline  SortedCollection<T, TRAITS>::SortedCollection (const SortedCollection<T, TRAITS>& src)
                : inherited (static_cast <const inherited&> (src))
            {
                _AssertRepValidType ();
            }
            template    <typename T, typename TRAITS>
            inline  SortedCollection<T, TRAITS>::SortedCollection (const _SharedPtrIRep& src)
                : inherited (static_cast<const typename inherited::_SharedPtrIRep&> (src))
            {
                RequireNotNull (src);
                _AssertRepValidType ();
            }
            template    <typename T, typename TRAITS>
            inline  SortedCollection<T, TRAITS>::SortedCollection (_SharedPtrIRep&& src)
                : inherited (move<typename inherited::_SharedPtrIRep> (src))
            {
                //RequireNotNull (src); -- logically required, but we cannot test here, must test before mem-initializers
                _AssertRepValidType ();
            }
            template    <typename T, typename TRAITS>
            inline  SortedCollection<T, TRAITS>::SortedCollection (const initializer_list<T>& src)
                : inherited (move<inherited> (Concrete::SortedCollection_Factory<T, TRAITS>::mk ()))
            {
                _AssertRepValidType ();
                this->AddAll (src);
                _AssertRepValidType ();
            }
            template    <typename T, typename TRAITS>
            template    <typename CONTAINER_OF_T, typename ENABLE_IF>
            inline  SortedCollection<T, TRAITS>::SortedCollection (const CONTAINER_OF_T& src)
                : inherited (move<inherited> (Concrete::SortedCollection_Factory<T, TRAITS>::mk ()))
            {
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
