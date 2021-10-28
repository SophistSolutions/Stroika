/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Queue_inl_
#define _Stroika_Foundation_Containers_Queue_inl_

#include "../Configuration/Concepts.h"
#include "../Debug/Assertions.h"
#include "Factory/Queue_Factory.h"
#include "Private/IterableUtils.h"

namespace Stroika::Foundation::Containers {

    /*
     ********************************************************************************
     ************************************ Queue<T> **********************************
     ********************************************************************************
     */
    template <typename T>
    inline Queue<T>::Queue ()
        : inherited{Factory::Queue_Factory<T>{}()}
    {
        _AssertRepValidType ();
    }
    template <typename T>
    inline Queue<T>::Queue (const initializer_list<value_type>& src)
        : Queue{}
    {
        AddAllToTail (src);
        _AssertRepValidType ();
    }
    template <typename T>
    template <typename CONTAINER_OF_ADDABLE, enable_if_t<Configuration::IsIterableOfT_v<CONTAINER_OF_ADDABLE, T> and not is_base_of_v<Queue<T>, Configuration::remove_cvref_t<CONTAINER_OF_ADDABLE>>>*>
    inline Queue<T>::Queue (CONTAINER_OF_ADDABLE&& src)
        : Queue{}
    {
        AssertNotImplemented ();
        AddAllToTail (forward<CONTAINER_OF_ADDABLE> (src));
        _AssertRepValidType ();
    }
    template <typename T>
    inline Queue<T>::Queue (const _IRepSharedPtr& rep) noexcept
        : inherited{(RequireNotNull (rep), rep)}
    {
        _AssertRepValidType ();
        RequireNotNull (rep);
    }
    template <typename T>
    inline Queue<T>::Queue (_IRepSharedPtr&& rep) noexcept
        : inherited{(RequireNotNull (rep), move (rep))}
    {
        _AssertRepValidType ();
    }
    template <typename T>
    template <typename COPY_FROM_ITERATOR_OF_ADDABLE>
    inline Queue<T>::Queue (COPY_FROM_ITERATOR_OF_ADDABLE start, COPY_FROM_ITERATOR_OF_ADDABLE end)
        : Queue{}
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
    inline void Queue<T>::AddTail (ArgByValueType<value_type> item)
    {
        _SafeReadWriteRepAccessor<_IRep>{this}._GetWriteableRep ().AddTail (item);
    }
    template <typename T>
    inline auto Queue<T>::Head () const -> value_type
    {
        return _SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().Head ();
    }
    template <typename T>
    inline auto Queue<T>::HeadIf () const -> optional<value_type>
    {
        return _SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().HeadIf ();
    }
    template <typename T>
    inline auto Queue<T>::RemoveHead () -> value_type
    {
        Require (not this->empty ());
        return _SafeReadWriteRepAccessor<_IRep>{this}._GetWriteableRep ().RemoveHead ();
    }
    template <typename T>
    inline auto Queue<T>::RemoveHeadIf () -> optional<value_type>
    {
        return _SafeReadWriteRepAccessor<_IRep>{this}._GetWriteableRep ().RemoveHeadIf ();
    }
    template <typename T>
    inline void Queue<T>::Enqueue (ArgByValueType<value_type> item)
    {
        _SafeReadWriteRepAccessor<_IRep>{this}._GetWriteableRep ().AddTail (item);
    }
    template <typename T>
    inline auto Queue<T>::Dequeue () -> value_type
    {
        Require (not this->empty ());
        return _SafeReadWriteRepAccessor<_IRep>{this}._GetWriteableRep ().RemoveHead ();
    }
    template <typename T>
    template <typename CONTAINER_OF_ADDABLE, enable_if_t<Configuration::IsIterable_v<CONTAINER_OF_ADDABLE>>*>
    inline void Queue<T>::AddAllToTail (CONTAINER_OF_ADDABLE&& s)
    {
        _SafeReadWriteRepAccessor<_IRep> tmp{this};
        for (auto i : s) {
            tmp._GetWriteableRep ().AddTail (i);
        }
    }
    template <typename T>
    template <typename COPY_FROM_ITERATOR_OF_ADDABLE>
    inline void Queue<T>::AddAllToTail (COPY_FROM_ITERATOR_OF_ADDABLE start, COPY_FROM_ITERATOR_OF_ADDABLE end)
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
    inline void Queue<T>::_AssertRepValidType () const noexcept
    {
#if qDebug
        _SafeReadRepAccessor<_IRep>{this};
#endif
    }
#if __cpp_impl_three_way_comparison >= 201907
    template <typename T>
    inline bool Queue<T>::operator== (const Queue& rhs) const
    {
        return EqualsComparer<>{}(*this, rhs);
    }
    template <typename T>
    inline auto Queue<T>::operator<=> (const Queue& rhs) const
    {
        return ThreeWayComparer<>{}(*this, rhs);
    }
#endif

#if __cpp_impl_three_way_comparison < 201907
    /*
     ********************************************************************************
     ***************************** Queue operators **********************************
     ********************************************************************************
     */
    template <typename T>
    inline bool operator== (const Queue<T>& lhs, const Queue<T>& rhs)
    {
        return typename Queue<T>::EqualsComparer{}(lhs, rhs);
    }
    template <typename T>
    inline bool operator!= (const Queue<T>& lhs, const Queue<T>& rhs)
    {
        return not typename Queue<T>::EqualsComparer{}(lhs, rhs);
    }
#endif

}

#endif /* _Stroika_Foundation_Containers_Queue_inl_ */
