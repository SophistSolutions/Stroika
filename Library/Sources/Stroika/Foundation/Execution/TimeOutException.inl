/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_TimeOutException_inl_
#define _Stroika_Foundation_Execution_TimeOutException_inl_    1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */


namespace   Stroika {
    namespace   Foundation {
        namespace   Execution {


            //redeclare to avoid having to #include Thread.h
            void    CheckForThreadAborting ();


            /*
             ********************************************************************************
             ******************* Execution::ThrowTimeoutExceptionAfter **********************
             ********************************************************************************
             */
            template    <typename   EXCEPTION>
            inline  void    ThrowTimeoutExceptionAfter (Time::DurationSecondsType afterTickCount, const EXCEPTION& exception2Throw)
            {
                if (Time::GetTickCount () > afterTickCount) {
                    DoThrow (exception2Throw);
                }
                CheckForThreadAborting ();
            }
            inline  void    ThrowTimeoutExceptionAfter (Time::DurationSecondsType afterTickCount)
            {
                static  const   TimeOutException    kTO_    =   TimeOutException ();
                ThrowTimeoutExceptionAfter (afterTickCount, kTO_);
            }


            /*
             ********************************************************************************
             ************************* Execution::TryLockUntil ******************************
             ********************************************************************************
             */
            template    <typename   TIMED_MUTEX, typename   EXCEPTION>
            inline  void    TryLockUntil (TIMED_MUTEX& m, Time::DurationSecondsType afterTickCount, const EXCEPTION& exception2Throw)
            {
                if (not m.try_lock_until (afterTickCount)) {
                    DoThrow (exception2Throw);
                }
            }
            template    <typename   TIMED_MUTEX>
            void    TryLockUntil (TIMED_MUTEX& m, Time::DurationSecondsType afterTickCount)
            {
                static  const   TimeOutException    kTO_    =   TimeOutException ();
                TryLockUntil (m, afterTickCount, kTO_);
            }


        }
    }
}
#endif  /*_Stroika_Foundation_Execution_TimeOutException_inl_*/
