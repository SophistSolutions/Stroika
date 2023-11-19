/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
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
void Execution::Sleep (Time::Duration seconds2Wait)
{
    Require (seconds2Wait >= 0s);
    Thread::CheckForInterruption ();
    Time::DurationSeconds timeRemaining = seconds2Wait;
    while (timeRemaining > 0s) {
        Sleep (timeRemaining, &timeRemaining);
    }
}