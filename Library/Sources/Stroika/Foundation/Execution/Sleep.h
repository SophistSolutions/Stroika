/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_Sleep_h_
#define _Stroika_Foundation_Execution_Sleep_h_  1

#include    "../StroikaPreComp.h"

#include    "../Configuration/Common.h"
#include    "../Time/Realtime.h"


namespace   Stroika {
    namespace   Foundation {
        namespace   Execution {

            /**
             * The portable Sleep() function - will wait the given amount of time - blocking the running thread.
             * It CAN be interupted. If interupted, one overload will return the amount of time remaining, allowing
             * easy re-sleeping. The other overload (/1) - will check for aborting, but otherwise keep sleeping
             * through interupts until the time has elapsed.
             *
             * Causes for interuption are platform specific. Some platform specific notes:
             *
             *  qPlatform_Windows:
             *      MAIN reason to use this - is it sets the 'alertable' flag on the sleep, so the
             *      QueueUserAPC () stuff works!  which allows Thread::Abort () to work properly...
             *
             *  qPlatform_POSIX:
             *      Sleep can be interupted by receipt of a signal (see POSIX nanosleep()). But the second form can
             *      be used to easily continue the sleep.
             */
            void    Sleep (Time::DurationSecondsType seconds2Wait);
            void    Sleep (Time::DurationSecondsType seconds2Wait, Time::DurationSecondsType* remainingInSleep);

        }
    }
}






/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "Sleep.inl"

#endif  /*_Stroika_Foundation_Execution_Sleep_h_*/
