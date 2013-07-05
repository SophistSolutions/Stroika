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
             ***************************** Range<T, TRAITS> *********************************
             ********************************************************************************
             */
            template    <typename T, typename TRAITS>
            Range<T, TRAITS>::Range (const Memory::Optional<T>& min, const Memory::Optional<T>& max)
                : fMin_ (min)
                , fEffectiveMin_ (min.IsPresent () ? *min : TRAITS::kMin)
                , fMax_ (max)
                , fEffectiveMax_ (max.IsPresent () ? *max : TRAITS::kMax)
            {
            }
            template    <typename T, typename TRAITS>
            inline  bool    Range<T, TRAITS>::Contains (T v) const
            {
                return fEffectiveMin_ <= v and v <= fEffectiveMax_;
            }
            template    <typename T, typename TRAITS>
            bool    Range<T, TRAITS>::Overlaps (const Range<T, TRAITS>& v) const
            {
                return Math::Overlaps (
                           pair<T, T> (GetEffectiveMin (), GetEffectiveMax ())),
                       pair<T, T> (v.GetEffectiveMin (), v.GetEffectiveMax ()))
                );
            }
            template    <typename T, typename TRAITS>
            Range<T, TRAITS> Range<T, TRAITS>::Intersection (const Range<T, TRAITS>& v) const
            {
                //tmphack
                AssertNotImplemented ();
                return v;
            }
            template    <typename T, typename TRAITS>
            Range<T, TRAITS> Range<T, TRAITS>::ExpandedUnion (const Range<T, TRAITS>& v) const
            {
                return Range<T, TRAITS> (min (GetEffectiveMin (), v.GetEffectiveMin ()), max (GetEffectiveMax (), v.GetEffectiveMax ()));
            }
            template    <typename T, typename TRAITS>
            inline  Memory::Optional<T>    Range<T, TRAITS>::GetMin () const
            {
                return fMin_;
            }
            template    <typename T, typename TRAITS>
            inline  Memory::Optional<T>    Range<T, TRAITS>::GetMax () const
            {
                return fMax_;
            }
            template    <typename T, typename TRAITS>
            inline  T    Range<T, TRAITS>::GetEffectiveMin () const
            {
                return fEffectiveMin_;
            }
            template    <typename T, typename TRAITS>
            inline  T    Range<T, TRAITS>::GetEffectiveMax () const
            {
                return fEffectiveMax_;
            }


        }
    }
}
#endif /* _Stroika_Foundation_Traversal_Range_inl_ */
