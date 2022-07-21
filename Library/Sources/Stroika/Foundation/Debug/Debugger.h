/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#ifndef _Stroika_Foundation_Debug_Debugger_h_
#define _Stroika_Foundation_Debug_Debugger_h_ 1

#include "../StroikaPreComp.h"

#include "../Configuration/Common.h"

namespace Stroika::Foundation::Debug {

    /**
     * \note this may do bad things (like abort) if the debugger is not present, as there isn't a good universal way to tell you are running
     *       under the debugger.
     */
    void DropIntoDebuggerIfPresent ();

}

#endif /*_Stroika_Foundation_Debug_Debugger_h_*/
