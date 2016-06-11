/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    "SilentException.h"



using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Execution;






/*
 ********************************************************************************
 ******************************** SilentException *******************************
 ********************************************************************************
 */
const   SilentException SilentException::kThe   {};




/*
 ********************************************************************************
 ******************************** Execution::Throw ******************************
 ********************************************************************************
 */
template    <>
[[noreturn]]    void    Execution::Throw (const SilentException& e2Throw)
{
    DbgTrace (SDKSTR ("Throwing SilentException"));
    throw e2Throw;
}

