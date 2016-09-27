/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Deque_inl_
#define _Stroika_Foundation_Containers_Deque_inl_

#include    "../Debug/Assertions.h"
#include    "Factory/Deque_Factory.h"


namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {


            /*
             ********************************************************************************
             *********************************** Deque<T> ***********************************
             ********************************************************************************
             */
            template    <typename T>
            inline  Deque<T>::Deque (ContainerUpdateIteratorSafety containerUpdateSafetyPolicy)
                : inherited (move (Concrete::Deque_Factory<T>::mk ()))
            {
                _AssertRepValidType ();
            }
            template    <typename T>
            inline  Deque<T>::Deque (const Deque<T>& src)
                : inherited (static_cast<const inherited&> (src))
            {
                _AssertRepValidType ();
            }
            template    <typename T>
            inline  Deque<T>::Deque (const initializer_list<T>& src, ContainerUpdateIteratorSafety containerUpdateSafetyPolicy)
                : inherited (move (Concrete::Deque_Factory<T>::mk ()))
            {
                _AssertRepValidType ();
                this->AddAllToTail (src);
                _AssertRepValidType ();
            }
            template    <typename T>
            template    <typename CONTAINER_OF_T, typename ENABLE_IF>
            inline  Deque<T>::Deque (const CONTAINER_OF_T& src, ContainerUpdateIteratorSafety containerUpdateSafetyPolicy)
                : inherited (move (Concrete::Deque_Factory<T>::mk ()))
            {
                _AssertRepValidType ();
                this->AddAllToTail (src);
                _AssertRepValidType ();
            }
            template    <typename T>
            inline  Deque<T>::Deque (const _SharedPtrIRep& src)
                : inherited (src)
            {
                RequireNotNull (src);
                _AssertRepValidType ();
            }
            template    <typename T>
            inline  Deque<T>::Deque (_SharedPtrIRep&& src)
                : inherited (move (src))
            {
                //RequireNotNull (src); -- logically required, but we cannot test here, must test before mem-initializers
                _AssertRepValidType ();
            }
            template    <typename T>
            template    <typename COPY_FROM_ITERATOR_OF_T>
            inline Deque<T>::Deque (COPY_FROM_ITERATOR_OF_T start, COPY_FROM_ITERATOR_OF_T end, ContainerUpdateIteratorSafety containerUpdateSafetyPolicy)
                : inherited (move (Concrete::Deque_Factory<T>::mk ()))
            {
                _AssertRepValidType ();
                this->AddAllToTail (start, end);
                _AssertRepValidType ();
            }
            template    <typename T>
            inline  void    Deque<T>::AddHead (ArgByValueType<T> item)
            {
                _SafeReadWriteRepAccessor<_IRep> { this } ._GetWriteableRep ().AddHead (item);
            }
            template    <typename T>
            inline  T    Deque<T>::RemoveTail ()
            {
                return _SafeReadWriteRepAccessor<_IRep> { this } ._GetWriteableRep ().RemoveTail ();
            }
            template    <typename T>
            inline  T       Deque<T>::Tail () const
            {
                return _SafeReadRepAccessor<_IRep> { this } ._ConstGetRep ().Tail ();
            }
            template    <typename T>
            inline  void    Deque<T>::_AssertRepValidType () const
            {
#if     qDebug
                _SafeReadRepAccessor<_IRep> { this };
#endif
            }


        }
    }
}



#endif /* _Stroika_Foundation_Containers_Deque_inl_ */
