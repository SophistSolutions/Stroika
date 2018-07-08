/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Math_Common_h_
#define _Stroika_Foundation_Math_Common_h_ 1

#include "../StroikaPreComp.h"

#include <algorithm>
#include <cmath> // though perhaps not strictly needed, probably wanted if including Stroika/Foundation/Math/Common.h
#include <limits>
#include <type_traits>

/**
 *  TODO:
 *      @todo   This rounding code (RoundUpTo/RoundDownTo) doesn't work for floating point numbers.
 *              That should either be fixed or documented better why.
 *
 *      @todo   Think through (and reconsider) the epsilon parameter (10000 times numeric_limits<>::epsilon())
 *              to NearlyEquals ().
 */
namespace Stroika::Foundation {
    using namespace std;
}
namespace Stroika::Foundation::Math {

    using std::numeric_limits;

    /**
     */
    template <typename T>
    T nan ();

    constexpr double kE  = 2.71828182845904523536;
    constexpr double kPi = 3.14159265358979323846;

    /**
     * RoundUpTo() - round towards positive infinity.
     *
     *  T can be any of int, long, or long long, or unsigned int, or unsigned long, or unsigned long long
     *  (after review of this API/implementation, probably should add short/char with unsigned variants
     *  to this list)
     */
    template <typename T>
    T RoundUpTo (T x, T toNearest);

    /**
     *  RoundDownTo() - round towards negative infinity.
     *
     *  T can be any of int, long, or long long, or unsigned int, or unsigned long, or unsigned long long
     *  (after review of this API/implementation, probably should add short/char with unsigned variants
     *  to this list)
     */
    template <typename T>
    T RoundDownTo (T x, T toNearest);

    /**
     *  Convert from a floating point value to an integer value - like std::round () - except that round () returns a floating
     *  point value that must be manually converted to an integer. That conversion - if there is overflow - is undefined.
     *
     *  From http://open-std.org/jtc1/sc22/wg21/docs/papers/2013/n3797.pdf - section 4.9 Floating-integral conversion
     *      The conversion truncates; that is, the fractional part is discarded. The behavior is undefined
     *      if the truncated value cannot be represented in the destination type
     */
    template <typename INT_TYPE, typename FLOAT_TYPE>
    INT_TYPE Round (FLOAT_TYPE x);

    /**
     *  NearlyEquals() can be used as a utility for floating point comparisons.
     *  But, so that it is more effective with templates, it can also be used with any type,
     *  and becomes require equality (==).
     */
    template <typename T1, typename T2, typename TC = typename std::common_type<T1, T2>::type>
    bool NearlyEquals (T1 l, T2 r, enable_if_t<is_floating_point_v<TC>>* = 0);
    template <typename T1, typename T2, typename EPSILON_TYPE, typename TC = typename std::common_type<T1, T2>::type>
    bool NearlyEquals (T1 l, T2 r, EPSILON_TYPE epsilon, enable_if_t<is_floating_point_v<TC>>* = 0);
    template <typename T1, typename T2, typename TC = typename std::common_type<T1, T2>::type>
    bool NearlyEquals (T1 l, T2 r, enable_if_t<is_integral_v<TC>>* = 0);
    template <typename T1, typename T2, typename TC = typename std::common_type<T1, T2>::type>
    bool NearlyEquals (T1 l, T2 r, enable_if_t<!std::is_integral_v<TC> && !is_floating_point_v<TC>>* = 0);

    /**
     *  \brief  PinToSpecialPoint() returns its first argument, or someting NearlyEquals() to it (but better)
     *
     *  Sometimes with floating point arithmatic you get points to move slightly. For example, if you
     *  want a number between 0.0 and 1.0, you might do some arithmatic and get -0.000000001; This might
     *  cause the value to be out of the range 0..1. You might want to assert/assure the value never goes below
     *  zero or above one, but this is outside that range.
     *
     *  This helper allows values near a special value (like the endpoint of that range) to 'pin' to be
     *  exactly that endpoint.
     *
     *  But PinToSpecialPoint () always returns its first argument, or something NearlyEquals() to it.
     */
    template <typename T>
    T PinToSpecialPoint (T p, T special);
    template <typename T>
    T PinToSpecialPoint (T p, T special, T epsilon);

    /**
     *  Compare initialValue with lowerBound and upperBound with min/max/operator<, and return a
     *  value which equals initialValue, or lowerBound or upperBound to get it into that range.
     *
     *  @see Range<>::Pin ()
     *  @see AtLeast
     *  @see AtMost
     */
    template <typename T>
    T PinInRange (T initialValue, T lowerBound, T upperBound);

    /**
     *  Return a value at this at least the given value. This is the same as "max" as it turns out,
     *  but writing it this way I find more clear
     *
     *  @see PinInRange
     *  @see AtLeast
     */
    template <typename T>
    T AtLeast (T initialValue, T lowerBound);

    /**
     *  Return a value at this at no more than the given value. This is the same as "min" as it turns out,
     *  but writing it this way I find more clear
     *
     *  @see PinInRange
     *  @see AtLeast
     */
    template <typename T>
    T AtMost (T initialValue, T upperBound);

    /**
     *  This returns the given value, as the new type NEW_T, but if the value wont fit in NEW_T, it returns
     *  the largest value that would.
     *
     *  This is handy for stuff like ::write() system calls, where you might be given a ptrdiff_t, and have to
     *  call something taking int, and given how these types might be differnt sizes, it simplifies picking
     *  the right size to call write with.
     *
     *  \par Example Usage
     *      \code
     *          size_t  nRequested  =   intoEnd - intoStart;
     *          return static_cast<size_t> (Execution::ThrowErrNoIfNegative (::_read (fFD_, intoStart, Math::PinToMaxForType<unsigned int> (nRequested))));
     *      \endcode
     */
    template <typename NEW_T, typename T>
    NEW_T PinToMaxForType (T initialValue);

    /**
     *      \note   we define this as an alertnative to std::abs () - since that is not extendible to other types (just an overload for floating point and int types)
     *
     *  \note   when we port stroika bignum package - this shoudl support those bignums.
     */
    template <typename T, typename RESULT_TYPE = T>
    RESULT_TYPE Abs (T v);

    /**
     *  \note - when we port stroika bignum package - this shoudl support those bignums.
     */
    template <typename T>
    bool IsOdd (T v);

    /**
     *  Note - when we port stroika bignum package - this shoudl support those bignums.
     */
    template <typename T>
    bool IsEven (T v);

    /**
     *  Note - when we port stroika bignum package - this shoudl support those bignums.
     *
     *  @todo   Also - we want multiple algiorths for this - including simple search (done now), sieve, and
     *          partitioned sieves (so use less ram).
     *
     *  \req v >= 0
     */
    template <typename T>
    bool IsPrime (T v);
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Common.inl"

#endif /*_Stroika_Foundation_Math_Common_h_*/
