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
             ********* DefaultRangeTraits<T, SIGNED_DIFF_TYPE, UNSIGNED_DIFF_TYPE> **********
             ********************************************************************************
             */
#if     !qCompilerAndStdLib_Supports_constexpr_StaticDataMember
            template    <typename T, typename SIGNED_DIFF_TYPE, typename UNSIGNED_DIFF_TYPE>
            const T DefaultRangeTraits<T, SIGNED_DIFF_TYPE, UNSIGNED_DIFF_TYPE>::kMin   =   numeric_limits<T>::min ();
            template    <typename T, typename SIGNED_DIFF_TYPE, typename UNSIGNED_DIFF_TYPE>
            const T DefaultRangeTraits<T, SIGNED_DIFF_TYPE, UNSIGNED_DIFF_TYPE>::kMax   =   numeric_limits<T>::max ();
#endif


            /*
             ********************************************************************************
             ****** DefaultIntegerRangeTraits<T, SIGNED_DIFF_TYPE, UNSIGNED_DIFF_TYPE> ******
             ********************************************************************************
             */
#if     !qCompilerAndStdLib_Supports_constexpr_StaticDataMember
            template    <typename T, typename SIGNED_DIFF_TYPE, typename UNSIGNED_DIFF_TYPE, T MIN, T MAX>
            const T DefaultIntegerRangeTraits<T, SIGNED_DIFF_TYPE, UNSIGNED_DIFF_TYPE, MIN, MAX>::kMin   =   MIN;
            template    <typename T, typename SIGNED_DIFF_TYPE, typename UNSIGNED_DIFF_TYPE, T MIN, T MAX>
            const T DefaultIntegerRangeTraits<T, SIGNED_DIFF_TYPE, UNSIGNED_DIFF_TYPE, MIN, MAX>::kMax   =   MAX;
#endif


            /*
             ********************************************************************************
             ***************************** Range<T, TRAITS> *********************************
             ********************************************************************************
             */
            template    <typename T, typename TRAITS>
            Range<T, TRAITS>::Range ()
                : fBegin_ (TRAITS::kMin)
                , fEnd_ (TRAITS::kMax)
            {
            }
            template    <typename T, typename TRAITS>
            Range<T, TRAITS>::Range (const Memory::Optional<T>& begin, const Memory::Optional<T>& end)
                : fBegin_ (begin.IsPresent () ? *begin : TRAITS::kMin)
                , fEnd_ (end.IsPresent () ? *end : TRAITS::kMax)
            {
                Require (fBegin_ <= fEnd_);
            }
            template    <typename T, typename TRAITS>
            inline  bool    Range<T, TRAITS>::empty () const
            {
                return fBegin_ == fEnd_;
            }
            template    <typename T, typename TRAITS>
            inline  typename TRAITS::UnsignedDifferenceType    Range<T, TRAITS>::size () const
            {
                return fEnd_ - fBegin_;
            }
            template    <typename T, typename TRAITS>
            inline  bool    Range<T, TRAITS>::Contains (const T& r) const
            {
                return fBegin_ <= r and r < fEnd_;
            }
            template    <typename T, typename TRAITS>
            inline  bool    Range<T, TRAITS>::Equals (const Range<T, TRAITS>& rhs) const
            {
                if (empty ()) {
                    return rhs.empty ();
                }
                return fBegin_ == rhs.fBegin_ and fEnd_ == rhs.fEnd_;
            }
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
            template    <typename T, typename TRAITS>
            Range<T, TRAITS> Range<T, TRAITS>::Intersection (const Range<T, TRAITS>& rhs) const
            {
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
