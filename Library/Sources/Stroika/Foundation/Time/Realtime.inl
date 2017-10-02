/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
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

namespace Stroika {
    namespace Foundation {
        namespace Time {

            namespace Private_ {
                template <class Clock>
                DurationSecondsType GetClockTickountOffset_ ();

                template <>
                DurationSecondsType GetClockTickountOffset_<std::chrono::steady_clock> ();
            }

            /*
             ********************************************************************************
             ***************************** time_point2DurationSeconds ***********************
             ********************************************************************************
             */
            template <class Clock, class Duration>
            inline DurationSecondsType time_point2DurationSeconds (const time_point<Clock, Duration>& tp)
            {
                // nb: AtLeast() needed because first time through, steady_clock::now () for param called before one inside GetClockTickountOffset_
                // Cannot use  Math::AtLeast<DurationSecondsType> <> () because according to IEEE floating point -0 == 0 (but still prints as neg zero)
                // So do more carefully creafted if-test instead
                using std::chrono::duration;
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
            template <class Clock, class Duration>
            time_point<Clock, Duration> DurationSeconds2time_point (DurationSecondsType t)
            {
                Require (t >= 0);
                using std::chrono::duration;
                using std::chrono::duration_cast;
                using std::chrono::time_point;
#if 1
                // @todo - understand why (on windows at least) regtest
                // Verify (Time::DurationSeconds2time_point (Time::GetTickCount () + Time::kInfinite) == time_point<chrono::steady_clock>::max ());
                // fails with below code - not above. Issue is tmp variable convert from float to int, somehow goes negative... Not sure my bug or MSFT - must look more carefully
                DurationSecondsType dMin = duration_cast<duration<DurationSecondsType>> (Duration::min ()).count ();
                DurationSecondsType dMax = duration_cast<duration<DurationSecondsType>> (Duration::max ()).count ();
                DurationSecondsType dv   = t + Private_::GetClockTickountOffset_<Clock> ();
                if (dv <= dMin) {
                    return time_point<Clock, Duration>::min ();
                }
                if (dv >= dMax) {
                    return time_point<Clock, Duration>::max ();
                }
                return time_point<Clock, Duration> (duration_cast<Duration> (std::chrono::duration<DurationSecondsType>{dv}));
#else
                time_point<Clock, Duration> tmp = time_point<Clock, Duration> (duration_cast<Duration> (duration<DurationSecondsType>{t + Private_::GetClockTickountOffset_<Clock> ()}));
                return Math::PinInRange (tmp, time_point<Clock, Duration>::min (), time_point<Clock, Duration>::max ());
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
                    return time_point2DurationSeconds (std::chrono::steady_clock::now ());
                }
                catch (...) {
                    // Cannot DbgTrace here because DbgTrace () calls this...
                    return 0;
                }
            }
        }
    }
}
#endif /*_Stroika_Foundation_Time_Realtime_inl_*/
