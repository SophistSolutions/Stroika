/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#if     qPlatform_POSIX
#include    <string>
#include    <unistd.h>
#include    <sys/ptrace.h>
#include    <signal.h>
#include    <cstdio>
#elif   qPlatform_Windows
#include    <Windows.h>
#endif
#include    "../Math/Common.h"

#include    "Debugger.h"

using   namespace   Stroika::Foundation;

void    Debug::DropIntoDebuggerIfPresent ()
{
#if     qPlatform_Windows
    if (::IsDebuggerPresent ()) {
        ::DebugBreak ();
    }
#elif   qPlatform_POSIX
#if     0
    // not sure this is right, but its close...
    if (ptrace (PTRACE_TRACEME, 0, NULL, 0) == -1) {
        raise (SIGTRAP);
    }
#elif   1
    // BAD, but least bad, way I know...
    char pathBuf[1024];
    sprintf(pathBuf, "/proc/%d/exe", ::getppid ());
    char dataBuf[1024];
    ssize_t n   =   :: readlink (pathBuf, dataBuf, sizeof (dataBuf) - 1);
    n = Math::PinInRange<size_t> (0, sizeof (dataBuf) - 1, static_cast<size_t> (n));
    dataBuf[n] = '\0';
    if (string (dataBuf).find ("gdb") != -1) {
        ::raise (SIGTRAP);
    }
#else
    // NYI
#endif
#else
    // not sure (yet) how to tell if being debugged...
    //abort ();
#endif
}
