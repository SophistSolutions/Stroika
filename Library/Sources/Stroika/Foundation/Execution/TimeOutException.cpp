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
    : Exception (L"Timeout Expired"sv)
{
}

TimeOutException::TimeOutException (const Characters::String& message)
    : Exception (message)
{
}
