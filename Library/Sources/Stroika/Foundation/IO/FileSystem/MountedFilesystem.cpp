/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#if qPlatform_Windows
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
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       174

namespace {
    /* 
     *  Something like this is used on many unix systems.
     */
    Collection<MountedFilesystemType> ReadMountInfo_MTabLikeFile_ (const String& filename)
    {
        /*
         *  I haven't found this clearly documented yet, but it appears that a filesystem can be over-mounted.
         *  See https://www.kernel.org/doc/Documentation/filesystems/ramfs-rootfs-initramfs.txt
         *
         *  So the last one with a given mount point in the file wins.
         */
        Collection<MountedFilesystemType>                      results;
        DataExchange::Variant::CharacterDelimitedLines::Reader reader{{' ', '\t'}};
        // Note - /procfs files always unseekable
        for (Sequence<String> line : reader.ReadMatrix (FileInputStream::mk (filename, FileInputStream::eNotSeekable))) {
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
        static const String_Constant kUseFile2List_{L"/proc/mounts"};
        return ReadMountInfo_MTabLikeFile_ (kUseFile2List_);
    }
}
#endif
#if qPlatform_Linux or qPlatform_MacOS
namespace {
    Collection<MountedFilesystemType> ReadMountInfo_ETC_MTAB_ ()
    {
        static const String_Constant kUseFile2List_{L"/etc/mtab"};
        return ReadMountInfo_MTabLikeFile_ (kUseFile2List_);
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
    Set<DynamicDiskIDType> GetDisksForVolume_ (String volumeName)
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
            HANDLE hHandle = ::CreateFileW (volumeName.c_str (), GENERIC_READ /*|GENERIC_WRITE*/, FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
            if (hHandle == INVALID_HANDLE_VALUE) {
                return Set<String> ();
            }
            DWORD dwBytesReturned = 0;
            BOOL  bResult         = ::DeviceIoControl (hHandle, IOCTL_VOLUME_GET_VOLUME_DISK_EXTENTS, NULL, 0, &volumeDiskExtents, sizeof (volumeDiskExtents), &dwBytesReturned, NULL);
            ::CloseHandle (hHandle);
            if (not bResult) {
                return Set<String> ();
            }
        }
        Set<DynamicDiskIDType> result;
        for (DWORD n = 0; n < volumeDiskExtents.NumberOfDiskExtents; ++n) {
            PDISK_EXTENT pDiskExtent = &volumeDiskExtents.Extents[n];
#if 0
            _tprintf (_T ("Disk number: %d\n"), pDiskExtent->DiskNumber);
            _tprintf (_T ("DBR start sector: %I64d\n"), pDiskExtent->StartingOffset.QuadPart / 512);
#endif
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
    sb += L"Device-Path: '" + Characters::ToString (fDevicePaths) + L"', ";
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
    return ReadMountInfo_ETC_MTAB_ ();
#elif qPlatform_Windows
    return GetMountedFilesystems_Windows_ ();
#else
    AssertNotImplemented ();
    return {};
#endif
}
