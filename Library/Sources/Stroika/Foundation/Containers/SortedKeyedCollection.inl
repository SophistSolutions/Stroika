/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_SortedKeyedCollection_inl_
#define _Stroika_Foundation_Containers_SortedKeyedCollection_inl_

#include "../Debug/Assertions.h"
//#include    "Factory/SortedKeyedCollection_Factory.h"

namespace Stroika::Foundation::Containers {

    /*
            ********************************************************************************
            ******************* SortedKeyedCollection<T, TRAITS> ***************************
            ********************************************************************************
            */
#if 0
        template    <typename T, typename TRAITS>
        inline  SortedKeyedCollection<T, TRAITS>::SortedKeyedCollection ()
            : inherited (move (Concrete::SortedCollection_Factory<T, TRAITS> () ()))
        {
            _AssertRepValidType ();
        }
        template    <typename T, typename TRAITS>
        inline  SortedCollection<T, TRAITS>::SortedCollection (const SortedCollection<T, TRAITS>& src)
            : inherited (src)
        {
            _AssertRepValidType ();
        }
        template    <typename T, typename TRAITS>
        inline  SortedCollection<T, TRAITS>::SortedCollection (const _SharedPtrIRep& src)
            : inherited (src)
        {
            RequireNotNull (src);
            _AssertRepValidType ();
        }
        template    <typename T, typename TRAITS>
        inline  SortedCollection<T, TRAITS>::SortedCollection (_SharedPtrIRep&& src)
            : inherited (move (src))
        {
            //RequireNotNull (src); -- logically required, but we cannot test here, must test before mem-initializers
            _AssertRepValidType ();
        }
        template    <typename T, typename TRAITS>
        inline  SortedCollection<T, TRAITS>::SortedCollection (const initializer_list<T>& src)
            : inherited (move (Concrete::SortedCollection_Factory<T, TRAITS>::New ()))
        {
            _AssertRepValidType ();
            this->AddAll (src);
            _AssertRepValidType ();
        }
        template    <typename T, typename TRAITS>
        template    <typename CONTAINER_OF_ADDABLE, typename ENABLE_IF>
        inline  SortedCollection<T, TRAITS>::SortedCollection (const CONTAINER_OF_ADDABLE& src)
            : inherited (move (Concrete::SortedCollection_Factory<T, TRAITS>::New ()))
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
#if qDebug
            AssertMember (&inherited::_ConstGetRep (), _IRep);
#endif
        }
#endif

}

#endif /* _Stroika_Foundation_Containers_SortedKeyedCollection_inl_ */
