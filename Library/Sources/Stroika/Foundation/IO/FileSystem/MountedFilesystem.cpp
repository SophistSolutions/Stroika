/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#if qPlatform_Linux
#include <fcntl.h>
#include <poll.h>
#include <unistd.h>
#elif qPlatform_MacOS
#include <fstab.h>
#elif qPlatform_Windows
#include <Windows.h>
#include <winioctl.h>
#endif

#include "../../Characters/CString/Utilities.h"
#include "../../Characters/Format.h"
#include "../../Characters/String.h"
#include "../../Characters/StringBuilder.h"
#include "../../Characters/String_Constant.h"
#include "../../Characters/ToString.h"
#include "../../DataExchange/Variant/CharacterDelimitedLines/Reader.h"
#include "../../Debug/Sanitizer.h"
#include "../../Execution/Finally.h"
#include "../../Execution/Synchronized.h"
#include "../../Memory/SmallStackBuffer.h"

#include "FileInputStream.h"
#include "FileSystem.h"
#include "PathName.h"

#include "MountedFilesystem.h"

using std::byte;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::IO;
using namespace Stroika::Foundation::IO::FileSystem;

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1

#if qPlatform_Linux
namespace {
    // This is quirky, and only works for Linux, and /proc/mounts
    struct Watcher_Proc_Mounts_ {
        int mfd;
        Watcher_Proc_Mounts_ (const filesystem::path& fn)
            : mfd (::open (fn.c_str (), O_RDONLY, 0))
        {
        }
        ~Watcher_Proc_Mounts_ ()
        {
            ::close (mfd);
        }
        bool IsNewAvail () const
        {
            // according to http://stackoverflow.com/questions/5070801/monitoring-mount-point-changes-via-proc-mounts
            // have to use poll with POLLPRI | POLLERR flags
            //
            // and from https://linux.die.net/man/5/proc
            //      /proc/[pid]/mounts (since Linux 2.4.19)...
            //      Since kernel version 2.6.15, this file is pollable: after opening the file for reading, a change in this file
            //      (i.e., a file system mount or unmount) causes select(2) to mark the file descriptor as readable, and poll(2)
            //      and epoll_wait(2) mark the file as having an error condition.
            struct pollfd pfd;
            int           rv;
            pfd.fd      = mfd;
            pfd.events  = POLLERR | POLLPRI;
            pfd.revents = 0;
            if ((rv = poll (&pfd, 1, 0)) >= 0) {
                if (pfd.revents & POLLERR) {
                    return true;
                }
            }
            return false;
        }
    };
}
#endif

#if qPlatform_MacOS
namespace {
    // this also works on Linux, but is a horrible API
    Collection<MountedFilesystemType> ReadMountInfo_getfsent_ ()
    {
        // @todo - note - this only appears to capture 'fixed disks' - not network mounts, and and virtual mount points like /dev/
        Collection<MountedFilesystemType> results{};
        static mutex                      sMutex_; // this API (getfsent) is NOT threadsafe, but we can at least make our use re-entrant
        [[maybe_unused]] auto&&           critSec = lock_guard{sMutex_};
        [[maybe_unused]] auto&&           cleanup = Execution::Finally ([&] () noexcept { ::endfsent (); });
        while (fstab* fs = ::getfsent ()) {
            results += MountedFilesystemType{fs->fs_file, Containers::Set<filesystem::path>{fs->fs_spec}, String::FromNarrowSDKString (fs->fs_vfstype)};
        }
        return results;
    }
}
#endif

namespace {
    /* 
     *  Something like this is used on many unix systems.
     */
    Collection<MountedFilesystemType> ReadMountInfo_MTabLikeFile_ (const Streams::InputStream<byte>::Ptr& readStream)
    {
        /*
         *  I haven't found this clearly documented yet, but it appears that a filesystem can be over-mounted.
         *  See https://www.kernel.org/doc/Documentation/filesystems/ramfs-rootfs-initramfs.txt
         *
         *  So the last one with a given mount point in the file wins.
         *
         *  EXAMPLE OUTPUT (from ubuntu on WSL):
         *       cat /proc/mounts
         *       /dev/sdb / ext4 rw,relatime,discard,errors=remount-ro,data=ordered 0 0
         *       tmpfs /mnt/wsl tmpfs rw,relatime 0 0
         *       tools /init 9p ro,dirsync,relatime,aname=tools;fmask=022,loose,access=client,trans=fd,rfd=6,wfd=6 0 0
         *       none /dev devtmpfs rw,nosuid,relatime,size=6474068k,nr_inodes=1618517,mode=755 0 0
         *       sysfs /sys sysfs rw,nosuid,nodev,noexec,noatime 0 0
         *       ...
         */
        Collection<MountedFilesystemType>                      results;
        DataExchange::Variant::CharacterDelimitedLines::Reader reader{{' ', '\t'}};
        for (Sequence<String> line : reader.ReadMatrix (readStream)) {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
            DbgTrace (L"in IO::FileSystem::{}::ReadMountInfo_MTabLikeFile_ linesize=%d, line[0]=%s", line.size (), line.empty () ? L"" : line[0].c_str ());
#endif
            //
            // https://www.centos.org/docs/5/html/5.2/Deployment_Guide/s2-proc-mounts.html
            //
            //  1 - device name
            //  2 - mounted on
            //  3 - fstype
            //
            if (line.size () >= 3) {
                String devName = line[0];
                // procfs/mounts often contains symbolic links to device files
                // e.g. /dev/disk/by-uuid/e1d70192-1bb0-461d-b89f-b054e45bfa00
                if (devName.StartsWith (L"/")) {
                    IgnoreExceptionsExceptThreadInterruptForCall (devName = IO::FileSystem::FromPath (filesystem::canonical (IO::FileSystem::ToPath (devName))));
                }
                filesystem::path    mountedAt = ToPath (line[1]);
                String              fstype    = line[2];
                static const String kNone_{L"none"_k};
                results.Add (MountedFilesystemType{mountedAt, devName == kNone_ ? Set<filesystem::path>{} : Set<filesystem::path>{IO::FileSystem::ToPath (devName)}, fstype}); // special name none often used when there is no name
            }
        }
        return results;
    }
}
#if qPlatform_Linux
namespace {
    Collection<MountedFilesystemType> ReadMountInfo_FromProcFSMounts_ ()
    {
        // Note - /procfs files always unseekable
        static const filesystem::path kUseFile2List_{"/proc/mounts"};
        // https://stroika.atlassian.net/browse/STK-665
        // kUseWatcher_ seems safe and much faster. But on gcc8, produces a mysterious error with address sanitizer, do disable
        // for now
        //  @todo fix address sanitizer issue? APPEARS not my bug --LGP 2018-09-28
        //  RETRY AFTER gcc8
        //
        constexpr bool kUseWatcher_{Debug::kBuiltWithAddressSanitizer ? false : true};
        if (kUseWatcher_) {
            static const Watcher_Proc_Mounts_                                 sWatcher_{kUseFile2List_};
            static Execution::Synchronized<Collection<MountedFilesystemType>> sLastResult_;
            static bool                                                       sFirstTime_{true};
            if (sFirstTime_ or sWatcher_.IsNewAvail ()) {
                sLastResult_ = ReadMountInfo_MTabLikeFile_ (FileInputStream::New (kUseFile2List_, FileInputStream::eNotSeekable));
                sFirstTime_  = false;
            }
            return sLastResult_;
        }
        else {
            return ReadMountInfo_MTabLikeFile_ (FileInputStream::New (kUseFile2List_, FileInputStream::eNotSeekable));
        }
    }
}
#endif
#if qPlatform_Linux
namespace {
    Collection<MountedFilesystemType> ReadMountInfo_ETC_MTAB_ ()
    {
        // Note - /procfs files always unseekable and this is sklink to /procfs
        static const filesystem::path kUseFile2List_{"/etc/mtab"};
        return ReadMountInfo_MTabLikeFile_ (FileInputStream::New (kUseFile2List_, FileInputStream::eNotSeekable));
    }
}
#endif
#if qPlatform_Windows
namespace {
    using DynamicDiskIDType_ = filesystem::path;
    DynamicDiskIDType_ GetPhysNameForDriveNumber_ (unsigned int i)
    {
        // This format is NOT super well documented, and was mostly derived from reading the remarks section
        // of https://msdn.microsoft.com/en-us/library/windows/desktop/aa363216%28v=vs.85%29.aspx?f=255&MSPPError=-2147217396
        // (DeviceIoControl function)
        return ToPath (Characters::Format (L"\\\\.\\PhysicalDrive%d", i));
    }
    DISABLE_COMPILER_MSC_WARNING_START (6262) // stack usage OK
    optional<Set<DynamicDiskIDType_>> GetDisksForVolume_ (String volumeName)
    {
        wchar_t volPathsBuf[10 * 1024]; // intentionally uninitialized since we don't use it if GetVolumePathNamesForVolumeNameW () returns error, and its an OUT only parameter
        DWORD   retLen = 0;
        DWORD   x      = ::GetVolumePathNamesForVolumeNameW (volumeName.c_str (), volPathsBuf, static_cast<DWORD> (NEltsOf (volPathsBuf)), &retLen);
        if (x == 0) {
            return {}; // missing - no known - not empty - answer
        }
        else if (retLen <= 1) {
            return Set<DynamicDiskIDType_>{};
        }
        Assert (1 <= Characters::CString::Length (volPathsBuf) and Characters::CString::Length (volPathsBuf) < NEltsOf (volPathsBuf));
        volumeName = L"\\\\.\\" + String::FromSDKString (volPathsBuf).SubString (0, -1);

        // @todo - rewrite this - must somehow otherwise callocate this to be large enuf (dynamic alloc) - if we want more disk exents, but not sure when that happens...
        VOLUME_DISK_EXTENTS volumeDiskExtents;
        {
            /*
             *  For reasons I don't understand (maybe a hit at http://superuser.com/questions/733687/give-regular-user-permission-to-access-physical-drive-on-windows)
             *  this only works with admin privilges
             */
            HANDLE hHandle = ::CreateFileW (volumeName.c_str (), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
            if (hHandle == INVALID_HANDLE_VALUE) {
                return {};
            }
            DWORD dwBytesReturned = 0;
            BOOL  bResult         = ::DeviceIoControl (hHandle, IOCTL_VOLUME_GET_VOLUME_DISK_EXTENTS, nullptr, 0, &volumeDiskExtents, sizeof (volumeDiskExtents), &dwBytesReturned, NULL);
            ::CloseHandle (hHandle);
            if (not bResult) {
                return {};
            }
        }
        Set<DynamicDiskIDType_> result;
        for (DWORD n = 0; n < volumeDiskExtents.NumberOfDiskExtents; ++n) {
            result.Add (GetPhysNameForDriveNumber_ (volumeDiskExtents.Extents[n].DiskNumber));
        }
        return result;
    }
    DISABLE_COMPILER_MSC_WARNING_END (6262)

    DISABLE_COMPILER_MSC_WARNING_START (6262) // stack usage OK
    Collection<MountedFilesystemType> GetMountedFilesystems_Windows_ ()
    {
        Collection<MountedFilesystemType> results{};
        TCHAR                             volumeNameBuf[1024]; // intentionally uninitialized since OUT parameter and not used unless FindFirstVolume success

        HANDLE                  hVol    = INVALID_HANDLE_VALUE;
        [[maybe_unused]] auto&& cleanup = Execution::Finally ([&] () noexcept { if (hVol != INVALID_HANDLE_VALUE) { ::CloseHandle (hVol); } });

        for (hVol = ::FindFirstVolume (volumeNameBuf, static_cast<DWORD> (NEltsOf (volumeNameBuf))); hVol != INVALID_HANDLE_VALUE;) {
            DWORD lpMaximumComponentLength;
            DWORD dwSysFlags;
            TCHAR fileSysNameBuf[1024];
            if (::GetVolumeInformation (volumeNameBuf, nullptr, static_cast<DWORD> (NEltsOf (volumeNameBuf)), nullptr, &lpMaximumComponentLength, &dwSysFlags, fileSysNameBuf, static_cast<DWORD> (NEltsOf (fileSysNameBuf)))) {
                MountedFilesystemType v;
                v.fFileSystemType = String::FromSDKString (fileSysNameBuf);
                v.fVolumeID       = String::FromSDKString (volumeNameBuf);
                v.fDevicePaths    = GetDisksForVolume_ (volumeNameBuf);

                TCHAR volPathsBuf[10 * 1024]; // intentionally uninitialized
                DWORD retLen = 0;
                DWORD x      = ::GetVolumePathNamesForVolumeName (volumeNameBuf, volPathsBuf, static_cast<DWORD> (NEltsOf (volPathsBuf)), &retLen);
                if (x == 0) {
                    DbgTrace (SDKSTR ("Ignoring error getting paths (volume='%s')"), volumeNameBuf);
                }
                else if (volPathsBuf[0] == 0) {
                    // Ignore - unmounted!
                    DbgTrace (SDKSTR ("Ignoring unmounted filesystem (volume='%s')"), volumeNameBuf);
                }
                else {
                    for (const TCHAR* NameIdx = volPathsBuf; NameIdx[0] != L'\0'; NameIdx += Characters::CString::Length (NameIdx) + 1) {
                        v.fMountedOn = filesystem::path (NameIdx);
                        results.Add (v);
                    }
                }
            }

            // find next
            if (not ::FindNextVolume (hVol, volumeNameBuf, static_cast<DWORD> (NEltsOf (volumeNameBuf)))) {
                ::FindVolumeClose (hVol);
                hVol = INVALID_HANDLE_VALUE;
            }
        }
        return results;
    }
    DISABLE_COMPILER_MSC_WARNING_END (6262)
}
#endif

/*
 ********************************************************************************
 ******************** IO::FileSystem::MountedFilesystemType *********************
 ********************************************************************************
 */
String MountedFilesystemType::ToString () const
{
    StringBuilder sb;
    sb += L"{";
    sb += L"Mounted-On: " + Characters::ToString (fMountedOn) + L", ";
    if (fDevicePaths) {
        sb += L"Device-Paths: " + Characters::ToString (*fDevicePaths) + L", ";
    }
    if (fFileSystemType) {
        sb += L"FileSystem-Type: '" + *fFileSystemType + L"', ";
    }
    if (fVolumeID) {
        sb += L"Volume-ID: '" + *fVolumeID + L"', ";
    }
    sb += L"}";
    return sb.str ();
}

/*
 ********************************************************************************
 ******************** IO::FileSystem::GetMountedFilesystems *********************
 ********************************************************************************
 */
Containers::Collection<MountedFilesystemType> IO::FileSystem::GetMountedFilesystems ()
{
#if qPlatform_Linux
    return ReadMountInfo_FromProcFSMounts_ ();
#elif qPlatform_MacOS
    return ReadMountInfo_getfsent_ ();
#elif qPlatform_Windows
    return GetMountedFilesystems_Windows_ ();
#else
    // @todo - maybe a start on macos would be to walk the directory /Volumes
    WeakAssertNotImplemented ();
    return {};
#endif
}
