/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_TimeOutException_h_
#define _Stroika_Foundation_Execution_TimeOutException_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <condition_variable> // for cv_status

#include "Stroika/Foundation/Time/Realtime.h"

#include "Exceptions.h"

namespace Stroika::Foundation::Execution {

    /**
     *  Throw this when something (typically a waitable event, but could  be anything code is waiting for) times out.
     *
     *  \note - Many low level functions map OS / platform exceptions to this type as appropriate (via @see ThrowPOSIXErrNo and @see ThrowSystemErrNo)
     *
     *  \note Though you can use
     *      \code
     *          catch (const TimeOutException&) {
     *              ...
     *          }
     *      \endcode
     *
     *      and that will work for catching nearly any timeout exception thrown by Stroika, it is often
     *      better to use:
     *
     *      \code
     *          catch (const system_error& e) {
     *              if (e.code () == errc::timed_out) { // and maybe check errc::stream_timeout
     *                  ...
     *              }
     *          }
     *      \endcode
     *
     *      since that will catch timeout exceptions thrown by non-Stroika-based components, and this form
     *      will still work with Activities and UNICODE exception message handling, if you use
     *      Characters::ToString (e); alternatively you can use:
     *          catch (const SystemErrorException& e) {
     *              if (e.code () == errc::timed_out) {
     *                  ...
     *              }
     *              // now directly look at String message and Activies etc...
     *          }
     *          catch (const system_error& e) {
     *              if (e.code () == errc::timed_out) {
     *                  ...
     *              }
     *          }
     *
     */
    class TimeOutException : public Execution::SystemErrorException<> {
    public:
        /**
         *  when not specified, the error_code defaults to make_error_code (errc::timed_out)
         */
        TimeOutException ();
        TimeOutException (error_code ec);
        TimeOutException (const Characters::String& message);
        TimeOutException (error_code ec, const Characters::String& message);

    public:
        /**
         *  CAN be used when there is no message argument.
         */
        static const TimeOutException kThe;
    };
    inline const TimeOutException TimeOutException::kThe;

    /**
     *  \brief  Execution::Throw (Execution::TimeOutException::kThe);
     *          but can be more easily forward-declared, so no include deadly embrace
     */
    void ThrowTimeOutException ();

    /**
     *  \brief  Throw TimeOutException if the @Time::GetTickCount () is >= the given value.
     *
     *  This function facilitates writing code like:
     *      Time::DurationSeconds timeoutAfter =   Time::GetTickCount () + 1.0;
     *      do_someting_dont_know_how_long_it_will_take();
     *      Execution::ThrowTimeoutExceptionAfter (timeoutAfter);
     *
     *  \note   ***Cancelation Point***
     */
    template <typename EXCEPTION>
    void ThrowTimeoutExceptionAfter (Time::TimePointSeconds afterTickCount, EXCEPTION&& exception2Throw);
    void ThrowTimeoutExceptionAfter (Time::TimePointSeconds afterTickCount);

    /**
     *  Translate timed_mutex, or recursive_timed_mutex try_lock_until () calls which fail into TimeOutException exceptions.
     */
    template <typename TIMED_MUTEX, typename EXCEPTION>
    void TryLockUntil (TIMED_MUTEX& m, Time::TimePointSeconds afterTickCount, EXCEPTION&& exception2Throw);
    template <typename TIMED_MUTEX>
    void TryLockUntil (TIMED_MUTEX& m, Time::TimePointSeconds afterTickCount);

    /**
     *  \note - this function may not be called outside the context of a running main.
     */
    template <typename EXCEPTION>
    void ThrowIfTimeout (cv_status conditionVariableStatus, EXCEPTION&& exception2Throw);
    void ThrowIfTimeout (cv_status conditionVariableStatus);

    /**
     *  Simple wrapper on construction of unique_lock<TIMED_MUTEX> - which translates the timeout into a TimeOutException.
     * 
     *  \note if this function returns (doesn't throw) - the required unique_lock<> OWNS the mutex.
     *
     *  \see also TryLockUntil
     *  \see also TimedLockGuard
     */
    template <typename TIMED_MUTEX, typename EXCEPTION>
    unique_lock<TIMED_MUTEX> UniqueLock (TIMED_MUTEX& m, const chrono::duration<double>& d, EXCEPTION&& exception2Throw);
    template <typename TIMED_MUTEX>
    unique_lock<TIMED_MUTEX> UniqueLock (TIMED_MUTEX& m, const chrono::duration<double>& d);

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "TimeOutException.inl"

#endif /*_Stroika_Foundation_Execution_TimeOutException_h_*/
