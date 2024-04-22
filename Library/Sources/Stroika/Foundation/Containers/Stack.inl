/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
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
#if qCompilerAndStdLib_RequiresNotMatchInlineOutOfLineForTemplateClassBeingDefined_Buggy
    template <typename T>
    template <typename ITERABLE_OF_ADDABLE, enable_if_t<IIterableOf<ITERABLE_OF_ADDABLE, T> and not derived_from<remove_cvref_t<ITERABLE_OF_ADDABLE>, Stack<T>>>*>
    inline Stack<T>::Stack (ITERABLE_OF_ADDABLE&& src)
        : Stack{begin (src), end (src)}
    {
    }
#else
    template <typename T>
    template <IIterableOf<T> ITERABLE_OF_ADDABLE>
    inline Stack<T>::Stack (ITERABLE_OF_ADDABLE&& src)
        requires (not derived_from<remove_cvref_t<ITERABLE_OF_ADDABLE>, Stack<T>>)
        : Stack{begin (src), end (src)}
    {
    }
#endif
    template <typename T>
    inline Stack<T>::Stack (const shared_ptr<_IRep>& src) noexcept
        : inherited{(RequireExpression (src != nullptr), src)}
    {
        _AssertRepValidType ();
    }
    template <typename T>
    inline Stack<T>::Stack (shared_ptr<_IRep>&& src) noexcept
        : inherited{(RequireExpression (src != nullptr), move (src))}
    {
        _AssertRepValidType ();
    }
    template <typename T>
    template <IInputIterator<T> ITERATOR_OF_ADDABLE>
    inline Stack<T>::Stack (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end)
        : inherited{Factory::Stack_Factory<T>::Default () (forward<ITERATOR_OF_ADDABLE> (start), forward<ITERATOR_OF_ADDABLE> (end))}
    {
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
