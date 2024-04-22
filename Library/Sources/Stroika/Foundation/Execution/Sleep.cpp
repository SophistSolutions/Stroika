/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Time/Duration.h"

#include "Sleep.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Execution;

/*
 ********************************************************************************
 ***************************** Execution::Sleep *********************************
 ********************************************************************************
 */
void Execution::Sleep (Time::Duration seconds2Wait)
{
    Require (seconds2Wait >= 0s);
    Thread::CheckForInterruption ();
    Time::DurationSeconds timeRemaining = seconds2Wait;
    while (timeRemaining > 0s) {
        Sleep (timeRemaining, &timeRemaining);
    }
}