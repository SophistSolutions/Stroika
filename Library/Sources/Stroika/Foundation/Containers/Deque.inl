/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Deque_inl_
#define _Stroika_Foundation_Containers_Deque_inl_

#include    "../Debug/Assertions.h"

#include    "Concrete/Deque_Factory.h"


namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {


            /*
             ********************************************************************************
             ************************************ Deque<T> **********************************
             ********************************************************************************
             */
            template    <typename T>
            inline  Deque<T>::Deque ()
                : inherited (static_cast<const inherited&> (Concrete::mkDeque_Default<T> ()))
            {
                AssertMember (&inherited::_GetRep (), _IRep);
            }
            template    <typename T>
            inline  Deque<T>::Deque (const Deque<T>& d)
                : inherited (static_cast<const inherited&> (d))
            {
                AssertMember (&inherited::_GetRep (), _IRep);
            }
            template    <typename T>
            template    <typename CONTAINER_OF_T>
            inline  Deque<T>::Deque (const CONTAINER_OF_T& d)
                : inherited (static_cast<const inherited&> (Concrete::mkDeque_Default<T> ()))
            {
                AssertMember (&inherited::_GetRep (), _IRep);
//                AddAll (s);
            }
            template    <typename T>
            inline  Deque<T>::Deque (const _SharedPtrIRep& rep)
                : inherited (typename inherited::_SharedPtrIRep (rep))
            {
                AssertMember (&inherited::_GetRep (), _IRep);
                RequireNotNull (rep);
            }
            template    <typename T>
            template    <typename COPY_FROM_ITERATOR_OF_T>
            inline Deque<T>::Deque (COPY_FROM_ITERATOR_OF_T start, COPY_FROM_ITERATOR_OF_T end)
                : inherited (static_cast<const inherited&> (Concrete::mkDeque_Default<T> ()))
            {
                AssertMember (&inherited::_GetRep (), _IRep);
//                AddAll (start, end);
            }
            template    <typename T>
            inline  const typename  Deque<T>::_IRep&    Deque<T>::_GetRep () const
            {
                EnsureMember (&inherited::_GetRep (), _IRep);       // use static_cast cuz more efficient, but validate with assertion
                return *static_cast<const _IRep*> (&inherited::_GetRep ());
            }
            template    <typename T>
            inline  typename    Deque<T>::_IRep&  Deque<T>::_GetRep ()
            {
                EnsureMember (&inherited::_GetRep (), _IRep);       // use static_cast cuz more efficient, but validate with assertion
                return *static_cast<_IRep*> (&inherited::_GetRep ());
            }
            template    <typename T>
            inline  void    Deque<T>::AddHead (T item)
            {
                _GetRep ().AddHead (item);
            }
            template    <typename T>
            inline  T    Deque<T>::RemoveTail ()
            {
                return _GetRep ().RemoveTail ();
            }
            template    <typename T>
            inline  T       Deque<T>::Tail () const
            {
                return _GetRep ().Tail ();
            }


        }
    }
}



#endif /* _Stroika_Foundation_Containers_Deque_inl_ */
