/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#ifndef _Stroika_Foundation_Traversal_Range_inl_
#define _Stroika_Foundation_Traversal_Range_inl_

#include    "../Characters/Format.h"
#include    "../Characters/StringBuilder.h"
#include    "../Debug/Assertions.h"
#include    "../Math/Overlap.h"
#include    "DisjointRange.h"


namespace   Stroika {
    namespace   Foundation {
        namespace   Traversal {




            /*
             ********************************************************************************
             RangeTraits::ExplicitRangeTraits_Integral<T, MIN, MAX, LOWER_BOUND_OPEN, UPPER_BOUND_OPEN, SIGNED_DIFF_TYPE, UNSIGNED_DIFF_TYPE>
             ********************************************************************************
             */
            template    <typename T, Openness LOWER_BOUND_OPEN, Openness UPPER_BOUND_OPEN, typename SIGNED_DIFF_TYPE, typename UNSIGNED_DIFF_TYPE>
            template    <typename SFINAE>
            inline  T   RangeTraits::ExplicitRangeTraitsWithoutMinMax<T, LOWER_BOUND_OPEN, UPPER_BOUND_OPEN, SIGNED_DIFF_TYPE, UNSIGNED_DIFF_TYPE>::GetNext (value_type i, typename enable_if <std::is_integral<SFINAE>::value>::type*)
            {
                return i == numeric_limits<value_type>::max () ? i : static_cast<value_type> (i + 1);
            }
            template    <typename T, Openness LOWER_BOUND_OPEN, Openness UPPER_BOUND_OPEN, typename SIGNED_DIFF_TYPE, typename UNSIGNED_DIFF_TYPE>
            template    <typename SFINAE>
            inline  T   RangeTraits::ExplicitRangeTraitsWithoutMinMax<T, LOWER_BOUND_OPEN, UPPER_BOUND_OPEN, SIGNED_DIFF_TYPE, UNSIGNED_DIFF_TYPE>::GetNext (value_type i, typename enable_if <std::is_floating_point<SFINAE>::value>::type*)
            {
                return i == numeric_limits<value_type>::max () ? i : nextafter (i, numeric_limits<value_type>::max ());
            }
            template    <typename T, Openness LOWER_BOUND_OPEN, Openness UPPER_BOUND_OPEN, typename SIGNED_DIFF_TYPE, typename UNSIGNED_DIFF_TYPE>
            template    <typename SFINAE>
            inline  T   RangeTraits::ExplicitRangeTraitsWithoutMinMax<T, LOWER_BOUND_OPEN, UPPER_BOUND_OPEN, SIGNED_DIFF_TYPE, UNSIGNED_DIFF_TYPE>::GetPrevious (value_type i, typename enable_if <std::is_integral<SFINAE>::value>::type*)
            {
                return i == numeric_limits<value_type>::min () ? i : static_cast<value_type> (i - 1);
            }
            template    <typename T, Openness LOWER_BOUND_OPEN, Openness UPPER_BOUND_OPEN, typename SIGNED_DIFF_TYPE, typename UNSIGNED_DIFF_TYPE>
            template    <typename SFINAE>
            inline  T   RangeTraits::ExplicitRangeTraitsWithoutMinMax<T, LOWER_BOUND_OPEN, UPPER_BOUND_OPEN, SIGNED_DIFF_TYPE, UNSIGNED_DIFF_TYPE>::GetPrevious (value_type i, typename enable_if <std::is_floating_point<SFINAE>::value>::type*)
            {
                return i == numeric_limits<value_type>::min () ? i : nextafter (i, numeric_limits<value_type>::min ());
            }
            template    <typename T, Openness LOWER_BOUND_OPEN, Openness UPPER_BOUND_OPEN, typename SIGNED_DIFF_TYPE, typename UNSIGNED_DIFF_TYPE>
            template    < typename TYPE2CHECK, typename SFINAE_CAN_CONVERT_TYPE_TO_SIGNEDDIFFTYPE>
            inline  constexpr   auto    RangeTraits::ExplicitRangeTraitsWithoutMinMax<T, LOWER_BOUND_OPEN, UPPER_BOUND_OPEN, SIGNED_DIFF_TYPE, UNSIGNED_DIFF_TYPE>::Difference (Configuration::ArgByValueType<value_type> lhs, Configuration::ArgByValueType<value_type> rhs, SFINAE_CAN_CONVERT_TYPE_TO_SIGNEDDIFFTYPE*) -> SignedDifferenceType {
                return static_cast<SIGNED_DIFF_TYPE> (rhs) - static_cast<SIGNED_DIFF_TYPE> (lhs);
            }
            template    <typename T, Openness LOWER_BOUND_OPEN, Openness UPPER_BOUND_OPEN, typename SIGNED_DIFF_TYPE, typename UNSIGNED_DIFF_TYPE>
            inline  constexpr   auto    RangeTraits::ExplicitRangeTraitsWithoutMinMax<T, LOWER_BOUND_OPEN, UPPER_BOUND_OPEN, SIGNED_DIFF_TYPE, UNSIGNED_DIFF_TYPE>::Difference (Configuration::ArgByValueType<value_type> lhs, Configuration::ArgByValueType<value_type> rhs, ...) -> SignedDifferenceType {
                return static_cast<SIGNED_DIFF_TYPE> (rhs - lhs);
            }


            /*
             ********************************************************************************
             RangeTraits::ExplicitRangeTraits_Integral<T, MIN, MAX, LOWER_BOUND_OPEN, UPPER_BOUND_OPEN, SIGNED_DIFF_TYPE, UNSIGNED_DIFF_TYPE>
             ********************************************************************************
             */
            template    <typename T, T MIN, T MAX , Openness LOWER_BOUND_OPEN, Openness UPPER_BOUND_OPEN, typename SIGNED_DIFF_TYPE, typename UNSIGNED_DIFF_TYPE>
            constexpr T RangeTraits::ExplicitRangeTraits_Integral<T, MIN, MAX, LOWER_BOUND_OPEN, UPPER_BOUND_OPEN, SIGNED_DIFF_TYPE, UNSIGNED_DIFF_TYPE>::kLowerBound;
            template    <typename T, T MIN, T MAX , Openness LOWER_BOUND_OPEN, Openness UPPER_BOUND_OPEN, typename SIGNED_DIFF_TYPE, typename UNSIGNED_DIFF_TYPE>
            constexpr T RangeTraits::ExplicitRangeTraits_Integral<T, MIN, MAX, LOWER_BOUND_OPEN, UPPER_BOUND_OPEN, SIGNED_DIFF_TYPE, UNSIGNED_DIFF_TYPE>::kUpperBound;


            /*
             ********************************************************************************
             ****************** RangeTraits::DefaultRangeTraits<T> **************************
             ********************************************************************************
             */
            template    <typename T>
            constexpr T RangeTraits::DefaultRangeTraits<T>::kLowerBound;
            template    <typename T>
            constexpr T RangeTraits::DefaultRangeTraits<T>::kUpperBound;


            /*
             ********************************************************************************
             ***************************** Range<T, TRAITS> *********************************
             ********************************************************************************
             */
            template    <typename T, typename TRAITS>
#if     !qCompilerAndStdLib_constexpr_somtimes_cannot_combine_constexpr_with_constexpr_Buggy
            constexpr
#endif
            inline  Range<T, TRAITS>::Range ()
#if     qCompilerAndStdLib_constexpr_with_delegated_construction_Buggy
                : fBegin_ (TRAITS::kUpperBound)
                , fEnd_ (TRAITS::kLowerBound)
                , fBeginOpenness_ (TRAITS::kLowerBoundOpenness)
                , fEndOpenness_ (TRAITS::kUpperBoundOpenness)
#else
                : Range (TRAITS::kLowerBoundOpenness, TRAITS::kUpperBoundOpenness)
#endif
            {
#if     !qCompilerAndStdLib_constexpr_functions_cpp14Constaints_Buggy
                Ensure (empty ());
#endif
            }
            template    <typename T, typename TRAITS>
            template    <typename T2, typename TRAITS2>
            constexpr   inline  Range<T, TRAITS>::Range (const Range<T2, TRAITS>& src)
                : Range (src.GetLowerBound (), src.GetUpperBound (), src.GetLowerBoundOpenness (), src.GetUpperBoundOpenness ())
            {
            }
            template    <typename T, typename TRAITS>
            constexpr   inline  Range<T, TRAITS>::Range (Configuration::ArgByValueType<T> begin, Configuration::ArgByValueType<T> end)
#if     qCompilerAndStdLib_constexpr_with_delegated_construction_Buggy
                : fBegin_ (begin)
                , fEnd_ (end)
                , fBeginOpenness_ (TRAITS::kLowerBoundOpenness)
                , fEndOpenness_ (TRAITS::kUpperBoundOpenness)
#else
                : Range (begin, end, TRAITS::kLowerBoundOpenness, TRAITS::kUpperBoundOpenness)
#endif
            {
            }
            template    <typename T, typename TRAITS>
            inline  Range<T, TRAITS>::Range (const Memory::Optional<T>& begin, const Memory::Optional<T>& end)
                : Range (begin.IsPresent () ? * begin : TRAITS::kLowerBound, end.IsPresent () ? * end : TRAITS::kUpperBound, TRAITS::kLowerBoundOpenness, TRAITS::kUpperBoundOpenness)
            {
            }
            template    <typename T, typename TRAITS>
            constexpr   inline  Range<T, TRAITS>::Range (Openness lhsOpen, Openness rhsOpen)
                : fBegin_ (TRAITS::kUpperBound)
                , fEnd_ (TRAITS::kLowerBound)
                , fBeginOpenness_ (lhsOpen)
                , fEndOpenness_ (rhsOpen)
            {
#if     !qCompilerAndStdLib_constexpr_functions_cpp14Constaints_Buggy
                Ensure (empty ());
#endif
            }
            template    <typename T, typename TRAITS>
            constexpr   inline  Range<T, TRAITS>::Range (Configuration::ArgByValueType<T> begin, Configuration::ArgByValueType<T> end, Openness lhsOpen, Openness rhsOpen)
                : fBegin_ (begin)
                , fEnd_ (end)
                , fBeginOpenness_ (lhsOpen)
                , fEndOpenness_ (rhsOpen)
            {
#if     !qCompilerAndStdLib_constexpr_functions_cpp14Constaints_Buggy
                Require  (TRAITS::kLowerBound <= TRAITS::kUpperBound);    // always required for class
                Require (TRAITS::kLowerBound <= begin);
                Require (begin <= end);
                Require (end <= TRAITS::kUpperBound);
#endif
            }
            template    <typename T, typename TRAITS>
            inline  Range<T, TRAITS>::Range (const Memory::Optional<T>& begin, const Memory::Optional<T>& end, Openness lhsOpen, Openness rhsOpen)
                : Range (begin.IsPresent () ? * begin : TRAITS::kLowerBound, end.IsPresent () ? * end : TRAITS::kUpperBound, lhsOpen, rhsOpen)
            {
            }
            template    <typename T, typename TRAITS>
            inline
#if     !qCompilerAndStdLib_constexpr_somtimes_cannot_combine_constexpr_with_constexpr_Buggy
            constexpr
#endif
            Range<T, TRAITS>    Range<T, TRAITS>::ContainedRange (Configuration::ArgByValueType<T> begin, Configuration::ArgByValueType<T> end)
            {
                return begin >= end ? Range<T, TRAITS> {} :
                       Range<T, TRAITS> { begin, end };
            }
            template    <typename T, typename TRAITS>
            inline
#if     !qCompilerAndStdLib_constexpr_somtimes_cannot_combine_constexpr_with_constexpr_Buggy
            constexpr
#endif
            Range<T, TRAITS>    Range<T, TRAITS>::FullRange ()
            {
                return Range<T, TRAITS> (
                           TraitsType::kLowerBound, TraitsType::kUpperBound,
                           TraitsType::kLowerBoundOpenness, TraitsType::kUpperBoundOpenness
                       );
            }
            template    <typename T, typename TRAITS>
            inline  constexpr   bool    Range<T, TRAITS>::empty () const
            {
#if     qCompilerAndStdLib_constexpr_functions_cpp14Constaints_Buggy
                return
                    fBegin_ > fEnd_ ?
                    true :
                    ((fBegin_ == fEnd_) ?
                     (fBeginOpenness_ == Openness::eOpen and fEndOpenness_ == Openness::eOpen) :
                     false
                    )
                    ;
#else
                if (fBegin_ > fEnd_) {
                    // internal hack done in Range<T, TRAITS>::Range() - empty range - otherwise not possible to create this situation
                    return true;
                }
                else if (fBegin_ == fEnd_) {
                    return fBeginOpenness_ == Openness::eOpen and fEndOpenness_ == Openness::eOpen;
                }
                return false;
#endif
            }
            template    <typename T, typename TRAITS>
            inline  constexpr   typename Range<T, TRAITS>::UnsignedDifferenceType    Range<T, TRAITS>::GetDistanceSpanned () const
            {
#if     qCompilerAndStdLib_constexpr_functions_cpp14Constaints_Buggy
                return
                    empty () ?
                    static_cast<UnsignedDifferenceType> (0) :
                    static_cast<UnsignedDifferenceType> (typename TRAITS::Difference (fBegin_, fEnd_))
                    ;
#else
                if (empty ()) {
                    return static_cast<UnsignedDifferenceType> (0);
                }
                return static_cast<UnsignedDifferenceType> (typename TRAITS::Difference (fBegin_, fEnd_));
#endif
            }
            template    <typename T, typename TRAITS>
            inline  constexpr   T    Range<T, TRAITS>::GetMidpoint () const
            {
#if     !qCompilerAndStdLib_constexpr_functions_cpp14Constaints_Buggy
                Require (not empty ());
#endif
                return GetLowerBound () + GetDistanceSpanned () / 2;
            }
            template    <typename T, typename TRAITS>
            inline  T    Range<T, TRAITS>::Pin (T v) const
            {
                if (v < fBegin_) {
                    return fBeginOpenness_ == Openness::eClosed ? fBegin_ : TraitsType::GetNext (fBegin_);
                }
                else if (v > fEnd_) {
                    return fEndOpenness_ == Openness::eClosed ? fEnd_ : TraitsType::GetPrevious (fEnd_);
                }
                return v;
            }
            template    <typename T, typename TRAITS>
            inline  constexpr   bool    Range<T, TRAITS>::Contains (Configuration::ArgByValueType<T> r) const
            {
#if     qCompilerAndStdLib_constexpr_functions_cpp14Constaints_Buggy
                return
                    empty () ?
                    false :
                    (
                        (fBegin_ < r and r < fEnd_) or
                        (fBeginOpenness_ == Openness::eClosed and r == fBegin_) or
                        (fEndOpenness_ == Openness::eClosed and r == fEnd_)
                    )
                    ;
#else
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
#endif
            }
            template    <typename T, typename TRAITS>
            inline  bool    Range<T, TRAITS>::Contains (const Range<T, TRAITS>& containee) const
            {
                // @todo fix - NOT QUITE RIGHT!!!
                if (containee.empty ()) {
                    return true;        // \forall A: \emptyset  \subseteq A
                }
                if (GetLowerBound () < containee.GetLowerBound () and containee.GetUpperBound () < GetUpperBound ()) {
                    return true;
                }
                // @todo fix - NOT QUITE RIGHT!!!
                // NOT quite right because of openness
#if 0
                if (fBeginOpenness_ == Openness::eClosed and r == fBegin_) {
                    return true;
                }
#endif
                return GetLowerBound () <= containee.GetLowerBound () and containee.GetUpperBound () <= GetUpperBound ();
            }
            template    <typename T, typename TRAITS>
            inline  constexpr   Range<T, TRAITS>    Range<T, TRAITS>::Closure () const
            {
#if     !qCompilerAndStdLib_constexpr_functions_cpp14Constaints_Buggy
                Require (not empty ());
#endif
                return Range<T, TRAITS> (GetLowerBound (), GetUpperBound (), Openness::eClosed, Openness::eClosed);
            }
            template    <typename T, typename TRAITS>
            template    <typename T2, typename TRAITS2>
            inline  bool    Range<T, TRAITS>::Equals (const Range<T2, TRAITS2>& rhs) const
            {
                if (empty ()) {
                    return rhs.empty ();
                }
                return fBegin_ == rhs.fBegin_ and fEnd_ == rhs.fEnd_ and fBeginOpenness_ == rhs.fBeginOpenness_ and fBeginOpenness_ == rhs.fBeginOpenness_;
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
            template    <typename T, typename TRAITS>
            template    <typename T2, typename TRAITS2>
            bool    Range<T, TRAITS>::Intersects (const Range<T2, TRAITS2>& rhs) const
            {
                if (empty () or rhs.empty ()) {
                    return false;
                }
                T   l   =   max (fBegin_, rhs.GetLowerBound ());
                T   r   =   min (fEnd_, rhs.GetUpperBound ());
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
            template    <typename T, typename TRAITS>
            Range<T, TRAITS> Range<T, TRAITS>::Intersection (const Range<T, TRAITS>& rhs) const
            {
                if (empty () or rhs.empty ()) {
                    return Range ();
                }
                T   l   =   max (fBegin_, rhs.fBegin_);
                T   r   =   min (fEnd_, rhs.fEnd_);
                if (l <= r) {
                    // lhs/rhs ends are closed iff BOTH lhs/rhs contains that point
                    Openness lhsO = Contains (l) and rhs.Contains (l) ? Openness::eClosed : Openness::eOpen;
                    Openness rhsO = Contains (r) and rhs.Contains (r) ? Openness::eClosed : Openness::eOpen;
                    return Range<T, TRAITS> (l, r, lhsO, rhsO);
                }
                else {
                    return Range ();
                }
            }
            template    <typename T, typename TRAITS>
            inline   DisjointRange<T, Range<T, TRAITS>> Range<T, TRAITS>::Union (const Range<T, TRAITS>& rhs) const
            {
                return DisjointRange<T, Range<T, TRAITS>> { { *this, rhs } };
            }
            template    <typename T, typename TRAITS>
            Range<T, TRAITS> Range<T, TRAITS>::UnionBounds (const Range<T, TRAITS>& rhs) const
            {
                if (empty ()) {
                    return rhs;
                }
                if (rhs.empty ()) {
                    return *this;
                }
                T   l   =   min (GetLowerBound (), rhs.GetLowerBound ());
                T   r   =   max (GetUpperBound (), rhs.GetUpperBound ());
                Range<T, TRAITS>   result;
                if (l <= r) {
                    // lhs/rhs ends are closed iff BOTH lhs/rhs contains that point
                    Openness lhsO = Contains (l) and rhs.Contains (l) ? Openness::eClosed : Openness::eOpen;
                    Openness rhsO = Contains (r) and rhs.Contains (r) ? Openness::eClosed : Openness::eOpen;
                    result = Range<T, TRAITS> (l, r, lhsO, rhsO);
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
            template    <typename T, typename TRAITS>
            inline  constexpr   T    Range<T, TRAITS>::GetLowerBound () const
            {
#if     !qCompilerAndStdLib_constexpr_functions_cpp14Constaints_Buggy
                Require (not empty ());
#endif
                return fBegin_;
            }
            template    <typename T, typename TRAITS>
            inline  constexpr   Openness    Range<T, TRAITS>::GetLowerBoundOpenness () const
            {
                return fBeginOpenness_;
            }
            template    <typename T, typename TRAITS>
            inline  constexpr   T    Range<T, TRAITS>::GetUpperBound () const
            {
#if     !qCompilerAndStdLib_constexpr_functions_cpp14Constaints_Buggy
                Require (not empty ());
#endif
                return fEnd_;
            }
            template    <typename T, typename TRAITS>
            inline  constexpr   Openness    Range<T, TRAITS>::GetUpperBoundOpenness () const
            {
                return fEndOpenness_;
            }
            template    <typename T, typename TRAITS>
            inline  auto    Range<T, TRAITS>::Offset (SignedDifferenceType o) const -> Range
            {
                Require (not empty ());
                return Range (static_cast<T> (GetLowerBound () + o), static_cast<T> (GetUpperBound () + o), GetLowerBoundOpenness (), GetUpperBoundOpenness ());
            }
            template    <typename T, typename TRAITS>
            Characters::String  Range<T, TRAITS>::ToString (const function<Characters::String(T)>& eltToString) const
            {
                Characters::StringBuilder out;
                if (empty ()) {
                    out += L"{}";
                }
                else if (GetLowerBound () == GetUpperBound ()) {
                    // if single point, open and closed must be same (or always must be closed?
                    bool closed = true; // cuz otherwise would be empty set
                    out += L"[";
                    out += eltToString (GetLowerBound ());
                    out +=  L"]";
                }
                else {
                    out += (GetLowerBoundOpenness () == Openness::eClosed) ? L"[" : L"(";
                    out += eltToString (GetLowerBound ());
                    out +=  L" ... ";
                    out += eltToString (GetUpperBound ());
                    out += (GetUpperBoundOpenness () == Openness::eClosed) ? L"]" : L")";
                }
                return out.str ();
            }


            /*
             ********************************************************************************
             *********************************** operator+ **********************************
             ********************************************************************************
             */
            template    <typename T, typename TRAITS>
            inline  DisjointRange<T, Range<T, TRAITS>>   operator+ (const Range<T, TRAITS>& lhs, const Range<T, TRAITS>& rhs)
            {
                return lhs.Union (rhs);
            }


            /*
             ********************************************************************************
             *********************************** operator^ **********************************
             ********************************************************************************
             */
            template    <typename T, typename TRAITS>
            inline  Range<T, TRAITS>   operator^ (const Range<T, TRAITS>& lhs, const Range<T, TRAITS>& rhs)
            {
                return lhs.Intersection (rhs);
            }


            /*
             ********************************************************************************
             ********************************** operator== **********************************
             ********************************************************************************
             */
            template    <typename T, typename TRAITS>
            inline  bool   operator== (const Range<T, TRAITS>& lhs, const Range<T, TRAITS>& rhs)
            {
                return lhs.Equals (rhs);
            }


            /*
             ********************************************************************************
             ********************************** operator!= **********************************
             ********************************************************************************
             */
            template    <typename T, typename TRAITS>
            inline  bool   operator!= (const Range<T, TRAITS>& lhs, const Range<T, TRAITS>& rhs)
            {
                return not lhs.Equals (rhs);
            }


        }
    }
}
namespace Stroika {
    namespace Foundation {
        namespace Configuration {
            template<>
            struct   DefaultNames<Traversal::Openness> : EnumNames<Traversal::Openness> {
                static  constexpr   EnumNames<Traversal::Openness>    k {
                    EnumNames<Traversal::Openness>::BasicArrayInitializer {
                        {
                            { Traversal::Openness::eOpen, L"Open" },
                            { Traversal::Openness::eClosed, L"Closed" },
                        }
                    }
                };
                DefaultNames () : EnumNames<Traversal::Openness> (k) {}
            };
        }
    }
}
#endif /* _Stroika_Foundation_Traversal_Range_inl_ */
