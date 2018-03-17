/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Queue_inl_
#define _Stroika_Foundation_Containers_Queue_inl_

#include "../Configuration/Concepts.h"
#include "../Debug/Assertions.h"
#include "Factory/Queue_Factory.h"
#include "Private/IterableUtils.h"

namespace Stroika {
    namespace Foundation {
        namespace Containers {

            /*
             ********************************************************************************
             ************************************ Queue<T> **********************************
             ********************************************************************************
             */
            template <typename T>
            inline Queue<T>::Queue ()
                : inherited (move (Factory::Queue_Factory<T> () ()))
            {
                _AssertRepValidType ();
            }
            template <typename T>
            inline Queue<T>::Queue (const initializer_list<T>& src)
                : Queue ()
            {
                AddAllToTail (src);
                _AssertRepValidType ();
            }
            template <typename T>
            template <typename CONTAINER_OF_T, typename ENABLE_IF>
            inline Queue<T>::Queue (const CONTAINER_OF_T& src)
                : Queue ()
            {
                AssertNotImplemented ();
                AddAllToTail (src);
                _AssertRepValidType ();
            }
            template <typename T>
            inline Queue<T>::Queue (const _QueueRepSharedPtr& rep) noexcept
                : inherited ((RequireNotNull (rep), rep))
            {
                _AssertRepValidType ();
                RequireNotNull (rep);
            }
            template <typename T>
            inline Queue<T>::Queue (_QueueRepSharedPtr&& rep) noexcept
                : inherited ((RequireNotNull (rep), move (rep)))
            {
                _AssertRepValidType ();
            }
            template <typename T>
            template <typename COPY_FROM_ITERATOR_OF_T>
            inline Queue<T>::Queue (COPY_FROM_ITERATOR_OF_T start, COPY_FROM_ITERATOR_OF_T end)
                : Queue ()
            {
                AddAllToTail (start, end);
                _AssertRepValidType ();
            }
#if qDebug
            template <typename T>
            Queue<T>::~Queue ()
            {
                if (this->_GetSharingState () != Memory::SharedByValue_State::eNull) {
                    // SharingState can be NULL because of MOVE semantics
                    _SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().AssertNoIteratorsReferenceOwner (this);
                }
            }
#endif
            template <typename T>
            inline void Queue<T>::AddTail (ArgByValueType<T> item)
            {
                _SafeReadWriteRepAccessor<_IRep>{this}._GetWriteableRep ().AddTail (item);
            }
            template <typename T>
            inline T Queue<T>::Head () const
            {
                return _SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().Head ();
            }
            template <typename T>
            inline Memory::Optional<T> Queue<T>::HeadIf () const
            {
                return _SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().HeadIf ();
            }
            template <typename T>
            inline T Queue<T>::RemoveHead ()
            {
                return _SafeReadWriteRepAccessor<_IRep>{this}._GetWriteableRep ().RemoveHead ();
            }
            template <typename T>
            inline Memory::Optional<T> Queue<T>::RemoveHeadIf ()
            {
                return _SafeReadWriteRepAccessor<_IRep>{this}._GetWriteableRep ().RemoveHeadIf ();
            }
            template <typename T>
            inline void Queue<T>::Enqueue (ArgByValueType<T> item)
            {
                _SafeReadWriteRepAccessor<_IRep>{this}._GetWriteableRep ().AddTail (item);
            }
            template <typename T>
            inline T Queue<T>::Dequeue ()
            {
                return _SafeReadWriteRepAccessor<_IRep>{this}._GetWriteableRep ().RemoveHead ();
            }
            template <typename T>
            template <typename CONTAINER_OF_T, typename ENABLE_IF>
            inline void Queue<T>::AddAllToTail (const CONTAINER_OF_T& s)
            {
                _SafeReadWriteRepAccessor<_IRep> tmp{this};
                for (auto i : s) {
                    tmp._GetWriteableRep ().AddTail (i);
                }
            }
            template <typename T>
            template <typename COPY_FROM_ITERATOR_OF_T>
            inline void Queue<T>::AddAllToTail (COPY_FROM_ITERATOR_OF_T start, COPY_FROM_ITERATOR_OF_T end)
            {
                _SafeReadWriteRepAccessor<_IRep> tmp{this};
                for (auto i = start; i != end; ++i) {
                    tmp._GetWriteableRep ().AddTail (*i);
                }
            }
            template <typename T>
            inline void Queue<T>::RemoveAll ()
            {
                _SafeReadWriteRepAccessor<_IRep> tmp{this};
                if (not tmp._ConstGetRep ().IsEmpty ()) {
                    tmp._UpdateRep (tmp._ConstGetRep ().CloneEmpty (this));
                }
            }
            template <typename T>
            inline void Queue<T>::clear ()
            {
                RemoveAll ();
            }
            template <typename T>
            template <typename EQUALS_COMPARER>
            bool Queue<T>::Equals (const Queue<T>& rhs) const
            {
                return Private::Equals_<T, EQUALS_COMPARER> (*this, rhs);
            }
            template <typename T>
            inline void Queue<T>::_AssertRepValidType () const noexcept
            {
#if qDebug
                _SafeReadRepAccessor<_IRep>{this};
#endif
            }

            /*
             ********************************************************************************
             ***************************** Queue operators **********************************
             ********************************************************************************
             */
            template <typename T>
            inline bool operator== (const Queue<T>& lhs, const Queue<T>& rhs)
            {
                return lhs.Equals (rhs);
            }
            template <typename T>
            inline bool operator!= (const Queue<T>& lhs, const Queue<T>& rhs)
            {
                return not lhs.Equals (rhs);
            }
        }
    }
}

#endif /* _Stroika_Foundation_Containers_Queue_inl_ */
