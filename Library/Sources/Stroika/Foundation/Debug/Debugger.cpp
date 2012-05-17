/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#if     qPlatform_Windows
#include    <Windows.h>
#endif

#include    "Debugger.h"

using   namespace   Stroika::Foundation;

void    Debug::DropIntoDebuggerIfPresent () {
#if     qPlatform_Windows
    if (::IsDebuggerPresent ()) {
        ::DebugBreak ();
    }
#else
    // not sure (yet) how to tell if being debugged...
    //abort ();
#endif
}
