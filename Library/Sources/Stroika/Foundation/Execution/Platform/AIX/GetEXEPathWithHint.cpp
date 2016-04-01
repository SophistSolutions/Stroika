/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#include    "../../../StroikaPreComp.h"

#include    <unistd.h>

#if     qPlatform_AIX
#include    <dirent.h>
#include    <sys/sysmacros.h>
#include    <sys/types.h>
#include    <unistd.h>
#include    <fstream>
#endif

#include    "../../../Cache/CallerStalenessCache.h"
#include    "../../../Cache/LRUCache.h"
#include    "../../../Characters/String_Constant.h"
#include    "../../../Debug/TimingTrace.h"
#include    "../../../Memory/SmallStackBuffer.h"
#include    "../../ErrNoException.h"
#include    "../../Finally.h"

#include    "GetEXEPathWithHint.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Execution;
using   namespace   Stroika::Foundation::Execution::Platform::AIX;

using   Characters::String;
using   Characters::String_Constant;


//
// Comment this in to turn on aggressive noisy DbgTrace in this module
// NB: CURRENTLY safe to turn on (2016-04-01) - but risky if triggered loading with TraceLog file getting its name from
// calling here...
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1
//


//
// Comment this in to turn on somewhat noisy timing DbgTrace in this module
// NB: This timing trace code contains HACK - to NOT report anyhting for processID==getpid () - since
// that is called early on trying to get the EXE name for this process - before main - for the tracelog
// and doing DbgTrace calls in response causes grave disorder
//
//#define   USE_TIMING_TRACE_IN_THIS_MODULE_       1
//




namespace {
    bool    IsProcessSafeToLogFor_ (pid_t p)
    {
        return p != getpid ();
    }
}


namespace {
    // find %s -xdev -type f -inum %lld -print 2> /dev/null - but much faster.
    // @todo - xdev part. Not only possible speed hack, but also needed for correctness since
    // we could match the wrong inode if we crossed filesystems.
    SDKString   FindPath2Inode_ (const SDKString& dir, dev_t restrictToDev, ino_t inodeNumber)
    {
        DIR*       dirIt    { ::opendir (dir.c_str ()) };
        if (dirIt == nullptr) {
            return SDKString ();
        }
        auto&& cleanup  =   Finally ([dirIt] () noexcept { ::closedir (dirIt); });
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
        const   Time::DurationSecondsType   kTimeValidFor_      =   5 * 60.0;   // no good way to parameterize
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
    SDKString   GetBlockDeviceFileName2MajorMinorDev_ (int majorDev, int minorDev)
    {
        /*
         *
         *  if (majorDev != -1 and minorDev != -1) {
         *      // ls -l /dev/ | egrep \"^b.*%d, *%d.+$\"", majorDev, minorDev
         *      char buf[1024];
         *      (void)::snprintf (buf, NEltsOf (buf), "ls -l /dev/ | egrep \"^b.*%d, *%d.+$\"", majorDev, minorDev);
         *      SDKString   devfsline = myProcessRunnerFirstLine_ (buf);
         *      int     beforeFSName     =  devfsline.rfind (' ');
         *      if (beforeFSName != string::npos) {
         *          fsBlockName = devfsline.substr (beforeFSName + 1);
         *      }
         *  }
         */
        // NOTE - this could probably be memoized/cached, as the list of block devices on a machine probably doesn't change much ;-)
        DIR*       dirIt    { ::opendir ("/dev") };
        if (dirIt != nullptr) {
            auto&& cleanup  =   Finally ([dirIt] () noexcept { ::closedir (dirIt); });
            for (dirent* cur = ::readdir (dirIt); cur != nullptr; cur = ::readdir (dirIt)) {
                SDKString   filePath = SDKString { "/dev/" } + cur->d_name;
                struct  stat    s;
                if (::stat (filePath.c_str (), &s) == 0) {
                    if (S_ISBLK(s.st_mode) and major (s.st_rdev) == majorDev and minor (s.st_rdev) == minorDev) {
                        return filePath;
                    }
                }
            }
        }
        return SDKString {};
    }
    SDKString   GetFSNameForBlockName_ (const SDKString& blockDeviceName)
    {
        /*
         *  What we used to do:
         *      if (not fsBlockName.empty ()) {
         *          char buf[1024];
         *          (void)::snprintf (buf, NEltsOf (buf), "df | grep %s", fsBlockName.c_str ());    // not - unreliable - could match multiple...
         *          SDKString   devfsline = myProcessRunnerFirstLine_ (buf);
         *          int     beforeFSName     =  devfsline.rfind (' ');
         *          if (beforeFSName != string::npos) {
         *              return devfsline.substr (beforeFSName + 1);
         *          }
         *      }
         *
         *  Weak way - but probably better then what we used to do:
         *      read /etc/filesystem
         */
        ifstream    fsFile ("/etc/filesystems");
        char line[4 * 1024];
        string  readingFilesystem;
        string  forDev;
        while (fsFile.getline (line, NEltsOf (line))) {
            if (line[0] == '/') {
                char* p = ::strchr (line, ':');
                if (p != nullptr) {
                    *p = '\0';
                    readingFilesystem = line;
                    forDev = string {};
                }
            }
            if (not readingFilesystem.empty () and forDev.empty () and ::strstr (line, "dev") != nullptr) {
                char* p = ::strchr (line, '=');
                if (p != nullptr) {
                    p++;
                    while (*p and isspace (*p)) {
                        ++p;
                    }
                }
                if (p != nullptr and * p != '\0') {
                    forDev = p;
                }
            }
            if (blockDeviceName == forDev) {
                return readingFilesystem;
            }
        }
        return SDKString {};
    }
    SDKString   FindFSForMajorMinorDev_LowLevel_ (int majorDev, int minorDev)
    {
        SDKString fsBlockName = GetBlockDeviceFileName2MajorMinorDev_ (majorDev, minorDev);
        if (not fsBlockName.empty ()) {
            return GetFSNameForBlockName_ (fsBlockName);
        }
        return SDKString {};
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
    SDKString   AIX_GET_EXE_PATH_ (pid_t pid, const String* hint)
    {
        /*
         *  What a PITA!
         *      http://unix.stackexchange.com/questions/109175/how-to-identify-executable-path-with-its-pid-on-aix-5-or-more
         *
         *  This is INSANELY slow and kludgey. If we need to support this, we will need to find a better way.
         *
         *  In the meantime, I at least rewrote it all without popen, so a bit faster, but still aweful.
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
                auto&& cleanup  =   Finally ([dirIt] () noexcept { ::closedir (dirIt); });
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
        static  Synchronized<Cache::LRUCache<String, SDKString, Cache::LRUCacheSupport::DefaultTraits<String, SDKString, 7>>>   sHintCache_ (256);
        if (hint != nullptr) {
            using   Memory::Optional;
            if (Optional<SDKString> o   =   sHintCache_->Lookup (*hint)) {
                struct  stat    hintStats;
                if (::stat (o->c_str (), &hintStats) == 0) {
                    if (hintStats.st_ino == inode and hintStats.st_dev == makedev (majorDev, minorDev)) {
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
                        if (IsProcessSafeToLogFor_ (pid)) {
                            DbgTrace (L"CacheHit for hint '%s' (maps to '%s')", hint->c_str (), String::FromSDKString (*o).c_str ());
                        }
#endif
                        return *o;
                    }
                }
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
                if (IsProcessSafeToLogFor_ (pid)) {
                    DbgTrace (L"CacheMiss for hint '%s'", hint->c_str ());
                }
#endif
            }
        }
        SDKString   fsName  =   FindFSForMajorMinorDev_WithCaching_ (majorDev, minorDev);
        string      exeName;
        if (not fsName.empty ()) {
            exeName = FindPath2Inode_ (fsName + "/", inode);
        }
        if (hint != nullptr and not exeName.empty ()) {
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
            if (IsProcessSafeToLogFor_ (pid)) {
                DbgTrace ("Priming cache for hint '%s' (maps to '%s')", hint->c_str (), exeName.c_str ());
            }
#endif
            sHintCache_->Add (*hint, exeName);
        }
        return exeName;
    }
}





/*
 ********************************************************************************
 ************************ Platform::AIX::GetEXEPathWithHintT ********************
 ********************************************************************************
 */
SDKString   Execution::Platform::AIX::GetEXEPathWithHintT (pid_t processID)
{
#if     USE_TIMING_TRACE_IN_THIS_MODULE_
    Debug::TimingTrace ctx { L"Platform::AIX::GetEXEPathWithHintT/1", IsProcessSafeToLogFor_ (processID) ? 0.001 : 1000.0 };
#endif
    return AIX_GET_EXE_PATH_ (processID, nullptr);
}
SDKString   Execution::Platform::AIX::GetEXEPathWithHintT (pid_t processID, const SDKString& associationHint)
{
#if     USE_TIMING_TRACE_IN_THIS_MODULE_
    Debug::TimingTrace ctx { L"Platform::AIX::GetEXEPathWithHintT/2", IsProcessSafeToLogFor_ (processID) ? 0.001 : 1000.0 };
#endif
    String tmp { String::FromSDKString (associationHint) };
    return AIX_GET_EXE_PATH_ (processID, &tmp);
}





/*
 ********************************************************************************
 ************************ Platform::AIX::GetEXEPathWithHint *********************
 ********************************************************************************
 */
String   Execution::Platform::AIX::GetEXEPathWithHint (pid_t processID)
{
#if     USE_TIMING_TRACE_IN_THIS_MODULE_
    Debug::TimingTrace ctx { L"Platform::AIX::GetEXEPathWithHint/1", IsProcessSafeToLogFor_ (processID) ? 0.001 : 1000.0 };
#endif
    return String::FromSDKString (AIX_GET_EXE_PATH_ (processID, nullptr));
}
String   Execution::Platform::AIX::GetEXEPathWithHint (pid_t processID, const String& associationHint)
{
#if     USE_TIMING_TRACE_IN_THIS_MODULE_
    Debug::TimingTrace ctx { L"Platform::AIX::GetEXEPathWithHint/2", IsProcessSafeToLogFor_ (processID) ? 0.001 : 1000.0 };
#endif
    return String::FromSDKString (AIX_GET_EXE_PATH_ (processID, &associationHint));
}
