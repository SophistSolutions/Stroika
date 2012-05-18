/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    "WaitTimedOutException.h"



using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Execution;



/*
 ********************************************************************************
 ***************************** WaitTimedOutException ****************************
 ********************************************************************************
 */
WaitTimedOutException::WaitTimedOutException ()
    : StringException (L"WAIT timed out")
{
}

