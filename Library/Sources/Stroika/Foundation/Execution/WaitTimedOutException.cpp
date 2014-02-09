/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    "../Characters/String_Constant.h"

#include    "WaitTimedOutException.h"



using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Execution;

using   Characters::String_Constant;


/*
 ********************************************************************************
 ***************************** WaitTimedOutException ****************************
 ********************************************************************************
 */
WaitTimedOutException::WaitTimedOutException ()
    : StringException (String_Constant (L"WAIT timed out"))
{
}

