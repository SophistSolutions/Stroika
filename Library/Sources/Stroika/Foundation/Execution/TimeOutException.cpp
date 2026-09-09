/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include "TimeOutException.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Execution;

/*
 ********************************************************************************
 ******************** TimeOutException [[deprecated]] ***************************
 ********************************************************************************
 */
DISABLE_COMPILER_MSC_WARNING_START (4996);
DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Wdeprecated-declarations\"");
DISABLE_COMPILER_CLANG_WARNING_START ("clang diagnostic ignored \"-Wdeprecated-declarations\"");
TimeOutException::TimeOutException ()
    : TimeOutException{"Timeout Expired"sv}
{
}

TimeOutException::TimeOutException (error_code ec)
    : TimeOutException{ec, "Timeout Expired"sv}
{
}

TimeOutException::TimeOutException (const Characters::String& message)
    : TimeOutException{make_error_code (errc::timed_out), message}
{
}
DISABLE_COMPILER_MSC_WARNING_END (4996);
DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Wdeprecated-declarations\"");
DISABLE_COMPILER_CLANG_WARNING_END ("clang diagnostic ignored \"-Wdeprecated-declarations\"");

/*
 ********************************************************************************
 ************************ Execution::ThrowTimeOutException **********************
 ********************************************************************************
 */
DISABLE_COMPILER_MSC_WARNING_START (4996);
DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Wdeprecated-declarations\"");
DISABLE_COMPILER_CLANG_WARNING_START ("clang diagnostic ignored \"-Wdeprecated-declarations\"");
void Execution::ThrowTimeOutException ()
{
    // Deliberately via ThrowError () rather than throwing directly, so this goes through the ONE place that
    // applies the documented promotions - which currently still promotes errc::timed_out to the deprecated
    // TimeOutException. That is what keeps existing catch (const TimeOutException&) clauses matching Stroika's
    // own timeouts. When that promotion is finally removed, this needs no change.
    // Also note the exception object is constructed at the THROW POINT (inside ThrowError), not shared: Exception<>
    // captures the current Activity stack when CONSTRUCTED, so the old shared static TimeOutException::kThe
    // always carried an empty one.
    ThrowError (errc::timed_out);
}
DISABLE_COMPILER_MSC_WARNING_END (4996);
DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Wdeprecated-declarations\"");
DISABLE_COMPILER_CLANG_WARNING_END ("clang diagnostic ignored \"-Wdeprecated-declarations\"");
