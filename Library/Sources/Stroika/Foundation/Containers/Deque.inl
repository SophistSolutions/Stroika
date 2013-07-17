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
             **************************** Deque<T, TRAITS> **********************************
             ********************************************************************************
             */
            template    <typename T, typename TRAITS>
            inline  Deque<T, TRAITS>::Deque ()
                : inherited (static_cast<const inherited&> (Concrete::mkDeque_Default<T, TRAITS> ()))
            {
                AssertMember (&inherited::_GetRep (), _IRep);
            }
            template    <typename T, typename TRAITS>
            inline  Deque<T, TRAITS>::Deque (const Deque<T, TRAITS>& src)
                : inherited (static_cast<const inherited&> (src))
            {
                AssertMember (&inherited::_GetRep (), _IRep);
            }
            template    <typename T, typename TRAITS>
            template    <typename CONTAINER_OF_T>
            inline  Deque<T, TRAITS>::Deque (const CONTAINER_OF_T& src)
                : inherited (static_cast<const inherited&> (Concrete::mkDeque_Default<T, TRAITS> ()))
            {
                AssertMember (&inherited::_GetRep (), _IRep);
//                AddAll (s);
            }
            template    <typename T, typename TRAITS>
            inline  Deque<T, TRAITS>::Deque (const _SharedPtrIRep& rep)
                : inherited (typename inherited::_SharedPtrIRep (rep))
            {
                AssertMember (&inherited::_GetRep (), _IRep);
                RequireNotNull (rep);
            }
            template    <typename T, typename TRAITS>
            template    <typename COPY_FROM_ITERATOR_OF_T>
            inline Deque<T, TRAITS>::Deque (COPY_FROM_ITERATOR_OF_T start, COPY_FROM_ITERATOR_OF_T end)
                : inherited (static_cast<const inherited&> (Concrete::mkDeque_Default<T, TRAITS> ()))
            {
                AssertMember (&inherited::_GetRep (), _IRep);
//                AddAll (start, end);
            }
            template    <typename T, typename TRAITS>
            inline  const typename  Deque<T, TRAITS>::_IRep&    Deque<T, TRAITS>::_GetRep () const
            {
                EnsureMember (&inherited::_GetRep (), _IRep);       // use static_cast cuz more efficient, but validate with assertion
                return *static_cast<const _IRep*> (&inherited::_GetRep ());
            }
            template    <typename T, typename TRAITS>
            inline  typename    Deque<T, TRAITS>::_IRep&  Deque<T, TRAITS>::_GetRep ()
            {
                EnsureMember (&inherited::_GetRep (), _IRep);       // use static_cast cuz more efficient, but validate with assertion
                return *static_cast<_IRep*> (&inherited::_GetRep ());
            }
            template    <typename T, typename TRAITS>
            inline  void    Deque<T, TRAITS>::AddHead (T item)
            {
                _GetRep ().AddHead (item);
            }
            template    <typename T, typename TRAITS>
            inline  T    Deque<T, TRAITS>::RemoveTail ()
            {
                return _GetRep ().RemoveTail ();
            }
            template    <typename T, typename TRAITS>
            inline  T       Deque<T, TRAITS>::Tail () const
            {
                return _GetRep ().Tail ();
            }


        }
    }
}



#endif /* _Stroika_Foundation_Containers_Deque_inl_ */
