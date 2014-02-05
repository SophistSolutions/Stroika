/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#include    "../../StroikaPreComp.h"

#if     qPlatform_Windows
#include    <Windows.h>
#endif

#include    "../../../Foundation/Characters/Format.h"
#include    "../../../Foundation/Characters/Tokenize.h"
#include    "../../../Foundation/Containers/Sequence.h"
#include    "../../../Foundation/Debug/Assertions.h"
#include    "../../../Foundation/Execution/ProcessRunner.h"
#include    "../../../Foundation/Streams/BasicBinaryInputOutputStream.h"
#include    "../../../Foundation/Streams/TextInputStreamBinaryAdapter.h"

#include    "../CommonMeasurementTypes.h"

#include    "MountedFilesystemUsage.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Containers;
using   namespace   Stroika::Foundation::Execution;
using   namespace   Stroika::Foundation::Memory;

using   namespace   Stroika::Frameworks;
using   namespace   Stroika::Frameworks::SystemPerformance;



namespace {
    struct VolumeInfo_ {
        String  fFileSystemType;
        String  fDeviceOrVolumeName;
        String  fVolumeID;
        String  fMountedOnName;
        Optional<double>    fDiskSizeInBytes;
        Optional<double>    fUsedSizeInBytes;
    };
}



namespace {
#if     qPlatform_Windows
    Sequence<VolumeInfo_> capture_ ()
    {
        Sequence<VolumeInfo_>   result;
        TCHAR volumeNameBuf[1024];
        for (HANDLE hVol = FindFirstVolume (volumeNameBuf, NEltsOf(volumeNameBuf)); hVol != INVALID_HANDLE_VALUE; ) {
            DWORD lpMaximumComponentLength;
            DWORD dwSysFlags;
            TCHAR FileSysNameBuf[1024];
            if (::GetVolumeInformation( volumeNameBuf, NULL, NEltsOf(volumeNameBuf), NULL, &lpMaximumComponentLength, &dwSysFlags, FileSysNameBuf, NEltsOf(FileSysNameBuf))) {
                VolumeInfo_ v;
                v.fFileSystemType = String::FromSDKString (FileSysNameBuf);
                v.fVolumeID = String::FromSDKString (volumeNameBuf);
                {
                    // @todo - use
                    //      http://msdn.microsoft.com/en-us/library/windows/desktop/cc542456(v=vs.85).aspx
                    //      CharCount = QueryDosDeviceW(&VolumeName[4], DeviceName, ARRAYSIZE(DeviceName));
                    //      to get DEVICENAME
                    //
                    TCHAR volPathsBuf[1024];
                    DWORD retLen = 0;
                    DWORD x = GetVolumePathNamesForVolumeName (volumeNameBuf, volPathsBuf, NEltsOf(volPathsBuf), &retLen);
                    if (volPathsBuf[0] == 0) {
                        v.fMountedOnName.clear ();
                        result.push_back (v);
                    }
                    else {
                        for (const TCHAR* NameIdx = volPathsBuf; NameIdx[0] != L'\0'; NameIdx += wcslen(NameIdx) + 1) {
                            v.fMountedOnName = String::FromSDKString (NameIdx);
                            {
                                ULARGE_INTEGER freeBytesAvailable;
                                ULARGE_INTEGER totalNumberOfBytes;
                                ULARGE_INTEGER totalNumberOfFreeBytes;
                                memset (&freeBytesAvailable, 0, sizeof (freeBytesAvailable));
                                memset (&totalNumberOfBytes, 0, sizeof (totalNumberOfBytes));
                                memset (&totalNumberOfFreeBytes, 0, sizeof (totalNumberOfFreeBytes));
                                DWORD xxx = GetDiskFreeSpaceEx (v.fMountedOnName.c_str (), &freeBytesAvailable, &totalNumberOfBytes, &totalNumberOfFreeBytes);
                                v.fDiskSizeInBytes = static_cast<double> (totalNumberOfBytes.QuadPart);
                                v.fUsedSizeInBytes = *v.fDiskSizeInBytes  - freeBytesAvailable.QuadPart;
                            }
                            result.push_back (v);
                        }
                    }
                }
            }
            else {
                // warning...
            }

            // find next
            if (not ::FindNextVolume (hVol, volumeNameBuf, NEltsOf(volumeNameBuf))) {
                FindVolumeClose (hVol);
                hVol = INVALID_HANDLE_VALUE;
            }
        }
        return result;
    }
#elif   qPlatform_POSIX
    Sequence<VolumeInfo_> capture_ ()
    {
        // I looked through the /proc filesystem stuff and didnt see anything obvious to retrive this info...
        // run def with ProcessRunner
        Sequence<VolumeInfo_>   result;
        ProcessRunner pr (SDKSTR ("/bin/df"));
#if 1
        Streams::BasicBinaryInputOutputStream   useStdOut;
        pr.SetStdOut (useStdOut);
        pr.Run ();
        String out;
        Streams::TextInputStreamBinaryAdapter   stdOut  =   Streams::TextInputStreamBinaryAdapter (useStdOut);
        bool skippedHeader = false;
        for (String i = stdOut.ReadLine (); not i.empty (); i = stdOut.ReadLine ()) {
            if (not skippedHeader) {
                skippedHeader = true;
                continue;
            }
            Sequence<String>    l    =  Characters::Tokenize<String> (i, L" ");
            if (l.size () < 6) {
                DbgTrace ("skipping line cuz len=%d", l.size ());
                continue;
            }
            VolumeInfo_ v;
            v.fMountedOnName = l[5];
            v.fDeviceOrVolumeName = l[0];
            v.fDiskSizeInBytes = Characters::String2Float (l[1]) * 1024;
            v.fUsedSizeInBytes = Characters::String2Float (l[2]) * 1024;
            result.Append (v);
        }

        //String out =  stdOut.ReadAll ();
        // redo with a bunch of readlines
#else
        String out = pr.Run (L"");
#endif
#if 0
        VolumeInfo_ foo;
        foo.fMountedOnName = out;
        result.Append (foo);    //tmphack to test
#endif
        return result;
    }
#endif
}





/*
 ********************************************************************************
 ***************** Instruments::GetMountedFilesystemUsage ***********************
 ********************************************************************************
 */
Instrument  SystemPerformance::Instruments::GetMountedFilesystemUsage ()
{
    static  Instrument  kInstrument_    = Instrument (
            InstrumentNameType (L"Mounted-Filesystem-Usage"),
    [] () -> Measurements {
        Measurements    results;
        DateTime    before = DateTime::Now ();
        Sequence<VolumeInfo_> volumes   =   capture_ ();
        results.fMeasuredAt = DateTimeRange (before, DateTime::Now ());
        Sequence<VariantValue>  volumnesAsVariants;
        for (VolumeInfo_ v : volumes)
        {
            Mapping<String, VariantValue> vv;
            vv.Add (L"Filesystem-Type", v.fFileSystemType);
            if (not v.fDeviceOrVolumeName.empty ()) {
                vv.Add (L"Device-Name", v.fDeviceOrVolumeName);
            }
            vv.Add (L"Volume-ID", v.fVolumeID);
            vv.Add (L"Mounted-On", v.fMountedOnName);
            if (v.fDiskSizeInBytes.IsPresent ()) {
                vv.Add (L"Disk-Size", *v.fDiskSizeInBytes);
            }
            if (v.fUsedSizeInBytes.IsPresent ()) {
                vv.Add (L"Disk-Used-Size", *v.fUsedSizeInBytes);
            }
            volumnesAsVariants.Append (VariantValue (vv));
        }
        Measurement m;
        m.fValue = volumnesAsVariants;
        m.fType = kMountedVolumeUsage;
        results.fMeasurements.Add (m);
        return results;
    },
    {kMountedVolumeUsage}
                                          );
    return kInstrument_;
}
