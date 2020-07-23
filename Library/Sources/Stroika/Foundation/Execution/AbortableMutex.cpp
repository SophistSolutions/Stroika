/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include "../Configuration/StroikaConfig.h"

#include "Thread.h"

#include "AbortableMutex.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Execution;

/*
 ********************************************************************************
 ******************************** AbortableMutex ********************************
 ********************************************************************************
 */
void AbortableMutex::lock ()
{
    while (not fM_.try_lock_for (chrono::milliseconds{10})) {
        Thread::CheckForInterruption ();
    }
}

void AbortableMutex::unlock ()
{
    fM_.unlock ();
}
