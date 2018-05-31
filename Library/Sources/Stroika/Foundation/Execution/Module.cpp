/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#include "../StroikaPreComp.h"

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

#include "../Execution/ErrNoException.h"
#include "../Execution/Exceptions.h"
#include "../Execution/Synchronized.h"
#include "../IO/FileSystem/PathName.h"
#include "../Memory/SmallStackBuffer.h"

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
String Execution::GetEXEDir ()
{
    return String::FromSDKString (GetEXEDirT ());
}

/*
 ********************************************************************************
 ***************************** Execution::GetEXEDirT *****************************
 ********************************************************************************
 */
SDKString Execution::GetEXEDirT ()
{
    // Currently this impl depends on String - we may want to redo one cleanly without any dependency on String()...
    // Docs call for this - but I'm not sure its needed
    return IO::FileSystem::GetFileDirectory (GetEXEPath ()).AsSDKString ();
}

/*
 ********************************************************************************
 **************************** Execution::GetEXEPath *****************************
 ********************************************************************************
 */
String Execution::GetEXEPath ()
{
    return String::FromSDKString (GetEXEPathT ());
}

/*
 ********************************************************************************
 **************************** Execution::GetEXEPathT ****************************
 ********************************************************************************
 */
SDKString Execution::GetEXEPathT ()
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
    Memory::SmallStackBuffer<char> buf (bufSize);
    Verify (_NSGetExecutablePath (buf.begin (), &bufSize) == 0);
    return buf.begin ();
#elif qPlatform_POSIX && qSupport_Proc_Filesystem
    // readlink () isn't clear about finding the right size. The only way to tell it wasn't enuf (maybe) is
    // if all the bytes passed in are used. That COULD mean it all fit, or there was more. If we get that -
    // double buf size and try again
    Memory::SmallStackBuffer<Characters::SDKChar> buf (1024);
    ssize_t                                       n;
    while ((n = readlink ("/proc/self/exe", buf, buf.GetSize ())) == buf.GetSize ()) {
        buf.GrowToSize (buf.GetSize () * 2);
    }
    if (n < 0) {
        errno_ErrorException::Throw (errno);
    }
    Assert (n <= buf.GetSize ()); // could leave no room for NUL-byte, but not needed
    return SDKString (buf.begin (), buf.begin () + n);
#elif qPlatform_Windows
    Characters::SDKChar buf[MAX_PATH];
    //memset (buf, 0, sizeof (buf));
    Verify (::GetModuleFileName (nullptr, buf, static_cast<DWORD> (NEltsOf (buf))));
    buf[NEltsOf (buf) - 1] = '\0'; // cheaper and just as safe as memset() - more even. Buffer always nul-terminated, and if GetModuleFileName succeeds will be nul-terminated
    return buf;
#else
    AssertNotImplemented ();
    return SDKString ();
#endif
}

String Execution::GetEXEPath (pid_t processID)
{
#if qPlatform_MacOS
    char pathbuf[PROC_PIDPATHINFO_MAXSIZE];
    int  ret = ::proc_pidpath (processID, pathbuf, sizeof (pathbuf));
    if (ret <= 0) {
        Execution::Throw (StringException (L"proc_pidpath failed")); // @todo - horrible reporting, but not obvious what this API is? proc_pidpath?
    }
    else {
        return String::FromSDKString (pathbuf);
    }
#elif qPlatform_POSIX && qSupport_Proc_Filesystem
    // readlink () isn't clear about finding the right size. The only way to tell it wasn't enuf (maybe) is
    // if all the bytes passed in are used. That COULD mean it all fit, or there was more. If we get that -
    // double buf size and try again
    Memory::SmallStackBuffer<Characters::SDKChar> buf (1024);
    ssize_t                                       n;
    char                                          linkNameBuf[1024];
    (void)std::snprintf (linkNameBuf, sizeof (linkNameBuf), "/proc/%ld/exe", static_cast<long> (processID));
    while ((n = ::readlink (linkNameBuf, buf, buf.GetSize ())) == buf.GetSize ()) {
        buf.GrowToSize (buf.GetSize () * 2);
    }
    if (n < 0) {
        errno_ErrorException::Throw (errno);
    }
    Assert (n <= buf.GetSize ()); // could leave no room for NUL-byte, but not needed
    return String::FromSDKString (SDKString (buf.begin (), buf.begin () + n));
#elif qPlatform_Windows
    // https://msdn.microsoft.com/en-us/library/windows/desktop/ms682621(v=vs.85).aspx but a bit of work
    // not needed yet
    AssertNotImplemented ();
    return String ();
#else
    AssertNotImplemented ();
    return String ();
#endif
}
