/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    "Sleep.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Execution;



/*
 ********************************************************************************
 ***************************** Execution::Sleep *********************************
 ********************************************************************************
 */
void    Execution::Sleep (Time::DurationSecondsType seconds2Wait)
{
    Require (seconds2Wait >= 0.0);
    CheckForThreadAborting ();
    Time::DurationSecondsType   timeRemaining   =   seconds2Wait;
    while (timeRemaining > 0) {
        Sleep (timeRemaining, &timeRemaining);
    }
}
