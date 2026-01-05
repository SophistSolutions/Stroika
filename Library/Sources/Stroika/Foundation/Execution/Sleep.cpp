/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Time/Duration.h"

#include "Sleep.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Execution;
using namespace Stroika::Foundation::Time;


/*
 ********************************************************************************
 *************************** Execution::SleepUntil ******************************
 ********************************************************************************
 */
void Execution::SleepUntil (Time::TimePointSeconds untilTickCount)
{
    Thread::CheckForInterruption ();    // assure always check at least once, since interruption point
    for (TimePointSeconds now = Time::GetTickCount (); untilTickCount > now; now = Time::GetTickCount ()) {
        DurationSeconds ignored{};
        Sleep (untilTickCount - now, &ignored);
    }
}