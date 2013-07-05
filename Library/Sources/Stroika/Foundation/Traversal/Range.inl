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
            Range<T, TRAITS>::Range ()
                : fEmpty_ (true)
                , fMin_ ()
                , fEffectiveMin_ ()
                , fMax_ ()
                , fEffectiveMax_ ()
            {
            }
            template    <typename T, typename TRAITS>
            Range<T, TRAITS>::Range (const Memory::Optional<T>& min, const Memory::Optional<T>& max)
                : fEmpty_ (false)
                , fMin_ (min)
                , fEffectiveMin_ (min.IsPresent () ? *min : TRAITS::kMin)
                , fMax_ (max)
                , fEffectiveMax_ (max.IsPresent () ? *max : TRAITS::kMax)
            {
                Require (fEffectiveMin_ <= fEffectiveMax_);
            }
            template    <typename T, typename TRAITS>
            inline  bool    Range<T, TRAITS>::Contains (const T& v) const
            {
                if (fEmpty_) {
                    return false;
                }
                return fEffectiveMin_ <= v and v <= fEffectiveMax_;
            }
            template    <typename T, typename TRAITS>
            bool    Range<T, TRAITS>::Overlaps (const Range<T, TRAITS>& v) const
            {
                if (fEmpty_) {
                    return false;
                }
                return Math::Overlaps (
                           pair<T, T> (GetEffectiveMin (), GetEffectiveMax ()),
                           pair<T, T> (v.GetEffectiveMin (), v.GetEffectiveMax ())
                       );
            }
            template    <typename T, typename TRAITS>
            Range<T, TRAITS> Range<T, TRAITS>::Intersection (const Range<T, TRAITS>& v) const
            {
                if (fEmpty_) {
                    return Range<T, TRAITS> ();
                }
                if (v.fEmpty_) {
                    return Range<T, TRAITS> ();
                }
                T   lb = GetEffectiveMin ();
                T   ub = GetEffectiveMax ();
                Assert (lb <= ub);
                T   rlb = v.GetEffectiveMin ();
                T   rub = v.GetEffectiveMax ();
                Assert (rlb <= rub);

                // CANNOT DO UNTIL WE HAVE 'EMPTY' CTOR
                AssertNotImplemented ();
                return Range<T, TRAITS> (lb, ub);
            }
            template    <typename T, typename TRAITS>
            Range<T, TRAITS> Range<T, TRAITS>::ExpandedUnion (const Range<T, TRAITS>& v) const
            {
                if (fEmpty_) {
                    return v;
                }
                if (v.fEmpty_) {
                    return *this;
                }
                Range<T, TRAITS>    result  =   Range<T, TRAITS> (min (GetEffectiveMin (), v.GetEffectiveMin ()), max (GetEffectiveMax (), v.GetEffectiveMax ()));
                Ensure (result.GetEffectiveMin () <= GetEffectiveMin ());
                Ensure (result.GetEffectiveMin () <= GetEffectiveMax ());
                Ensure (result.GetEffectiveMin () <= v.GetEffectiveMin ());
                Ensure (result.GetEffectiveMin () <= v.GetEffectiveMax ());
                Ensure (result.GetEffectiveMax () >= GetEffectiveMin ());
                Ensure (result.GetEffectiveMax () >= GetEffectiveMax ());
                Ensure (result.GetEffectiveMax () >= v.GetEffectiveMin ());
                Ensure (result.GetEffectiveMax () >= v.GetEffectiveMax ());
                return result;
            }
            template    <typename T, typename TRAITS>
            inline  Memory::Optional<T>    Range<T, TRAITS>::GetMin () const
            {
                Require (not fEmpty_);
                return fMin_;
            }
            template    <typename T, typename TRAITS>
            inline  Memory::Optional<T>    Range<T, TRAITS>::GetMax () const
            {
                Require (not fEmpty_);
                return fMax_;
            }
            template    <typename T, typename TRAITS>
            inline  T    Range<T, TRAITS>::GetEffectiveMin () const
            {
                Require (not fEmpty_);
                return fEffectiveMin_;
            }
            template    <typename T, typename TRAITS>
            inline  T    Range<T, TRAITS>::GetEffectiveMax () const
            {
                Require (not fEmpty_);
                return fEffectiveMax_;
            }


        }
    }
}
#endif /* _Stroika_Foundation_Traversal_Range_inl_ */
