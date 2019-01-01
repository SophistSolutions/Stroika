/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include "../Characters/String_Constant.h"

#include "TimeOutException.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Execution;

using Characters::String_Constant;

/*
 ********************************************************************************
 ********************************* TimeOutException *****************************
 ********************************************************************************
 */
const TimeOutException TimeOutException::kThe;

TimeOutException::TimeOutException ()
    : StringException (String_Constant (L"Timeout Expired"))
{
}

TimeOutException::TimeOutException (const Characters::String& message)
    : StringException (message)
{
}
