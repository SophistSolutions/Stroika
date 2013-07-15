/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Stack_inl_
#define _Stroika_Foundation_Containers_Stack_inl_

#include    "../Configuration/Concepts.h"
#include    "../Debug/Assertions.h"

#include    "Concrete/Stack_Factory.h"
#include    "Private/IterableUtils.h"


namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {


            /*
             ********************************************************************************
             ************************************ Stack<T> **********************************
             ********************************************************************************
             */
            template    <typename T>
            inline  Stack<T>::Stack ()
                : inherited (static_cast<const inherited&> (Concrete::mkStack_Default<T> ()))
            {
                AssertMember (&inherited::_GetRep (), _IRep);
            }
            template    <typename T>
            inline  Stack<T>::Stack (const Stack<T>& s)
                : inherited (static_cast<const inherited&> (s))
            {
                AssertMember (&inherited::_GetRep (), _IRep);
            }
            template    <typename T>
            template    <typename CONTAINER_OF_T>
            inline  Stack<T>::Stack (const CONTAINER_OF_T& s)
                : inherited (static_cast<const inherited&> (Concrete::mkStack_Default<T> ()))
            {
                AssertMember (&inherited::_GetRep (), _IRep);
                AssertNotImplemented ();
//                AddAll (s);
            }
            template    <typename T>
            inline  Stack<T>::Stack (const _SharedPtrIRep& rep)
                : inherited (typename inherited::_SharedPtrIRep (rep))
            {
                AssertMember (&inherited::_GetRep (), _IRep);
                RequireNotNull (rep);
            }
            template    <typename T>
            template    <typename COPY_FROM_ITERATOR_OF_T>
            inline Stack<T>::Stack (COPY_FROM_ITERATOR_OF_T start, COPY_FROM_ITERATOR_OF_T end)
                : inherited (static_cast<const inherited&> (Concrete::mkStack_Default<T> ()))
            {
                AssertMember (&inherited::_GetRep (), _IRep);
                AssertNotImplemented ();
//                AddAll (start, end);
            }
            template    <typename T>
            inline  const typename  Stack<T>::_IRep&    Stack<T>::_GetRep () const
            {
                EnsureMember (&inherited::_GetRep (), _IRep);       // use static_cast cuz more efficient, but validate with assertion
                return *static_cast<const _IRep*> (&inherited::_GetRep ());
            }
            template    <typename T>
            inline  typename    Stack<T>::_IRep&  Stack<T>::_GetRep ()
            {
                EnsureMember (&inherited::_GetRep (), _IRep);       // use static_cast cuz more efficient, but validate with assertion
                return *static_cast<_IRep*> (&inherited::_GetRep ());
            }
            template    <typename T>
            inline  void    Stack<T>::Push (T item)
            {
                _GetRep ().Push (item);
            }
            template    <typename T>
            inline  T       Stack<T>::Pop ()
            {
                return _GetRep ().Pop ();
            }
            template    <typename T>
            inline  T       Stack<T>::Top () const
            {
                return _GetRep ().Top ();
            }
            template    <typename T>
            inline  void    Stack<T>::RemoveAll ()
            {
                _GetRep ().RemoveAll ();
            }
            template    <typename T>
            bool    Stack<T>::Equals (const Stack<T>& rhs) const
            {
                RequireConceptAppliesToTypeInFunction(RequireOperatorEquals, T);
                return Private::Equals_<T, Common::ComparerWithEquals<T>> (*this, rhs);        // Because we define ordering as compare for equals
            }
            template    <typename T>
            inline  bool  Stack<T>::operator== (const Stack<T>& rhs) const
            {
                return Equals (rhs);
            }
            template    <typename T>
            inline  bool    Stack<T>::operator!= (const Stack<T>& rhs) const
            {
                return (not Equals (rhs));
            }


            /*
             ********************************************************************************
             ***************************** Stack<T>::_IRep **********************************
             ********************************************************************************
             */
            template    <typename T>
            inline  Stack<T>::_IRep::_IRep ()
            {
            }
            template    <typename T>
            inline  Stack<T>::_IRep::~_IRep ()
            {
            }


        }
    }
}



#endif /* _Stroika_Foundation_Containers_Stack_inl_ */
