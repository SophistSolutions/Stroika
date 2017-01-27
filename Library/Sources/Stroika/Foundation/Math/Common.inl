/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#ifndef _Stroika_Foundation_Math_Common_inl_
#define _Stroika_Foundation_Math_Common_inl_ 1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "../Debug/Assertions.h"


namespace   Stroika {
    namespace   Foundation {
        namespace   Math {


            /*
             ********************************************************************************
             ************************************ Math::nan *********************************
             ********************************************************************************
             */
            template    <typename T>
            inline  T  nan ()
            {
                return numeric_limits<T>::quiet_NaN ();
            }


            /*
             ********************************************************************************
             ****************************** Math::RoundUpTo *********************************
             ********************************************************************************
             */
            namespace   Private {
                template    <typename T>
                T   RoundUpTo_UnSignedHelper_ (T x, T toNearest);
                template    <typename T, typename UNSIGNED_T>
                inline  T   RoundUpTo_SignedHelper_ (T x, T toNearest)
                {
                    Require (toNearest > 0);
                    if (x < 0) {
                        return (- static_cast<T> (RoundDownTo (static_cast<UNSIGNED_T> (-x), static_cast<UNSIGNED_T> (toNearest))));
                    }
                    else {
                        return static_cast<T> (RoundUpTo_UnSignedHelper_<UNSIGNED_T> (x, toNearest));
                    }
                }
                template    <typename T>
                inline  T   RoundUpTo_UnSignedHelper_ (T x, T toNearest)
                {
                    return (((x + toNearest - 1u) / toNearest) * toNearest);
                }
                template    <typename T, typename UNSIGNED_T>
                inline  T   RoundDownTo_SignedHelper_ (T x, T toNearest)
                {
                    Require (toNearest > 0);
                    if (x < 0) {
                        return (- static_cast<T> (RoundUpTo (static_cast<UNSIGNED_T> (-x), static_cast<UNSIGNED_T> (toNearest))));
                    }
                    else {
                        return (RoundDownTo (static_cast<UNSIGNED_T> (x), static_cast<UNSIGNED_T> (toNearest)));
                    }
                }
                template    <typename T>
                inline  T   RoundDownTo_UnSignedHelper_ (T x, T toNearest)
                {
                    return ((x / toNearest) * toNearest);
                }
            }
            template    <>
            inline  int RoundUpTo (int x, int toNearest)
            {
                return Private::RoundUpTo_SignedHelper_<int, unsigned int> (x, toNearest);
            }
            template    <>
            inline  long    RoundUpTo (long x, long toNearest)
            {
                return Private::RoundUpTo_SignedHelper_<long, unsigned long> (x, toNearest);
            }
            template    <>
            inline  long long   RoundUpTo (long long x, long long toNearest)
            {
                return Private::RoundUpTo_SignedHelper_<long long, unsigned long long> (x, toNearest);
            }
            template    <>
            inline  unsigned int    RoundUpTo (unsigned int x, unsigned int toNearest)
            {
                return Private::RoundUpTo_UnSignedHelper_<unsigned int> (x, toNearest);
            }
            template    <>
            inline  unsigned long   RoundUpTo (unsigned long x, unsigned long toNearest)
            {
                return Private::RoundUpTo_UnSignedHelper_<unsigned long> (x, toNearest);
            }
            template    <>
            inline  unsigned long long  RoundUpTo (unsigned long long x, unsigned long long toNearest)
            {
                return Private::RoundUpTo_UnSignedHelper_<unsigned long long> (x, toNearest);
            }


            /*
             ********************************************************************************
             **************************** Math::RoundDownTo *********************************
             ********************************************************************************
             */
            template    <>
            inline  int RoundDownTo (int x, int toNearest)
            {
                return Private::RoundDownTo_SignedHelper_<int, unsigned int> (x, toNearest);
            }
            template    <>
            inline  long    RoundDownTo (long x, long toNearest)
            {
                return Private::RoundDownTo_SignedHelper_<long, unsigned long> (x, toNearest);
            }
            template    <>
            inline  long long   RoundDownTo (long long x, long long toNearest)
            {
                return Private::RoundDownTo_SignedHelper_<long long, unsigned long long> (x, toNearest);
            }
            template    <>
            inline  unsigned int    RoundDownTo (unsigned int x, unsigned int toNearest)
            {
                return Private::RoundDownTo_UnSignedHelper_<unsigned int> (x, toNearest);
            }
            template    <>
            inline  unsigned long   RoundDownTo (unsigned long x, unsigned long toNearest)
            {
                return Private::RoundDownTo_UnSignedHelper_<unsigned long> (x, toNearest);
            }
            template    <>
            inline  unsigned long long  RoundDownTo (unsigned long long x, unsigned long long toNearest)
            {
                return Private::RoundDownTo_UnSignedHelper_<unsigned long long> (x, toNearest);
            }


            /*
             ********************************************************************************
             **************************** Math::NearlyEquals ********************************
             ********************************************************************************
             */
            namespace   Private_ {
                template    <typename TC>
                // @todo see if we can make this constexpr somehow?
                const   inline  TC   mkCompareEpsilon_ (TC l, TC r)
                {
                    static_assert (std::is_floating_point<TC>::value, "can only be used for float values");
#if 1
                    if (l < -10 or l > 10) {
                        static  const   TC   kScale_     =   pow (static_cast <TC> (10), - (numeric_limits<TC>::digits10 - 1)); // @todo constexpr? is pow() constexpr?
                        return fabs (l) * kScale_;
                    }
#endif
                    return (10000 * numeric_limits<TC>::epsilon ());
                }
            }
            template    <typename   T1, typename T2, typename EPSILON_TYPE, typename TC>
            inline  bool    NearlyEquals (T1 l, T2 r, EPSILON_TYPE epsilon, typename std::enable_if<std::is_floating_point<TC>::value >::type*)
            {
                Require (epsilon >= 0);
                TC diff = (l - r);
                if (std::isnan (diff)) {
                    // nan-nan, or inf-inf
                    // maybe other cases shouldnt be considered nearly equals?
                    return std::fpclassify (l) == std::fpclassify (r);
                }
                if (std::isinf (diff)) {
                    static  const   TC   kEpsilon_ = Private_::mkCompareEpsilon_ (numeric_limits<TC>::max (), numeric_limits<TC>::max ());
                    if (not std::isinf (l) and std::fabs (l - numeric_limits<TC>::max ()) <= kEpsilon_) {
                        l = numeric_limits<T1>::infinity ();
                    }
                    if (not std::isinf (l) and std::fabs (l - numeric_limits<TC>::lowest ()) <= kEpsilon_) {
                        l = -numeric_limits<T1>::infinity ();
                    }
                    if (not std::isinf (r) and std::fabs (r - numeric_limits<TC>::max ()) <= kEpsilon_) {
                        r = numeric_limits<T2>::infinity ();
                    }
                    if (not std::isinf (r) and std::fabs (r - numeric_limits<TC>::lowest ()) <= kEpsilon_) {
                        r = -numeric_limits<T2>::infinity ();
                    }
                    if (std::isinf (l) and std::isinf (r)) {
                        return (l > 0) == (r > 0);
                    }
                }
                return std::fabs (diff) <= epsilon;
            }
            template    <typename   T1, typename T2, typename TC>
            inline  bool    NearlyEquals (T1 l, T2 r, typename std::enable_if<std::is_integral<TC>::value >::type*)
            {
                return l == r;
            }
            template    <typename   T1, typename T2, typename TC>
            inline  bool    NearlyEquals (T1 l, T2 r, typename std::enable_if<std::is_floating_point<TC>::value >::type*)
            {
                return NearlyEquals (l, r, Private_::mkCompareEpsilon_<TC> (l, r));
            }
            template    <typename   T1, typename T2, typename TC>
            inline  bool    NearlyEquals (T1 l, T2 r, typename std::enable_if < !std::is_integral<TC>::value&&  !std::is_floating_point<TC>::value >::type*)
            {
                return l == r;
            }


            /*
             ********************************************************************************
             ************************* Math::PinToSpecialPoint ******************************
             ********************************************************************************
             */
            template    <typename   T>
            T   PinToSpecialPoint (T p, T special)
            {
                return PinToSpecialPoint (p, special, Private_::mkCompareEpsilon_ (special, p));
            }
            template    <typename   T>
            T   PinToSpecialPoint (T p, T special, T epsilon)
            {
                if (Math::NearlyEquals (p, special, epsilon)) {
                    return special;
                }
                return p;
            }


            /*
             ********************************************************************************
             ***************************** Math::PinInRange *********************************
             ********************************************************************************
             */
            template    <typename   T>
            inline  T   PinInRange (T initialValue, T lowerBound, T upperBound)
            {
                Require (lowerBound <= upperBound);
                return  max (lowerBound, min (upperBound, initialValue));
            }


            /*
             ********************************************************************************
             ******************************** Math::AtLeast *********************************
             ********************************************************************************
             */
            template    <typename   T>
            inline  T   AtLeast (T initialValue, T lowerBound)
            {
                return max (initialValue, lowerBound);
            }


            /*
             ********************************************************************************
             ******************************** Math::AtMost *********************************
             ********************************************************************************
             */
            template    <typename   T>
            inline  T   AtMost (T initialValue, T upperBound)
            {
                return min (initialValue, upperBound);
            }


            /*
             ********************************************************************************
             ************************** Math::PinToMaxForType *******************************
             ********************************************************************************
             */
            template    <typename NEW_T, typename   T>
            NEW_T   PinToMaxForType (T initialValue)
            {
                using   LargerType  =   decltype (NEW_T () + T ()); // maybe should use conditional<> for this?
                return static_cast<NEW_T> (min<LargerType> (initialValue, numeric_limits<NEW_T>::max ()));
            }


            /*
             ********************************************************************************
             ********************************** Math::IsOdd *********************************
             ********************************************************************************
             */
            template    <typename   T>
            bool    IsOdd (T v)
            {
                return v % 2 == 1;
            }


            /*
             ********************************************************************************
             ********************************* Math::IsEven *********************************
             ********************************************************************************
             */
            template    <typename   T>
            bool    IsEven (T v)
            {
                return v % 2 == 0;
            }


            /*
             ********************************************************************************
             ******************************** Math::IsPrime *********************************
             ********************************************************************************
             */
            template    <typename   T>
            bool    IsPrime (T v)
            {
                Require  (v >= 0);  // no negative numbers
                // @todo - redo this as http://en.wikipedia.org/wiki/Sieve_of_Eratosthenes but this is simpler and
                // good enuf for me to test...
                if (v == 1) {
                    return false;
                }
                if (v == 2) {
                    return true;    // special case
                }
                T   checkUpTo = T (sqrt (v) + 1);
                // Check each number from 3 up to checkUpTo and see if its a divisor
                for (T d = 2; d <= checkUpTo; ++d) {
                    if (v % d == 0) {
                        return false;
                    }
                }
                return true;
            }


        }
    }
}



#endif  /*_Stroika_Foundation_Math_Common_inl_*/

