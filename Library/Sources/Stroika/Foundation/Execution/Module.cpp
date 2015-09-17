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
#if     qPlatform_AIX
#include    <dirent.h>
#include    <sys/sysmacros.h>
#include    <sys/types.h>
#include    <unistd.h>
#endif

#include    "../Cache/CallerStalenessCache.h"
#include    "../Execution/ErrNoException.h"
#include    "../Execution/Exceptions.h"
#include    "../Execution/Synchronized.h"
#if     qPlatform_AIX
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
#if     qPlatform_AIX
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
    // find %s -xdev -type f -inum %lld -print 2> /dev/null - but much faster.
    // @todo - xdev part. Not only possible speed hack, but also needed for correctness since
    // we could match the wrong inode if we crossed filesystems.
    SDKString   FindPath2Inode_ (const SDKString& dir, dev_t restrictToDev, ino_t inodeNumber)
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
                if (s.st_dev != restrictToDev) {
                    // another mounted volume
                    continue;
                }
                if (s.st_mode & S_IFDIR) {
                    SDKString   tmp = FindPath2Inode_ (filePath + "/", restrictToDev, inodeNumber);
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
    SDKString   FindPath2Inode_Caching_ (const SDKString& dir, dev_t restrictToDev, ino_t inodeNumber)
    {
        const   Time::DurationSecondsType   kTimeValidFor_      =   60.0;   // no good way to parameterize
        using   KEY = tuple<SDKString, dev_t, ino_t>;
        static  Synchronized<Cache::CallerStalenessCache<KEY, SDKString>>   sCache_;
        Time::DurationSecondsType       noOlderThan = Time::GetTickCount () - kTimeValidFor_;
        return sCache_->Lookup (KEY (dir, restrictToDev, inodeNumber), noOlderThan, [noOlderThan, dir, restrictToDev, inodeNumber] () -> SDKString {
            // must find better time/way todo this
            sCache_->ClearOlderThan (noOlderThan);
            return FindPath2Inode_ (dir, restrictToDev, inodeNumber);
        });
    }
    SDKString   FindPath2Inode_ (const SDKString& dir, ino_t inodeNumber)
    {
        struct  stat    s;
        if (::stat (dir.c_str (), &s) == 0) {
            if (s.st_mode & S_IFDIR) {
                return FindPath2Inode_Caching_ (dir, s.st_dev, inodeNumber);
            }
        }
        return SDKString ();
    }
    SDKString   FindFSForMajorMinorDev_LowLevel_ (int majorDev, int minorDev)
    {
        // @todo - this isn't truely safe... Esp df|grep part...
        string  fsBlockName;
        if (majorDev != -1 and minorDev != -1) {
            // ls -l /dev/ | egrep \"^b.*%d, *%d.+$\"", majorDev, minorDev
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
        return fsName;
    }
    SDKString   FindFSForMajorMinorDev_WithCaching_ (int majorDev, int minorDev)
    {
        const   Time::DurationSecondsType   kTimeValidFor_      =   60 * 60.0;   // no good way to parameterize
        using   KEY = pair<int, int>;
        static  Synchronized<Cache::CallerStalenessCache<KEY, SDKString>>   sCache_;
        Time::DurationSecondsType       noOlderThan = Time::GetTickCount () - kTimeValidFor_;
        return sCache_->Lookup (KEY (majorDev, minorDev), noOlderThan, [noOlderThan, majorDev, minorDev] () -> SDKString {
            // must find better time/way todo this
            sCache_->ClearOlderThan (noOlderThan);
            return FindFSForMajorMinorDev_LowLevel_ (majorDev, minorDev);
        });
    }
    SDKString   AIX_GET_EXE_PATH_ (pid_t pid, const SDKString* hint)
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
            struct  stat    s;
            char aoutFileBuf[1024];
            snprintf (aoutFileBuf, NEltsOf (aoutFileBuf), "/proc/%d/object/a.out", pid);
            if (::stat (aoutFileBuf, &s) == 0) {
                inode = s.st_ino;
            }
        }
        if (inode == 0) {
            return SDKString ();
        }
        int majorDev { -1 };
        int minorDev { -1 };
        {
            // @todo - can we simplify this and look at st_dev of the file???
            // ls -li /proc/%d/object/ | egrep \"%lld$\""
            char procObjectDir[1024];
            (void)::snprintf (procObjectDir, NEltsOf (procObjectDir), "/proc/%d/object/", pid);
            DIR*       dirIt    { ::opendir (procObjectDir) };
            if (dirIt != nullptr) {
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
                char    endsWithBuffer[1024];
                endsWithBuffer[0] = '\0';
                (void)::snprintf (endsWithBuffer, NEltsOf (endsWithBuffer), ".%lld", static_cast<long long> (inode));
                size_t  endsWithBufferLen = ::strlen (endsWithBuffer);
                for (dirent* cur = ::readdir (dirIt); cur != nullptr; cur = ::readdir (dirIt)) {
                    size_t  l   =   ::strlen (cur->d_name);
                    if (l > endsWithBufferLen) {
                        if (::strcmp (cur->d_name + l - endsWithBufferLen, endsWithBuffer) == 0) {
                            string  tmp { cur->d_name };
                            int     beforeMajorIdx   =  tmp.find ('.');
                            int     beforeMinorIdx   =  tmp.find ('.', beforeMajorIdx + 1);
                            if (beforeMajorIdx != string::npos and beforeMinorIdx != string::npos) {
                                majorDev   =   ::strtoll (tmp.c_str () + beforeMajorIdx + 1, nullptr, 10);
                                minorDev   =   ::strtoll (tmp.c_str () + beforeMinorIdx + 1, nullptr, 10);
                            }
                        }
                    }
                }
            }
        }
        if (hint != nullptr) {
            struct  stat    hintStats;
            if (::stat (hint->c_str (), &hintStats) == 0) {
                if (hintStats.st_ino == inode and hintStats.st_dev == makedev (majorDev, minorDev)) {
                    return *hint;
                }
            }
        }
        SDKString   fsName  =   FindFSForMajorMinorDev_WithCaching_ (majorDev, minorDev);
        string      exeName;
        if (not fsName.empty ()) {
            exeName = FindPath2Inode_ (fsName + "/", inode);
        }
        return exeName;
    }
    SDKString   AIX_GET_EXE_PATH_ ()
    {
        static  SDKString   kCached_    =   AIX_GET_EXE_PATH_ (Execution::GetCurrentProcessID (), nullptr);  // since cannot change, and now very slow to compute
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
#elif   qPlatform_AIX
    return AIX_GET_EXE_PATH_ ();
#else
    AssertNotImplemented ();
    return SDKString ();
#endif
}




String Execution::GetEXEPath (pid_t processID)
{
#if     qPlatform_AIX
    return String::FromSDKString (AIX_GET_EXE_PATH_ (processID, nullptr));
#elif   qPlatform_POSIX && qSupport_Proc_Filesystem
    // readlink () isn't clear about finding the right size. The only way to tell it wasn't enuf (maybe) is
    // if all the bytes passed in are used. That COULD mean it all fit, or there was more. If we get that -
    // double buf size and try again
    Memory::SmallStackBuffer<Characters::SDKChar> buf (1024);
    ssize_t n;
    char    linkNameBuf[1024];
    (void)::snprintf (linkNameBuf, sizeof (linkNameBuf), "/proc/%ld/exe", static_cast<long> (processID));
    while ( (n = ::readlink (linkNameBuf, buf, buf.GetSize ())) == buf.GetSize ()) {
        buf.GrowToSize (buf.GetSize () * 2);
    }
    if (n < 0) {
        errno_ErrorException::DoThrow (errno);
    }
    Assert (n <= buf.GetSize ());   // could leave no room for NUL-byte, but not needed
    return String::FromSDKString (SDKString (buf.begin (), buf.begin () + n));
#elif   qPlatform_Windows
    // https://msdn.microsoft.com/en-us/library/windows/desktop/ms682621(v=vs.85).aspx but a bit of work
    // not needed yet
    AssertNotImplemented ();
    return String ();
#else
    AssertNotImplemented ();
    return String ();
#endif
}

#if     qPlatform_AIX
SDKString   Execution::GetEXEPathWithHint (pid_t processID, const SDKString& hint)
{
    return AIX_GET_EXE_PATH_ (processID, &hint);
}
#endif
