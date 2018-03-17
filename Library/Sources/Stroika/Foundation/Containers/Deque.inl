/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Deque_inl_
#define _Stroika_Foundation_Containers_Deque_inl_

#include "../Debug/Assertions.h"
#include "Factory/Deque_Factory.h"

namespace Stroika {
    namespace Foundation {
        namespace Containers {

            /*
             ********************************************************************************
             *********************************** Deque<T> ***********************************
             ********************************************************************************
             */
            template <typename T>
            inline Deque<T>::Deque ()
                : inherited (move (Factory::Deque_Factory<T> () ()))
            {
                _AssertRepValidType ();
            }
            template <typename T>
            inline Deque<T>::Deque (const initializer_list<T>& src)
                : Deque ()
            {
                this->AddAllToTail (src);
                _AssertRepValidType ();
            }
            template <typename T>
            template <typename CONTAINER_OF_T, typename ENABLE_IF>
            inline Deque<T>::Deque (const CONTAINER_OF_T& src)
                : Deque ()
            {
                this->AddAllToTail (src);
                _AssertRepValidType ();
            }
            template <typename T>
            inline Deque<T>::Deque (const _DequeRepSharedPtr& src) noexcept
                : inherited ((RequireNotNull (src), src))
            {
                _AssertRepValidType ();
            }
            template <typename T>
            inline Deque<T>::Deque (_DequeRepSharedPtr&& src) noexcept
                : inherited ((RequireNotNull (src), move (src)))
            {
                _AssertRepValidType ();
            }
            template <typename T>
            template <typename COPY_FROM_ITERATOR_OF_T>
            inline Deque<T>::Deque (COPY_FROM_ITERATOR_OF_T start, COPY_FROM_ITERATOR_OF_T end)
                : Deque ()
            {
                this->AddAllToTail (start, end);
                _AssertRepValidType ();
            }
            template <typename T>
            inline void Deque<T>::AddHead (ArgByValueType<T> item)
            {
                _SafeReadWriteRepAccessor<_IRep>{this}._GetWriteableRep ().AddHead (item);
            }
            template <typename T>
            inline T Deque<T>::RemoveTail ()
            {
                return _SafeReadWriteRepAccessor<_IRep>{this}._GetWriteableRep ().RemoveTail ();
            }
            template <typename T>
            inline T Deque<T>::Tail () const
            {
                return _SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().Tail ();
            }
            template <typename T>
            inline void Deque<T>::_AssertRepValidType () const
            {
#if qDebug
                _SafeReadRepAccessor<_IRep>{this};
#endif
            }
        }
    }
}

#endif /* _Stroika_Foundation_Containers_Deque_inl_ */
