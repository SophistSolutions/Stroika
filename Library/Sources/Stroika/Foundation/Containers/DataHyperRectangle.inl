/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_DataHyperRectangle_inl_
#define _Stroika_Foundation_Containers_DataHyperRectangle_inl_

#include "../Configuration/Concepts.h"
#include "../Debug/Assertions.h"
#include "Private/IterableUtils.h"

namespace Stroika::Foundation::Containers {

    /*
     ********************************************************************************
     ************************ DataHyperRectangle<T, INDEXES> ************************
     ********************************************************************************
     */
    template <typename T, typename... INDEXES>
    inline DataHyperRectangle<T, INDEXES...>::DataHyperRectangle (const DataHyperRectangle<T, INDEXES...>& src) noexcept
        : inherited (src)
    {
        _AssertRepValidType ();
    }
    template <typename T, typename... INDEXES>
    inline DataHyperRectangle<T, INDEXES...>::DataHyperRectangle (DataHyperRectangle<T, INDEXES...>&& src) noexcept
        : inherited (move (src))
    {
        _AssertRepValidType ();
    }
    template <typename T, typename... INDEXES>
    inline DataHyperRectangle<T, INDEXES...>::DataHyperRectangle (const _DataHyperRectangleRepSharedPtr& src) noexcept
        : inherited (src)
    {
        RequireNotNull (src);
        _AssertRepValidType ();
    }
    template <typename T, typename... INDEXES>
    inline DataHyperRectangle<T, INDEXES...>::DataHyperRectangle (_DataHyperRectangleRepSharedPtr&& src) noexcept
        : inherited ((RequireNotNull (src), move (src)))
    {
        _AssertRepValidType ();
    }
    template <typename T, typename... INDEXES>
    inline T DataHyperRectangle<T, INDEXES...>::GetAt (INDEXES... indexes) const
    {
        return _SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().GetAt (forward<INDEXES> (indexes)...);
    }
    template <typename T, typename... INDEXES>
    inline void DataHyperRectangle<T, INDEXES...>::SetAt (INDEXES... indexes, Configuration::ArgByValueType<T> v)
    {
        _SafeReadWriteRepAccessor<_IRep>{this}._GetWriteableRep ().SetAt (forward<INDEXES> (indexes)..., v);
    }
    template <typename T, typename... INDEXES>
    inline void DataHyperRectangle<T, INDEXES...>::_AssertRepValidType () const
    {
#if qDebug
        _SafeReadRepAccessor<_IRep>{this};
#endif
    }

    /*
     ********************************************************************************
     ************* DataHyperRectangle<T, INDEXES...>::EqualsComparer ****************
     ********************************************************************************
     */
    template <typename T, typename... INDEXES>
    template <typename ELEMENT_EQUALS_COMPARER>
    constexpr DataHyperRectangle<T, INDEXES...>::EqualsComparer<ELEMENT_EQUALS_COMPARER>::EqualsComparer (const ELEMENT_EQUALS_COMPARER& elementComparer)
        : fElementComparer_{elementComparer}
    {
    }
    template <typename T, typename... INDEXES>
    template <typename ELEMENT_EQUALS_COMPARER>
    inline bool DataHyperRectangle<T, INDEXES...>::EqualsComparer<ELEMENT_EQUALS_COMPARER>::operator() (const DataHyperRectangle& lhs, const DataHyperRectangle& rhs) const
    {
        return lhs.SequenceEquals (rhs, fElementComparer_);
    }

    /*
     ********************************************************************************
     *************** DataHyperRectangle comparison operators ************************
     ********************************************************************************
     */
    template <typename T, typename... INDEXES>
    inline bool operator== (const DataHyperRectangle<T, INDEXES...>& lhs, const DataHyperRectangle<T, INDEXES...>& rhs)
    {
        return typename DataHyperRectangle<T, INDEXES...>::EqualsComparer{}(lhs, rhs);
    }
    template <typename T, typename... INDEXES>
    inline bool operator!= (const DataHyperRectangle<T, INDEXES...>& lhs, const DataHyperRectangle<T, INDEXES...>& rhs)
    {
        return not typename DataHyperRectangle<T, INDEXES...>::EqualsComparer{}(lhs, rhs);
    }

}

#endif /* _Stroika_Foundation_Containers_DataHyperRectangle_inl_ */
