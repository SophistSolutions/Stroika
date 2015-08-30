/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    <thread>

#include    "Thread.h"

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
    std::this_thread::yield ();
    CheckForThreadInterruption<100> ();
}
