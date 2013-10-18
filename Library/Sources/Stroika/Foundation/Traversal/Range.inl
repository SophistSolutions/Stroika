/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
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
             ExplicitRangeTraits_Integral<T, MIN, MAX, BEGIN_OPEN, END_OPEN, SIGNED_DIFF_TYPE, UNSIGNED_DIFF_TYPE>
             ********************************************************************************
             */
#if     !qCompilerAndStdLib_Supports_constexpr_StaticDataMember
            template    <typename T, T MIN, T MAX , RangeBase::Openness BEGIN_OPEN, RangeBase::Openness END_OPEN, typename SIGNED_DIFF_TYPE, typename UNSIGNED_DIFF_TYPE>
            const T ExplicitRangeTraits_Integral<T, MIN, MAX, BEGIN_OPEN, END_OPEN, SIGNED_DIFF_TYPE, UNSIGNED_DIFF_TYPE>::kMin   =   MIN;
            template    <typename T, T MIN, T MAX , RangeBase::Openness BEGIN_OPEN, RangeBase::Openness END_OPEN, typename SIGNED_DIFF_TYPE, typename UNSIGNED_DIFF_TYPE>
            const T ExplicitRangeTraits_Integral<T, MIN, MAX, BEGIN_OPEN, END_OPEN, SIGNED_DIFF_TYPE, UNSIGNED_DIFF_TYPE>::kMax   =   MAX;
#endif


            /*
             ********************************************************************************
             *************************** DefaultRangeTraits<T> ******************************
             ********************************************************************************
             */
#if     !qCompilerAndStdLib_Supports_constexpr_StaticDataMember
            template    <typename T>
            const T DefaultRangeTraits<T>::kMin   =   numeric_limits<T>::lowest ();
            template    <typename T>
            const T DefaultRangeTraits<T>::kMax   =   numeric_limits<T>::max ();
#endif


            /*
             ********************************************************************************
             ***************************** Range<T, TRAITS> *********************************
             ********************************************************************************
             */
            template    <typename T, typename TRAITS>
            inline  Range<T, TRAITS>::Range ()
                : fBegin_ (TRAITS::kMax)
                , fEnd_ (TRAITS::kMin)
            {
                Require  (TRAITS::kMin <= TRAITS::kMax);    // always required for class
                Require (TRAITS::kMin != TRAITS::kMax);     // you cannot make an empty range if min=max
                Ensure (empty ());
            }
            template    <typename T, typename TRAITS>
            inline  Range<T, TRAITS>::Range (const T& begin, const T& end)
                : fBegin_ (begin)
                , fEnd_ (end)
            {
                Require  (TRAITS::kMin <= TRAITS::kMax);    // always required for class
                Require (fBegin_ <= fEnd_);
            }
            template    <typename T, typename TRAITS>
            inline  Range<T, TRAITS>::Range (const Memory::Optional<T>& begin, const Memory::Optional<T>& end)
                : fBegin_ (begin.IsPresent () ? *begin : TRAITS::kMin)
                , fEnd_ (end.IsPresent () ? *end : TRAITS::kMax)
            {
                Require  (TRAITS::kMin <= TRAITS::kMax);    // always required for class
                Require (fBegin_ <= fEnd_);
            }
            template    <typename T, typename TRAITS>
            inline  Range<T, TRAITS>    Range<T, TRAITS>::FullRange ()
            {
                return Range<T, TRAITS> (TRAITS::kMin, TRAITS::kMax);
            }
            template    <typename T, typename TRAITS>
            inline  bool    Range<T, TRAITS>::empty () const
            {
                if (fBegin_ > fEnd_) {
                    // internal hack done in Range<T, TRAITS>::Range/0 () - empty range - otherwise not possible to create this situation
                    return true;
                }
                else if (fBegin_ == fEnd_) {
                    return TRAITS::kBeginOpenness == RangeBase::Openness::eClosed or TRAITS::kEndOpenness == RangeBase::Openness::eClosed;
                }
                return false;
            }
            template    <typename T, typename TRAITS>
            inline  typename TRAITS::UnsignedDifferenceType    Range<T, TRAITS>::size () const
            {
                if (empty ()) {
                    return 0;
                }
                return fEnd_ - fBegin_;
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
                if (TRAITS::kBeginOpenness == Openness::eClosed and r == fBegin_) {
                    return true;
                }
                if (TRAITS::kEndOpenness == Openness::eClosed and r == fEnd_) {
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
                Range<T, TRAITS>    result  =   Range<T, TRAITS> (min (begin (), rhs.begin ()), max (end (), rhs.end ()));
                Ensure (result.begin () <= begin ());
                Ensure (result.begin () <= end ());
                Ensure (result.begin () <= rhs.begin ());
                Ensure (result.begin () <= rhs.end ());
                Ensure (result.end () >= begin ());
                Ensure (result.end () >= end ());
                Ensure (result.end () >= rhs.begin ());
                Ensure (result.end () >= rhs.end ());
                return result;
            }
            template    <typename T, typename TRAITS>
            inline  T    Range<T, TRAITS>::begin () const
            {
                Require (not empty ());
                return fBegin_;
            }
            template    <typename T, typename TRAITS>
            inline  T    Range<T, TRAITS>::end () const
            {
                Require (not empty ());
                return fEnd_;
            }
            template    <typename T, typename TRAITS>
            inline     RangeBase::Openness    Range<T, TRAITS>::GetBeginOpenness ()
            {
                return TRAITS::kBeginOpenness;
            }
            template    <typename T, typename TRAITS>
            inline     RangeBase::Openness    Range<T, TRAITS>::GetEndOpenness ()
            {
                return TRAITS::kEndOpenness;
            }
            template    <typename T, typename TRAITS>
            inline     T    Range<T, TRAITS>::GetBeginMin ()
            {
                return TRAITS::kMin;
            }
            template    <typename T, typename TRAITS>
            inline     T    Range<T, TRAITS>::GetEndMax ()
            {
                return TRAITS::kMax;
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
