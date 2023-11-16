/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
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
        : inherited{Factory::Deque_Factory<T>::Default () ()}
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
#if !qCompilerAndStdLib_RequiresNotMatchInlineOutOfLineForTemplateClassBeingDefined_Buggy
    template <typename T>
    template <IIterableOf<T> ITERABLE_OF_ADDABLE>
    inline Deque<T>::Deque (ITERABLE_OF_ADDABLE&& src)
        requires (not derived_from<remove_cvref_t<ITERABLE_OF_ADDABLE>, Deque<T>>)
        : Deque{}
    {
        this->AddAllToTail (forward<ITERABLE_OF_ADDABLE> (src));
        _AssertRepValidType ();
    }
#endif
    template <typename T>
    inline Deque<T>::Deque (const shared_ptr<_IRep>& src) noexcept
        : inherited{(RequireExpression (src != nullptr), src)}
    {
        _AssertRepValidType ();
    }
    template <typename T>
    inline Deque<T>::Deque (shared_ptr<_IRep>&& src) noexcept
        : inherited{(RequireExpression (src != nullptr), move (src))}
    {
        _AssertRepValidType ();
    }
    template <typename T>
    template <IInputIterator<T> ITERATOR_OF_ADDABLE>
    inline Deque<T>::Deque (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end)
        : Deque{}
    {
        this->AddAllToTail (forward<ITERATOR_OF_ADDABLE> (start), forward<ITERATOR_OF_ADDABLE> (end));
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
        if constexpr (qDebug) {
            _SafeReadRepAccessor<_IRep>{this};
        }
    }

}

#endif /* _Stroika_Foundation_Containers_Deque_inl_ */
