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
     ***************************** DEPRECATED (v3.0d25) *****************************
     ********************************************************************************
     */
    /**
     *  \deprecated Since Stroika v3.0d25 - use ThrowError (errc::timed_out).
     */
    [[deprecated ("Since Stroika v3.0d25 - use ThrowError (errc::timed_out)")]] [[noreturn]] void ThrowTimeOutException ();

    DISABLE_COMPILER_MSC_WARNING_START (4996);
    DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Wdeprecated-declarations\"");
    DISABLE_COMPILER_CLANG_WARNING_START ("clang diagnostic ignored \"-Wdeprecated-declarations\"");
    /**
     *  \deprecated Since Stroika v3.0d25 - catch the CONDITION instead of this type:
     *      \code
     *          catch (const system_error& e) {
     *              if (e.code () == errc::timed_out) { ... }
     *          }
     *      \endcode
     *      That also matches timeouts raised outside Stroika, which catching by type never did.
     */
    class [[deprecated ("Since Stroika v3.0d25 - catch (const system_error&) and test e.code () == errc::timed_out")]] TimeOutException
        : public Execution::SystemErrorException {
    public:
        TimeOutException ();
        TimeOutException (error_code ec);
        TimeOutException (const Characters::String& message);
        TimeOutException (error_code ec, const Characters::String& message);

    public:
        static const TimeOutException kThe;
    };
    inline const TimeOutException TimeOutException::kThe;
    DISABLE_COMPILER_MSC_WARNING_END (4996);
    DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Wdeprecated-declarations\"");
    DISABLE_COMPILER_CLANG_WARNING_END ("clang diagnostic ignored \"-Wdeprecated-declarations\"");

    /*
     ********************************************************************************
     ******************* Execution::TimeOutException [[deprecated]] *****************
     ********************************************************************************
     */
    DISABLE_COMPILER_MSC_WARNING_START (4996);
    DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Wdeprecated-declarations\"");
    DISABLE_COMPILER_CLANG_WARNING_START ("clang diagnostic ignored \"-Wdeprecated-declarations\"");
    inline TimeOutException::TimeOutException (error_code ec, const Characters::String& message)
        : SystemErrorException{ec, message}
    {
    }
    DISABLE_COMPILER_MSC_WARNING_END (4996);
    DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Wdeprecated-declarations\"");
    DISABLE_COMPILER_CLANG_WARNING_END ("clang diagnostic ignored \"-Wdeprecated-declarations\"");

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
        if (not m.try_lock_until (Time::Pin2SafeSeconds (afterTickCount))) {
            ThrowError (errc::timed_out);
        }
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
        if (conditionVariableStatus == cv_status::timeout) {
            ThrowError (errc::timed_out);
        }
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
        unique_lock<TIMED_MUTEX> lock{m, d};
        if (not lock.owns_lock ()) {
            ThrowError (errc::timed_out);
        }
        return lock;
    }

}
