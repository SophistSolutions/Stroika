/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_TimeOutException_inl_
#define _Stroika_Foundation_Execution_TimeOutException_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include <condition_variable>

namespace Stroika::Foundation::Execution {

    //redeclare to avoid having to #include Thread.h
    void CheckForThreadInterruption ();

    /*
     ********************************************************************************
     ******************* Execution::ThrowTimeoutExceptionAfter **********************
     ********************************************************************************
     */
    template <typename EXCEPTION>
    inline void ThrowTimeoutExceptionAfter (Time::DurationSecondsType afterTickCount, const EXCEPTION& exception2Throw)
    {
        if (Time::GetTickCount () > afterTickCount) {
            Throw (exception2Throw);
        }
        CheckForThreadInterruption ();
    }
    inline void ThrowTimeoutExceptionAfter (Time::DurationSecondsType afterTickCount)
    {
        static const TimeOutException kTO_ = TimeOutException ();
        ThrowTimeoutExceptionAfter (afterTickCount, kTO_);
    }

    /*
     ********************************************************************************
     ************************* Execution::TryLockUntil ******************************
     ********************************************************************************
     */
    template <typename TIMED_MUTEX, typename EXCEPTION>
    inline void TryLockUntil (TIMED_MUTEX& m, Time::DurationSecondsType afterTickCount, const EXCEPTION& exception2Throw)
    {
        if (not m.try_lock_until (afterTickCount)) {
            Throw (exception2Throw);
        }
    }
    template <typename TIMED_MUTEX>
    void TryLockUntil (TIMED_MUTEX& m, Time::DurationSecondsType afterTickCount)
    {
        static const TimeOutException kTO_ = TimeOutException ();
        TryLockUntil (m, afterTickCount, kTO_);
    }

    /*
     ********************************************************************************
     ************************* Execution::ThrowIfTimeout ****************************
     ********************************************************************************
     */
    template <typename EXCEPTION>
    inline void ThrowIfTimeout (cv_status conditionVariableStatus, const EXCEPTION& exception2Throw)
    {
        if (conditionVariableStatus == cv_status::timeout) {
            Throw (exception2Throw);
        }
    }
    inline void ThrowIfTimeout (cv_status conditionVariableStatus)
    {
        static const TimeOutException kTO_ = TimeOutException ();
        ThrowIfTimeout (conditionVariableStatus, kTO_);
    }

}

#endif /*_Stroika_Foundation_Execution_TimeOutException_inl_*/
