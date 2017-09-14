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
            inline time_point<Clock, Duration> DurationSeconds2time_point (DurationSecondsType t)
            {
                Require (t >= 0);
                using std::chrono::duration;
                return time_point<Clock, Duration> (std::chrono::duration_cast<Duration> (duration<DurationSecondsType>{t + Private_::GetClockTickountOffset_<Clock> ()}));
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
