/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include "../Time/Duration.h"

#include "Sleep.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Execution;

/*
 ********************************************************************************
 ***************************** Execution::Sleep *********************************
 ********************************************************************************
 */
void Execution::Sleep (Time::DurationSecondsType seconds2Wait)
{
    Require (seconds2Wait >= 0.0);
    CheckForThreadInterruption ();
    Time::DurationSecondsType timeRemaining = seconds2Wait;
    while (timeRemaining > 0) {
        Sleep (timeRemaining, &timeRemaining);
    }
}

void Execution::Sleep (const Time::Duration& wait)
{
    Sleep (wait.As<Time::DurationSecondsType> ());
}

void Execution::Sleep (const Time::Duration& wait, Time::Duration* remainingInSleep)
{
    RequireNotNull (remainingInSleep);
    Time::DurationSecondsType remaining = 0;
    Sleep (wait.As<Time::DurationSecondsType> (), &remaining);
    *remainingInSleep = Time::Duration (remaining);
}
