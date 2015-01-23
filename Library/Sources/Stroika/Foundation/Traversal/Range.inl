/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
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
            inline  T   RangeTraits::ExplicitRangeTraitsWithoutMinMax<T, LOWER_BOUND_OPEN, UPPER_BOUND_OPEN, SIGNED_DIFF_TYPE, UNSIGNED_DIFF_TYPE>::GetNext (ElementType i, typename enable_if <std::is_integral<SFINAE>::value>::type*)
            {
                return i == numeric_limits<ElementType>::max () ? i : static_cast<ElementType> (i + 1);
            }
            template    <typename T, Openness LOWER_BOUND_OPEN, Openness UPPER_BOUND_OPEN, typename SIGNED_DIFF_TYPE, typename UNSIGNED_DIFF_TYPE>
            template    <typename SFINAE>
            inline  T   RangeTraits::ExplicitRangeTraitsWithoutMinMax<T, LOWER_BOUND_OPEN, UPPER_BOUND_OPEN, SIGNED_DIFF_TYPE, UNSIGNED_DIFF_TYPE>::GetNext (ElementType i, typename enable_if <std::is_floating_point<SFINAE>::value>::type*)
            {
                return i == numeric_limits<ElementType>::max () ? i : nextafter (i, numeric_limits<ElementType>::max ());
            }
            template    <typename T, Openness LOWER_BOUND_OPEN, Openness UPPER_BOUND_OPEN, typename SIGNED_DIFF_TYPE, typename UNSIGNED_DIFF_TYPE>
            template    <typename SFINAE>
            inline  T   RangeTraits::ExplicitRangeTraitsWithoutMinMax<T, LOWER_BOUND_OPEN, UPPER_BOUND_OPEN, SIGNED_DIFF_TYPE, UNSIGNED_DIFF_TYPE>::GetPrevious (ElementType i, typename enable_if <std::is_integral<SFINAE>::value>::type*)
            {
                return i == numeric_limits<ElementType>::min () ? i : static_cast<ElementType> (i - 1);
            }
            template    <typename T, Openness LOWER_BOUND_OPEN, Openness UPPER_BOUND_OPEN, typename SIGNED_DIFF_TYPE, typename UNSIGNED_DIFF_TYPE>
            template    <typename SFINAE>
            inline  T   RangeTraits::ExplicitRangeTraitsWithoutMinMax<T, LOWER_BOUND_OPEN, UPPER_BOUND_OPEN, SIGNED_DIFF_TYPE, UNSIGNED_DIFF_TYPE>::GetPrevious (ElementType i, typename enable_if <std::is_floating_point<SFINAE>::value>::type*)
            {
                return i == numeric_limits<ElementType>::min () ? i : nextafter (i, numeric_limits<ElementType>::min ());
            }
            template    <typename T, Openness LOWER_BOUND_OPEN, Openness UPPER_BOUND_OPEN, typename SIGNED_DIFF_TYPE, typename UNSIGNED_DIFF_TYPE>
            template    <typename SFINAE>
            inline  static  Characters::String  RangeTraits::ExplicitRangeTraitsWithoutMinMax<T, LOWER_BOUND_OPEN, UPPER_BOUND_OPEN, SIGNED_DIFF_TYPE, UNSIGNED_DIFF_TYPE>::Format (ElementType v, typename enable_if <is_integral<SFINAE>::value>::type* = 0)
            {
                return Characters::Format (L"%d", static_cast<int> (v));
            }
            template    <typename T, Openness LOWER_BOUND_OPEN, Openness UPPER_BOUND_OPEN, typename SIGNED_DIFF_TYPE, typename UNSIGNED_DIFF_TYPE>
            template    <typename SFINAE>
            inline  static  Characters::String  RangeTraits::ExplicitRangeTraitsWithoutMinMax<T, LOWER_BOUND_OPEN, UPPER_BOUND_OPEN, SIGNED_DIFF_TYPE, UNSIGNED_DIFF_TYPE>::Format (ElementType v, typename enable_if <is_floating_point<SFINAE>::value>::type* = 0)
            {
                return Characters::Format (L"%f", static_cast<double> (v));
            }
            template    <typename T, Openness LOWER_BOUND_OPEN, Openness UPPER_BOUND_OPEN, typename SIGNED_DIFF_TYPE, typename UNSIGNED_DIFF_TYPE>
            template    <typename SFINAE>
            inline  static  Characters::String  RangeTraits::ExplicitRangeTraitsWithoutMinMax<T, LOWER_BOUND_OPEN, UPPER_BOUND_OPEN, SIGNED_DIFF_TYPE, UNSIGNED_DIFF_TYPE>::Format (ElementType v, typename enable_if < !is_integral<SFINAE>::value and !is_floating_point<SFINAE>::value >::type* = 0)
            {
                return v.Format ();
            }


            /*
             ********************************************************************************
             RangeTraits::ExplicitRangeTraits_Integral<T, MIN, MAX, LOWER_BOUND_OPEN, UPPER_BOUND_OPEN, SIGNED_DIFF_TYPE, UNSIGNED_DIFF_TYPE>
             ********************************************************************************
             */
#if     qCompilerAndStdLib_constexpr_Buggy
            template    <typename T, T MIN, T MAX , Openness LOWER_BOUND_OPEN, Openness UPPER_BOUND_OPEN, typename SIGNED_DIFF_TYPE, typename UNSIGNED_DIFF_TYPE>
            const T RangeTraits::ExplicitRangeTraits_Integral<T, MIN, MAX, LOWER_BOUND_OPEN, UPPER_BOUND_OPEN, SIGNED_DIFF_TYPE, UNSIGNED_DIFF_TYPE>::kLowerBound   =   MIN;
            template    <typename T, T MIN, T MAX , Openness LOWER_BOUND_OPEN, Openness UPPER_BOUND_OPEN, typename SIGNED_DIFF_TYPE, typename UNSIGNED_DIFF_TYPE>
            const T RangeTraits::ExplicitRangeTraits_Integral<T, MIN, MAX, LOWER_BOUND_OPEN, UPPER_BOUND_OPEN, SIGNED_DIFF_TYPE, UNSIGNED_DIFF_TYPE>::kUpperBound   =   MAX;
#else
            template    <typename T, T MIN, T MAX , Openness LOWER_BOUND_OPEN, Openness UPPER_BOUND_OPEN, typename SIGNED_DIFF_TYPE, typename UNSIGNED_DIFF_TYPE>
            constexpr T RangeTraits::ExplicitRangeTraits_Integral<T, MIN, MAX, LOWER_BOUND_OPEN, UPPER_BOUND_OPEN, SIGNED_DIFF_TYPE, UNSIGNED_DIFF_TYPE>::kLowerBound;
            template    <typename T, T MIN, T MAX , Openness LOWER_BOUND_OPEN, Openness UPPER_BOUND_OPEN, typename SIGNED_DIFF_TYPE, typename UNSIGNED_DIFF_TYPE>
            constexpr T RangeTraits::ExplicitRangeTraits_Integral<T, MIN, MAX, LOWER_BOUND_OPEN, UPPER_BOUND_OPEN, SIGNED_DIFF_TYPE, UNSIGNED_DIFF_TYPE>::kUpperBound;
#endif


            /*
             ********************************************************************************
             ****************** RangeTraits::DefaultRangeTraits<T> **************************
             ********************************************************************************
             */
#if     qCompilerAndStdLib_constexpr_Buggy
            template    <typename T>
            const T RangeTraits::DefaultRangeTraits<T>::kLowerBound     =   numeric_limits<T>::lowest ();
            template    <typename T>
            const T RangeTraits::DefaultRangeTraits<T>::kUpperBound     =   numeric_limits<T>::max ();
#else
            template    <typename T>
            constexpr T RangeTraits::DefaultRangeTraits<T>::kLowerBound;
            template    <typename T>
            constexpr T RangeTraits::DefaultRangeTraits<T>::kUpperBound;
#endif


            /*
             ********************************************************************************
             ***************************** Range<T, TRAITS> *********************************
             ********************************************************************************
             */
            template    <typename T, typename TRAITS>
#if     !qCompilerAndStdLib_constexpr_Buggy
            constexpr
#endif
            inline  Range<T, TRAITS>::Range ()
                : Range (TRAITS::kLowerBoundOpenness, TRAITS::kUpperBoundOpenness)
            {
#if     !qCompilerAndStdLib_constexpr_functions_cpp14Constaints_Buggy
                Ensure (empty ());
#endif
            }
            template    <typename T, typename TRAITS>
            template    <typename T2, typename TRAITS2>
#if     !qCompilerAndStdLib_constexpr_Buggy
            constexpr
#endif
            inline  Range<T, TRAITS>::Range (const Range<T2, TRAITS>& src)
                : Range (src.GetLowerBound (), src.GetUpperBound (), src.GetLowerBoundOpenness (), src.GetUpperBoundOpenness ())
            {
            }
            template    <typename T, typename TRAITS>
#if     !qCompilerAndStdLib_constexpr_Buggy
            constexpr
#endif
            inline  Range<T, TRAITS>::Range (const T& begin, const T& end)
                : Range (begin, end, TRAITS::kLowerBoundOpenness, TRAITS::kUpperBoundOpenness)
            {
            }
            template    <typename T, typename TRAITS>
            inline  Range<T, TRAITS>::Range (const Memory::Optional<T>& begin, const Memory::Optional<T>& end)
                : Range (begin.IsPresent () ? *begin : TRAITS::kLowerBound, end.IsPresent () ? *end : TRAITS::kUpperBound, TRAITS::kLowerBoundOpenness, TRAITS::kUpperBoundOpenness)
            {
            }
            template    <typename T, typename TRAITS>
#if     !qCompilerAndStdLib_constexpr_Buggy
            constexpr
#endif
            inline  Range<T, TRAITS>::Range (Openness lhsOpen, Openness rhsOpen)
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
#if     !qCompilerAndStdLib_constexpr_Buggy
            constexpr
#endif
            inline  Range<T, TRAITS>::Range (const T& begin, const T& end, Openness lhsOpen, Openness rhsOpen)
                : fBegin_ (begin)
                , fEnd_ (end)
                , fBeginOpenness_ (lhsOpen)
                , fEndOpenness_ (rhsOpen)
            {
#if     qCompilerAndStdLib_constexpr_Buggy || !qCompilerAndStdLib_constexpr_functions_cpp14Constaints_Buggy
                Require  (TRAITS::kLowerBound <= TRAITS::kUpperBound);    // always required for class
                Require (TRAITS::kLowerBound <= begin);
                Require (begin <= end);
                Require (end <= TRAITS::kUpperBound);
#endif
            }
            template    <typename T, typename TRAITS>
            inline  Range<T, TRAITS>::Range (const Memory::Optional<T>& begin, const Memory::Optional<T>& end, Openness lhsOpen, Openness rhsOpen)
                : Range (begin.IsPresent () ? *begin : TRAITS::kLowerBound, end.IsPresent () ? *end : TRAITS::kUpperBound, lhsOpen, rhsOpen)
            {
            }
            template    <typename T, typename TRAITS>
            inline  constexpr   Range<T, TRAITS>    Range<T, TRAITS>::FullRange ()
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
                    (fEnd_ - fBegin_)
                    ;
#else
                if (empty ()) {
                    return static_cast<UnsignedDifferenceType> (0);
                }
                return fEnd_ - fBegin_;
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
            inline  constexpr   bool    Range<T, TRAITS>::Contains (const T& r) const
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
#if 0
            template    <typename T, typename TRAITS>
            template    <typename... ARGS>
            inline  Characters::String  Range<T, TRAITS>::Format (ARGS&& ... args) const
            {
                if (GetLowerBound () == GetUpperBound ()) {
                    return GetLowerBound ().Format (forward<ARGS> (args)...);
                }
                else {
                    return GetLowerBound ().Format (forward<ARGS> (args)...) + L" - " + GetUpperBound  ().Format (forward<ARGS> (args)...);
                }
            }
#endif
            template    <typename T, typename TRAITS>
            Characters::String  Range<T, TRAITS>::Format (const function<Characters::String(T)>& formatBound) const
            {
                Characters::StringBuilder out;
                if (empty ()) {
                    out += L"{}";
                }
                else if (GetLowerBound () == GetUpperBound ()) {
                    // if single point, open and closed must be same (or always must be closed?
                    bool closed = true; // cuz otherwise would be empty set
                    out += L"[";
                    out += formatBound (GetLowerBound ());
                    out +=  L"]";
                }
                else {
                    out += (GetLowerBoundOpenness () == Openness::eClosed) ? L"[" : L"(";
                    out += formatBound (GetLowerBound ());
                    out +=  L" ... ";
                    out += formatBound (GetUpperBound ());
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
#endif /* _Stroika_Foundation_Traversal_Range_inl_ */
