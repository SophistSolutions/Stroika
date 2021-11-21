/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Deque_inl_
#define _Stroika_Foundation_Containers_Deque_inl_

#include "../Debug/Assertions.h"
#include "Factory/Deque_Factory.h"

namespace Stroika::Foundation::Containers {

    /*
     ********************************************************************************
     *********************************** Deque<T> ***********************************
     ********************************************************************************
     */
    template <typename T>
    inline Deque<T>::Deque ()
        : inherited{Factory::Deque_Factory<T> () ()}
    {
        _AssertRepValidType ();
    }
    template <typename T>
    inline Deque<T>::Deque (const initializer_list<value_type>& src)
        : Deque{}
    {
        this->AddAllToTail (src);
        _AssertRepValidType ();
    }
    template <typename T>
    template <typename CONTAINER_OF_ADDABLE, enable_if_t<Configuration::IsIterableOfT_v<CONTAINER_OF_ADDABLE, T> and not is_base_of_v<Deque<T>, Configuration::remove_cvref_t<CONTAINER_OF_ADDABLE>>>*>
    inline Deque<T>::Deque (CONTAINER_OF_ADDABLE&& src)
        : Deque{}
    {
        this->AddAllToTail (forward<CONTAINER_OF_ADDABLE> (src));
        _AssertRepValidType ();
    }
    template <typename T>
    inline Deque<T>::Deque (const _IRepSharedPtr& src) noexcept
        : inherited{(RequireNotNull (src), src)}
    {
        _AssertRepValidType ();
    }
    template <typename T>
    inline Deque<T>::Deque (_IRepSharedPtr&& src) noexcept
        : inherited{(RequireNotNull (src), move (src))}
    {
        _AssertRepValidType ();
    }
    template <typename T>
    template <typename COPY_FROM_ITERATOR_OF_ADDABLE>
    inline Deque<T>::Deque (COPY_FROM_ITERATOR_OF_ADDABLE start, COPY_FROM_ITERATOR_OF_ADDABLE end)
        : Deque{}
    {
        this->AddAllToTail (start, end);
        _AssertRepValidType ();
    }
    template <typename T>
    inline void Deque<T>::AddHead (ArgByValueType<value_type> item)
    {
        _SafeReadWriteRepAccessor<_IRep>{this}._GetWriteableRep ().AddHead (item);
    }
    template <typename T>
    inline auto Deque<T>::RemoveTail () -> value_type
    {
        return _SafeReadWriteRepAccessor<_IRep>{this}._GetWriteableRep ().RemoveTail ();
    }
    template <typename T>
    inline auto Deque<T>::Tail () const -> value_type
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

#endif /* _Stroika_Foundation_Containers_Deque_inl_ */
