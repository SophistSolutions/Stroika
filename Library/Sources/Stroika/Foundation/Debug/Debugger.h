/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#ifndef _Stroika_Foundation_Debug_Debugger_h_
#define _Stroika_Foundation_Debug_Debugger_h_ 1

#include "../StroikaPreComp.h"

#include "../Configuration/Common.h"

namespace Stroika::Foundation::Debug {

    /**
     * There is no perfect cross-platform way to check this. But this function tries, and returns true/false if it
     *  has a good guess and nullopt if it has no idea.
     */
    optional<bool>  IsThisProcessBeingDebugged ();

    /**
     *  Try to drop into the debugger, if IsThisProcessBeingDebugged () returns true.
     * 
     *  \note - highly imperfect, may false positive/negative, especially debugging on quirks of your platform/OS.
     */
    void DropIntoDebuggerIfPresent ();

}

#endif /*_Stroika_Foundation_Debug_Debugger_h_*/
