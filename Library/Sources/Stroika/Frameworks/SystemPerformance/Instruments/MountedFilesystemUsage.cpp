/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#include    "../../StroikaPreComp.h"

#if     qPlatform_Windows
#include    <Windows.h>
#endif

#include    "../../../Foundation/Characters/String_Constant.h"
#include    "../../../Foundation/Characters/String2Float.h"
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
using   namespace   Stroika::Foundation::DataExchange;
using   namespace   Stroika::Foundation::Execution;
using   namespace   Stroika::Foundation::Memory;

using   namespace   Stroika::Frameworks;
using   namespace   Stroika::Frameworks::SystemPerformance;
using   namespace   Stroika::Frameworks::SystemPerformance::Instruments::MountedFilesystemUsage;

using   Characters::String_Constant;





namespace {
    Sequence<VolumeInfo> capture_ ()
    {
        Sequence<VolumeInfo>   result;
#if     qPlatform_Windows
        TCHAR volumeNameBuf[1024];
        for (HANDLE hVol = FindFirstVolume (volumeNameBuf, NEltsOf(volumeNameBuf)); hVol != INVALID_HANDLE_VALUE; ) {
            DWORD lpMaximumComponentLength;
            DWORD dwSysFlags;
            TCHAR FileSysNameBuf[1024];
            if (::GetVolumeInformation( volumeNameBuf, NULL, NEltsOf(volumeNameBuf), NULL, &lpMaximumComponentLength, &dwSysFlags, FileSysNameBuf, NEltsOf(FileSysNameBuf))) {
                VolumeInfo v;
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
#elif   qPlatform_POSIX
        //
        // I looked through the /proc filesystem stuff and didnt see anything obvious to retrive this info...
        // run def with ProcessRunner
        //
        //  NEW NOTE - I THINK ITS IN THERE.... RE-EXAMINE proc/filesystems proc/partitions, and http://en.wikipedia.org/wiki/Procfs
        //      -- LGP 2014-08-01
        ProcessRunner pr (SDKSTR ("/bin/df -k -T"));
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
            Sequence<String>    l    =  Characters::Tokenize<String> (i, String_Constant (L" "));
            if (l.size () < 7) {
                DbgTrace ("skipping line cuz len=%d", l.size ());
                continue;
            }
            VolumeInfo v;
            v.fFileSystemType = l[1].Trim ();
            v.fMountedOnName = l[6].Trim ();
            {
                String  d   =   l[0].Trim ();
                if (not d.empty () and d != L"none") {
                    v.fDeviceOrVolumeName = d;
                }
            }
            v.fDiskSizeInBytes = Characters::String2Float<double> (l[2]) * 1024;
            v.fUsedSizeInBytes = Characters::String2Float<double> (l[3]) * 1024;
            result.Append (v);
        }
#endif
        return result;
    }
}






/*
 ********************************************************************************
 ********** Instruments::MountedFilesystemUsage::GetObjectVariantMapper *********
 ********************************************************************************
 */
ObjectVariantMapper Instruments::MountedFilesystemUsage::GetObjectVariantMapper ()
{
    ObjectVariantMapper sMapper_ = [] () -> ObjectVariantMapper {
        ObjectVariantMapper mapper;
        mapper.AddCommonType<Optional<double>> ();
        mapper.AddCommonType<Optional<String>> ();
        DISABLE_COMPILER_CLANG_WARNING_START("clang diagnostic ignored \"-Winvalid-offsetof\"");   // Really probably an issue, but not to debug here -- LGP 2014-01-04
        DISABLE_COMPILER_GCC_WARNING_START("GCC diagnostic ignored \"-Winvalid-offsetof\"");       // Really probably an issue, but not to debug here -- LGP 2014-01-04
        mapper.AddClass<VolumeInfo> ({
            ObjectVariantMapper_StructureFieldInfo_Construction_Helper (VolumeInfo, fFileSystemType, L"Filesystem-Type"),
            ObjectVariantMapper_StructureFieldInfo_Construction_Helper (VolumeInfo, fDeviceOrVolumeName, L"Device-Name"),
            ObjectVariantMapper_StructureFieldInfo_Construction_Helper (VolumeInfo, fVolumeID, L"Volume-ID"),
            ObjectVariantMapper_StructureFieldInfo_Construction_Helper (VolumeInfo, fMountedOnName, L"Mounted-On"),
            ObjectVariantMapper_StructureFieldInfo_Construction_Helper (VolumeInfo, fDiskSizeInBytes, L"Disk-Size"),
            ObjectVariantMapper_StructureFieldInfo_Construction_Helper (VolumeInfo, fUsedSizeInBytes, L"Disk-Used-Size"),
        });
        DISABLE_COMPILER_GCC_WARNING_END("GCC diagnostic ignored \"-Winvalid-offsetof\"");
        DISABLE_COMPILER_CLANG_WARNING_END("clang diagnostic ignored \"-Winvalid-offsetof\"");
        mapper.AddCommonType<Sequence<VolumeInfo>> ();
        return mapper;
    } ();
    return sMapper_;
}



/*
 ********************************************************************************
 ************* Instruments::MountedFilesystemUsage::GetInstrument ***************
 ********************************************************************************
 */
Instrument  SystemPerformance::Instruments::MountedFilesystemUsage::GetInstrument ()
{
    static  Instrument  kInstrument_    = Instrument (
            InstrumentNameType (String_Constant (L"Mounted-Filesystem-Usage")),
    [] () -> MeasurementSet {
        MeasurementSet    results;
        DateTime    before = DateTime::Now ();
        Sequence<VolumeInfo> volumes   =   capture_ ();
        results.fMeasuredAt = DateTimeRange (before, DateTime::Now ());
#if 0
        // @todo - make sure our above code doesnt emit NULL items!!!
        Mapping<String, VariantValue> vv;
        if (not v.fFileSystemType.empty ())
        {
            vv.Add (String_Constant (L"Filesystem-Type"), v.fFileSystemType);
        }
        if (not v.fDeviceOrVolumeName.empty ())
        {
            vv.Add (String_Constant (L"Device-Name"), v.fDeviceOrVolumeName);
        }
        if (not v.fVolumeID.empty ())
        {
            vv.Add (String_Constant (L"Volume-ID"), v.fVolumeID);
        }
        vv.Add (String_Constant (L"Mounted-On"), v.fMountedOnName);
        if (v.fDiskSizeInBytes.IsPresent ())
        {
            vv.Add (String_Constant (L"Disk-Size"), *v.fDiskSizeInBytes);
        }
        if (v.fUsedSizeInBytes.IsPresent ())
        {
            vv.Add (String_Constant (L"Disk-Used-Size"), *v.fUsedSizeInBytes);
        }
        volumnesAsVariants.Append (VariantValue (vv));
    }
#endif
    Measurement m;
    m.fValue = MountedFilesystemUsage::GetObjectVariantMapper ().FromObject (volumes);
               m.fType = kMountedVolumeUsage;
               results.fMeasurements.Add (m);
               return results;
},
{kMountedVolumeUsage}
                                      );
return kInstrument_;
}

Instrument  SystemPerformance::Instruments::GetMountedFilesystemUsage ()
{
    return MountedFilesystemUsage::GetInstrument ();
}
