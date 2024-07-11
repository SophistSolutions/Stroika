/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include <cmath>
#include <cstdlib>

#include "Stroika/Foundation/Configuration/Concepts.h"
#include "Stroika/Foundation/Configuration/StdCompat.h"
#include "Stroika/Foundation/Debug/Assertions.h"

namespace Stroika::Foundation::Math {

    /*
     ********************************************************************************
     ************************************ Math::nan *********************************
     ********************************************************************************
     */
    template <floating_point T>
    constexpr T nan ()
    {
        return numeric_limits<T>::quiet_NaN ();
    }

    /*
     ********************************************************************************
     ************************************ Math::nan *********************************
     ********************************************************************************
     */
    template <floating_point T>
    constexpr T infinity ()
    {
        return numeric_limits<T>::infinity ();
    }

    /*
     ********************************************************************************
     ****************************** Math::RoundUpTo *********************************
     ********************************************************************************
     */
    namespace Private {
        template <unsigned_integral T>
        constexpr T RoundUpTo_UnSignedHelper_ (T x, T toNearest)
        {
            return (((x + toNearest - 1u) / toNearest) * toNearest);
        }
        template <unsigned_integral T>
        constexpr T RoundDownTo_UnSignedHelper_ (T x, T toNearest)
        {
            return ((x / toNearest) * toNearest);
        }
        template <signed_integral T>
        constexpr T RoundUpTo_SignedHelper_ (T x, T toNearest)
        {
            using UNSIGNED_T = make_unsigned_t<T>;
            Require (toNearest > 0);
            if (x < 0) {
                return (-static_cast<T> (RoundDownTo_UnSignedHelper_ (static_cast<UNSIGNED_T> (-x), static_cast<UNSIGNED_T> (toNearest))));
            }
            else {
                return static_cast<T> (RoundUpTo_UnSignedHelper_<UNSIGNED_T> (x, toNearest));
            }
        }
        template <signed_integral T>
        constexpr T RoundDownTo_SignedHelper_ (T x, T toNearest)
        {
            using UNSIGNED_T = make_unsigned_t<T>;
            Require (toNearest > 0);
            if (x < 0) {
                return (-static_cast<T> (RoundUpTo_UnSignedHelper_ (static_cast<UNSIGNED_T> (-x), static_cast<UNSIGNED_T> (toNearest))));
            }
            else {
                return (RoundDownTo_UnSignedHelper_ (static_cast<UNSIGNED_T> (x), static_cast<UNSIGNED_T> (toNearest)));
            }
        }
        template <signed_integral T>
        constexpr T RoundUpTo_ (T x, T toNearest)
        {
            return Private::RoundUpTo_SignedHelper_<T> (x, toNearest);
        }
        template <unsigned_integral T>
        constexpr T RoundUpTo_ (T x, T toNearest)
        {
            return Private::RoundUpTo_UnSignedHelper_<T> (x, toNearest);
        }
        template <signed_integral T>
        constexpr T RoundDownTo_ (T x, T toNearest)
        {
            return Private::RoundDownTo_SignedHelper_<T> (x, toNearest);
        }
        template <unsigned_integral T>
        constexpr T RoundDownTo_ (T x, T toNearest)
        {
            return Private::RoundDownTo_UnSignedHelper_<T> (x, toNearest);
        }

    }
    template <typename T>
    constexpr T RoundUpTo (T x, T toNearest)
        requires (is_arithmetic_v<T>)
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
        requires (is_arithmetic_v<T>)
    {
        return Private::RoundDownTo_ (x, toNearest);
    }

    /*
     ********************************************************************************
     ********************************* Math::Round **********************************
     ********************************************************************************
     */
    template <integral INT_TYPE, floating_point FLOAT_TYPE>
    constexpr INT_TYPE Round (FLOAT_TYPE x)
    {
        FLOAT_TYPE tmp = ::round (x);
        if (tmp > 0) {
#if (defined(__clang_major__) && !defined(__APPLE__) && (__clang_major__ >= 10)) ||                                                        \
    (defined(__clang_major__) && defined(__APPLE__) && (__clang_major__ >= 13))
            DISABLE_COMPILER_CLANG_WARNING_START ("clang diagnostic ignored \"-Wimplicit-int-float-conversion\""); // warning: implicit conversion from 'std::__1::numeric_limits<long>::type' (aka 'long') to 'double' changes value from 9223372036854775807 to 9223372036854775808
#endif
            return tmp >= numeric_limits<INT_TYPE>::max () ? numeric_limits<INT_TYPE>::max () : static_cast<INT_TYPE> (tmp);
#if (defined(__clang_major__) && !defined(__APPLE__) && (__clang_major__ >= 10)) ||                                                        \
    (defined(__clang_major__) && defined(__APPLE__) && (__clang_major__ >= 13))
            DISABLE_COMPILER_CLANG_WARNING_END ("clang diagnostic ignored \"-Wimplicit-int-float-conversion\"");
#endif
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
    template <typename T1, typename T2, typename EPSILON_TYPE>
    constexpr bool NearlyEquals (T1 l, T2 r, EPSILON_TYPE epsilon)
        requires (is_arithmetic_v<T1> and is_arithmetic_v<T2>)
    {
        using Configuration::StdCompat::isinf;
        using Configuration::StdCompat::isnan;
        if (isnan (l) or isnan (r)) [[unlikely]] {
            return isnan (l) and isnan (r);
        }
        else if (isinf (l) or isinf (r)) [[unlikely]] {
            // only 'equal' if inf and of same sign
            // https://stackoverflow.com/questions/41834621/c-ieee-floats-inf-equal-inf
            return l == r;
        }
        auto diff = l - r;
        Require (epsilon >= 0); // other cases we ignore epsilon
        Assert (not isnan (l) and not isnan (r) and not isinf (l) and not isinf (r));
        return Abs (diff) <= epsilon;
    }
    template <typename T1, typename T2>
    constexpr bool NearlyEquals (T1 l, T2 r)
        requires (is_arithmetic_v<T1> and is_arithmetic_v<T2>)
    {
        using TC = common_type_t<T1, T2>;
        if constexpr (floating_point<TC>) {
            constexpr TC kEpsilon_ = 10000 * numeric_limits<TC>::epsilon (); // pick more than epsilon cuz some math functions have more error than a single bit... - even 1000x not enuf sometimes
            // \see https://realtimecollisiondetection.net/blog/?p=89
            // using relTol = absTol
            TC useEpsilon = kEpsilon_ * std::max<TC> ({static_cast<TC> (1.0), static_cast<TC> (Abs (l)), static_cast<TC> (Abs (r))});
            return NearlyEquals (l, r, useEpsilon);
        }
        else {
            return l == r;
        }
    }

    /*
     ********************************************************************************
     ************************* Math::PinToSpecialPoint ******************************
     ********************************************************************************
     */
    template <floating_point T>
    T PinToSpecialPoint (T p, T special)
    {
        if (Math::NearlyEquals (p, special)) {
            return special;
        }
        return p;
    }
    template <floating_point T>
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
     ******************************** Math::AtMost **********************************
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
    constexpr NEW_T PinToMaxForType (T initialValue)
    {
        using LargerType = decltype (NEW_T{} + T{}); // maybe should use conditional<> for this?
        return static_cast<NEW_T> (min<LargerType> (initialValue, numeric_limits<NEW_T>::max ()));
    }

    /*
     ********************************************************************************
     ********************************** Math::Abs ***********************************
     ********************************************************************************
     */
    template <typename T, typename RESULT_TYPE>
    constexpr RESULT_TYPE Abs (T v)
        requires (is_arithmetic_v<T>)
    {
#if __cplusplus >= kStrokia_Foundation_Configuration_cplusplus_23
        if constexpr (Configuration::IAnyOf<T, int, intmax_t>) {
            return std::abs (v);
        }
        else if constexpr (Configuration::IAnyOf<T, long>) {
            return std::labs (v);
        }
        else if constexpr (Configuration::IAnyOf<T, long long>) {
            return std::llabs (v);
        }
#endif
        return v < 0 ? -v : v;
    }

    /*
     ********************************************************************************
     ********************************** Math::IsOdd *********************************
     ********************************************************************************
     */
    template <integral T>
    constexpr bool IsOdd (T v)
    {
        return v % 2 == 1;
    }

    /*
     ********************************************************************************
     ********************************* Math::IsEven *********************************
     ********************************************************************************
     */
    template <integral T>
    constexpr bool IsEven (T v)
    {
        static_assert (integral<T>);
        return v % 2 == 0;
    }

    /*
     ********************************************************************************
     ******************************** Math::IsPrime *********************************
     ********************************************************************************
     */
    template <integral T>
    bool IsPrime (T v)
    {
        Require (v >= 0); // no negative numbers
        // @todo - redo this as http://en.wikipedia.org/wiki/Sieve_of_Eratosthenes but this is simpler and
        // has no memory requirements and is good enuf for me to test...
        if (v == 1) {
            return false;
        }
        if (v == 2) {
            return true; // special case
        }
        T checkUpTo = static_cast<T> (::sqrt (v)) + static_cast<T> (1);
        // Check each number from 3 up to checkUpTo and see if its a divisor
        for (T d = 2; d <= checkUpTo; ++d) {
            if (v % d == 0) {
                return false;
            }
        }
        return true;
    }

}
