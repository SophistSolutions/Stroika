/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_WaitTimedOutException_h_
#define _Stroika_Foundation_Execution_WaitTimedOutException_h_  1

#include    "../StroikaPreComp.h"

#include    "../Time/Realtime.h"

#include    "Exceptions.h"
#include    "StringException.h"



namespace   Stroika {
    namespace   Foundation {
        namespace   Execution {


            class   WaitTimedOutException : public StringException {
            public:
                WaitTimedOutException ();
            };


            /*
             *  Throw WaitTimedOutException if the @Time::GetTickCount () is >= the given value.
             *
             *  This function facilitates writing code like:
             *      Time::DurationSeconds timeoutAfter =   Time::GetTickCount () + 1.0;
             *      do_someting_dont_know_how_long_it_will_take();
             *      Execution::ThrowTimeoutExceptionAfter (timeoutAfter);
             *
             *  (NOTE - IN THE NEAR FUTURE WE WILL CHANGE EXCEPTION THROWN)
             *
             *  \note   This always calls CheckForThreadAborting () at least once, but may not call Sleep() if not needed.
             */
            inline  void    ThrowTimeoutExceptionAfter (Time::DurationSecondsType afterTickCount)
            {
                if (Time::GetTickCount () > afterTickCount) {
                    DoThrow (WaitTimedOutException ());
                }
            }


        }
    }
}



#endif  /*_Stroika_Foundation_Execution_WaitTimedOutException_h_*/
