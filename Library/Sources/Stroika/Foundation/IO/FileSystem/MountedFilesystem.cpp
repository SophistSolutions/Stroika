/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include "../../Characters/CString/Utilities.h"
#include "../../Characters/String.h"
#include "../../Characters/StringBuilder.h"
#include "../../Characters/String_Constant.h"
#include "../../DataExchange/Variant/CharacterDelimitedLines/Reader.h"
#include "../../Execution/Finally.h"
#include "../../Memory/SmallStackBuffer.h"

#include "MountedFilesystem.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::IO;
using namespace Stroika::Foundation::IO::FileSystem;

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1

#if qPlatform_Linux
namespace {
    Collection<MountedFilesystemType> ReadMountInfo_FromProcFSMounts_ ()
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
        static const String_Constant kProcMountsFileName_{L"/proc/mounts"};
        for (Sequence<String> line : reader.ReadMatrix (FileInputStream::mk (kProcMountsFileName_, FileInputStream::eNotSeekable))) {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
            DbgTrace (L"in IO::FileSystem::{}::ReadMountInfo_FromProcFSMounts_ linesize=%d, line[0]=%s", line.size (), line.empty () ? L"" : line[0].c_str ());
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
                result.Add (MountedFilesystemType{mountedAt, devName, fstype});
            }
        }
        return results;
    }
}
#elif qPlatform_Windows
namespace {
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
    sb += L"Device-Path: '" + fDevicePath + L"', ";
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
Containers::Collection<MountedFilesystemType> FileSystem::GetMountedFilesystems ()
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
