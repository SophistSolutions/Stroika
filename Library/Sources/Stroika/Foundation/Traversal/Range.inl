/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Traversal_Range_inl_
#define _Stroika_Foundation_Traversal_Range_inl_

#include    "../Debug/Assertions.h"
#include    "../Math/Overlap.h"


namespace   Stroika {
    namespace   Foundation {
        namespace   Traversal {


            /*
             ********************************************************************************
             RangeTraits::ExplicitRangeTraits_Integral<T, MIN, MAX, LOWER_BOUND_OPEN, UPPER_BOUND_OPEN, SIGNED_DIFF_TYPE, UNSIGNED_DIFF_TYPE>
             ********************************************************************************
             */
#if     qCompilerAndStdLib_constexpr_StaticDataMember_Buggy
            template    <typename T, T MIN, T MAX , Openness LOWER_BOUND_OPEN, Openness UPPER_BOUND_OPEN, typename SIGNED_DIFF_TYPE, typename UNSIGNED_DIFF_TYPE>
            const T RangeTraits::ExplicitRangeTraits_Integral<T, MIN, MAX, LOWER_BOUND_OPEN, UPPER_BOUND_OPEN, SIGNED_DIFF_TYPE, UNSIGNED_DIFF_TYPE>::kLowerBound   =   MIN;
            template    <typename T, T MIN, T MAX , Openness LOWER_BOUND_OPEN, Openness UPPER_BOUND_OPEN, typename SIGNED_DIFF_TYPE, typename UNSIGNED_DIFF_TYPE>
            const T RangeTraits::ExplicitRangeTraits_Integral<T, MIN, MAX, LOWER_BOUND_OPEN, UPPER_BOUND_OPEN, SIGNED_DIFF_TYPE, UNSIGNED_DIFF_TYPE>::kUpperBound   =   MAX;
#endif


            /*
             ********************************************************************************
             ****************** RangeTraits::DefaultRangeTraits<T> **************************
             ********************************************************************************
             */
#if     qCompilerAndStdLib_constexpr_StaticDataMember_Buggy
            template    <typename T>
            const T RangeTraits::DefaultRangeTraits<T>::kLowerBound   =   numeric_limits<T>::lowest ();
            template    <typename T>
            const T RangeTraits::DefaultRangeTraits<T>::kUpperBound   =   numeric_limits<T>::max ();
#endif


            /*
             ********************************************************************************
             ***************************** Range<T, TRAITS> *********************************
             ********************************************************************************
             */
            template    <typename T, typename TRAITS>
            inline  Range<T, TRAITS>::Range ()
                : fBegin_ (TRAITS::kUpperBound)
                , fEnd_ (TRAITS::kLowerBound)
            {
                Require  (TRAITS::kLowerBound <= TRAITS::kUpperBound);    // always required for class
                Require (TRAITS::kLowerBound != TRAITS::kUpperBound);     // you cannot make an empty range if min=max
                Ensure (empty ());
            }
            template    <typename T, typename TRAITS>
            inline  Range<T, TRAITS>::Range (const T& begin, const T& end)
                : fBegin_ (begin)
                , fEnd_ (end)
            {
                Require  (TRAITS::kLowerBound <= TRAITS::kUpperBound);    // always required for class
                Require (TRAITS::kLowerBound <= begin);
                Require (begin <= end);
                Require (end <= TRAITS::kUpperBound);
            }
            template    <typename T, typename TRAITS>
            inline  Range<T, TRAITS>::Range (const Memory::Optional<T>& begin, const Memory::Optional<T>& end)
                : fBegin_ (begin.IsPresent () ? *begin : TRAITS::kLowerBound)
                , fEnd_ (end.IsPresent () ? *end : TRAITS::kUpperBound)
            {
                Require  (TRAITS::kLowerBound <= TRAITS::kUpperBound);    // always required for class
                Require (TRAITS::kLowerBound <= fBegin_);
                Require (fBegin_ <= fEnd_);
                Require (fEnd_ <= TRAITS::kUpperBound);
            }
            template    <typename T, typename TRAITS>
            inline  Range<T, TRAITS>    Range<T, TRAITS>::FullRange ()
            {
                return Range<T, TRAITS> (TRAITS::kLowerBound, TRAITS::kUpperBound);
            }
            template    <typename T, typename TRAITS>
            inline  bool    Range<T, TRAITS>::empty () const
            {
                if (fBegin_ > fEnd_) {
                    // internal hack done in Range<T, TRAITS>::Range() - empty range - otherwise not possible to create this situation
                    return true;
                }
                else if (fBegin_ == fEnd_) {
                    return TRAITS::kLowerBoundOpenness == Openness::eOpen and TRAITS::kUpperBoundOpenness == Openness::eOpen;
                }
                return false;
            }
            template    <typename T, typename TRAITS>
            inline  typename TRAITS::UnsignedDifferenceType    Range<T, TRAITS>::GetDistanceSpanned () const
            {
                if (empty ()) {
                    return static_cast<typename Traits::UnsignedDifferenceType> (0);
                }
                return fEnd_ - fBegin_;
            }
            template    <typename T, typename TRAITS>
            inline  T    Range<T, TRAITS>::GetMidpoint () const
            {
                Require (not empty ());
                return GetLowerBound () + GetDistanceSpanned () / 2;
            }
            template    <typename T, typename TRAITS>
            inline  bool    Range<T, TRAITS>::Contains (const T& r) const
            {
                if (empty ()) {
                    return false;
                }
                if (fBegin_ < r and r < fEnd_) {
                    return true;
                }
                if (TRAITS::kLowerBoundOpenness == Openness::eClosed and r == fBegin_) {
                    return true;
                }
                if (TRAITS::kUpperBoundOpenness == Openness::eClosed and r == fEnd_) {
                    return true;
                }
                return false;
            }
            template    <typename T, typename TRAITS>
            inline  bool    Range<T, TRAITS>::Equals (const Range<T, TRAITS>& rhs) const
            {
                if (empty ()) {
                    return rhs.empty ();
                }
                return fBegin_ == rhs.fBegin_ and fEnd_ == rhs.fEnd_;
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
            Range<T, TRAITS> Range<T, TRAITS>::Intersection (const Range<T, TRAITS>& rhs) const
            {
                if (empty () or rhs.empty ()) {
                    return Range ();
                }
                T   l   =   max (fBegin_, rhs.fBegin_);
                T   r   =   min (fEnd_, rhs.fEnd_);
                if (l <= r) {
                    return Range<T, TRAITS> (l, r);
                }
                else {
                    return Range ();
                }
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
                Range<T, TRAITS>    result  =   Range<T, TRAITS> (min (GetLowerBound (), rhs.GetLowerBound ()), max (GetUpperBound (), rhs.GetUpperBound ()));
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
            inline  T    Range<T, TRAITS>::GetLowerBound () const
            {
                Require (not empty ());
                return fBegin_;
            }
            template    <typename T, typename TRAITS>
            inline  T    Range<T, TRAITS>::GetUpperBound () const
            {
                Require (not empty ());
                return fEnd_;
            }
            template    <typename T, typename TRAITS>
            inline     Openness    Range<T, TRAITS>::GetTraitsLowerBoundOpenness ()
            {
                return TRAITS::kLowerBoundOpenness;
            }
            template    <typename T, typename TRAITS>
            inline     Openness    Range<T, TRAITS>::GetTraitsUpperBoundOpenness ()
            {
                return TRAITS::kUpperBoundOpenness;
            }
            template    <typename T, typename TRAITS>
            inline     T    Range<T, TRAITS>::GetTraitsLowerBound ()
            {
                return TRAITS::kLowerBound;
            }
            template    <typename T, typename TRAITS>
            inline     T    Range<T, TRAITS>::GetTraitsUpperBound ()
            {
                return TRAITS::kUpperBound;
            }
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
            template    <typename T, typename TRAITS>
            inline  bool    Range<T, TRAITS>::operator== (const Range<T, TRAITS>& rhs) const
            {
                return Equals (rhs);
            }
            template    <typename T, typename TRAITS>
            inline  bool    Range<T, TRAITS>::operator!= (const Range<T, TRAITS>& rhs) const
            {
                return not Equals (rhs);
            }


        }
    }
}
#endif /* _Stroika_Foundation_Traversal_Range_inl_ */
