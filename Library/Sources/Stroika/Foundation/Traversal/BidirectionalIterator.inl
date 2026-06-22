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
    inline bool BidirectionalIterator<T, ITERATOR_TRAITS>::AtStart () const
    {
        return GetRep ().AtStart ();
    }
    template <typename T, typename ITERATOR_TRAITS>
    inline auto BidirectionalIterator<T, ITERATOR_TRAITS>::operator-- () -> BidirectionalIterator&
    {
        Require (not this->AtStart ());
        this->_fCurrentValue = GetRep ().Back ();
        return *this;
    }
    template <typename T, typename ITERATOR_TRAITS>
    inline auto BidirectionalIterator<T, ITERATOR_TRAITS>::operator-- (int) -> BidirectionalIterator
    {
        BidirectionalIterator result = *this;
        --result;
        return result;
    }
    template <typename T, typename ITERATOR_TRAITS>
    inline auto BidirectionalIterator<T, ITERATOR_TRAITS>::operator- (ptrdiff_t i) const -> BidirectionalIterator
    {
        Require (i >= 0);
        BidirectionalIterator result{*this};
        for (ptrdiff_t j = 0; j < i; ++j) {
            --result;
        }
        return result;
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

}
