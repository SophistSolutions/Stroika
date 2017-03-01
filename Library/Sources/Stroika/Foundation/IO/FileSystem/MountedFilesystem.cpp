/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#if qPlatform_Linux
#include <poll.h>
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
#include "../../Execution/Finally.h"
#include "../../Execution/Synchronized.h"
#include "../../Memory/SmallStackBuffer.h"

#include "FileInputStream.h"
#include "FileSystem.h"

#include "MountedFilesystem.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::IO;
using namespace Stroika::Foundation::IO::FileSystem;

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1

//#define qUseWATCHER_ 1
#ifndef qUseWATCHER_
#define qUseWATCHER_ qPlatform_POSIX
#endif

#if qUseWATCHER_
namespace {
    // experimental basis for file watcher
    struct Watcher_ {
        int mfd;
        Watcher_ (const String& fn)
            : mfd (::open (fn.AsNarrowSDKString ().c_str (), O_RDONLY, 0))
        {
        }
        ~Watcher_ ()
        {
            ::close (mfd);
        }

        bool IsNewAvail () const
        {
            struct pollfd pfd;
            int           rv;
            int           changes = 0;
            pfd.fd                = mfd;
            pfd.events            = POLLERR | POLLPRI;
            pfd.revents           = 0;
            if ((rv = poll (&pfd, 1, 5)) >= 0) {
                if (pfd.revents & POLLERR) {
                    return true;
                }
            }
        }
    };
}
#endif

namespace {
    /* 
     *  Something like this is used on many unix systems.
     */
    Collection<MountedFilesystemType> ReadMountInfo_MTabLikeFile_ (const Streams::InputStream<Memory::Byte>& readStream)
    {
        /*
         *  I haven't found this clearly documented yet, but it appears that a filesystem can be over-mounted.
         *  See https://www.kernel.org/doc/Documentation/filesystems/ramfs-rootfs-initramfs.txt
         *
         *  So the last one with a given mount point in the file wins.
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
                    IgnoreExceptionsExceptThreadAbortForCall (devName = IO::FileSystem::FileSystem::Default ().CanonicalizeName (devName));
                }
                String mountedAt = line[1];
                String fstype    = line[2];
                results.Add (MountedFilesystemType{mountedAt, Set<String>{devName}, fstype});
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
        static const String_Constant kUseFile2List_{L"/proc/mounts"};
#if qUseWATCHER_
        static const Watcher_                                             sWatcher_{kUseFile2List_};
        static Execution::Syncrhonized<Collection<MountedFilesystemType>> sLastResult_;
        static bool                                                       sFirstTime_{true};
        if (sFirstTime_ or sWatcher_.IsNewAvail ()) {
            sLastResult_ = ReadMountInfo_MTabLikeFile_ (FileInputStream::mk (kUseFile2List_, FileInputStream::eNotSeekable));
        }
        return sLastResult_;
#else
        return ReadMountInfo_MTabLikeFile_ (FileInputStream::mk (kUseFile2List_, FileInputStream::eNotSeekable));
#endif
    }
}
#endif
#if qPlatform_Linux
namespace {
    Collection<MountedFilesystemType> ReadMountInfo_ETC_MTAB_ ()
    {
        // Note - /procfs files always unseekable and this is sklink to /procfs
        static const String_Constant kUseFile2List_{L"/etc/mtab"};
        return ReadMountInfo_MTabLikeFile_ (FileInputStream::mk (kUseFile2List_, FileInputStream::eNotSeekable));
    }
}
#endif
#if qPlatform_Windows
namespace {
    using DynamicDiskIDType = String;
    static String GetPhysNameForDriveNumber_ (unsigned int i)
    {
        // This format is NOT super well documented, and was mostly derived from reading the remarks section
        // of https://msdn.microsoft.com/en-us/library/windows/desktop/aa363216%28v=vs.85%29.aspx?f=255&MSPPError=-2147217396
        // (DeviceIoControl function)
        return Characters::Format (L"\\\\.\\PhysicalDrive%d", i);
    }
    Optional<Set<DynamicDiskIDType>> GetDisksForVolume_ (String volumeName)
    {
        wchar_t volPathsBuf[10 * 1024] = {0};
        DWORD   retLen                 = 0;
        DWORD   x                      = ::GetVolumePathNamesForVolumeNameW (volumeName.c_str (), volPathsBuf, static_cast<DWORD> (NEltsOf (volPathsBuf)), &retLen);
        if (x == 0 or retLen <= 1) {
            return Set<String> ();
        }
        Assert (1 <= Characters::CString::Length (volPathsBuf) and Characters::CString::Length (volPathsBuf) < NEltsOf (volPathsBuf));
        volumeName = L"\\\\.\\" + String::FromSDKString (volPathsBuf).CircularSubString (0, -1);

        // @todo - rewrite this - must somehow otherwise callocate this to be large enuf (dynamic alloc) - if we want more disk exents, but not sure when that happens...
        VOLUME_DISK_EXTENTS volumeDiskExtents;
        {
            /*
             *  For reasons I don't understand (maybe a hit at http://superuser.com/questions/733687/give-regular-user-permission-to-access-physical-drive-on-windows)
             *  this only works with admin privilges
             */
            HANDLE hHandle = ::CreateFileW (volumeName.c_str (), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
            if (hHandle == INVALID_HANDLE_VALUE) {
                return {};
            }
            DWORD dwBytesReturned = 0;
            BOOL  bResult         = ::DeviceIoControl (hHandle, IOCTL_VOLUME_GET_VOLUME_DISK_EXTENTS, NULL, 0, &volumeDiskExtents, sizeof (volumeDiskExtents), &dwBytesReturned, NULL);
            ::CloseHandle (hHandle);
            if (not bResult) {
                return {};
            }
        }
        Set<DynamicDiskIDType> result;
        for (DWORD n = 0; n < volumeDiskExtents.NumberOfDiskExtents; ++n) {
            PDISK_EXTENT pDiskExtent = &volumeDiskExtents.Extents[n];
            result.Add (GetPhysNameForDriveNumber_ (pDiskExtent->DiskNumber));
        }
        return result;
    }

    Collection<MountedFilesystemType> GetMountedFilesystems_Windows_ ()
    {
        Collection<MountedFilesystemType> results{};
        TCHAR                             volumeNameBuf[1024];

        HANDLE hVol    = INVALID_HANDLE_VALUE;
        auto&& cleanup = Execution::Finally ([&]() noexcept { if (hVol != INVALID_HANDLE_VALUE) { ::CloseHandle (hVol); } });

        for (HANDLE hVol = ::FindFirstVolume (volumeNameBuf, static_cast<DWORD> (NEltsOf (volumeNameBuf))); hVol != INVALID_HANDLE_VALUE;) {
            DWORD lpMaximumComponentLength;
            DWORD dwSysFlags;
            TCHAR fileSysNameBuf[1024];
            if (::GetVolumeInformation (volumeNameBuf, nullptr, static_cast<DWORD> (NEltsOf (volumeNameBuf)), nullptr, &lpMaximumComponentLength, &dwSysFlags, fileSysNameBuf, static_cast<DWORD> (NEltsOf (fileSysNameBuf)))) {
                MountedFilesystemType v;
                v.fFileSystemType = String::FromSDKString (fileSysNameBuf);
                v.fVolumeID       = String::FromSDKString (volumeNameBuf);
                v.fDevicePaths    = GetDisksForVolume_ (volumeNameBuf);

                ///
                TCHAR volPathsBuf[10 * 1024];
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
                        v.fMountedOn = String::FromSDKString (NameIdx);
                        results.Add (v);
                    }
                }
            }

            // find next
            if (not::FindNextVolume (hVol, volumeNameBuf, static_cast<DWORD> (NEltsOf (volumeNameBuf)))) {
                ::FindVolumeClose (hVol);
                hVol = INVALID_HANDLE_VALUE;
            }
        }
        return results;
    }
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
    sb += L"Mounted-On: '" + fMountedOn + L"', ";
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
#elif qPlatform_Windows
    return GetMountedFilesystems_Windows_ ();
#else
    AssertNotImplemented ();
    return {};
#endif
}
