/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_TimeOutException_h_
#define _Stroika_Foundation_Execution_TimeOutException_h_  1

#include    "../StroikaPreComp.h"

#include    "../Time/Realtime.h"

#include    "Exceptions.h"
#include    "StringException.h"



namespace   Stroika {
    namespace   Foundation {
        namespace   Execution {


            /**
             *  Throw this when something (typically a waitable event, but could  be anything code is waiting for) times out.
             *
             *  \note - Many low level functions map OS / platform exceptions to this type as appropriate
             */
            class   TimeOutException : public StringException {
            public:
                TimeOutException ();
                TimeOutException (const Characters::String& message);
            };


            /**
             *  \brief  Throw TimeOutException if the @Time::GetTickCount () is >= the given value.
             *
             *  This function facilitates writing code like:
             *      Time::DurationSeconds timeoutAfter =   Time::GetTickCount () + 1.0;
             *      do_someting_dont_know_how_long_it_will_take();
             *      Execution::ThrowTimeoutExceptionAfter (timeoutAfter);
             *
             *  \note   This always calls CheckForThreadAborting () at least once, but may not call Sleep() if not needed.
             */
            template    <typename   EXCEPTION>
            void    ThrowTimeoutExceptionAfter (Time::DurationSecondsType afterTickCount, const EXCEPTION& exception2Throw = EXCEPTION ());
            void    ThrowTimeoutExceptionAfter (Time::DurationSecondsType afterTickCount);


            /**
             *  Translate timed_mutex, or recursive_timed_mutex try_lock_until () calls which fail into TimeOutException exceptions.
             */
            template    <typename   TIMED_MUTEX, typename   EXCEPTION>
            void    TryLockUntil (TIMED_MUTEX& m, Time::DurationSecondsType afterTickCount, const EXCEPTION& exception2Throw = EXCEPTION ());
            template    <typename   TIMED_MUTEX>
            void    TryLockUntil (TIMED_MUTEX& m, Time::DurationSecondsType afterTickCount);


        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "TimeOutException.inl"

#endif  /*_Stroika_Foundation_Execution_TimeOutException_h_*/
