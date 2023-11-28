/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#ifndef _Stroika_Foundation_Time_Common_h_
#define _Stroika_Foundation_Time_Common_h_ 1

#include "../StroikaPreComp.h"

#include <chrono>
#include <ctime>

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md">Alpha-Late</a>

 */

namespace Stroika::Foundation::Time {

    /**
     *    \note   Design Note
     *            About time_t
     *
     *            This is generally a 64-bit (but sometimes, like for 32-bit AIX, its 32-bit) signed (but not guaranteed signed) quantity. It generally
     *            represents a number of seconds since midnight Jan 1, 1970 in UTC.
     *
     *            From http://en.cppreference.com/w/cpp/chrono/c/time_t
     *                Arithmetic type capable of representing times.
     *
     *            Although not defined, this is almost always an integral value holding the number of seconds 
     *            (not counting leap seconds) since 00:00, Jan 1 1970 UTC, corresponding to POSIX time.
     *
     *            Because of this, basically ALL the Stroika APIs (unless an obvious or stated exception) treat time_t as
     *            in UTC.
     *
     *            The Stroika APIs do NOT assume 64-bit, nor signedness, but when we require signedness, we use make_signed_t<time_t>
     */

    /**
     *  Several APIs internally (or explicitly) use duration_cast, and frequently those APIs do duration_cast to
     *  chrono::seconds.
     * 
     *     https://en.cppreference.com/w/cpp/chrono/duration/duration_cast
     *         Casting from a floating-point duration to an integer duration is subject to undefined behavior when the 
     *         floating-point value is NaN, infinity, or too large to be representable by the target's integer type. 
     *         Otherwise, casting to an integer duration is subject to truncation as with any static_cast to an integer type.
     * 
     *  In particular, Stroika makes use of INF values (Time::kInfinity) for durations because it so easily works with
     *  TimePoints and adding etc.
     * 
     *  But - we must be careful when calling std c++ APIs (like timed_mutex::wait_until()) to provide conforming timepoint values
     *  (even though https://en.cppreference.com/w/cpp/thread/condition_variable/wait_until says nothing about passing in inf floating point values).
     * 
     *  With Ubuntu 22.04, ubsan on clang++15 and libc++ seem to require this safety check.
     */
    template <typename CLOCK_T, typename DURATION_T = typename CLOCK_T::duration>
    auto Pin2SafeSeconds (const chrono::time_point<CLOCK_T, DURATION_T>& tp) -> chrono::time_point<CLOCK_T, DURATION_T>;

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Common.inl"

#endif /*_Stroika_Foundation_Time_Common_h_*/
