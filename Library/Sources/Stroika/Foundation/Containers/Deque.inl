/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
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
                : inherited (static_cast < const inherited&& > (Concrete::Deque_Factory<T, TRAITS>::mk ()))
            {
                _AssertRepValidType ();
            }
            template    <typename T, typename TRAITS>
            inline  Deque<T, TRAITS>::Deque (const Deque<T, TRAITS>& src)
                : inherited (static_cast < const inherited&& > (src))
            {
                _AssertRepValidType ();
            }
            template    <typename T, typename TRAITS>
            inline  Deque<T, TRAITS>::Deque (const initializer_list<T>& d)
                : inherited (static_cast < const inherited&& > (Concrete::Deque_Factory<T, TRAITS>::mk ()))
            {
                _AssertRepValidType ();
                this->AddAllToTail (d);
                _AssertRepValidType ();
            }
            template    <typename T, typename TRAITS>
            template    <typename CONTAINER_OF_T>
            inline  Deque<T, TRAITS>::Deque (const CONTAINER_OF_T& src)
                : inherited (static_cast < const inherited&& > (Concrete::Deque_Factory<T, TRAITS>::mk ()))
            {
                _AssertRepValidType ();
                this->AddAllToTail (src);
                _AssertRepValidType ();
            }
            template    <typename T, typename TRAITS>
            inline  Deque<T, TRAITS>::Deque (const _SharedPtrIRep& rep)
                : inherited (typename inherited::_SharedPtrIRep (rep))
            {
                _AssertRepValidType ();
                RequireNotNull (rep);
            }
            template    <typename T, typename TRAITS>
            template    <typename COPY_FROM_ITERATOR_OF_T>
            inline Deque<T, TRAITS>::Deque (COPY_FROM_ITERATOR_OF_T start, COPY_FROM_ITERATOR_OF_T end)
                : inherited (static_cast < const inherited&& > (Concrete::Deque_Factory<T, TRAITS>::mk ()))
            {
                _AssertRepValidType ();
                this->AddAllToTail (start, end);
                _AssertRepValidType ();
            }
            template    <typename T, typename TRAITS>
            inline  void    Deque<T, TRAITS>::AddHead (T item)
            {
                _SafeReadWriteRepAccessor<_IRep> { this } ._GetWriteableRep ().AddHead (item);
            }
            template    <typename T, typename TRAITS>
            inline  T    Deque<T, TRAITS>::RemoveTail ()
            {
                return _SafeReadWriteRepAccessor<_IRep> { this } ._GetWriteableRep ().RemoveTail ();
            }
            template    <typename T, typename TRAITS>
            inline  T       Deque<T, TRAITS>::Tail () const
            {
                return _SafeReadRepAccessor<_IRep> { this } ._ConstGetRep ().Tail ();
            }
            template    <typename T, typename TRAITS>
            inline  void    Deque<T, TRAITS>::_AssertRepValidType () const
            {
#if     qDebug
                AssertMember (&inherited::_ConstGetRep (), _IRep);
#endif
            }


        }
    }
}



#endif /* _Stroika_Foundation_Containers_Deque_inl_ */
