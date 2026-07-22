/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
namespace Stroika::Foundation::Traversal {

    /*
     ********************************************************************************
     ****************** RandomAccessIterator<T, ITERATOR_TRAITS> ********************
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
    inline void RandomAccessIterator<T, ITERATOR_TRAITS>::Advance (difference_type i)
    {
        GetRep ().Advance (i);
    }
    template <typename T, typename ITERATOR_TRAITS>
    auto RandomAccessIterator<T, ITERATOR_TRAITS>::Difference (const RandomAccessIterator& rhs) const -> difference_type
    {
        if (this->AtEnd ()) {
            if (rhs.AtEnd ()) {
                return 0;
            }
            else {
                return -rhs.Difference (*this);
            }
        }
        else {
            if (rhs.AtEnd ()) {
                return ConstGetRep ().Difference (nullptr);
            }
            else {
                return ConstGetRep ().Difference (&rhs.ConstGetRep ());
            }
        }
    }
    template <typename T, typename ITERATOR_TRAITS>
    inline auto RandomAccessIterator<T, ITERATOR_TRAITS>::operator++ () -> RandomAccessIterator&
    {
        inherited::operator++ ();
        return *this;
    }
    template <typename T, typename ITERATOR_TRAITS>
    inline auto RandomAccessIterator<T, ITERATOR_TRAITS>::operator++ (int) -> RandomAccessIterator
    {
        RandomAccessIterator result{*this};
        inherited::operator++ ();
        return result;
    }
    template <typename T, typename ITERATOR_TRAITS>
    inline auto RandomAccessIterator<T, ITERATOR_TRAITS>::operator-- () -> RandomAccessIterator&
    {
        inherited::operator-- ();
        return *this;
    }
    template <typename T, typename ITERATOR_TRAITS>
    inline auto RandomAccessIterator<T, ITERATOR_TRAITS>::operator-- (int) -> RandomAccessIterator
    {
        auto result = *this;
        inherited::operator-- ();
        return result;
    }
    template <typename T, typename ITERATOR_TRAITS>
    inline auto RandomAccessIterator<T, ITERATOR_TRAITS>::operator+ (difference_type i) const -> RandomAccessIterator
    {
        RandomAccessIterator result{*this};
        result.Advance (i);
        return result;
    }
    template <typename T, typename ITERATOR_TRAITS>
    inline auto RandomAccessIterator<T, ITERATOR_TRAITS>::operator- (difference_type i) const -> RandomAccessIterator
    {
        RandomAccessIterator result{*this};
        result.Advance (-i);
        return result;
    }
    template <typename T, typename ITERATOR_TRAITS>
    inline auto RandomAccessIterator<T, ITERATOR_TRAITS>::operator+= (difference_type i) -> RandomAccessIterator&
    {
        GetRep ().Advance (i);
        return *this;
    }
    template <typename T, typename ITERATOR_TRAITS>
    inline auto RandomAccessIterator<T, ITERATOR_TRAITS>::operator-= (difference_type i) -> RandomAccessIterator&
    {
        GetRep ().Advance (-i);
        return *this;
    }
    template <typename T, typename ITERATOR_TRAITS>
    inline const T& RandomAccessIterator<T, ITERATOR_TRAITS>::operator[] (difference_type i) const
    {
        return *GetRep ().PeekAtElement (i);
    }
    template <typename T, typename ITERATOR_TRAITS>
    inline strong_ordering RandomAccessIterator<T, ITERATOR_TRAITS>::operator<=> (const RandomAccessIterator& rhs) const
    {
        // logically, the ordering comes from the difference between the iterators if its negative - this is less, and if its positive, this is greater
        return Difference (rhs) <=> 0;
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

    template <typename T, typename ITERATOR_TRAITS>
    inline RandomAccessIterator<T, ITERATOR_TRAITS> operator+ (typename RandomAccessIterator<T, ITERATOR_TRAITS>::difference_type i,
                                                               const RandomAccessIterator<T, ITERATOR_TRAITS>&                    it)
    {
        return it + i;
    }
    template <typename T, typename ITERATOR_TRAITS>
    inline RandomAccessIterator<T, ITERATOR_TRAITS> operator- (typename RandomAccessIterator<T, ITERATOR_TRAITS>::difference_type i,
                                                               const RandomAccessIterator<T, ITERATOR_TRAITS>&                    it)
    {
        return -(it - i);
    }
    template <typename T, typename ITERATOR_TRAITS>
    inline auto operator- (const RandomAccessIterator<T, ITERATOR_TRAITS>& lhs, const RandomAccessIterator<T, ITERATOR_TRAITS>& rhs) ->
        typename RandomAccessIterator<T, ITERATOR_TRAITS>::difference_type
    {
        return lhs.Difference (rhs);
    }

}
