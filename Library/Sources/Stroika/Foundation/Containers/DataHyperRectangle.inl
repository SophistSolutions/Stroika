/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_DataHyperRectangle_inl_
#define _Stroika_Foundation_Containers_DataHyperRectangle_inl_

#include "../Configuration/Concepts.h"
#include "../Debug/Assertions.h"
#include "Private/IterableUtils.h"

namespace Stroika {
    namespace Foundation {
        namespace Containers {

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
                return _SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().GetAt (std::forward<INDEXES> (indexes)...);
            }
            template <typename T, typename... INDEXES>
            inline void DataHyperRectangle<T, INDEXES...>::SetAt (INDEXES... indexes, Configuration::ArgByValueType<T> v)
            {
                _SafeReadWriteRepAccessor<_IRep>{this}._GetWriteableRep ().SetAt (std::forward<INDEXES> (indexes)..., v);
            }
            template <typename T, typename... INDEXES>
            template <typename EQUALS_COMPARER>
            bool DataHyperRectangle<T, INDEXES...>::Equals (const DataHyperRectangle& rhs, const EQUALS_COMPARER& equalsComparer) const
            {
                return Private::Equals__<T, EQUALS_COMPARER> (*this, rhs, equalsComparer);
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
             ********************* DataHyperRectangle operators *****************************
             ********************************************************************************
             */
            template <typename T, typename... INDEXES>
            inline bool operator== (const DataHyperRectangle<T, INDEXES...>& lhs, const DataHyperRectangle<T, INDEXES...>& rhs)
            {
                return lhs.Equals (rhs);
            }
            template <typename T, typename... INDEXES>
            inline bool operator!= (const DataHyperRectangle<T, INDEXES...>& lhs, const DataHyperRectangle<T, INDEXES...>& rhs)
            {
                return not lhs.Equals (rhs);
            }
        }
    }
}

#endif /* _Stroika_Foundation_Containers_DataHyperRectangle_inl_ */
