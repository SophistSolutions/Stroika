/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#ifndef _Stroika_Foundation_Debug_Main_h_
#define _Stroika_Foundation_Debug_Main_h_ 1

#include "../StroikaPreComp.h"

#include <optional>

#include "../Configuration/Common.h"

namespace Stroika::Foundation::Debug {

    /**
     *  This returns true iff it is highly likely that we are executing after the start of main (in the main thread)
     *  and before the end of main.
     * 
     *  This is IMPORTANT, because certain things are required/allowed by C++ only after the start and before the end of main.
     *  But alas, C++ provides no reliable way to test this.
     * 
     *  This function - since unreliable - is only to be used for generating assertions.
     * 
     *  \note - this function may false POSITIVE, but will never false NEGATIVE.
     * 
     *  That is to say - it's perfectly safe to say
     *      Assert (Debug::AppearsDuringMainLifetime ());  // in threaded code
     *
     *  The assertion may sometimes fail to trigger when you would want it to, but it will never trigger falsely.
     * 
     *  \todo    FIND CLEAR DOCS on WHY this is hard, and clear docs on WHY this is useful (threads). I found neither on
     *          a quick search of the C++ specification.
     * 
     *  \note Stroika's AllThreadsDeadDetector_ will attempt to detect/assert if any threads left running after main.
     */
    bool AppearsDuringMainLifetime ();

}

#endif /*_Stroika_Foundation_Debug_Main_h_*/
