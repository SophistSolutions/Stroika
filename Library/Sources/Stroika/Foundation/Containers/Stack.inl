/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
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
             ************************************* Stack<T> *********************************
             ********************************************************************************
             */
            template    <typename T>
            inline  Stack<T>::Stack ()
                : inherited (static_cast < const inherited&& > (Concrete::Stack_Factory<T>::mk ()))
            {
                _AssertRepValidType ();
            }
            template    <typename T>
            inline  Stack<T>::Stack (const Stack<T>& src)
                : inherited (static_cast<const inherited&> (src))
            {
                _AssertRepValidType ();
            }
            template    <typename T>
            template    <typename CONTAINER_OF_T>
            inline  Stack<T>::Stack (const CONTAINER_OF_T& src)
                : inherited (static_cast < const inherited&& > (Concrete::Stack_Factory<T>::mk ()))
            {
                _AssertRepValidType ();
                AssertNotImplemented ();
//                AddAll (s);
            }
            template    <typename T>
            inline  Stack<T>::Stack (const _SharedPtrIRep& rep)
                : inherited (typename inherited::_SharedPtrIRep (rep))
            {
                _AssertRepValidType ();
                RequireNotNull (rep);
            }
            template    <typename T>
            template    <typename COPY_FROM_ITERATOR_OF_T>
            inline Stack<T>::Stack (COPY_FROM_ITERATOR_OF_T start, COPY_FROM_ITERATOR_OF_T end)
                : inherited (static_cast < const inherited&& > (Concrete::Stack_Factory<T>::mk ()))
            {
                _AssertRepValidType ();
                AssertNotImplemented ();
//                AddAll (start, end);
            }
            template    <typename T>
            inline  const typename  Stack<T>::_IRep&    Stack<T>::_ConstGetRep () const
            {
                EnsureMember (&inherited::_ConstGetRep (), _IRep);       // use static_cast cuz more efficient, but validate with assertion
                return *static_cast<const _IRep*> (&inherited::_ConstGetRep ());
            }
            template    <typename T>
            inline  void    Stack<T>::Push (T item)
            {
                _SafeReadWriteRepAccessor<_IRep> { this } ._GetWriteableRep ().Push (item);
            }
            template    <typename T>
            inline  T       Stack<T>::Pop ()
            {
                return _SafeReadWriteRepAccessor<_IRep> { this } ._GetWriteableRep ().Pop ();
            }
            template    <typename T>
            inline  T       Stack<T>::Top () const
            {
                return _SafeReadRepAccessor<_IRep> { this } ._ConstGetRep ().Top ();
            }
            template    <typename T>
            inline  void    Stack<T>::RemoveAll ()
            {
                _SafeReadWriteRepAccessor<_IRep> { this } ._GetWriteableRep ().RemoveAll ();
            }
            template    <typename T>
            template    <typename EQUALS_COMPARER>
            bool    Stack<T>::Equals (const Stack<T>& rhs) const
            {
                return Private::Equals_<T, EQUALS_COMPARER> (*this, rhs);
            }
            template    <typename T>
            inline  bool  Stack<T>::operator== (const Stack<T>& rhs) const
            {
                return Equals (rhs);
            }
            template    <typename T>
            inline  bool    Stack<T>::operator!= (const Stack<T>& rhs) const
            {
                return not Equals (rhs);
            }
            template    <typename T>
            inline  void    Stack<T>::_AssertRepValidType () const
            {
#if     qDebug
                AssertMember (&inherited::_ConstGetRep (), _IRep);
#endif
            }


        }
    }
}



#endif /* _Stroika_Foundation_Containers_Stack_inl_ */
