/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    <thread>

#include    "SpinLock.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Execution;


/*
 ********************************************************************************
 ************************** Execution::SpinLock *********************************
 ********************************************************************************
 */
void    SpinLock::Yield_ ()
{
#if     qCompilerAndStdLib_threadYield_Buggy
    sleep (0);
#else
    std::this_thread::yield ();
#endif
}
