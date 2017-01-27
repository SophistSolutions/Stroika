/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Stack_inl_
#define _Stroika_Foundation_Containers_Stack_inl_

#include    "../Configuration/Concepts.h"
#include    "../Debug/Assertions.h"
#include    "Factory/Stack_Factory.h"
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
                : inherited (move (Concrete::Stack_Factory<T>::mk ()))
            {
                _AssertRepValidType ();
            }
            template    <typename T>
            inline  Stack<T>::Stack (const Stack<T>& src) noexcept
                : inherited (src)
            {
                _AssertRepValidType ();
            }
            template    <typename T>
            inline  Stack<T>::Stack (Stack<T>&& src) noexcept
                : inherited (move (src))
            {
                _AssertRepValidType ();
            }
            template    <typename T>
            template    <typename CONTAINER_OF_T, typename ENABLE_IF>
            inline  Stack<T>::Stack (const CONTAINER_OF_T& src)
                : Stack ()
            {
                AssertNotImplemented ();
//                AddAll (s);
            }
            template    <typename T>
            inline  Stack<T>::Stack (const _SharedPtrIRep& src) noexcept
                : inherited (src)
            {
                RequireNotNull (src);
                _AssertRepValidType ();
            }
            template    <typename T>
            inline  Stack<T>::Stack (_SharedPtrIRep&& src) noexcept
                : inherited ((RequireNotNull (src), move (src)))
            {
                _AssertRepValidType ();
            }
            template    <typename T>
            template    <typename COPY_FROM_ITERATOR_OF_T>
            inline Stack<T>::Stack (COPY_FROM_ITERATOR_OF_T start, COPY_FROM_ITERATOR_OF_T end)
                : Stack ()
            {
                AssertNotImplemented ();
//                AddAll (start, end);
            }
            template    <typename T>
            inline  void    Stack<T>::Push (ArgByValueType<T> item)
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
                _SafeReadWriteRepAccessor<_IRep> tmp { this };
                if (not tmp._ConstGetRep ().IsEmpty ()) {
                    tmp._UpdateRep (tmp._ConstGetRep ().CloneEmpty (this));
                }
            }
            template    <typename T>
            template    <typename EQUALS_COMPARER>
            bool    Stack<T>::Equals (const Stack<T>& rhs) const
            {
                return Private::Equals_<T, EQUALS_COMPARER> (*this, rhs);
            }
            template    <typename T>
            inline  void    Stack<T>::_AssertRepValidType () const
            {
#if     qDebug
                _SafeReadRepAccessor<_IRep> { this };
#endif
            }


            /*
             ********************************************************************************
             **************************** Stack operators ***********************************
             ********************************************************************************
             */
            template    <typename T>
            inline  bool  operator== (const Stack<T>& lhs, const Stack<T>& rhs)
            {
                return lhs.Equals (rhs);
            }
            template    <typename T>
            inline  bool    operator!= (const Stack<T>& lhs, const Stack<T>& rhs)
            {
                return not lhs.Equals (rhs);
            }


        }
    }
}



#endif /* _Stroika_Foundation_Containers_Stack_inl_ */
