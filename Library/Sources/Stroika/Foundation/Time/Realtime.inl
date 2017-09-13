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

            /*
             ********************************************************************************
             ***************************** time_point2DurationSeconds ***********************
             ********************************************************************************
             */
            template <class Clock, class Duration>
            inline DurationSecondsType time_point2DurationSeconds (const time_point<Clock, Duration>& tp)
            {
                return std::chrono::duration<DurationSecondsType>{tp.time_since_epoch ()}.count ();
            }

            /*
             ********************************************************************************
             ***************************** DurationSeconds2time_point ***********************
             ********************************************************************************
             */
            template <class Clock, class Duration>
            inline time_point<Clock, Duration> DurationSeconds2time_point (DurationSecondsType t)
            {
                return time_point<Clock, Duration> (Duration (duration<DurationSecondsType>{t}));
            }
        }
    }
}
#endif /*_Stroika_Foundation_Time_Realtime_inl_*/
