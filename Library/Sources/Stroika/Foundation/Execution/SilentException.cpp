/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    "SilentException.h"



using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Execution;



/*
 ********************************************************************************
 ******************************** Execution::DoThrow ****************************
 ********************************************************************************
 */
template    <>
void    _NoReturn_  Execution::DoThrow (const SilentException& e2Throw)
{
    DbgTrace (TSTR ("Throwing SilentException"));
    throw e2Throw;
}

