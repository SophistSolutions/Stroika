/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Queue_inl_
#define _Stroika_Foundation_Containers_Queue_inl_

#include    "../Configuration/Concepts.h"
#include    "../Debug/Assertions.h"

#include    "Concrete/Queue_Factory.h"
#include    "Private/IterableUtils.h"


namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {


            /*
             ********************************************************************************
             ************************************ Queue<T> **********************************
             ********************************************************************************
             */
            template    <typename T>
            inline  Queue<T>::Queue ()
                : inherited (static_cast<const inherited&> (Concrete::mkQueue_Default<T> ()))
            {
                AssertMember (&inherited::_GetRep (), _IRep);
            }
            template    <typename T>
            inline  Queue<T>::Queue (const Queue<T>& q)
                : inherited (static_cast<const inherited&> (q))
            {
                AssertMember (&inherited::_GetRep (), _IRep);
            }
            template    <typename T>
            template    <typename CONTAINER_OF_T>
            inline  Queue<T>::Queue (const CONTAINER_OF_T& q)
                : inherited (static_cast<const inherited&> (Concrete::mkQueue_Default<T> ()))
            {
                AssertMember (&inherited::_GetRep (), _IRep);
                AssertNotImplemented ();
//                AddAll (q);
            }
            template    <typename T>
            inline  Queue<T>::Queue (const _SharedPtrIRep& rep)
                : inherited (typename inherited::_SharedPtrIRep (rep))
            {
                AssertMember (&inherited::_GetRep (), _IRep);
                RequireNotNull (rep);
            }
            template    <typename T>
            template    <typename COPY_FROM_ITERATOR_OF_T>
            inline Queue<T>::Queue (COPY_FROM_ITERATOR_OF_T start, COPY_FROM_ITERATOR_OF_T end)
                : inherited (static_cast<const inherited&> (Concrete::mkQueue_Default<T> ()))
            {
                AssertMember (&inherited::_GetRep (), _IRep);
                AssertNotImplemented ();
//                AddAll (start, end);
            }
            template    <typename T>
            inline  const typename  Queue<T>::_IRep&    Queue<T>::_GetRep () const
            {
                EnsureMember (&inherited::_GetRep (), _IRep);       // use static_cast cuz more efficient, but validate with assertion
                return *static_cast<const _IRep*> (&inherited::_GetRep ());
            }
            template    <typename T>
            inline  typename    Queue<T>::_IRep&  Queue<T>::_GetRep ()
            {
                EnsureMember (&inherited::_GetRep (), _IRep);       // use static_cast cuz more efficient, but validate with assertion
                return *static_cast<_IRep*> (&inherited::_GetRep ());
            }
            template    <typename T>
            inline  void    Queue<T>::AddTail (T item)
            {
                _GetRep ().AddTail (item);
            }
            template    <typename T>
            inline  T       Queue<T>::Head () const
            {
                return _GetRep ().Head ();
            }
            template    <typename T>
            inline  T       Queue<T>::RemoveHead ()
            {
                return _GetRep ().RemoveHead ();
            }
            template    <typename T>
            inline  void    Queue<T>::Enqueue (T item)
            {
                _GetRep ().AddTail (item);
            }
            template    <typename T>
            inline  T       Queue<T>::Dequeue ()
            {
                return _GetRep ().RemoveHead ();
            }
            template    <typename T>
            inline  void    Queue<T>::RemoveAll ()
            {
                _GetRep ().RemoveAll ();
            }
            template    <typename T>
            bool    Queue<T>::Equals (const Queue<T>& rhs) const
            {
                RequireConceptAppliesToTypeInFunction(RequireOperatorEquals, T);
                return Private::Equals_<T> (*this, rhs);        // Because we define ordering as compare for equals
            }
            template    <typename T>
            inline  bool  Queue<T>::operator== (const Queue<T>& rhs) const
            {
                return Equals (rhs);
            }
            template    <typename T>
            inline  bool    Queue<T>::operator!= (const Queue<T>& rhs) const
            {
                return (not Equals (rhs));
            }


            /*
             ********************************************************************************
             ***************************** Queue<T>::_IRep **********************************
             ********************************************************************************
             */
            template    <typename T>
            inline  Queue<T>::_IRep::_IRep ()
            {
            }
            template    <typename T>
            inline  Queue<T>::_IRep::~_IRep ()
            {
            }


        }
    }
}



#endif /* _Stroika_Foundation_Containers_Queue_inl_ */
