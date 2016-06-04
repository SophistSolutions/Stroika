/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    "UserCanceledException.h"



using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Execution;



/*
 ********************************************************************************
 *************************** UserCanceledException ******************************
 ********************************************************************************
 */
const   UserCanceledException   UserCanceledException::kThe;



/*
 ********************************************************************************
 ******************************** Execution::Throw ******************************
 ********************************************************************************
 */
template    <>
void    _NoReturn_  Execution::Throw (const UserCanceledException& e2Throw)
{
    DbgTrace (SDKSTR ("Throwing UserCanceledException"));
    throw e2Throw;
}

