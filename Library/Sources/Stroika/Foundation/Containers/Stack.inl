/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Stack_inl_
#define _Stroika_Foundation_Containers_Stack_inl_

#include <vector>

#include "../Configuration/Concepts.h"
#include "../Debug/Assertions.h"
#include "Factory/Stack_Factory.h"
#include "Private/IterableUtils.h"

namespace Stroika::Foundation::Containers {

    /*
     ********************************************************************************
     ************************************* Stack<T> *********************************
     ********************************************************************************
     */
    template <typename T>
    inline Stack<T>::Stack ()
        : inherited{Factory::Stack_Factory<T>::Default () ()}
    {
        _AssertRepValidType ();
    }
#if !qCompilerAndStdLib_RequiresNotMatchInlineOutOfLineForTemplateClassBeingDefined_Buggy
    template <typename T>
    template <IIterableOf<T> ITERABLE_OF_ADDABLE>
    inline Stack<T>::Stack (ITERABLE_OF_ADDABLE&& src)
        requires (not derived_from<remove_cvref_t<ITERABLE_OF_ADDABLE>, Stack<T>>)
        : Stack{}
    {
        // sadly intrinsically expensive to copy an Iterable using the stack API
        // @todo find a more efficient way - for example - if there is a way to get a reverse-iterator from 'src' this can be much cheaper!
        vector<T> tmp;
        for (const auto& si : src) {
            tmp.push_back (si);
        }
        for (const auto& si : tmp) {
            Push (si);
        }
    }
#endif
    template <typename T>
    inline Stack<T>::Stack (const shared_ptr<_IRep>& src) noexcept
        : inherited{(RequireNotNull (src), src)}
    {
        _AssertRepValidType ();
    }
    template <typename T>
    inline Stack<T>::Stack (shared_ptr<_IRep>&& src) noexcept
        : inherited{(RequireNotNull (src), move (src))}
    {
        _AssertRepValidType ();
    }
    template <typename T>
    template <IInputIterator<T> ITERATOR_OF_ADDABLE>
    inline Stack<T>::Stack (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end)
        : Stack{}
    {
        // sadly intrinsically expensive to copy an Iterable using the stack API
        // @todo find a more efficient way - for example - if there is a way to get a reverse-iterator from 'src' this can be much cheaper!
        vector<T> tmp;
        for (auto i = forward<ITERATOR_OF_ADDABLE> (start); i != end; ++i) {
            tmp.push_back (*i);
        }
        for (const auto& si : tmp) {
            Push (si);
        }
    }
    template <typename T>
    inline void Stack<T>::Push (ArgByValueType<value_type> item)
    {
        _SafeReadWriteRepAccessor<_IRep>{this}._GetWriteableRep ().Push (item);
    }
    template <typename T>
    inline auto Stack<T>::Pop () -> value_type
    {
        return _SafeReadWriteRepAccessor<_IRep>{this}._GetWriteableRep ().Pop ();
    }
    template <typename T>
    inline auto Stack<T>::Top () const -> value_type
    {
        return _SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().Top ();
    }
    template <typename T>
    inline void Stack<T>::RemoveAll ()
    {
        _SafeReadRepAccessor<_IRep> tmp{this}; // important to use READ not WRITE accessor, because write accessor would have already cloned the data
        if (not tmp._ConstGetRep ().empty ()) {
            this->_fRep = tmp._ConstGetRep ().CloneEmpty ();
        }
    }
    template <typename T>
    inline void Stack<T>::clear ()
    {
        RemoveAll ();
    }
    template <typename T>
    inline void Stack<T>::_AssertRepValidType () const
    {
        if constexpr (qDebug) {
            _SafeReadRepAccessor<_IRep> ignored{this};
        }
    }
    template <typename T>
    inline bool Stack<T>::operator== (const Stack& rhs) const
    {
        return EqualsComparer<>{}(*this, rhs);
    }
    template <typename T>
    inline auto Stack<T>::operator<=> (const Stack& rhs) const
    {
        return ThreeWayComparer<>{}(*this, rhs);
    }

}

#endif /* _Stroika_Foundation_Containers_Stack_inl_ */
