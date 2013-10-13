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


#if     !qSupportTemplateParamterOfNumericLimitsMinMax
            template    <typename T, RangeBase::Openness beginOpen, RangeBase::Openness endOpen, T MIN, T MAX, typename SIGNED_DIFF_TYPE = int, typename UNSIGNED_DIFF_TYPE = unsigned int>
            struct  DefaultRangeTraits_Template_numericLimitsBWA : public RangeBase {
                typedef T                   ElementType;
                typedef SIGNED_DIFF_TYPE    SignedDifferenceType;
                typedef UNSIGNED_DIFF_TYPE  UnsignedDifferenceType;

                static  constexpr   Openness    kBeginOpenness  =   beginOpen;
                static  constexpr   Openness    kEndOpenness    =   endOpen;

#if     qCompilerAndStdLib_Supports_constexpr_StaticDataMember
                static  constexpr T kMin = MIN;
                static  constexpr T kMax = MAX;
#else
                static  const T kMin;
                static  const T kMax;
#endif
            };
#if     !qCompilerAndStdLib_Supports_constexpr_StaticDataMember
            template    <typename T, RangeBase::Openness beginOpen, RangeBase::Openness endOpen, T MIN, T MAX, typename SIGNED_DIFF_TYPE, typename UNSIGNED_DIFF_TYPE>
            const T DefaultRangeTraits_Template_numericLimitsBWA<T, beginOpen, endOpen, MIN, MAX, SIGNED_DIFF_TYPE, UNSIGNED_DIFF_TYPE>::kMin   =   MIN;
            template    <typename T, RangeBase::Openness beginOpen, RangeBase::Openness endOpen, T MIN, T MAX, typename SIGNED_DIFF_TYPE, typename UNSIGNED_DIFF_TYPE>
            const T DefaultRangeTraits_Template_numericLimitsBWA<T, beginOpen, endOpen, MIN, MAX, SIGNED_DIFF_TYPE, UNSIGNED_DIFF_TYPE>::kMax   =   MAX;
#endif
#if 0
            template    <>
            struct  DefaultRangeTraits<int, RangeBase::Openness::eClosed, RangeBase::Openness::eOpen, int, unsigned int> : DefaultRangeTraits_Template_numericLimitsBWA<int, RangeBase::Openness::eClosed, RangeBase::Openness::eOpen, INT_MIN, INT_MAX> {};
            template    <>
            struct  DefaultRangeTraits<unsigned int, RangeBase::Openness::eClosed, RangeBase::Openness::eOpen, int, unsigned int> : DefaultRangeTraits_Template_numericLimitsBWA<unsigned int, RangeBase::Openness::eClosed, RangeBase::Openness::eOpen, 0, UINT_MAX> {};
#endif
#endif


            /*
             ********************************************************************************
             DefaultRangeTraits<T,Openness,Openness,T,T,SIGNED_DIFF_TYPE,UNSIGNED_DIFF_TYPE>
             ********************************************************************************
             */
#if     !qSupportTemplateParamterOfNumericLimitsMinMax
            template    <typename T, RangeBase::Openness beginOpen, RangeBase::Openness endOpen, typename SIGNED_DIFF_TYPE, typename UNSIGNED_DIFF_TYPE>
            const T DefaultRangeTraits<T, beginOpen, endOpen, SIGNED_DIFF_TYPE, UNSIGNED_DIFF_TYPE>::kMin   =   numeric_limits<T>::min ();
            template    <typename T, RangeBase::Openness beginOpen, RangeBase::Openness endOpen, typename SIGNED_DIFF_TYPE, typename UNSIGNED_DIFF_TYPE>
            const T DefaultRangeTraits<T, beginOpen, endOpen, SIGNED_DIFF_TYPE, UNSIGNED_DIFF_TYPE>::kMax   =   numeric_limits<T>::max ();
#endif


            /*
             ********************************************************************************
             ***************************** Range<T, TRAITS> *********************************
             ********************************************************************************
             */
            template    <typename T, typename TRAITS>
            Range<T, TRAITS>::Range (const Memory::Optional<T>& begin, const Memory::Optional<T>& end)
                : fBegin_ (begin.IsPresent () ? *begin : TRAITS::kMin)
                , fEnd_ (end.IsPresent () ? *end : TRAITS::kMax)
            {
            }
            template    <typename T, typename TRAITS>
            inline  Range<T, TRAITS>    Range<T, TRAITS>::EmptyRange ()
            {
                return Range<T, TRAITS> (TRAITS::kMax, TRAITS::kMin);
            }
            template    <typename T, typename TRAITS>
            inline  Range<T, TRAITS>    Range<T, TRAITS>::FullRange ()
            {
                return Range<T, TRAITS> (TRAITS::kMin, TRAITS::kMax);
            }
            template    <typename T, typename TRAITS>
            inline  bool    Range<T, TRAITS>::empty () const
            {
                if (TRAITS::kMin == TRAITS::kMax) {
                    return false;
                }
                if (TRAITS::kBeginOpenness == Openness::eClosed and TRAITS::kEndOpenness == Openness::eClosed) {
                    return fBegin_ > fEnd_;
                    //return false;
                }
                else {
                    return fBegin_ >= fEnd_;
                }
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
                // @todo - FIX FOR NEW OPEN/CLOSED FLAGS
                T   l   =   max (fBegin_, rhs.fBegin_);
                T   r   =   min (fEnd_, rhs.fEnd_);
                if (l < r) {
                    return Range<T, TRAITS> (l, r);
                }
                else {
                    return Range<T, TRAITS> ();
                }
            }
            template    <typename T, typename TRAITS>
            Range<T, TRAITS> Range<T, TRAITS>::UnionBounds (const Range<T, TRAITS>& rhs) const
            {
                // @todo - FIX FOR NEW OPEN/CLOSED FLAGS
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
                return fBegin_;
            }
            template    <typename T, typename TRAITS>
            inline  T    Range<T, TRAITS>::end () const
            {
                return fEnd_;
            }
            template    <typename T, typename TRAITS>
            inline  constexpr   RangeBase::Openness    Range<T, TRAITS>::GetBeginOpenness ()
            {
                return TRAITS::kBeginOpenness;
            }
            template    <typename T, typename TRAITS>
            inline constexpr    RangeBase::Openness    Range<T, TRAITS>::GetEndOpenness ()
            {
                return TRAITS::kEndOpenness;
            }
            template    <typename T, typename TRAITS>
            inline  constexpr   T    Range<T, TRAITS>::GetBeginMin ()
            {
                return TRAITS::kMin;
            }
            template    <typename T, typename TRAITS>
            inline constexpr    T    Range<T, TRAITS>::GetEndMax ()
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
