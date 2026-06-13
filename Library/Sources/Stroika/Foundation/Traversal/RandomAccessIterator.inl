/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
namespace Stroika::Foundation::Traversal {

    /*
     ********************************************************************************
     ****************** RandomAccessIterator<T, ITERATOR_TRAITS> *******************
     ********************************************************************************
     */
    template <typename T, typename ITERATOR_TRAITS>
    inline RandomAccessIterator<T, ITERATOR_TRAITS>::RandomAccessIterator (const unique_ptr<IRep>& rep) noexcept
        : inherited{rep}
    {
    }
    template <typename T, typename ITERATOR_TRAITS>
    inline RandomAccessIterator<T, ITERATOR_TRAITS>::RandomAccessIterator (unique_ptr<IRep>&& rep) noexcept
        : inherited{move (rep)}
    {
    }
    template <typename T, typename ITERATOR_TRAITS>
    constexpr RandomAccessIterator<T, ITERATOR_TRAITS>::RandomAccessIterator (const default_sentinel_t&) noexcept
        : inherited{default_sentinel}
    {
    }
    template <typename T, typename ITERATOR_TRAITS>
    constexpr RandomAccessIterator<T, ITERATOR_TRAITS>::RandomAccessIterator (nullptr_t) noexcept
        : inherited{nullptr}
    {
    }
    template <typename T, typename ITERATOR_TRAITS>
    constexpr RandomAccessIterator<T, ITERATOR_TRAITS>::RandomAccessIterator () noexcept
        : inherited{}
    {
    }
    template <typename T, typename ITERATOR_TRAITS>
    inline bool RandomAccessIterator<T, ITERATOR_TRAITS>::AtStart () const
    {
        return GetRep ().AtStart ();
    }
    template <typename T, typename ITERATOR_TRAITS>
    inline typename RandomAccessIterator<T, ITERATOR_TRAITS>::IRep& RandomAccessIterator<T, ITERATOR_TRAITS>::GetRep ()
    {
        return Debug::UncheckedDynamicCast<IRep&> (inherited::GetRep ());
    }
    template <typename T, typename ITERATOR_TRAITS>
    inline const typename RandomAccessIterator<T, ITERATOR_TRAITS>::IRep& RandomAccessIterator<T, ITERATOR_TRAITS>::ConstGetRep () const
    {
        return Debug::UncheckedDynamicCast<const IRep&> (inherited::ConstGetRep ());
    }

}
