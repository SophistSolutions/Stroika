/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_Timeout_h_
#define _Stroika_Foundation_Execution_Timeout_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <condition_variable> // for cv_status

#include "Stroika/Foundation/Execution/Exceptions.h"
#include "Stroika/Foundation/Time/Realtime.h"

namespace Stroika::Foundation::Execution {

    /**
     *  \brief  How Stroika reports a timeout.
     *
     *  Stroika has no dedicated timeout exception TYPE. A timeout is reported the way every other error is -
     *  as a SystemErrorException (which IS a std::system_error) carrying errc::timed_out - so it is caught
     *  and tested the standard C++ way:
     *
     *      \code
     *          catch (const system_error& e) {
     *              if (e.code () == errc::timed_out) {
     *                  ...
     *              }
     *          }
     *      \endcode
     *
     *  To raise one, say @see ThrowError (errc::timed_out).
     *
     *  \note   Test the CONDITION (errc::timed_out), NEVER a particular code such as
     *          error_code{ETIMEDOUT, system_category ()}. Only the condition matches timeouts from every
     *          source - libcurl, getaddrinfo, HRESULT, the OS, and Stroika's own waiting primitives.
     *          @see ThrowError for why, and for what errc::timed_out means to Stroika.
     */

    /**
     *  \brief  Throw a timeout (@see ThrowError (errc::timed_out)) if @Time::GetTickCount () is >= the given value.
     *
     *  This function facilitates writing code like:
     *      Time::TimePointSeconds timeoutAfter =   Time::GetTickCount () + 1.0;
     *      do_someting_dont_know_how_long_it_will_take();
     *      Execution::ThrowTimeoutExceptionAfter (timeoutAfter);
     *
     *  \note   ***Cancelation Point***
     */
    template <typename EXCEPTION>
    void ThrowTimeoutExceptionAfter (Time::TimePointSeconds afterTickCount, EXCEPTION&& exception2Throw);
    void ThrowTimeoutExceptionAfter (Time::TimePointSeconds afterTickCount);

    /**
     *  \brief  Throw a timeout (@see ThrowError (errc::timed_out)) if the given condition_variable wait timed out.
     *
     *  \note - this function may not be called outside the context of a running main.
     */
    template <typename EXCEPTION>
    void ThrowIfTimeout (cv_status conditionVariableStatus, EXCEPTION&& exception2Throw);
    void ThrowIfTimeout (cv_status conditionVariableStatus);

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Timeout.inl"

#endif /*_Stroika_Foundation_Execution_Timeout_h_*/
