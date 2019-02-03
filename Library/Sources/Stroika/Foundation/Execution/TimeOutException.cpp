/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include "../Characters/String_Constant.h"

#include "TimeOutException.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Execution;

/*
 ********************************************************************************
 ********************************* TimeOutException *****************************
 ********************************************************************************
 */
const TimeOutException TimeOutException::kThe;

TimeOutException::TimeOutException ()
    : TimeOutException (L"Timeout Expired"sv)
{
}

TimeOutException::TimeOutException (error_code ec)
    : TimeOutException (ec, L"Timeout Expired"sv)
{
}

TimeOutException::TimeOutException (const Characters::String& message)
    : TimeOutException (make_error_code (errc::timed_out), message)
{
}
