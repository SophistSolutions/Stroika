/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include "Stroika/Foundation/Debug/Assertions.h"

namespace Stroika::Foundation::Traversal {

    /*
     ********************************************************************************
     ****************** BidirectionalIterator<T, ITERATOR_TRAITS> *******************
     ********************************************************************************
     */
    template <typename T, typename ITERATOR_TRAITS>
    inline BidirectionalIterator<T, ITERATOR_TRAITS>::BidirectionalIterator (const unique_ptr<IRep>& rep) noexcept
        : inherited{rep}
    {
    }
    template <typename T, typename ITERATOR_TRAITS>
    inline BidirectionalIterator<T, ITERATOR_TRAITS>::BidirectionalIterator (unique_ptr<IRep>&& rep) noexcept
        : inherited{move (rep)}
    {
    }
    template <typename T, typename ITERATOR_TRAITS>
    constexpr BidirectionalIterator<T, ITERATOR_TRAITS>::BidirectionalIterator (const default_sentinel_t&) noexcept
        : inherited{default_sentinel}
    {
    }
    template <typename T, typename ITERATOR_TRAITS>
    constexpr BidirectionalIterator<T, ITERATOR_TRAITS>::BidirectionalIterator (nullptr_t) noexcept
        : inherited{nullptr}
    {
    }
    template <typename T, typename ITERATOR_TRAITS>
    constexpr BidirectionalIterator<T, ITERATOR_TRAITS>::BidirectionalIterator () noexcept
        : inherited{}
    {
    }
    template <typename T, typename ITERATOR_TRAITS>
    inline typename BidirectionalIterator<T, ITERATOR_TRAITS>::IRep& BidirectionalIterator<T, ITERATOR_TRAITS>::GetRep ()
    {
        return Debug::UncheckedDynamicCast<IRep&> (inherited::GetRep ());
    }
    template <typename T, typename ITERATOR_TRAITS>
    inline const typename BidirectionalIterator<T, ITERATOR_TRAITS>::IRep& BidirectionalIterator<T, ITERATOR_TRAITS>::ConstGetRep () const
    {
        return Debug::UncheckedDynamicCast<const IRep&> (inherited::ConstGetRep ());
    }
    template <typename T, typename ITERATOR_TRAITS>
    inline auto BidirectionalIterator<T, ITERATOR_TRAITS>::operator-- () -> BidirectionalIterator&
    {
        GetRep ().Back (&this->_fCurrentValue);
        return *this;
    }

}
