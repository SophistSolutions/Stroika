/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
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
                : inherited (move (Concrete::Queue_Factory<T>::mk ()))
            {
                _AssertRepValidType ();
            }
            template    <typename T>
            inline  Queue<T>::Queue (const Queue<T>& src)
                : inherited (static_cast<const inherited&> (src))
            {
                _AssertRepValidType ();
            }
            template    <typename T>
            inline  Queue<T>::Queue (const initializer_list<T>& q)
                : inherited (move (Concrete::Queue_Factory<T>::mk ()))
            {
                _AssertRepValidType ();
                AddAllToTail (q);
                _AssertRepValidType ();
            }
            template    <typename T>
            template    <typename CONTAINER_OF_T>
            inline  Queue<T>::Queue (const CONTAINER_OF_T& src)
                : inherited (move (Concrete::Queue_Factory<T>::mk ()))
            {
                _AssertRepValidType ();
                AssertNotImplemented ();
                AddAllToTail (src);
                _AssertRepValidType ();
            }
            template    <typename T>
            inline  Queue<T>::Queue (const _SharedPtrIRep& rep)
                : inherited (static_cast<const typename inherited::_SharedPtrIRep&> (rep))
            {
                _AssertRepValidType ();
                RequireNotNull (rep);
            }
            template    <typename T>
            inline  Queue<T>::Queue (_SharedPtrIRep&& rep)
                : inherited (move<typename inherited::_SharedPtrIRep> (rep))
            {
                //RequireNotNull (rep); -- logically required, but we cannot test here, must test before mem-initializers
                _AssertRepValidType ();
            }
            template    <typename T>
            template    <typename COPY_FROM_ITERATOR_OF_T>
            inline Queue<T>::Queue (COPY_FROM_ITERATOR_OF_T start, COPY_FROM_ITERATOR_OF_T end)
                : inherited (static_cast < const inherited&& > (Concrete::Queue_Factory<T>::mk ()))
            {
                _AssertRepValidType ();
                AddAllToTail (start, end);
                _AssertRepValidType ();
            }
#if     qDebug
            template    <typename T>
            Queue<T>::~Queue ()
            {
                if (this->_GetSharingState () != Memory::SharedByValue_State::eNull) {
                    _ConstGetRep ().AssertNoIteratorsReferenceOwner (this);
                }
            }
#endif
            template    <typename T>
            inline  const typename  Queue<T>::_IRep&    Queue<T>::_ConstGetRep () const
            {
                EnsureMember (&inherited::_ConstGetRep (), _IRep);       // use static_cast cuz more efficient, but validate with assertion
                return *static_cast<const _IRep*> (&inherited::_ConstGetRep ());
            }
            template    <typename T>
            inline  void    Queue<T>::AddTail (T item)
            {
                _SafeReadWriteRepAccessor<_IRep> { this } ._GetWriteableRep ().AddTail (item);
            }
            template    <typename T>
            inline  T       Queue<T>::Head () const
            {
                return _SafeReadRepAccessor<_IRep> { this } ._ConstGetRep ().Head ();
            }
            template    <typename T>
            inline  Memory::Optional<T>       Queue<T>::HeadIf () const
            {
                return _SafeReadRepAccessor<_IRep> { this } ._ConstGetRep ().HeadIf ();
            }
            template    <typename T>
            inline  T       Queue<T>::RemoveHead ()
            {
                return _SafeReadWriteRepAccessor<_IRep> { this } ._GetWriteableRep ().RemoveHead ();
            }
            template    <typename T>
            inline  Memory::Optional<T>       Queue<T>::RemoveHeadIf ()
            {
                return _SafeReadWriteRepAccessor<_IRep> { this } ._GetWriteableRep ().RemoveHeadIf ();
            }
            template    <typename T>
            inline  void    Queue<T>::Enqueue (T item)
            {
                _SafeReadWriteRepAccessor<_IRep> { this } ._GetWriteableRep ().AddTail (item);
            }
            template    <typename T>
            inline  T       Queue<T>::Dequeue ()
            {
                return _SafeReadWriteRepAccessor<_IRep> { this } ._GetWriteableRep ().RemoveHead ();
            }
            template    <typename T>
            template    <typename CONTAINER_OF_T>
            inline  void    Queue<T>::AddAllToTail (const CONTAINER_OF_T& s)
            {
                _SafeReadWriteRepAccessor<_IRep> tmp { this };
                for (auto i : s) {
                    tmp._GetWriteableRep ().AddTail ( &i, &i + 1);
                }
            }
            template    <typename T>
            template    <typename COPY_FROM_ITERATOR_OF_T>
            inline void Queue<T>::AddAllToTail (COPY_FROM_ITERATOR_OF_T start, COPY_FROM_ITERATOR_OF_T end)
            {
                _SafeReadWriteRepAccessor<_IRep> tmp { this };
                for (auto i = start; i != end; ++i) {
                    tmp._GetWriteableRep ().AddTail (Iterator2Address (i), Iterator2Address (i) + 1);
                }
            }
            template    <typename T>
            inline  void    Queue<T>::RemoveAll ()
            {
                _SafeReadWriteRepAccessor<_IRep> tmp { this };
                if (not tmp._ConstGetRep ().IsEmpty ()) {
                    tmp._UpdateRep (tmp._ConstGetRep ().CloneEmpty (this));
                }
            }
            template    <typename T>
            template    <typename EQUALS_COMPARER>
            bool    Queue<T>::Equals (const Queue<T>& rhs) const
            {
                return Private::Equals_<T, EQUALS_COMPARER> (*this, rhs);
            }
            template    <typename T>
            inline  bool  Queue<T>::operator== (const Queue<T>& rhs) const
            {
                return Equals (rhs);
            }
            template    <typename T>
            inline  bool    Queue<T>::operator!= (const Queue<T>& rhs) const
            {
                return not Equals (rhs);
            }
            template    <typename T>
            inline  void    Queue<T>::_AssertRepValidType () const
            {
#if     qDebug
                AssertMember (&inherited::_ConstGetRep (), _IRep);
#endif
            }


        }
    }
}



#endif /* _Stroika_Foundation_Containers_Queue_inl_ */
