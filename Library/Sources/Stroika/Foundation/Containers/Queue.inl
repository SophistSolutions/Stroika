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
                AssertMember (&inherited::_ConstGetRep (), _IRep);
            }
            template    <typename T, typename TRAITS>
            inline  Queue<T, TRAITS>::Queue (const Queue<T, TRAITS>& src)
                : inherited (static_cast<const inherited&> (src))
            {
                AssertMember (&inherited::_ConstGetRep (), _IRep);
            }
            template    <typename T, typename TRAITS>
            inline  Queue<T, TRAITS>::Queue (const std::initializer_list<T>& q)
                : inherited (static_cast < const inherited&& > (Concrete::Queue_Factory<T, TRAITS>::mk ()))
            {
                AssertMember (&inherited::_ConstGetRep (), _IRep);
                AddAllToTail (q);
            }
            template    <typename T, typename TRAITS>
            template    <typename CONTAINER_OF_T>
            inline  Queue<T, TRAITS>::Queue (const CONTAINER_OF_T& src)
                : inherited (static_cast < const inherited&& > (Concrete::Queue_Factory<T, TRAITS>::mk ()))
            {
                AssertMember (&inherited::_ConstGetRep (), _IRep);
                AssertNotImplemented ();
                AddAllToTail (src);
            }
            template    <typename T, typename TRAITS>
            inline  Queue<T, TRAITS>::Queue (const _SharedPtrIRep& rep)
                : inherited (typename inherited::_SharedPtrIRep (rep))
            {
                AssertMember (&inherited::_ConstGetRep (), _IRep);
                RequireNotNull (rep);
            }
            template    <typename T, typename TRAITS>
            template    <typename COPY_FROM_ITERATOR_OF_T>
            inline Queue<T, TRAITS>::Queue (COPY_FROM_ITERATOR_OF_T start, COPY_FROM_ITERATOR_OF_T end)
                : inherited (static_cast < const inherited&& > (Concrete::Queue_Factory<T, TRAITS>::mk ()))
            {
                AssertMember (&inherited::_ConstGetRep (), _IRep);
                AddAllToTail (start, end);
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
            inline  const typename  Queue<T, TRAITS>::_IRep&    Queue<T, TRAITS>::_GetRep () const
            {
                EnsureMember (&inherited::_GetRep (), _IRep);       // use static_cast cuz more efficient, but validate with assertion
                return *static_cast<const _IRep*> (&inherited::_GetRep ());
            }
            template    <typename T, typename TRAITS>
            inline  typename    Queue<T, TRAITS>::_IRep&  Queue<T, TRAITS>::_GetRep ()
            {
                EnsureMember (&inherited::_GetRep (), _IRep);       // use static_cast cuz more efficient, but validate with assertion
                return *static_cast<_IRep*> (&inherited::_GetRep ());
            }
            template    <typename T, typename TRAITS>
            inline  void    Queue<T, TRAITS>::AddTail (T item)
            {
                _GetRep ().AddTail (item);
            }
            template    <typename T, typename TRAITS>
            inline  T       Queue<T, TRAITS>::Head () const
            {
                return _ConstGetRep ().Head ();
            }
            template    <typename T, typename TRAITS>
            inline  Memory::Optional<T>       Queue<T, TRAITS>::HeadIf () const
            {
                return _ConstGetRep ().HeadIf ();
            }
            template    <typename T, typename TRAITS>
            inline  T       Queue<T, TRAITS>::RemoveHead ()
            {
                return _GetRep ().RemoveHead ();
            }
            template    <typename T, typename TRAITS>
            inline  Memory::Optional<T>       Queue<T, TRAITS>::RemoveHeadIf ()
            {
                return _GetRep ().RemoveHeadIf ();
            }
            template    <typename T, typename TRAITS>
            inline  void    Queue<T, TRAITS>::Enqueue (T item)
            {
                _GetRep ().AddTail (item);
            }
            template    <typename T, typename TRAITS>
            inline  T       Queue<T, TRAITS>::Dequeue ()
            {
                return _GetRep ().RemoveHead ();
            }
            template    <typename T, typename TRAITS>
            template    <typename CONTAINER_OF_T>
            inline  void    Queue<T, TRAITS>::AddAllToTail (const CONTAINER_OF_T& s)
            {
                for (auto i : s) {
                    _GetRep ().AddTail ( &i, &i + 1);
                }
            }
            template    <typename T, typename TRAITS>
            template    <typename COPY_FROM_ITERATOR_OF_T>
            inline void Queue<T, TRAITS>::AddAllToTail (COPY_FROM_ITERATOR_OF_T start, COPY_FROM_ITERATOR_OF_T end)
            {
                for (auto i = start; i != end; ++i) {
                    _GetRep ().AddTail (&*i, (&*i) + 1);
                }
            }
            template    <typename T, typename TRAITS>
            inline  void    Queue<T, TRAITS>::RemoveAll ()
            {
                _GetRep ().RemoveAll ();
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


            /*
             ********************************************************************************
             ************************ Queue<T, TRAITS>::_IRep *******************************
             ********************************************************************************
             */
            template    <typename T, typename TRAITS>
            inline  Queue<T, TRAITS>::_IRep::_IRep ()
            {
            }
            template    <typename T, typename TRAITS>
            inline  Queue<T, TRAITS>::_IRep::~_IRep ()
            {
            }


        }
    }
}



#endif /* _Stroika_Foundation_Containers_Queue_inl_ */
