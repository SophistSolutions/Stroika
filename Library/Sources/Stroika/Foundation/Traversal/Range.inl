/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_Traversal_Range_inl_
#define _Stroika_Foundation_Traversal_Range_inl_

#include "../Characters/Format.h"
#include "../Characters/StringBuilder.h"
#include "../Debug/Assertions.h"
#include "../Math/Overlap.h"
#include "DisjointRange.h"

namespace Stroika::Foundation::Traversal {

    /*
     ********************************************************************************
     **** RangeTraits::ExplicitOpennessAndDifferenceType<T, OPENNESS, DIFF_TYPE> ****
     ********************************************************************************
     */
    template <typename T, typename OPENNESS, typename DIFF_TYPE>
    template <typename TYPE2CHECK, typename SFINAE_CAN_CONVERT_TYPE_TO_SIGNEDDIFFTYPE>
    inline constexpr auto RangeTraits::ExplicitOpennessAndDifferenceType<T, OPENNESS, DIFF_TYPE>::Difference (Configuration::ArgByValueType<value_type> lhs, Configuration::ArgByValueType<value_type> rhs, SFINAE_CAN_CONVERT_TYPE_TO_SIGNEDDIFFTYPE*) -> SignedDifferenceType
    {
        return static_cast<SignedDifferenceType> (rhs) - static_cast<SignedDifferenceType> (lhs);
    }
    template <typename T, typename OPENNESS, typename DIFF_TYPE>
    template <typename TYPE2CHECK, typename SFINAE_CANNOT_CONVERT_TYPE_TO_SIGNEDDIFFTYPE>
    inline constexpr auto RangeTraits::ExplicitOpennessAndDifferenceType<T, OPENNESS, DIFF_TYPE>::Difference (Configuration::ArgByValueType<value_type> lhs, Configuration::ArgByValueType<value_type> rhs, ...) -> SignedDifferenceType
    {
        return static_cast<SignedDifferenceType> (rhs - lhs);
    }

    /*
     ********************************************************************************
     ********** RangeTraits::Explicit<T, OPENNESS, BOUNDS, DIFF_TYPE> ***************
     ********************************************************************************
     */
    template <typename T, typename OPENNESS, typename BOUNDS, typename DIFF_TYPE>
    template <typename SFINAE>
    constexpr T RangeTraits::Explicit<T, OPENNESS, BOUNDS, DIFF_TYPE>::GetNext (value_type i, enable_if_t<is_integral_v<SFINAE> or is_enum_v<SFINAE>>*)
    {
        Require (i != kUpperBound);
        return static_cast<value_type> (static_cast<UnsignedDifferenceType> (i) + 1);
    }
    template <typename T, typename OPENNESS, typename BOUNDS, typename DIFF_TYPE>
    template <typename SFINAE>
    inline T RangeTraits::Explicit<T, OPENNESS, BOUNDS, DIFF_TYPE>::GetNext (value_type i, enable_if_t<is_floating_point_v<SFINAE>>*)
    {
        Require (i != kUpperBound);
        return nextafter (i, kUpperBound);
    }
    template <typename T, typename OPENNESS, typename BOUNDS, typename DIFF_TYPE>
    template <typename SFINAE>
    constexpr T RangeTraits::Explicit<T, OPENNESS, BOUNDS, DIFF_TYPE>::GetPrevious (value_type i, enable_if_t<is_integral_v<SFINAE> or is_enum_v<SFINAE>>*)
    {
        Require (i != kLowerBound);
        return static_cast<value_type> (static_cast<SignedDifferenceType> (i) - 1);
    }
    template <typename T, typename OPENNESS, typename BOUNDS, typename DIFF_TYPE>
    template <typename SFINAE>
    inline T RangeTraits::Explicit<T, OPENNESS, BOUNDS, DIFF_TYPE>::GetPrevious (value_type i, enable_if_t<is_floating_point_v<SFINAE>>*)
    {
        Require (i != kLowerBound);
        return nextafter (i, kLowerBound);
    }

    /*
     ********************************************************************************
     ***************************** Range<T, TRAITS> *********************************
     ********************************************************************************
     */
    template <typename T, typename TRAITS>
    constexpr Range<T, TRAITS>::Range ()
        : fBegin_{TRAITS::kUpperBound}
        , fEnd_{TRAITS::kLowerBound}
        , fBeginOpenness_{TRAITS::kLowerBoundOpenness}
        , fEndOpenness_{TRAITS::kUpperBoundOpenness}
    {
        Ensure (empty ());
    }
    template <typename T, typename TRAITS>
    template <typename T2, typename TRAITS2>
    constexpr Range<T, TRAITS>::Range (const Range<T2, TRAITS>& src)
        : Range{src.GetLowerBound (), src.GetUpperBound (), src.GetLowerBoundOpenness (), src.GetUpperBoundOpenness ()}
    {
    }
    template <typename T, typename TRAITS>
    constexpr Range<T, TRAITS>::Range (Configuration::ArgByValueType<T> begin, Configuration::ArgByValueType<T> end)
        : Range{begin, end, TRAITS::kLowerBoundOpenness, TRAITS::kUpperBoundOpenness}
    {
    }
    template <typename T, typename TRAITS>
    constexpr Range<T, TRAITS>::Range (const optional<T>& begin, const optional<T>& end)
        : Range{begin.has_value () ? *begin : TRAITS::kLowerBound, end.has_value () ? *end : TRAITS::kUpperBound, TRAITS::kLowerBoundOpenness, TRAITS::kUpperBoundOpenness}
    {
    }
    template <typename T, typename TRAITS>
    constexpr Range<T, TRAITS>::Range (Configuration::ArgByValueType<T> begin, Configuration::ArgByValueType<T> end, Openness lhsOpen, Openness rhsOpen)
        : fBegin_{begin}
        , fEnd_{end}
        , fBeginOpenness_{lhsOpen}
        , fEndOpenness_{rhsOpen}
    {
        Require (TRAITS::kLowerBound <= TRAITS::kUpperBound); // always required for class
        Require (TRAITS::kLowerBound <= begin);
        Require (begin <= end);
        Require (end <= TRAITS::kUpperBound);
        Require (begin < end or (lhsOpen == Openness::eClosed and rhsOpen == Openness::eClosed));
        Ensure (not empty ());
    }
    template <typename T, typename TRAITS>
    constexpr Range<T, TRAITS>::Range (const optional<T>& begin, const optional<T>& end, Openness lhsOpen, Openness rhsOpen)
        : Range{begin.has_value () ? *begin : TRAITS::kLowerBound, end.has_value () ? *end : TRAITS::kUpperBound, lhsOpen, rhsOpen}
    {
    }
    template <typename T, typename TRAITS>
    constexpr Range<T, TRAITS> Range<T, TRAITS>::Ball (Configuration::ArgByValueType<T> center, Configuration::ArgByValueType<UnsignedDifferenceType> radius, Openness lhsOpen, Openness rhsOpen)
    {
        return Range{center - radius, center + radius, lhsOpen, rhsOpen};
    }
    template <typename T, typename TRAITS>
    constexpr Range<T, TRAITS> Range<T, TRAITS>::ContainedRange (Configuration::ArgByValueType<T> begin, Configuration::ArgByValueType<T> end)
    {
        // note the case of begin==end is depends on openness, and already handled in normal CTOR - just avoid assert for having begin/end reversed
        return begin > end ? Range{} : Range{begin, end};
    }
    template <typename T, typename TRAITS>
    constexpr Range<T, TRAITS> Range<T, TRAITS>::FullRange ()
    {
        return Range{
            TraitsType::kLowerBound, TraitsType::kUpperBound,
            TraitsType::kLowerBoundOpenness, TraitsType::kUpperBoundOpenness};
    }
    template <typename T, typename TRAITS>
    constexpr bool Range<T, TRAITS>::empty () const
    {
        if (fBegin_ > fEnd_) {
            // internal hack done in Range::Range() - empty range - otherwise not possible to create this situation
            return true;
        }
        else if (fBegin_ == fEnd_) {
            return fBeginOpenness_ == Openness::eOpen and fEndOpenness_ == Openness::eOpen;
        }
        return false;
    }
    template <typename T, typename TRAITS>
    constexpr typename Range<T, TRAITS>::UnsignedDifferenceType Range<T, TRAITS>::GetDistanceSpanned () const
    {
        if (empty ()) [[UNLIKELY_ATTR]] {
            return static_cast<UnsignedDifferenceType> (0);
        }
        return static_cast<UnsignedDifferenceType> (TraitsType::Difference (fBegin_, fEnd_));
    }
    template <typename T, typename TRAITS>
    constexpr T Range<T, TRAITS>::GetMidpoint () const
    {
        Require (not empty ());
        return GetLowerBound () + GetDistanceSpanned () / 2;
    }
    template <typename T, typename TRAITS>
    constexpr T Range<T, TRAITS>::Pin (T v) const
    {
        Require (not empty ());
        Assert (fBegin_ != fEnd_ or (fBeginOpenness_ == Openness::eClosed and fEndOpenness_ == Openness::eClosed));
        if (v < fBegin_ or (v == fBegin_ and fBeginOpenness_ == Openness::eOpen)) {
            // must advance
            T tmp{fBeginOpenness_ == Openness::eClosed ? fBegin_ : TraitsType::GetNext (fBegin_)};
            Require (Contains (tmp));
            return tmp;
        }
        else if (v > fEnd_ or (v == fEnd_ and fEndOpenness_ == Openness::eOpen)) {
            // must retreat
            T tmp{fEndOpenness_ == Openness::eClosed ? fEnd_ : TraitsType::GetPrevious (fEnd_)};
            Require (Contains (tmp));
            return tmp;
        }
        return v;
    }
    template <typename T, typename TRAITS>
    constexpr bool Range<T, TRAITS>::Contains (Configuration::ArgByValueType<T> r) const
    {
        if (empty ()) {
            return false;
        }
        if (fBegin_ < r and r < fEnd_) {
            return true;
        }
        if (fBeginOpenness_ == Openness::eClosed and r == fBegin_) {
            return true;
        }
        if (fEndOpenness_ == Openness::eClosed and r == fEnd_) {
            return true;
        }
        return false;
    }
    template <typename T, typename TRAITS>
    constexpr bool Range<T, TRAITS>::Contains (const Range& containee) const
    {
        /*
         *  First, ANY empty set is contained in any other set: \forall A: \emptyset  \subseteq A
         */
        if (containee.empty ()) {
            return true; // \forall A: \emptyset  \subseteq A
        }
        /*
         *  Roughly, a non-empty range is contained in a range iff both ends are contained - but we need to take care of openness
         *  edge conditions.
         * 
         *  if BOTH container and containee are open, the container wont CONTAIN the lower bound of the containee if the lower bounds are equal
         * 
         *  Check lower bound and upper bound separately.
         */
        switch (containee.GetLowerBoundOpenness ()) {
            case Openness::eClosed:
                if (not Contains (containee.GetLowerBound ())) {
                    return false;
                }
                [[fallthrough]];
            case Openness::eOpen:
                if (containee.GetLowerBound () < GetLowerBound ()) {
                    return false;
                }
                break;
        }
        switch (containee.GetUpperBoundOpenness ()) {
            case Openness::eClosed:
                if (not Contains (containee.GetUpperBound ())) {
                    return false;
                }
                [[fallthrough]];
            case Openness::eOpen:
                if (containee.GetUpperBound () > GetUpperBound ()) {
                    return false;
                }
        }
        return true;
    }
    template <typename T, typename TRAITS>
    constexpr Range<T, TRAITS> Range<T, TRAITS>::Closure () const
    {
        Require (not empty ());
        return Range{GetLowerBound (), GetUpperBound (), Openness::eClosed, Openness::eClosed};
    }
#if 0
    template    <typename T, typename TRAITS>
    bool    Range<T, TRAITS>::Overlaps (const Range<T, TRAITS>& rhs) const
    {
        /*
            *  @todo   RETHINK - because Range has semantics of exclude end - make sure overlap usuage
            *          here is correct??? Unsure -- LGP 2013-07-05
            */
        return Math::Overlaps (
                    pair<T, T> (fBegin_, fEnd_),
                    pair<T, T> (rhs.fBegin_, rhs.fEnd_)
                );
    }
#endif
    template <typename T, typename TRAITS>
    template <typename T2, typename TRAITS2>
    constexpr bool Range<T, TRAITS>::Intersects (const Range<T2, TRAITS2>& rhs) const
    {
        if (empty () or rhs.empty ()) {
            return false;
        }
        T l = max (fBegin_, rhs.GetLowerBound ());
        T r = min (fEnd_, rhs.GetUpperBound ());
        if (l < r) {
            return true;
        }
        else if (l == r) {
            // must check if the end that has 'l' for each Range that that end is closed. Contains()
            // is a shortcut for that
            return Contains (l) and rhs.Contains (l);
        }
        else {
            return false;
        }
    }
    template <typename T, typename TRAITS>
    constexpr Range<T, TRAITS> Range<T, TRAITS>::Intersection (const Range& rhs) const
    {
        if (empty () or rhs.empty ()) {
            return Range{};
        }
        T l = max (fBegin_, rhs.fBegin_);
        T r = min (fEnd_, rhs.fEnd_);
        if (l <= r) {
            // lhs/rhs ends are closed iff BOTH lhs/rhs contains that point
            Openness lhsO = Contains (l) and rhs.Contains (l) ? Openness::eClosed : Openness::eOpen;
            Openness rhsO = Contains (r) and rhs.Contains (r) ? Openness::eClosed : Openness::eOpen;
            if (l != r or (lhsO == Openness::eClosed and rhsO == Openness::eClosed)) {
                return Range{l, r, lhsO, rhsO};
            }
        }
        return Range{};
    }
    template <typename T, typename TRAITS>
    inline DisjointRange<T, Range<T, TRAITS>> Range<T, TRAITS>::Union (const Range& rhs) const
    {
        return DisjointRange<T, Range>{{*this, rhs}};
    }
    template <typename T, typename TRAITS>
    constexpr Range<T, TRAITS> Range<T, TRAITS>::UnionBounds (const Range& rhs) const
    {
        if (empty ()) {
            return rhs;
        }
        if (rhs.empty ()) {
            return *this;
        }
        T     l = min (GetLowerBound (), rhs.GetLowerBound ());
        T     r = max (GetUpperBound (), rhs.GetUpperBound ());
        Range result;
        if (l <= r) {
            // lhs/rhs ends are closed iff BOTH lhs/rhs contains that point
            Openness lhsO = Contains (l) and rhs.Contains (l) ? Openness::eClosed : Openness::eOpen;
            Openness rhsO = Contains (r) and rhs.Contains (r) ? Openness::eClosed : Openness::eOpen;
            result        = Range (l, r, lhsO, rhsO);
        }
        Ensure (result.GetLowerBound () <= GetLowerBound ());
        Ensure (result.GetLowerBound () <= GetUpperBound ());
        Ensure (result.GetLowerBound () <= rhs.GetLowerBound ());
        Ensure (result.GetLowerBound () <= rhs.GetUpperBound ());
        Ensure (result.GetUpperBound () >= GetLowerBound ());
        Ensure (result.GetUpperBound () >= GetUpperBound ());
        Ensure (result.GetUpperBound () >= rhs.GetLowerBound ());
        Ensure (result.GetUpperBound () >= rhs.GetUpperBound ());
        return result;
    }
    template <typename T, typename TRAITS>
    constexpr T Range<T, TRAITS>::GetLowerBound () const
    {
        Require (not empty ());
        return fBegin_;
    }
    template <typename T, typename TRAITS>
    constexpr Openness Range<T, TRAITS>::GetLowerBoundOpenness () const
    {
        return fBeginOpenness_;
    }
    template <typename T, typename TRAITS>
    constexpr T Range<T, TRAITS>::GetUpperBound () const
    {
        Require (not empty ());
        return fEnd_;
    }
    template <typename T, typename TRAITS>
    constexpr Openness Range<T, TRAITS>::GetUpperBoundOpenness () const
    {
        return fEndOpenness_;
    }
    template <typename T, typename TRAITS>
    constexpr auto Range<T, TRAITS>::Offset (SignedDifferenceType o) const -> Range
    {
        Require (not empty ());
        return Range (static_cast<T> (GetLowerBound () + o), static_cast<T> (GetUpperBound () + o), GetLowerBoundOpenness (), GetUpperBoundOpenness ());
    }
    template <typename T, typename TRAITS>
    constexpr auto Range<T, TRAITS>::Times (T o) const -> Range
    {
        Require (not empty ());
        return Range (static_cast<T> (GetLowerBound () * o), static_cast<T> (GetUpperBound () * o), GetLowerBoundOpenness (), GetUpperBoundOpenness ());
    }
    template <typename T, typename TRAITS>
    Characters::String Range<T, TRAITS>::ToString (const function<Characters::String (T)>& eltToString) const
    {
        Characters::StringBuilder out;
        if (empty ()) {
            out += L"{}";
        }
        else if (GetLowerBound () == GetUpperBound ()) {
            // if single point, open and closed must be same (or always must be closed?)
            out += L"[";
            out += eltToString (GetLowerBound ());
            out += L"]";
        }
        else {
            out += (GetLowerBoundOpenness () == Openness::eClosed) ? L"[" : L"(";
            out += eltToString (GetLowerBound ());
            out += L" ... ";
            out += eltToString (GetUpperBound ());
            out += (GetUpperBoundOpenness () == Openness::eClosed) ? L"]" : L")";
        }
        return out.str ();
    }
#if __cpp_impl_three_way_comparison >= 201907
    template <typename T, typename TRAITS>
    constexpr bool Range<T, TRAITS>::operator== (const Range& rhs) const
    {
        if (empty ()) {
            return rhs.empty ();
        }
        return GetLowerBound () == rhs.GetLowerBound () and GetUpperBound () == rhs.GetUpperBound () and GetLowerBoundOpenness () == rhs.GetLowerBoundOpenness () and GetUpperBoundOpenness () == rhs.GetUpperBoundOpenness ();
    }
#endif

#if __cpp_impl_three_way_comparison < 201907
    /*
     ********************************************************************************
     ********************* Range<T,TRAITS> Comparisons Operators ********************
     ********************************************************************************
     */
    template <typename T, typename TRAITS>
    inline bool operator== (const Range<T, TRAITS>& lhs, const Range<T, TRAITS>& rhs)
    {
        if (lhs.empty ()) {
            return rhs.empty ();
        }
        return lhs.GetLowerBound () == rhs.GetLowerBound () and lhs.GetUpperBound () == rhs.GetUpperBound () and lhs.GetLowerBoundOpenness () == rhs.GetLowerBoundOpenness () and lhs.GetUpperBoundOpenness () == rhs.GetUpperBoundOpenness ();
    }
    template <typename T, typename TRAITS>
    inline bool operator!= (const Range<T, TRAITS>& lhs, const Range<T, TRAITS>& rhs)
    {
        return not(lhs == rhs);
    }
#endif

    /*
     ********************************************************************************
     *************************** Range<T,TRAITS> Operators **************************
     ********************************************************************************
     */
    template <typename T, typename TRAITS>
    inline Range<T, TRAITS> operator+ (const T& lhs, const Range<T, TRAITS>& rhs)
    {
        return rhs.Offset (lhs);
    }
    template <typename T, typename TRAITS>
    inline Range<T, TRAITS> operator+ (const Range<T, TRAITS>& lhs, const T& rhs)
    {
        return lhs.Offset (rhs);
    }
    template <typename T, typename TRAITS>
    inline DisjointRange<T, Range<T, TRAITS>> operator+ (const Range<T, TRAITS>& lhs, const Range<T, TRAITS>& rhs)
    {
        return lhs.Union (rhs);
    }

    template <typename T, typename TRAITS>
    inline Range<T, TRAITS> operator* (const T& lhs, const Range<T, TRAITS>& rhs)
    {
        return rhs.Times (lhs);
    }
    template <typename T, typename TRAITS>
    inline Range<T, TRAITS> operator* (const Range<T, TRAITS>& lhs, const T& rhs)
    {
        return lhs.Times (rhs);
    }

    template <typename T, typename TRAITS>
    inline Range<T, TRAITS> operator^ (const Range<T, TRAITS>& lhs, const Range<T, TRAITS>& rhs)
    {
        return lhs.Intersection (rhs);
    }

}

namespace Stroika::Foundation::Configuration {
#if !qCompilerAndStdLib_template_specialization_internalErrorWithSpecializationSignifier_Buggy
    template <>
#endif
    constexpr EnumNames<Traversal::Openness> DefaultNames<Traversal::Openness>::k{
        EnumNames<Traversal::Openness>::BasicArrayInitializer{{
            {Traversal::Openness::eOpen, L"Open"},
            {Traversal::Openness::eClosed, L"Closed"},
        }}};
}

#endif /* _Stroika_Foundation_Traversal_Range_inl_ */
