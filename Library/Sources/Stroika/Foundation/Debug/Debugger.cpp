/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#include "../StroikaPreComp.h"

#if qPlatform_POSIX
#include <string>
#include <unistd.h>
#if defined(__APPLE__) && defined(__MACH__)
#include <sys/types.h>
#endif
#include <cstdio>
#include <signal.h>
#include <sys/ptrace.h>
#elif qPlatform_Windows
#include <Windows.h>
#endif
#include "../Math/Common.h"

#include "Debugger.h"

using namespace Stroika::Foundation;

/*
 ********************************************************************************
 ************************ Debug::DropIntoDebuggerIfPresent **********************
 ********************************************************************************
 */
void Debug::DropIntoDebuggerIfPresent ()
{
#if qPlatform_Windows
    if (::IsDebuggerPresent ()) {
        ::DebugBreak ();
    }
#elif qPlatform_POSIX
#if 0
    // not sure this is right, but its close...
    if (ptrace (PTRACE_TRACEME, 0, NULL, 0) == -1) {
        raise (SIGTRAP);
    }
#elif 0
    // NOT SURE THIS WORKS - ESP WITH GDBSERVER!
    // @todo - CONSIDER GETTING RID OF THIS. Not sure of the value...
    // BAD, but least bad, way I know...
    char pathBuf[1024];
    sprintf (pathBuf, "/proc/%d/exe", ::getppid ());
    char    dataBuf[1024];
    ssize_t n  = ::readlink (pathBuf, dataBuf, sizeof (dataBuf) - 1);
    n          = Math::PinInRange<size_t> (static_cast<size_t> (n), 0, sizeof (dataBuf) - 1);
    dataBuf[n] = '\0';
    if (string (dataBuf).find ("gdb") != -1) {
        ::raise (SIGTRAP);
    }
#else
// NYI
#endif
#else
    // not sure (yet) how to te     Test28.exe!`anonymous-namespace'::AllSSLEncrytionRoundtrip::DoRegressionTests_::__l2::<lambda>(const Stroika::Foundation::Cryptography::Encoding::OpenSSLCryptoParams & cryptoParams, Stroika::Foundation::Memory::BLOB src) Line 316   C++
    ll if being debugged...
//abort ();
#endif
}
