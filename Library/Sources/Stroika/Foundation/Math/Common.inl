/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Foundation_Math_Common_inl_
#define _Stroika_Foundation_Math_Common_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include <cmath>
#include <cstdlib>

#include "../Debug/Assertions.h"

namespace Stroika::Foundation::Math {

    /*
     ********************************************************************************
     ************************************ Math::nan *********************************
     ********************************************************************************
     */
    template <typename T>
    constexpr T nan ()
    {
        return numeric_limits<T>::quiet_NaN ();
    }

    /*
     ********************************************************************************
     ****************************** Math::RoundUpTo *********************************
     ********************************************************************************
     */
    namespace Private {
        template <typename T>
        constexpr T RoundUpTo_UnSignedHelper_ (T x, T toNearest)
        {
            static_assert (is_unsigned_v<T>);
            return (((x + toNearest - 1u) / toNearest) * toNearest);
        }
        template <typename T>
        constexpr T RoundDownTo_UnSignedHelper_ (T x, T toNearest)
        {
            static_assert (is_unsigned_v<T>);
            return ((x / toNearest) * toNearest);
        }
        template <typename T>
        constexpr T RoundUpTo_SignedHelper_ (T x, T toNearest)
        {
            static_assert (is_signed_v<T>);
            using UNSIGNED_T = make_unsigned_t<T>;
            Require (toNearest > 0);
            if (x < 0) {
                return (-static_cast<T> (RoundDownTo_UnSignedHelper_ (static_cast<UNSIGNED_T> (-x), static_cast<UNSIGNED_T> (toNearest))));
            }
            else {
                return static_cast<T> (RoundUpTo_UnSignedHelper_<UNSIGNED_T> (x, toNearest));
            }
        }
        template <typename T>
        constexpr T RoundDownTo_SignedHelper_ (T x, T toNearest)
        {
            static_assert (is_signed_v<T>);
            using UNSIGNED_T = make_unsigned_t<T>;
            Require (toNearest > 0);
            if (x < 0) {
                return (-static_cast<T> (RoundUpTo_UnSignedHelper_ (static_cast<UNSIGNED_T> (-x), static_cast<UNSIGNED_T> (toNearest))));
            }
            else {
                return (RoundDownTo_UnSignedHelper_ (static_cast<UNSIGNED_T> (x), static_cast<UNSIGNED_T> (toNearest)));
            }
        }

        template <typename T, enable_if_t<is_signed_v<T>>* = nullptr>
        constexpr T RoundUpTo_ (T x, T toNearest)
        {
            return Private::RoundUpTo_SignedHelper_<T> (x, toNearest);
        }
        template <typename T, enable_if_t<is_unsigned_v<T>>* = nullptr>
        constexpr T RoundUpTo_ (T x, T toNearest)
        {
            return Private::RoundUpTo_UnSignedHelper_<T> (x, toNearest);
        }
        template <typename T, enable_if_t<is_signed_v<T>>* = nullptr>
        constexpr T RoundDownTo_ (T x, T toNearest)
        {
            return Private::RoundDownTo_SignedHelper_<T> (x, toNearest);
        }
        template <typename T, enable_if_t<is_unsigned_v<T>>* = nullptr>
        constexpr T RoundDownTo_ (T x, T toNearest)
        {
            return Private::RoundDownTo_UnSignedHelper_<T> (x, toNearest);
        }

    }
    template <typename T>
    constexpr T RoundUpTo (T x, T toNearest)
    {
        return Private::RoundUpTo_ (x, toNearest);
    }

    /*
     ********************************************************************************
     **************************** Math::RoundDownTo *********************************
     ********************************************************************************
     */
    template <typename T>
    constexpr T RoundDownTo (T x, T toNearest)
    {
        return Private::RoundDownTo_ (x, toNearest);
    }

    /*
     ********************************************************************************
     ********************************* Math::Round **********************************
     ********************************************************************************
     */
    template <typename INT_TYPE, typename FLOAT_TYPE>
    constexpr INT_TYPE Round (FLOAT_TYPE x)
    {
        FLOAT_TYPE tmp = round (x);
        if (tmp > 0) {
            return tmp >= numeric_limits<INT_TYPE>::max () ? numeric_limits<INT_TYPE>::max () : static_cast<INT_TYPE> (tmp);
        }
        else {
            return tmp <= numeric_limits<INT_TYPE>::min () ? numeric_limits<INT_TYPE>::min () : static_cast<INT_TYPE> (tmp);
        }
    }

    /*
     ********************************************************************************
     **************************** Math::NearlyEquals ********************************
     ********************************************************************************
     */
    namespace Private_ {
        template <typename TC>
        // @todo see if we can make this constexpr somehow?
        const inline TC mkCompareEpsilon_ (TC l, [[maybe_unused]] TC r)
        {
            static_assert (is_floating_point_v<TC>, "can only be used for float values");
#if 1
            if (l < -10 or l > 10) {
                static const TC kScale_ = pow (static_cast<TC> (10), -(numeric_limits<TC>::digits10 - 1)); // @todo constexpr? is pow() constexpr?
                return fabs (l) * kScale_;
            }
#endif
            return (10000 * numeric_limits<TC>::epsilon ());
        }
    }
    template <typename T1, typename T2, typename EPSILON_TYPE, typename TC>
    inline bool NearlyEquals (T1 l, T2 r, EPSILON_TYPE epsilon, enable_if_t<is_floating_point_v<TC>>*)
    {
        Require (epsilon >= 0);
        TC diff = (l - r);
        if (isnan (diff)) {
            // nan-nan, or inf-inf
            // maybe other cases shouldnt be considered nearly equals?
            return fpclassify (l) == fpclassify (r);
        }
        if (isinf (diff)) {
            static const TC kEpsilon_ = Private_::mkCompareEpsilon_ (numeric_limits<TC>::max (), numeric_limits<TC>::max ());
            /* 
                *  Need to use a temporary of type TC, because T1 or T2 maybe a type of a special temporary value which cannot be assigned to (like Sequence<>::TemporaryItem....
                */
            TC useL = l;
            if (not isinf (useL) and fabs (useL - numeric_limits<TC>::max ()) <= kEpsilon_) {
                useL = numeric_limits<TC>::infinity ();
            }
            if (not isinf (useL) and fabs (useL - numeric_limits<TC>::lowest ()) <= kEpsilon_) {
                useL = -numeric_limits<TC>::infinity ();
            }
            TC useR = r;
            if (not isinf (useR) and fabs (useR - numeric_limits<TC>::max ()) <= kEpsilon_) {
                useR = numeric_limits<TC>::infinity ();
            }
            if (not isinf (useR) and fabs (useR - numeric_limits<TC>::lowest ()) <= kEpsilon_) {
                useR = -numeric_limits<TC>::infinity ();
            }
            if (isinf (useL) and isinf (useR)) {
                return (useL > 0) == (useR > 0);
            }
        }
        return fabs (diff) <= epsilon;
    }
    template <typename T1, typename T2, typename TC>
    inline bool NearlyEquals (T1 l, T2 r, enable_if_t<is_integral_v<TC>>*)
    {
        return l == r;
    }
    template <typename T1, typename T2, typename TC>
    inline bool NearlyEquals (T1 l, T2 r, enable_if_t<is_floating_point_v<TC>>*)
    {
        return NearlyEquals (l, r, Private_::mkCompareEpsilon_<TC> (l, r));
    }
    template <typename T1, typename T2, typename TC>
    inline bool NearlyEquals (T1 l, T2 r, enable_if_t<!is_integral_v<TC> and !is_floating_point_v<TC>>*)
    {
        return l == r;
    }

    /*
     ********************************************************************************
     ************************* Math::PinToSpecialPoint ******************************
     ********************************************************************************
     */
    template <typename T>
    T PinToSpecialPoint (T p, T special)
    {
        return PinToSpecialPoint (p, special, Private_::mkCompareEpsilon_ (special, p));
    }
    template <typename T>
    T PinToSpecialPoint (T p, T special, T epsilon)
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
    template <typename T>
    constexpr T PinInRange (T initialValue, T lowerBound, T upperBound)
    {
        Require (lowerBound <= upperBound);
        return max (lowerBound, min (upperBound, initialValue));
    }

    /*
     ********************************************************************************
     ******************************** Math::AtLeast *********************************
     ********************************************************************************
     */
    template <typename T>
    constexpr T AtLeast (T initialValue, T lowerBound)
    {
        return max (initialValue, lowerBound);
    }

    /*
     ********************************************************************************
     ******************************** Math::AtMost *********************************
     ********************************************************************************
     */
    template <typename T>
    constexpr T AtMost (T initialValue, T upperBound)
    {
        return min (initialValue, upperBound);
    }

    /*
     ********************************************************************************
     ************************** Math::PinToMaxForType *******************************
     ********************************************************************************
     */
    template <typename NEW_T, typename T>
    NEW_T PinToMaxForType (T initialValue)
    {
        using LargerType = decltype (NEW_T () + T ()); // maybe should use conditional<> for this?
        return static_cast<NEW_T> (min<LargerType> (initialValue, numeric_limits<NEW_T>::max ()));
    }

    /*
     ********************************************************************************
     ********************************** Math::Abs ***********************************
     ********************************************************************************
     */
    template <typename T, typename RESULT_TYPE>
    inline RESULT_TYPE Abs (T v)
    {
        return v < 0 ? -v : v;
    }
    template <>
    inline unsigned int Abs (char v)
    {
        return abs (v);
    }
    template <>
    inline unsigned int Abs (short v)
    {
        return abs (v);
    }
    template <>
    inline unsigned int Abs (int v)
    {
        return abs (v);
    }
    template <>
    inline unsigned long Abs (long v)
    {
        return labs (v);
    }
    template <>
    inline unsigned long long Abs (long long v)
    {
        return llabs (v);
    }
    template <>
    inline unsigned int Abs (unsigned char v)
    {
        return v;
    }
    template <>
    inline unsigned int Abs (unsigned short v)
    {
        return v;
    }
    template <>
    inline unsigned int Abs (unsigned int v)
    {
        return v;
    }
    template <>
    inline unsigned long Abs (unsigned long v)
    {
        return v;
    }
    template <>
    inline unsigned long long Abs (unsigned long long v)
    {
        return v;
    }
    template <>
    inline float Abs (float v)
    {
        return fabs (v);
    }
    template <>
    inline double Abs (double v)
    {
        return fabs (v);
    }
    template <>
    inline long double Abs (long double v)
    {
        return fabs (v);
    }

    /*
     ********************************************************************************
     ********************************** Math::IsOdd *********************************
     ********************************************************************************
     */
    template <typename T>
    bool IsOdd (T v)
    {
        return v % 2 == 1;
    }

    /*
     ********************************************************************************
     ********************************* Math::IsEven *********************************
     ********************************************************************************
     */
    template <typename T>
    bool IsEven (T v)
    {
        return v % 2 == 0;
    }

    /*
     ********************************************************************************
     ******************************** Math::IsPrime *********************************
     ********************************************************************************
     */
    template <typename T>
    bool IsPrime (T v)
    {
        Require (v >= 0); // no negative numbers
        // @todo - redo this as http://en.wikipedia.org/wiki/Sieve_of_Eratosthenes but this is simpler and
        // good enuf for me to test...
        if (v == 1) {
            return false;
        }
        if (v == 2) {
            return true; // special case
        }
        T checkUpTo = T (sqrt (v) + 1);
        // Check each number from 3 up to checkUpTo and see if its a divisor
        for (T d = 2; d <= checkUpTo; ++d) {
            if (v % d == 0) {
                return false;
            }
        }
        return true;
    }

}

#endif /*_Stroika_Foundation_Math_Common_inl_*/
