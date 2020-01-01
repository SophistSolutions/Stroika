/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Stack_inl_
#define _Stroika_Foundation_Containers_Stack_inl_

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
        : inherited (Factory::Stack_Factory<T> () ())
    {
        _AssertRepValidType ();
    }
    template <typename T>
    template <typename CONTAINER_OF_ADDABLE, enable_if_t<Configuration::IsIterableOfT_v<CONTAINER_OF_ADDABLE, T> and not is_base_of_v<Stack<T>, Configuration::remove_cvref_t<CONTAINER_OF_ADDABLE>>>*>
    inline Stack<T>::Stack (CONTAINER_OF_ADDABLE&& src)
        : Stack ()
    {
        AssertNotImplemented ();
        //                AddAll (s);
    }
    template <typename T>
    inline Stack<T>::Stack (const _StackRepSharedPtr& src) noexcept
        : inherited (src)
    {
        RequireNotNull (src);
        _AssertRepValidType ();
    }
    template <typename T>
    inline Stack<T>::Stack (_StackRepSharedPtr&& src) noexcept
        : inherited ((RequireNotNull (src), move (src)))
    {
        _AssertRepValidType ();
    }
    template <typename T>
    template <typename COPY_FROM_ITERATOR_OF_ADDABLE>
    inline Stack<T>::Stack (COPY_FROM_ITERATOR_OF_ADDABLE start, COPY_FROM_ITERATOR_OF_ADDABLE end)
        : Stack ()
    {
        AssertNotImplemented ();
        //                AddAll (start, end);
    }
    template <typename T>
    inline void Stack<T>::Push (ArgByValueType<T> item)
    {
        _SafeReadWriteRepAccessor<_IRep>{this}._GetWriteableRep ().Push (item);
    }
    template <typename T>
    inline T Stack<T>::Pop ()
    {
        return _SafeReadWriteRepAccessor<_IRep>{this}._GetWriteableRep ().Pop ();
    }
    template <typename T>
    inline T Stack<T>::Top () const
    {
        return _SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().Top ();
    }
    template <typename T>
    inline void Stack<T>::RemoveAll ()
    {
        _SafeReadWriteRepAccessor<_IRep> tmp{this};
        if (not tmp._ConstGetRep ().IsEmpty ()) {
            tmp._UpdateRep (tmp._ConstGetRep ().CloneEmpty (this));
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
#if qDebug
        _SafeReadRepAccessor<_IRep>{this};
#endif
    }

    /*
     ********************************************************************************
     **************************** Stack<T>::EqualsComparer **************************
     ********************************************************************************
     */
    template <typename T>
    template <typename T_EQUALS_COMPARER>
    constexpr Stack<T>::EqualsComparer<T_EQUALS_COMPARER>::EqualsComparer (const T_EQUALS_COMPARER& elementEqualsComparer)
        : fElementComparer{elementEqualsComparer}
    {
    }
    template <typename T>
    template <typename T_EQUALS_COMPARER>
    inline bool Stack<T>::EqualsComparer<T_EQUALS_COMPARER>::operator() (const Stack& lhs, const Stack& rhs) const
    {
        return lhs.SequenceEquals (rhs, fElementComparer);
    }

    /*
     ********************************************************************************
     **************************** Stack compare operators ***************************
     ********************************************************************************
     */
    template <typename T>
    inline bool operator== (const Stack<T>& lhs, const Stack<T>& rhs)
    {
        return typename Stack<T>::EqualsComparer{}(lhs, rhs);
    }
    template <typename T>
    inline bool operator!= (const Stack<T>& lhs, const Stack<T>& rhs)
    {
        return not typename Stack<T>::EqualsComparer{}(lhs, rhs);
    }

}

#endif /* _Stroika_Foundation_Containers_Stack_inl_ */
