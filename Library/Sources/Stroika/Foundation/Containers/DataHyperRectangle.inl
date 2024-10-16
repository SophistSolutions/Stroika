/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */

#include "Stroika/Foundation/Configuration/Concepts.h"
#include "Stroika/Foundation/Containers/Private/IterableUtils.h"
#include "Stroika/Foundation/Debug/Assertions.h"

namespace Stroika::Foundation::Containers {

    /*
     ********************************************************************************
     ************************ DataHyperRectangle<T, INDEXES> ************************
     ********************************************************************************
     */
    template <typename T, typename... INDEXES>
    inline DataHyperRectangle<T, INDEXES...>::DataHyperRectangle (const DataHyperRectangle<T, INDEXES...>& src) noexcept
        : inherited{src}
    {
        _AssertRepValidType ();
    }
    template <typename T, typename... INDEXES>
    inline DataHyperRectangle<T, INDEXES...>::DataHyperRectangle (DataHyperRectangle<T, INDEXES...>&& src) noexcept
        : inherited{move (src)}
    {
        _AssertRepValidType ();
    }
    template <typename T, typename... INDEXES>
    inline DataHyperRectangle<T, INDEXES...>::DataHyperRectangle (const shared_ptr<_IRep>& src) noexcept
        : inherited{src}
    {
        RequireNotNull (src);
        _AssertRepValidType ();
    }
    template <typename T, typename... INDEXES>
    inline DataHyperRectangle<T, INDEXES...>::DataHyperRectangle (shared_ptr<_IRep>&& src) noexcept
        : inherited{(RequireExpression (src != nullptr), move (src))}
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
        if constexpr (qDebug) {
            _SafeReadRepAccessor<_IRep>{this};
        }
    }
    template <typename T, typename... INDEXES>
    constexpr bool DataHyperRectangle<T, INDEXES...>::operator== (const DataHyperRectangle& rhs) const
        requires (equality_comparable<T>)
    {
        return EqualsComparer<>{}(*this, rhs);
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
    inline bool DataHyperRectangle<T, INDEXES...>::EqualsComparer<ELEMENT_EQUALS_COMPARER>::operator() (const DataHyperRectangle& lhs,
                                                                                                        const DataHyperRectangle& rhs) const
    {
        return lhs.SequenceEquals (rhs, fElementComparer_);
    }

}
