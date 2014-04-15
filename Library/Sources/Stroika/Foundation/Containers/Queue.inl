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
             **************************** Queue<T, TRAITS> **********************************
             ********************************************************************************
             */
            template    <typename T, typename TRAITS>
            inline  Queue<T, TRAITS>::Queue ()
                : inherited (static_cast < const inherited&& > (Concrete::Queue_Factory<T, TRAITS>::mk ()))
            {
                _AssertRepValidType ();
            }
            template    <typename T, typename TRAITS>
            inline  Queue<T, TRAITS>::Queue (const Queue<T, TRAITS>& src)
                : inherited (static_cast<const inherited&> (src))
            {
                _AssertRepValidType ();
            }
            template    <typename T, typename TRAITS>
            inline  Queue<T, TRAITS>::Queue (const std::initializer_list<T>& q)
                : inherited (static_cast < const inherited&& > (Concrete::Queue_Factory<T, TRAITS>::mk ()))
            {
                _AssertRepValidType ();
                AddAllToTail (q);
                _AssertRepValidType ();
            }
            template    <typename T, typename TRAITS>
            template    <typename CONTAINER_OF_T>
            inline  Queue<T, TRAITS>::Queue (const CONTAINER_OF_T& src)
                : inherited (static_cast < const inherited&& > (Concrete::Queue_Factory<T, TRAITS>::mk ()))
            {
                _AssertRepValidType ();
                AssertNotImplemented ();
                AddAllToTail (src);
                _AssertRepValidType ();
            }
            template    <typename T, typename TRAITS>
            inline  Queue<T, TRAITS>::Queue (const _SharedPtrIRep& rep)
                : inherited (typename inherited::_SharedPtrIRep (rep))
            {
                _AssertRepValidType ();
                RequireNotNull (rep);
            }
            template    <typename T, typename TRAITS>
            template    <typename COPY_FROM_ITERATOR_OF_T>
            inline Queue<T, TRAITS>::Queue (COPY_FROM_ITERATOR_OF_T start, COPY_FROM_ITERATOR_OF_T end)
                : inherited (static_cast < const inherited&& > (Concrete::Queue_Factory<T, TRAITS>::mk ()))
            {
                _AssertRepValidType ();
                AddAllToTail (start, end);
                _AssertRepValidType ();
            }
#if     qDebug
            template    <typename T, typename TRAITS>
            Queue<T, TRAITS>::~Queue ()
            {
                if (this->_GetSharingState () != Memory::SharedByValue_State::eNull) {
                    _ConstGetRep ().AssertNoIteratorsReferenceOwner (this);
                }
            }
#endif
            template    <typename T, typename TRAITS>
            inline  const typename  Queue<T, TRAITS>::_IRep&    Queue<T, TRAITS>::_ConstGetRep () const
            {
                EnsureMember (&inherited::_ConstGetRep (), _IRep);       // use static_cast cuz more efficient, but validate with assertion
                return *static_cast<const _IRep*> (&inherited::_ConstGetRep ());
            }
            template    <typename T, typename TRAITS>
            inline  void    Queue<T, TRAITS>::AddTail (T item)
            {
                _SafeReadWriteRepAccessor<_IRep> { this } ._GetWriteableRep ().AddTail (item);
            }
            template    <typename T, typename TRAITS>
            inline  T       Queue<T, TRAITS>::Head () const
            {
                return _SafeReadRepAccessor<_IRep> { this } ._ConstGetRep ().Head ();
            }
            template    <typename T, typename TRAITS>
            inline  Memory::Optional<T>       Queue<T, TRAITS>::HeadIf () const
            {
                return _SafeReadRepAccessor<_IRep> { this } ._ConstGetRep ().HeadIf ();
            }
            template    <typename T, typename TRAITS>
            inline  T       Queue<T, TRAITS>::RemoveHead ()
            {
                return _SafeReadWriteRepAccessor<_IRep> { this } ._GetWriteableRep ().RemoveHead ();
            }
            template    <typename T, typename TRAITS>
            inline  Memory::Optional<T>       Queue<T, TRAITS>::RemoveHeadIf ()
            {
                return _SafeReadWriteRepAccessor<_IRep> { this } ._GetWriteableRep ().RemoveHeadIf ();
            }
            template    <typename T, typename TRAITS>
            inline  void    Queue<T, TRAITS>::Enqueue (T item)
            {
                _SafeReadWriteRepAccessor<_IRep> { this } ._GetWriteableRep ().AddTail (item);
            }
            template    <typename T, typename TRAITS>
            inline  T       Queue<T, TRAITS>::Dequeue ()
            {
                return _SafeReadWriteRepAccessor<_IRep> { this } ._GetWriteableRep ().RemoveHead ();
            }
            template    <typename T, typename TRAITS>
            template    <typename CONTAINER_OF_T>
            inline  void    Queue<T, TRAITS>::AddAllToTail (const CONTAINER_OF_T& s)
            {
                for (auto i : s) {
                    _SafeReadWriteRepAccessor<_IRep> { this } ._GetWriteableRep ().AddTail ( &i, &i + 1);
                }
            }
            template    <typename T, typename TRAITS>
            template    <typename COPY_FROM_ITERATOR_OF_T>
            inline void Queue<T, TRAITS>::AddAllToTail (COPY_FROM_ITERATOR_OF_T start, COPY_FROM_ITERATOR_OF_T end)
            {
                for (auto i = start; i != end; ++i) {
                    _SafeReadWriteRepAccessor<_IRep> { this } ._GetWriteableRep ().AddTail (&*i, (&*i) + 1);
                }
            }
            template    <typename T, typename TRAITS>
            inline  void    Queue<T, TRAITS>::RemoveAll ()
            {
                _SafeReadWriteRepAccessor<_IRep> { this } ._GetWriteableRep ().RemoveAll ();
            }
            template    <typename T, typename TRAITS>
            bool    Queue<T, TRAITS>::Equals (const Queue<T, TRAITS>& rhs) const
            {
                RequireConceptAppliesToTypeInFunction(Concept_EqualsCompareFunctionType, EqualsCompareFunctionType);
                return Private::Equals_<T, EqualsCompareFunctionType> (*this, rhs);
            }
            template    <typename T, typename TRAITS>
            inline  bool  Queue<T, TRAITS>::operator== (const Queue<T, TRAITS>& rhs) const
            {
                return Equals (rhs);
            }
            template    <typename T, typename TRAITS>
            inline  bool    Queue<T, TRAITS>::operator!= (const Queue<T, TRAITS>& rhs) const
            {
                return not Equals (rhs);
            }
            template    <typename T, typename TRAITS>
            inline  void    Queue<T, TRAITS>::_AssertRepValidType () const
            {
#if     qDebug
                AssertMember (&inherited::_ConstGetRep (), _IRep);
#endif
            }


        }
    }
}



#endif /* _Stroika_Foundation_Containers_Queue_inl_ */
