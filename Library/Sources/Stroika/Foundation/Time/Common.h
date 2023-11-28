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
  * calls with ubsan on clang++15 and libc++ seem to require this safety check.
  * 
  * https://en.cppreference.com/w/cpp/thread/condition_variable/wait_until says nothing about passing in inf floating point values.
  * but internally it calls duration_cast<seconds> - and here the docs are clear: 
  * 
  *     https://en.cppreference.com/w/cpp/chrono/duration/duration_cast
  *         Casting from a floating-point duration to an integer duration is subject to undefined behavior when the 
  *         floating-point value is NaN, infinity, or too large to be representable by the target's integer type. 
  *         Otherwise, casting to an integer duration is subject to truncation as with any static_cast to an integer type.
  * 
  * Basically - this function should be used to sanitize possibly large (eg INF) time-values for timeouts before calling
  * lock wait_until() methods.
  */
    template <typename CLOCK_T, typename DURATION_T = typename CLOCK_T::duration>
    auto Pin2SafeSeconds (const chrono::time_point<CLOCK_T, DURATION_T>& tp) -> chrono::time_point<CLOCK_T, DURATION_T>
    {
        #if 1
        static constexpr auto kMin_ = chrono::time_point_cast<chrono::seconds>(chrono::time_point<CLOCK_T, DURATION_T>{chrono::seconds{chrono::seconds::min ().count()+1000}});;
        static constexpr auto kMax_ = chrono::time_point_cast<chrono::seconds>(chrono::time_point<CLOCK_T, DURATION_T>{chrono::seconds{chrono::seconds::max ().count()-1000}});;
        #else
        static constexpr auto kMin = chrono::time_point<CLOCK_T, DURATION_T>{chrono::seconds::min ()};
        static constexpr auto kMax_ = chrono::time_point<CLOCK_T, DURATION_T>{chrono::seconds::max ()};
        #endif
        typename DURATION_T::rep tpSeconds = tp.time_since_epoch ().count () *  DURATION_T::period::den /  DURATION_T::period::num;
        if (tpSeconds > static_cast<typename DURATION_T::rep> (chrono::seconds::max ().count ())) {
#if qDebug
                [[maybe_unused]]auto test  = chrono::time_point_cast<chrono::seconds>(kMax_);
#endif
            return kMax_;
        }
        if (tpSeconds < static_cast<typename DURATION_T::rep> (chrono::seconds::min ().count ())) [[unlikely]] {
#if qDebug
                [[maybe_unused]]auto test  = chrono::time_point_cast<chrono::seconds>(kMin_);
#endif
            return kMin_;
        }
        return tp;
    }
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Common.inl"

#endif /*_Stroika_Foundation_Time_Common_h_*/
