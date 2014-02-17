/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#if     qPlatform_Windows
#include    <windows.h>
#endif
#if     qPlatform_POSIX && qSupport_Proc_Filesystem
#include    <unistd.h>
#endif

#include    "../Execution/ErrNoException.h"
#include    "../Execution/Exceptions.h"
#include    "../Memory/SmallStackBuffer.h"
#include    "../IO/FileSystem/PathName.h"

#include    "Module.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Execution;





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
#if     qPlatform_Windows
    Characters::SDKChar   buf[MAX_PATH];
    //memset (buf, 0, sizeof (buf));
    Verify (::GetModuleFileName (nullptr, buf, NEltsOf (buf)));
    buf[NEltsOf (buf) - 1] = '\0';  // cheaper and just as safe as memset() - more even. Buffer always nul-terminated, and if GetModuleFileName succeeds will be nul-terminated
    return buf;
#elif   qPlatform_POSIX && qSupport_Proc_Filesystem
    // readlink () isn't clear about finding the right size. THe only way to tell it wasn't enuf (maybe) is if all the
    // bytes passed in are used. That COULD mean it all fit, or there was more. If we get that - double buf size and try again
    Memory::SmallStackBuffer<Characters::SDKChar> buf (1000);
    ssize_t n;
    while ( (n = readlink ("/proc/self/exe", buf, buf.GetSize ())) == buf.GetSize ()) {
        buf.GrowToSize (buf.GetSize () * 2);
    }
    if (n < 0) {
        errno_ErrorException::DoThrow (errno);
    }
    Assert (n <= buf.GetSize ());   // could leave no room for NUL-byte, but not needed
    return SDKString (buf.begin (), buf.begin () + n);
#else
    AssertNotImplemented ();
    return SDKString ();
#endif
}

