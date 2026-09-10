/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
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
        // note the == part important, so the case of TimeoutSeconds == 0s works as 'no blocking'
        if (Time::GetTickCount () >= afterTickCount) [[unlikely]] {
            ThrowError (errc::timed_out);
        }
        Thread::CheckForInterruption ();
    }

    /*
     ********************************************************************************
     ************************* Execution::ThrowIfTimeout ****************************
     ********************************************************************************
     */
    template <typename EXCEPTION>
    inline void ThrowIfTimeout (cv_status conditionVariableStatus, EXCEPTION&& exception2Throw)
    {
        if (conditionVariableStatus == cv_status::timeout) {
            Throw (forward<EXCEPTION> (exception2Throw));
        }
    }
    inline void ThrowIfTimeout (cv_status conditionVariableStatus)
    {
        if (conditionVariableStatus == cv_status::timeout) {
            ThrowError (errc::timed_out);
        }
    }

}
