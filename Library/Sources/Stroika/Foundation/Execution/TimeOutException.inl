/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include <condition_variable>

#include "Stroika/Foundation/Time/Common.h"

namespace Stroika::Foundation::Execution {

    //redeclare to avoid having to #include Thread.h
    namespace Thread {
        void CheckForInterruption ();
    }

    /*
     ********************************************************************************
     ************************* Execution::TimeOutException **************************
     ********************************************************************************
     */
    inline TimeOutException::TimeOutException (error_code ec, const Characters::String& message)
        : SystemErrorException{ec, message}
    {
    }

    /*
     ********************************************************************************
     ******************* Execution::ThrowTimeoutExceptionAfter **********************
     ********************************************************************************
     */
    template <typename EXCEPTION>
    inline void ThrowTimeoutExceptionAfter (Time::TimePointSeconds afterTickCount, EXCEPTION&& exception2Throw)
    {
        // note the == part important, so the case of TimeoutSeconds == 0s works as 'no blocking'
        if (Time::GetTickCount () >= afterTickCount) [[unlikely]] {
            Throw (forward<EXCEPTION> (exception2Throw));
        }
        Thread::CheckForInterruption ();
    }
    inline void ThrowTimeoutExceptionAfter (Time::TimePointSeconds afterTickCount)
    {
        ThrowTimeoutExceptionAfter (afterTickCount, TimeOutException::kThe);
    }

    /*
     ********************************************************************************
     ************************* Execution::TryLockUntil ******************************
     ********************************************************************************
     */
    template <typename TIMED_MUTEX, typename EXCEPTION>
    inline void TryLockUntil (TIMED_MUTEX& m, Time::TimePointSeconds afterTickCount, EXCEPTION&& exception2Throw)
    {
        if (not m.try_lock_until (Time::Pin2SafeSeconds (afterTickCount))) {
            Throw (forward<EXCEPTION> (exception2Throw));
        }
    }
    template <typename TIMED_MUTEX>
    void TryLockUntil (TIMED_MUTEX& m, Time::TimePointSeconds afterTickCount)
    {
        TryLockUntil (m, afterTickCount, TimeOutException::kThe);
    }

    /*
     ********************************************************************************
     ************************* Execution::ThrowIfTimeout ****************************
     ********************************************************************************
     */
    template <typename EXCEPTION>
    inline void ThrowIfTimeout (cv_status conditionVariableStatus, EXCEPTION& exception2Throw)
    {
        if (conditionVariableStatus == cv_status::timeout) {
            Throw (forward<EXCEPTION> (exception2Throw));
        }
    }
    inline void ThrowIfTimeout (cv_status conditionVariableStatus)
    {
        ThrowIfTimeout (conditionVariableStatus, TimeOutException::kThe);
    }

    /*
     ********************************************************************************
     ***************************** Execution::UniqueLock ****************************
     ********************************************************************************
     */
    template <typename TIMED_MUTEX, typename EXCEPTION>
    inline unique_lock<TIMED_MUTEX> UniqueLock (TIMED_MUTEX& m, const chrono::duration<double>& d, EXCEPTION&& exception2Throw)
    {
        unique_lock<TIMED_MUTEX> lock{m, d};
        if (not lock.owns_lock ()) {
            Throw (forward<EXCEPTION> (exception2Throw));
        }
        return lock;
    }
    template <typename TIMED_MUTEX>
    inline unique_lock<TIMED_MUTEX> UniqueLock (TIMED_MUTEX& m, const chrono::duration<double>& d)
    {
        return UniqueLock (m, d, TimeOutException::kThe);
    }

}
