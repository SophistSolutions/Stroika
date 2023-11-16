/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#ifndef _Stroika_Foundation_Time_Realtime_inl_
#define _Stroika_Foundation_Time_Realtime_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#include "../Debug/Assertions.h"
#include "../Math/Common.h"

namespace Stroika::Foundation::Time {

    namespace Private_ {
        template <typename Clock>
        DurationSecondsType GetClockTickountOffset_ ();

        template <>
        DurationSecondsType GetClockTickountOffset_<chrono::steady_clock> ();
    }

    /*
     ********************************************************************************
     ***************************** time_point2DurationSeconds ***********************
     ********************************************************************************
     */
    template <typename Clock, typename Duration>
    inline DurationSecondsType time_point2DurationSeconds (const time_point<Clock, Duration>& tp)
    {
        //
        // nb: AtLeast() needed because first time through, steady_clock::now () for param called before one inside GetClockTickountOffset_
        //
        // Cannot use  Math::AtLeast<DurationSecondsType> <> () because according to IEEE floating point -0 == 0 (but still prints as neg zero)
        // So do more carefully creafted if-test instead
        //
        using chrono::duration;
        DurationSecondsType tmp = duration<DurationSecondsType>{tp.time_since_epoch ()}.count () - Private_::GetClockTickountOffset_<Clock> ();
        if (tmp <= 0) {
            tmp = 0;
        }
        return tmp;
    }

    /*
     ********************************************************************************
     ***************************** DurationSeconds2time_point ***********************
     ********************************************************************************
     */
    template <typename Clock, typename Duration>
    time_point<Clock, Duration> DurationSeconds2time_point (DurationSecondsType t)
    {
        Require (t >= 0);
        /*
         *  @See https://stroika.atlassian.net/browse/STK-619
         *
         *  Tricky todo (maybe should use boost::numeric_cast<>) - but convert floating point # to duration without overflow.
         *  Err on the side of caution.
         *
         *  OFTEN - the arg 't' is MAX-DOUBLE (so 10E308) - and this gets converted to int64_t (using ratio nanoseconds - so effectively two int32_t s)
         *
         *  From /usr/include/c++/7.2.0/condition_variable:
         *       template<typename _Clock, typename _Duration>
         *       cv_status
         *       wait_until(unique_lock<mutex>& __lock,
         *       const chrono::time_point<_Clock, _Duration>& __atime)
         *       {
         *       // DR 887 - Sync unknown clock to known clock.
         *       const typename _Clock::time_point __c_entry = _Clock::now();
         *       const __clock_t::time_point __s_entry = __clock_t::now();
         *       const auto __delta = __atime - __c_entry;
         *       const auto __s_atime = __s_entry + __delta;
         */
        using chrono::duration;
        using chrono::duration_cast;
        using chrono::time_point;
// @todo - understand why (on windows at least) regtest
// Verify (Time::DurationSeconds2time_point (Time::GetTickCount () + Time::kInfinite) == time_point<chrono::steady_clock>::max ());
// fails but the alternative below it doesn't.
// Issue is tmp variable convert from float to int, somehow goes negative... Not sure my bug or MSFT - must look more carefully
#if 0
        time_point<Clock, Duration> tmp = time_point<Clock, Duration> (duration_cast<Duration> (duration<DurationSecondsType>{t + Private_::GetClockTickountOffset_<Clock> ()}));
        return Math::PinInRange (tmp, time_point<Clock, Duration>::min (), time_point<Clock, Duration>::max ());
#else
        DurationSecondsType dMin{};
        DurationSecondsType dMax = duration_cast<duration<DurationSecondsType>> (Duration::max ()).count ();
        dMax = numeric_limits<int32_t>::max (); // not cuz really needed logically, but because getting this to not overflow is complex, and this gives us a LONG time (about 80 years)
        DurationSecondsType dv = t + Private_::GetClockTickountOffset_<Clock> ();
        if (dv <= dMin) {
            return time_point<Clock, Duration> (duration_cast<Duration> (chrono::duration<DurationSecondsType>{dMin}));
        }
        if (dv >= dMax) {
            return time_point<Clock, Duration> (duration_cast<Duration> (chrono::duration<DurationSecondsType>{dMax}));
        }
        return time_point<Clock, Duration> (duration_cast<Duration> (chrono::duration<DurationSecondsType>{dv}));
#endif
    }

    /*
     ********************************************************************************
     ******************************* Time::GetTickCount *****************************
     ********************************************************************************
     */
    inline DurationSecondsType GetTickCount () noexcept
    {
        try {
            return time_point2DurationSeconds (chrono::steady_clock::now ());
        }
        catch (...) {
            // Cannot DbgTrace here because DbgTrace () calls this...
            return 0;
        }
    }
    inline DurationSecondsTimePoint New_GetTickCount () noexcept
    {
        return chrono::steady_clock::now ();
    }

}

#endif /*_Stroika_Foundation_Time_Realtime_inl_*/
