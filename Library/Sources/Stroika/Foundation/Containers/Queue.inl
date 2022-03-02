/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
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
    template <typename ITERABLE_OF_ADDABLE, enable_if_t<Configuration::IsIterable_v<ITERABLE_OF_ADDABLE> and not is_base_of_v<Queue<T>, decay_t<ITERABLE_OF_ADDABLE>>>*>
    inline Queue<T>::Queue (ITERABLE_OF_ADDABLE&& src)
        : Queue{}
    {
        static_assert (IsAddable_v<ExtractValueType_t<ITERABLE_OF_ADDABLE>>);
        AddAllToTail (forward<ITERABLE_OF_ADDABLE> (src));
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
    template <typename ITERATOR_OF_ADDABLE>
    inline Queue<T>::Queue (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end)
        : Queue{}
    {
        static_assert (IsAddable_v<ExtractValueType_t<ITERATOR_OF_ADDABLE>>);
        AddAllToTail (start, end);
        _AssertRepValidType ();
    }
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
    template <typename ITERABLE_OF_ADDABLE, enable_if_t<Configuration::IsIterable_v<ITERABLE_OF_ADDABLE>>*>
    inline void Queue<T>::AddAllToTail (ITERABLE_OF_ADDABLE&& s)
    {
        static_assert (IsAddable_v<ExtractValueType_t<ITERABLE_OF_ADDABLE>>);
        _SafeReadWriteRepAccessor<_IRep> tmp{this};
        for (const auto& i : s) {
            tmp._GetWriteableRep ().AddTail (i);
        }
    }
    template <typename T>
    template <typename ITERATOR_OF_ADDABLE>
    inline void Queue<T>::AddAllToTail (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end)
    {
        static_assert (IsAddable_v<ExtractValueType_t<ITERATOR_OF_ADDABLE>>);
        _SafeReadWriteRepAccessor<_IRep> tmp{this};
        for (auto i = forward<ITERATOR_OF_ADDABLE> (start); i != end; ++i) {
            tmp._GetWriteableRep ().AddTail (*i);
        }
    }
    template <typename T>
    inline void Queue<T>::RemoveAll ()
    {
        _SafeReadRepAccessor<_IRep> tmp{this}; // important to use READ not WRITE accessor, because write accessor would have already cloned the data
        if (not tmp._ConstGetRep ().empty ()) {
            this->_fRep = tmp._ConstGetRep ().CloneEmpty ();
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

}

#endif /* _Stroika_Foundation_Containers_Queue_inl_ */
