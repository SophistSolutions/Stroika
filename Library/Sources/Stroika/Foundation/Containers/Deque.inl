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
                AssertMember (&inherited::_GetRep (), _IRep);
            }
            template    <typename T, typename TRAITS>
            inline  Deque<T, TRAITS>::Deque (const Deque<T, TRAITS>& src)
                : inherited (static_cast < const inherited&& > (src))
            {
                AssertMember (&inherited::_GetRep (), _IRep);
            }
            template    <typename T, typename TRAITS>
            inline  Deque<T, TRAITS>::Deque (const std::initializer_list<T>& d)
                : inherited (static_cast < const inherited&& > (Concrete::Deque_Factory<T, TRAITS>::mk ()))
            {
                AssertMember (&inherited::_GetRep (), _IRep);
                this->AddAllToTail (d);
            }
            template    <typename T, typename TRAITS>
            template    <typename CONTAINER_OF_T>
            inline  Deque<T, TRAITS>::Deque (const CONTAINER_OF_T& src)
                : inherited (static_cast < const inherited&& > (Concrete::Deque_Factory<T, TRAITS>::mk ()))
            {
                AssertMember (&inherited::_GetRep (), _IRep);
                this->AddAllToTail (src);
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
                : inherited (static_cast < const inherited&& > (Concrete::Deque_Factory<T, TRAITS>::mk ()))
            {
                AssertMember (&inherited::_GetRep (), _IRep);
                this->AddAllToTail (start, end);
            }
            template    <typename T, typename TRAITS>
            inline  const typename  Deque<T, TRAITS>::_IRep&    Deque<T, TRAITS>::_ConstGetRep () const
            {
                EnsureMember (&inherited::_ConstGetRep (), _IRep);       // use static_cast cuz more efficient, but validate with assertion
                return *static_cast<const _IRep*> (&inherited::_ConstGetRep ());
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
