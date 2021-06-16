/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include "TimeOutException.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Execution;

/*
 ********************************************************************************
 ********************************* TimeOutException *****************************
 ********************************************************************************
 */
#if qCompiler_cpp17InlineStaticMemberOfClassDoubleDeleteAtExit_Buggy
const TimeOutException TimeOutException::kThe;
#endif
TimeOutException::TimeOutException ()
    : TimeOutException{L"Timeout Expired"sv}
{
}

TimeOutException::TimeOutException (error_code ec)
    : TimeOutException{ec, L"Timeout Expired"sv}
{
}

TimeOutException::TimeOutException (const Characters::String& message)
    : TimeOutException{make_error_code (errc::timed_out), message}
{
}

/*
 ********************************************************************************
 ************************ Execution::ThrowTimeOutException **********************
 ********************************************************************************
 */
void Execution::ThrowTimeOutException ()
{
    Throw (TimeOutException::kThe);
}
