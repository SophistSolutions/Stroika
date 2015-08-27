/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#if     qPlatform_Windows
#include    <windows.h>
#endif
#if     qPlatform_POSIX && qSupport_Proc_Filesystem
#include    <unistd.h>
#endif
#if     defined (_AIX)
#include    <dirent.h>
#include    <unistd.h>
#endif

#include    "../Execution/ErrNoException.h"
#include    "../Execution/Exceptions.h"
#if     defined (_AIX)
#include    "../Execution/Process.h"
#endif
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
#if     defined (_AIX)
namespace {
    SDKString   myProcessRunnerFirstLine_ (const SDKString& cmdLine)
    {
        /*
         *      NOTE - we intentionally use popen here avoid helpful Stroika code like ProcessRunner (), ThrowIfNull, etc,
         *      so that this code remains low level, and doesnt invoke any tracelog code, which would result in a deadlock/loop....
         *
         */
        FILE*   p = popen (cmdLine.c_str (), "r");
        string  accum;
        if (p != nullptr) {
            char    buf[10 * 1024];
            while (fgets (buf, NEltsOf (buf), p) != nullptr) {
                accum += buf;
                size_t i = accum.rfind ('\n');
                if (i != -1) {
                    accum.erase (i);
                }
                break;  // minor speed hack since we always use with just one line
            }
            int result  =   pclose (p);
            // cannot report to avoid TRACE ;-(
        }
        return accum;
    }
    // test if this is faster than /bin/find (maybe cuz it stops after first find and AIX find cannot seem to be told todo that).
    SDKString   FindPath2Inode_ (const SDKString& dir, ino_t inodeNumber)
    {
        DIR*       dirIt    { ::opendir (dir.c_str ()) };
        if (dirIt == nullptr) {
            return SDKString ();
        }
        struct CLEANUP_ {
            DIR*       fDirIt_;
            CLEANUP_ (DIR* d) : fDirIt_ (d) {}
            ~CLEANUP_ ()
            {
                if (fDirIt_ != nullptr) {
                    ::closedir (fDirIt_);
                }
            }
        };
        CLEANUP_ c { dirIt };
        for (dirent* cur = ::readdir (dirIt); cur != nullptr; cur = ::readdir (dirIt)) {
            if (::strcmp (cur->d_name, ".") == 0 or ::strcmp (cur->d_name, "..") == 0) {
                continue;
            }
            struct  stat    s;
            SDKString   filePath = dir + cur->d_name;
            if (::stat (filePath.c_str (), &s) == 0) {
                if (s.st_mode & S_IFDIR) {
                    SDKString   tmp = FindPath2Inode_ (filePath + "/", inodeNumber);
                    if (not tmp.empty ()) {
                        return tmp;
                    }
                }
                else if ((s.st_mode & S_IFREG) and s.st_ino == inodeNumber) {
                    return filePath;
                }
            }
        }
        return SDKString ();    // signal not found
    }
    SDKString   AIX_GET_EXE_PATH_ (pid_t pid)
    {
        /*
         *  What a PITA!
         *      http://unix.stackexchange.com/questions/109175/how-to-identify-executable-path-with-its-pid-on-aix-5-or-more
         *
         *  This is INSANELY slow and kludgey. If we need to support this, we will need to find a better way.
         *  We can code all this direclty in C++, without popen, but with a bit of work.
         *      --
         *
         *  NOTE - PART of this I think I can do better/faster with stat!!!
         */
        ino_t   inode {};
        {
            char buf[1024];
            snprintf (buf, NEltsOf (buf), "ls -i /proc/%d/object/a.out | cut -f 1 -d \" \"", pid);
            SDKString   inodeStr = myProcessRunnerFirstLine_ (buf);
            inode   =   strtoll (inodeStr.c_str (), nullptr, 10);
        }
        int majorDev { -1};
        int minorDev { -1};
        {
            char buf[1024];
            snprintf (buf, NEltsOf (buf), "ls -li /proc/%d/object/ | egrep \"%lld$\"", pid, static_cast<long long> (inode));
            SDKString deviceStr = myProcessRunnerFirstLine_ (buf);
            int     beforeMajorIdx   =  deviceStr.find ('.');
            int     beforeMinorIdx   =  deviceStr.find ('.', beforeMajorIdx + 1);
            if (beforeMajorIdx != string::npos and beforeMinorIdx != string::npos) {
                majorDev   =   strtoll (deviceStr.c_str () + beforeMajorIdx + 1, nullptr, 10);
                minorDev   =   strtoll (deviceStr.c_str () + beforeMinorIdx + 1, nullptr, 10);
            }
        }
        string  fsBlockName;
        if (majorDev != -1 and minorDev != -1) {
            char buf[1024];
            snprintf (buf, NEltsOf (buf), "ls -l /dev/ | egrep \"^b.*%d, *%d.+$\"", majorDev, minorDev);
            SDKString   devfsline = myProcessRunnerFirstLine_ (buf);
            int     beforeFSName     =  devfsline.rfind (' ');
            if (beforeFSName != string::npos) {
                fsBlockName = devfsline.substr (beforeFSName + 1);
            }
        }
        string  fsName;
        if (not fsBlockName.empty ()) {
            char buf[1024];
            snprintf (buf, NEltsOf (buf), "df | grep %s", fsBlockName.c_str ());    // not - unreliable - could match multiple...
            SDKString   devfsline = myProcessRunnerFirstLine_ (buf);
            int     beforeFSName     =  devfsline.rfind (' ');
            if (beforeFSName != string::npos) {
                fsName = devfsline.substr (beforeFSName + 1);
            }
        }
        string  exeName;
        if (not fsName.empty ()) {
            //char buf[1024];
            //snprintf (buf, NEltsOf (buf), "find %s -xdev -type f -inum %lld -print 2> /dev/null", fsName.c_str (), static_cast<long long> (inode));
            //exeName = myProcessRunnerFirstLine_ (buf);
            exeName = FindPath2Inode_ (fsName + "/", inode);
        }
        return exeName;
    }
    SDKString   AIX_GET_EXE_PATH_ ()
    {
        static  SDKString   kCached_    =   AIX_GET_EXE_PATH_ (Execution::GetCurrentProcessID ());  // since cannot change, and now very slow to compute
        return  kCached_;
    }
}
#endif
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
    // readlink () isn't clear about finding the right size. The only way to tell it wasn't enuf (maybe) is
    // if all the bytes passed in are used. That COULD mean it all fit, or there was more. If we get that -
    // double buf size and try again
    Memory::SmallStackBuffer<Characters::SDKChar> buf (1024);
    ssize_t n;
    while ( (n = readlink ("/proc/self/exe", buf, buf.GetSize ())) == buf.GetSize ()) {
        buf.GrowToSize (buf.GetSize () * 2);
    }
    if (n < 0) {
        errno_ErrorException::DoThrow (errno);
    }
    Assert (n <= buf.GetSize ());   // could leave no room for NUL-byte, but not needed
    return SDKString (buf.begin (), buf.begin () + n);
#elif   defined (_AIX)
    return AIX_GET_EXE_PATH_ ();
#else
    AssertNotImplemented ();
    return SDKString ();
#endif
}

