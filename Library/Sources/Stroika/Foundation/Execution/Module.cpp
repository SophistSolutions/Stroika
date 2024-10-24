/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include <cstdio>

#if qPlatform_MacOS
#include <libproc.h>
#include <mach-o/dyld.h>
#endif
#if qPlatform_POSIX && qSupport_Proc_Filesystem
#include <unistd.h>
#endif
#if qPlatform_Windows
#include <windows.h>
#endif

#include "Stroika/Foundation/Execution/Exceptions.h"
#include "Stroika/Foundation/Execution/Synchronized.h"
#include "Stroika/Foundation/Execution/Throw.h"
#include "Stroika/Foundation/IO/FileSystem/PathName.h"
#include "Stroika/Foundation/Memory/StackBuffer.h"

#include "Module.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Execution;

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1

/*
 ********************************************************************************
 ***************************** Execution::GetEXEDir *****************************
 ********************************************************************************
 */
filesystem::path Execution::GetEXEDir ()
{
    return GetEXEPath ().parent_path ();
}

/*
 ********************************************************************************
 **************************** Execution::GetEXEPath *****************************
 ********************************************************************************
 */
filesystem::path Execution::GetEXEPath ()
{
// See also http://stackoverflow.com/questions/1023306/finding-current-executables-path-without-proc-self-exe
//      Mac OS X: _NSGetExecutablePath() (man 3 dyld)
//      Linux: readlink /proc/self/exe
//      Solaris: getexecname()
//      FreeBSD: sysctl CTL_KERN KERN_PROC KERN_PROC_PATHNAME -1
//      BSD with procfs: readlink /proc/curproc/file
//      Windows: GetModuleFileName() with hModule = nullptr
//
#if qPlatform_MacOS
    uint32_t bufSize = 0;
    Verify (_NSGetExecutablePath (nullptr, &bufSize) == -1);
    Assert (bufSize > 0);
    Memory::StackBuffer<char> buf{Memory::eUninitialized, bufSize};
    Verify (_NSGetExecutablePath (buf.begin (), &bufSize) == 0);
    Assert (buf[bufSize - 1] == '\0');
    return buf.begin ();
#elif qPlatform_POSIX && qSupport_Proc_Filesystem
    // readlink () isn't clear about finding the right size. The only way to tell it wasn't enuf (maybe) is
    // if all the bytes passed in are used. That COULD mean it all fit, or there was more. If we get that -
    // double buf size and try again
    Memory::StackBuffer<Characters::SDKChar> buf{Memory::eUninitialized, 1024};
    ssize_t                                  n;
    while ((n = ::readlink ("/proc/self/exe", buf.data (), buf.GetSize ())) == buf.GetSize ()) {
        buf.GrowToSize_uninitialized (buf.GetSize () * 2);
    }
    if (n < 0) {
        ThrowPOSIXErrNo (errno);
    }
    Assert (n <= buf.GetSize ()); // could leave no room for NUL-byte, but not needed
    return SDKString{buf.begin (), buf.begin () + n};
#elif qPlatform_Windows
    Characters::SDKChar buf[MAX_PATH];
    //memset (buf, 0, sizeof (buf));
    Verify (::GetModuleFileName (nullptr, buf, static_cast<DWORD> (Memory::NEltsOf (buf))));
    buf[Memory::NEltsOf (buf) - 1] = '\0'; // cheaper and just as safe as memset() - more even. Buffer always nul-terminated, and if GetModuleFileName succeeds will be nul-terminated
    return buf;
#else
    AssertNotImplemented ();
    return filesystem::path{};
#endif
}

/*
 ********************************************************************************
 **************************** Execution::GetEXEPathT ****************************
 ********************************************************************************
 */
filesystem::path Execution::GetEXEPath ([[maybe_unused]] pid_t processID)
{
#if qPlatform_MacOS
    char pathbuf[PROC_PIDPATHINFO_MAXSIZE];
    int  ret = ::proc_pidpath (processID, pathbuf, sizeof (pathbuf));
    if (ret <= 0) {
        Execution::Throw (Exception{"proc_pidpath failed"sv}); // @todo - horrible reporting, but not obvious what this API is? proc_pidpath?
    }
    else {
        return pathbuf;
    }
#elif qPlatform_POSIX && qSupport_Proc_Filesystem
    // readlink () isn't clear about finding the right size. The only way to tell it wasn't enuf (maybe) is
    // if all the bytes passed in are used. That COULD mean it all fit, or there was more. If we get that -
    // double buf size and try again
    Memory::StackBuffer<Characters::SDKChar> buf{Memory::eUninitialized, 1024};
    ssize_t                                  n;
    char                                     linkNameBuf[1024];
    (void)std::snprintf (linkNameBuf, sizeof (linkNameBuf), "/proc/%ld/exe", static_cast<long> (processID));
    while ((n = ::readlink (linkNameBuf, buf.data (), buf.GetSize ())) == buf.GetSize ()) {
        buf.GrowToSize_uninitialized (buf.GetSize () * 2);
    }
    if (n < 0) {
        ThrowPOSIXErrNo (errno);
    }
    Assert (n <= buf.GetSize ()); // could leave no room for NUL-byte, but not needed
    return SDKString{buf.begin (), buf.begin () + n};
#elif qPlatform_Windows
    // https://msdn.microsoft.com/en-us/library/windows/desktop/ms682621(v=vs.85).aspx but a bit of work
    // not needed yet
    AssertNotImplemented ();
    return filesystem::path{};
#else
    AssertNotImplemented ();
    return filesystem::path{};
#endif
}
