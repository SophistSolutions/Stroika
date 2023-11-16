/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
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
        : inherited{Factory::Queue_Factory<T>::Default () ()}
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
#if !qCompilerAndStdLib_RequiresNotMatchInlineOutOfLineForTemplateClassBeingDefined_Buggy
    template <typename T>
    template <IIterableOf<T> ITERABLE_OF_ADDABLE>
        requires (not derived_from<remove_cvref_t<ITERABLE_OF_ADDABLE>, Queue<T>>)
    inline Queue<T>::Queue (ITERABLE_OF_ADDABLE&& src)
        : Queue{}
    {
        AddAllToTail (forward<ITERABLE_OF_ADDABLE> (src));
        _AssertRepValidType ();
    }
#endif
    template <typename T>
    inline Queue<T>::Queue (const shared_ptr<_IRep>& rep) noexcept
        : inherited{(RequireExpression (rep != nullptr), rep)}
    {
        _AssertRepValidType ();
        RequireNotNull (rep);
    }
    template <typename T>
    inline Queue<T>::Queue (shared_ptr<_IRep>&& rep) noexcept
        : inherited{(RequireExpression (rep != nullptr), move (rep))}
    {
        _AssertRepValidType ();
    }
    template <typename T>
    template <IInputIterator<T> ITERATOR_OF_ADDABLE>
    inline Queue<T>::Queue (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end)
        : Queue{}
    {
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
    template <IIterableOf<T> ITERABLE_OF_ADDABLE>
    inline void Queue<T>::AddAllToTail (ITERABLE_OF_ADDABLE&& s)
    {
        _SafeReadWriteRepAccessor<_IRep> tmp{this};
        for (const auto& i : s) {
            tmp._GetWriteableRep ().AddTail (i);
        }
    }
    template <typename T>
    template <IInputIterator<T> ITERATOR_OF_ADDABLE>
    inline void Queue<T>::AddAllToTail (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end)
    {
        _SafeReadWriteRepAccessor<_IRep> tmp{this};
        for (auto i = forward<ITERATOR_OF_ADDABLE> (start); i != forward<ITERATOR_OF_ADDABLE> (end); ++i) {
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
        if constexpr (qDebug) {
            _SafeReadRepAccessor<_IRep>{this};
        }
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
