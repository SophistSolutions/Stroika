/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#include    "../../StroikaPreComp.h"

#if     qPlatform_Windows
#include    <Windows.h>
#endif

#include    "../../../Foundation/Characters/CString/Utilities.h"
#include    "../../../Foundation/Characters/String_Constant.h"
#include    "../../../Foundation/Characters/String2Float.h"
#include    "../../../Foundation/Containers/Mapping.h"
#include    "../../../Foundation/Containers/Sequence.h"
#include    "../../../Foundation/DataExchange/CharacterDelimitedLines/Reader.h"
#include    "../../../Foundation/Debug/Assertions.h"
#include    "../../../Foundation/Execution/Sleep.h"
#include    "../../../Foundation/Execution/StringException.h"
#include    "../../../Foundation/Execution/ProcessRunner.h"
#include    "../../../Foundation/IO/FileSystem/BinaryFileInputStream.h"
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
using   Time::DurationSecondsType;


// for io stats
#ifndef qUseWMICollectionSupport_
#define qUseWMICollectionSupport_       qPlatform_Windows
#endif


#if     qUseWMICollectionSupport_
#include    "../Support/WMICollector.h"

using   SystemPerformance::Support::WMICollector;
#endif




namespace {
#if     qUseWMICollectionSupport_
    const   String_Constant     kDiskReadBytesPerSec_   { L"Disk Read Bytes/sec" };
    const   String_Constant     kDiskWriteBytesPerSec_  { L"Disk Write Bytes/sec" };
    const   String_Constant     kDiskReadsPerSec_       { L"Disk Reads/sec" };
    const   String_Constant     kDiskWritesPerSec_      { L"Disk Writes/sec" };
    const   String_Constant     kPctDiskReadTime_       { L"% Disk Read Time" };
    const   String_Constant     kPctDiskWriteTime_      { L"% Disk Write Time" };
#endif
}






#if     qPlatform_POSIX
namespace {
    struct  CapturerWithContext_POSIX_ {
        struct PerfStats_ {
            double  fSectorsRead;
            double  fTimeSpentReading;
            double  fReadsCompleted;
            double  fSectorsWritten;
            double  fTimeSpentWriting;
            double  fWritesCompleted;
        };
        Optional<Mapping<String, PerfStats_>>   fContextStats_;
        CapturerWithContext_POSIX_ (Options options)
        {
            capture_ ();        // for side-effect of setting fContextStats_
        }
        CapturerWithContext_POSIX_ (const CapturerWithContext_POSIX_& from) = default;
        Sequence<VolumeInfo> capture_ ()
        {
            Sequence<VolumeInfo>   results;
            results = capture_Process_Run_DF_ ();
            try {
                Mapping<String, PerfStats_> diskStats = capture_ProcFSDiskStats_ ();
                Sequence<VolumeInfo>    newV;
                for (VolumeInfo v : results) {
                    if (v.fDeviceOrVolumeName.IsPresent ()) {
                        if (fContextStats_) {
                            String  devNameLessSlashes = *v.fDeviceOrVolumeName;
                            size_t i = devNameLessSlashes.RFind ('/');
                            if (i != string::npos) {
                                devNameLessSlashes = devNameLessSlashes.SubString (i + 1);
                            }
                            Optional<PerfStats_>    oOld = fContextStats_->Lookup (devNameLessSlashes);
                            Optional<PerfStats_>    oNew = diskStats.Lookup (devNameLessSlashes);
                            if (oOld.IsPresent () and oNew.IsPresent ()) {
                                const unsigned int kSectorSizeTmpHack_ = 4 * 1024;      // @todo GET from disk stats
                                v.fReadIOStats.fBytesTransfered = (oNew->fSectorsRead - oOld->fSectorsRead) * kSectorSizeTmpHack_;
                                v.fReadIOStats.fTotalTransfers = oNew->fReadsCompleted - oOld->fReadsCompleted;
                                v.fReadIOStats.fTimeTransfering = (oNew->fTimeSpentReading - oOld->fTimeSpentReading);
                                v.fWriteIOStats.fBytesTransfered = (oNew->fSectorsWritten - oOld->fSectorsWritten) * kSectorSizeTmpHack_;
                                v.fWriteIOStats.fTotalTransfers = oNew->fWritesCompleted - oOld->fWritesCompleted;
                                v.fWriteIOStats.fTimeTransfering = oNew->fTimeSpentWriting - oOld->fTimeSpentWriting;

                                v.fIOStats.fBytesTransfered = *v.fReadIOStats.fBytesTransfered + *v.fWriteIOStats.fBytesTransfered;
                                v.fIOStats.fTotalTransfers = *v.fReadIOStats.fTotalTransfers + *v.fWriteIOStats.fTotalTransfers;
                                v.fIOStats.fTimeTransfering = *v.fReadIOStats.fTimeTransfering + *v.fWriteIOStats.fTimeTransfering;
                            }
                        }
                    }
                    newV.Append (v);
                }
                fContextStats_ = diskStats;
                results = newV;
            }
            catch (...) {
                DbgTrace ("Exception gathering procfs disk io stats");
            }
            return results;
        }
        Sequence<VolumeInfo> capture_Process_Run_DF_ (bool includeFSTypes)
        {
            Sequence<VolumeInfo>   result;
            //
            // I looked through the /proc filesystem stuff and didnt see anything obvious to retrive this info...
            // run def with ProcessRunner
            //
            //  NEW NOTE - I THINK ITS IN THERE.... RE-EXAMINE proc/filesystems proc/partitions, and http://en.wikipedia.org/wiki/Procfs
            //      -- LGP 2014-08-01
            ProcessRunner pr { includeFSTypes ? L"/bin/df -k -T" : L"/bin/df -k" };
            Streams::BasicBinaryInputOutputStream   useStdOut;
            pr.SetStdOut (useStdOut);
            std::exception_ptr runException;
            try {
                pr.Run ();
            }
            catch (...) {
                runException = current_exception ();
            }
            String out;
            Streams::TextInputStreamBinaryAdapter   stdOut  =   Streams::TextInputStreamBinaryAdapter (useStdOut);
            bool skippedHeader = false;
            for (String i = stdOut.ReadLine (); not i.empty (); i = stdOut.ReadLine ()) {
                if (not skippedHeader) {
                    skippedHeader = true;
                    continue;
                }
                Sequence<String>    l    =  i.Tokenize (Set<Characters::Character> { ' ' });
                if (l.size () < (includeFSTypes ? 7 : 6)) {
                    DbgTrace ("skipping line cuz len=%d", l.size ());
                    continue;
                }
                VolumeInfo v;
                if (includeFSTypes) {
                    v.fFileSystemType = l[1].Trim ();
                }
                v.fMountedOnName = l[includeFSTypes ? 6 : 5].Trim ();
                {
                    String  d   =   l[0].Trim ();
                    if (not d.empty () and d != L"none") {
                        v.fDeviceOrVolumeName = d;
                    }
                }
                v.fDiskSizeInBytes = Characters::String2Float<double> (l[includeFSTypes ? 2 : 1]) * 1024;
                v.fUsedSizeInBytes = Characters::String2Float<double> (l[includeFSTypes ? 3 : 2]) * 1024;
                result.Append (v);
            }
            // Sometimes (with busy box df especailly) we get bogus error return. So only rethrow if we found no good data
            if (runException and result.empty ()) {
                Execution::DoReThrow (runException);
            }
            return result;
        }
        Sequence<VolumeInfo> capture_Process_Run_DF_ ()
        {
            try {
                return capture_Process_Run_DF_ (true);
            }
            catch (...) {
                return capture_Process_Run_DF_ (false);
            }
        }
        Mapping<String, PerfStats_> capture_ProcFSDiskStats_ ()
        {
            using   IO::FileSystem::BinaryFileInputStream;
            using   Characters::String2Float;
            Mapping<String, PerfStats_>   result;
            DataExchange::CharacterDelimitedLines::Reader reader {{' ', '\t' }};
            const   String_Constant kProcMemInfoFileName_ { L"/proc/diskstats" };
            // Note - /procfs files always unseekable
            for (Sequence<String> line : reader.ReadMatrix (BinaryFileInputStream::mk (kProcMemInfoFileName_, BinaryFileInputStream::eNotSeekable))) {
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
                DbgTrace (L"***in Instruments::MountedFilesystemUsage::capture_ProcFSDiskStats_ linesize=%d, line[0]=%s", line.size(), line.empty () ? L"" : line[0].c_str ());
#endif
                //
                // https://www.kernel.org/doc/Documentation/ABI/testing/procfs-diskstats
                //
                //  3 - device name
                //  4 - reads completed successfully
                //  6 - sectors read
                //  7 - time spent reading (ms)
                //  8 - writes completed
                //  10 - sectors written
                //  11 - time spent writing (ms)
                //
                if (line.size () >= 13) {
                    String  devName = line[3 - 1];
                    String  readsCompleted = line[4 - 1];
                    String  sectorsRead = line[6 - 1];
                    String  timeSpentReadingMS = line[7 - 1];
                    String  writesCompleted = line[8 - 1];
                    String  sectorsWritten = line[10 - 1];
                    String  timeSpentWritingMS = line[11 - 1];
                    result.Add (
                        devName,
                    PerfStats_ {
                        String2Float (sectorsRead), String2Float (timeSpentReadingMS) / 1000,  String2Float (readsCompleted),
                        String2Float (sectorsWritten),  String2Float (timeSpentWritingMS) / 1000, String2Float (writesCompleted)
                    }
                    );
                }
            }
            return result;
        }
    };
}
#endif










#if     qPlatform_Windows
namespace {
    struct  CapturerWithContext_Windows_ {
#if   qUseWMICollectionSupport_
        WMICollector    fLogicalDiskWMICollector_;
#endif
        CapturerWithContext_Windows_ (Options options)
#if     qUseWMICollectionSupport_
            : fLogicalDiskWMICollector_ { String_Constant { L"LogicalDisk" }, {},  {kDiskReadBytesPerSec_, kDiskWriteBytesPerSec_, kDiskReadsPerSec_, kDiskWritesPerSec_,  kPctDiskReadTime_, kPctDiskWriteTime_ } }
#endif
        {
#if   qUseWMICollectionSupport_
            capture_Windows_GetVolumeInfo_ ();   // for side-effect of setting fLogicalDiskWMICollector_
            {
                const Time::DurationSecondsType kUseIntervalIfNoBaseline_ { 1.0 };
                Execution::Sleep (kUseIntervalIfNoBaseline_);
            }
#endif
        }
        CapturerWithContext_Windows_ (const CapturerWithContext_Windows_& from)
#if   qUseWMICollectionSupport_
            : fLogicalDiskWMICollector_ (from.fLogicalDiskWMICollector_)
#endif
        {
#if   qUseWMICollectionSupport_
            capture_Windows_GetVolumeInfo_ ();   // for side-effect of setting fLogicalDiskWMICollector_
            {
                const Time::DurationSecondsType kUseIntervalIfNoBaseline_ { 1.0 };
                Execution::Sleep (kUseIntervalIfNoBaseline_);
            }
#endif
        }
        Sequence<VolumeInfo> capture_ ()
        {
            Sequence<VolumeInfo>   results;
            results = capture_Windows_GetVolumeInfo_ ();
            return results;
        }
        Sequence<VolumeInfo> capture_Windows_GetVolumeInfo_ ()
        {
#if     qUseWMICollectionSupport_
            Time::DurationSecondsType   timeOfPrevCollection = fLogicalDiskWMICollector_.GetTimeOfLastCollection ();
            fLogicalDiskWMICollector_.Collect ();
            Time::DurationSecondsType   timeCollecting { fLogicalDiskWMICollector_.GetTimeOfLastCollection () - timeOfPrevCollection };
#endif
            Sequence<VolumeInfo>   result;
            TCHAR volumeNameBuf[1024];
            for (HANDLE hVol = FindFirstVolume (volumeNameBuf, NEltsOf(volumeNameBuf)); hVol != INVALID_HANDLE_VALUE; ) {
                DWORD lpMaximumComponentLength;
                DWORD dwSysFlags;
                TCHAR FileSysNameBuf[1024];
                if (::GetVolumeInformation ( volumeNameBuf, NULL, NEltsOf(volumeNameBuf), NULL, &lpMaximumComponentLength, &dwSysFlags, FileSysNameBuf, NEltsOf(FileSysNameBuf))) {
                    VolumeInfo v;
                    v.fFileSystemType = String::FromSDKString (FileSysNameBuf);
                    v.fVolumeID = String::FromSDKString (volumeNameBuf);
                    {
                        /*
                         *  On Windoze, each volume object can have multiple sub-volumes (logical volumes) and we return the size of each).
                         */


                        ///
                        /// @todo FIX TO USE get extended error information, call GetLastError. If the buffer is not large enough to hold the complete list,
                        /// the error code is ERROR_MORE_DATA a
                        /// so resize buffer accordingly...
                        ///

                        // @todo - use
                        //      http://msdn.microsoft.com/en-us/library/windows/desktop/cc542456(v=vs.85).aspx
                        //      CharCount = QueryDosDeviceW(&VolumeName[4], DeviceName, ARRAYSIZE(DeviceName));
                        //      to get DEVICENAME
                        //
                        TCHAR volPathsBuf[10 * 1024];
                        DWORD retLen = 0;
                        DWORD x = GetVolumePathNamesForVolumeName (volumeNameBuf, volPathsBuf, NEltsOf(volPathsBuf), &retLen);
                        if (volPathsBuf[0] == 0) {
                            v.fMountedOnName.clear ();
                            result.push_back (v);
                        }
                        else {
                            for (const TCHAR* NameIdx = volPathsBuf; NameIdx[0] != L'\0'; NameIdx += Characters::CString::Length (NameIdx) + 1) {
                                v.fMountedOnName = String::FromSDKString (NameIdx);
                                {
                                    ULARGE_INTEGER freeBytesAvailable;
                                    ULARGE_INTEGER totalNumberOfBytes;
                                    ULARGE_INTEGER totalNumberOfFreeBytes;
                                    memset (&freeBytesAvailable, 0, sizeof (freeBytesAvailable));
                                    memset (&totalNumberOfBytes, 0, sizeof (totalNumberOfBytes));
                                    memset (&totalNumberOfFreeBytes, 0, sizeof (totalNumberOfFreeBytes));
                                    DWORD xxx = GetDiskFreeSpaceEx (v.fMountedOnName.AsSDKString ().c_str (), &freeBytesAvailable, &totalNumberOfBytes, &totalNumberOfFreeBytes);
                                    v.fDiskSizeInBytes = static_cast<double> (totalNumberOfBytes.QuadPart);
                                    v.fUsedSizeInBytes = *v.fDiskSizeInBytes  - freeBytesAvailable.QuadPart;
#if     qUseWMICollectionSupport_
                                    String wmiInstanceName = v.fMountedOnName.RTrim ([] (Characters::Character c) { return c == '\\'; });
                                    fLogicalDiskWMICollector_.AddInstancesIf (wmiInstanceName);

                                    if (auto o = fLogicalDiskWMICollector_.PeekCurrentValue (wmiInstanceName, kDiskReadBytesPerSec_)) {
                                        v.fReadIOStats.fBytesTransfered = *o * timeCollecting;
                                    }
                                    if (auto o = fLogicalDiskWMICollector_.PeekCurrentValue (wmiInstanceName, kDiskWriteBytesPerSec_)) {
                                        v.fWriteIOStats.fBytesTransfered = *o * timeCollecting;
                                    }
                                    if (auto o = fLogicalDiskWMICollector_.PeekCurrentValue (wmiInstanceName, kDiskReadsPerSec_)) {
                                        v.fReadIOStats.fTotalTransfers = *o * timeCollecting;
                                    }
                                    if (auto o = fLogicalDiskWMICollector_.PeekCurrentValue (wmiInstanceName, kDiskWritesPerSec_)) {
                                        v.fWriteIOStats.fTotalTransfers = *o * timeCollecting;
                                    }
                                    if (auto o = fLogicalDiskWMICollector_.PeekCurrentValue (wmiInstanceName, kPctDiskReadTime_)) {
                                        v.fReadIOStats.fTimeTransfering = *o * timeCollecting / 100;
                                    }
                                    if (auto o = fLogicalDiskWMICollector_.PeekCurrentValue (wmiInstanceName, kPctDiskWriteTime_)) {
                                        v.fWriteIOStats.fTimeTransfering = *o * timeCollecting / 100;
                                    }

                                    if (v.fReadIOStats.fBytesTransfered or v.fWriteIOStats.fBytesTransfered) {
                                        v.fIOStats.fBytesTransfered = v.fReadIOStats.fBytesTransfered.Value () + v.fWriteIOStats.fBytesTransfered.Value ();
                                    }
                                    if (v.fReadIOStats.fTotalTransfers or v.fWriteIOStats.fTotalTransfers) {
                                        v.fIOStats.fTotalTransfers = v.fReadIOStats.fTotalTransfers.Value ()  + v.fWriteIOStats.fTotalTransfers.Value () ;
                                    }
                                    if (v.fReadIOStats.fTimeTransfering or v.fWriteIOStats.fTimeTransfering) {
                                        v.fIOStats.fTimeTransfering = v.fReadIOStats.fTimeTransfering.Value ()  + v.fWriteIOStats.fTimeTransfering.Value () ;
                                    }
#endif
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
    };
}
#endif











namespace {
    struct  CapturerWithContext_
            : Debug::AssertExternallySynchronizedLock
#if     qPlatform_POSIX
            , CapturerWithContext_POSIX_
#elif   qPlatform_Windows
            , CapturerWithContext_Windows_
#endif
    {
#if     qPlatform_POSIX
        using inherited = CapturerWithContext_POSIX_;
#elif   qPlatform_Windows
        using inherited = CapturerWithContext_Windows_;
#endif
        CapturerWithContext_ (Options options)
            : inherited (options)
        {
        }
        Sequence<VolumeInfo> capture_ ()
        {
            lock_guard<const AssertExternallySynchronizedLock> critSec { *this };
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
            Debug::TraceContextBumper ctx ("Instruments::Memory::Info capture_");
#endif
            return inherited::capture_ ();
        }
    };
}







/*
 ********************************************************************************
 ********** Instruments::MountedFilesystemUsage::GetObjectVariantMapper *********
 ********************************************************************************
 */
ObjectVariantMapper Instruments::MountedFilesystemUsage::GetObjectVariantMapper ()
{
    using   StructureFieldInfo = ObjectVariantMapper::StructureFieldInfo;
    ObjectVariantMapper sMapper_ = [] () -> ObjectVariantMapper {
        ObjectVariantMapper mapper;
        mapper.AddCommonType<Optional<double>> ();
        mapper.AddCommonType<Optional<String>> ();
        DISABLE_COMPILER_CLANG_WARNING_START("clang diagnostic ignored \"-Winvalid-offsetof\"");   // Really probably an issue, but not to debug here -- LGP 2014-01-04
        DISABLE_COMPILER_GCC_WARNING_START("GCC diagnostic ignored \"-Winvalid-offsetof\"");       // Really probably an issue, but not to debug here -- LGP 2014-01-04
        mapper.AddClass<VolumeInfo::IOStats> (initializer_list<StructureFieldInfo> {
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (VolumeInfo::IOStats, fBytesTransfered), String_Constant (L"Bytes"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (VolumeInfo::IOStats, fTimeTransfering), String_Constant (L"Time-Transfering"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (VolumeInfo::IOStats, fTotalTransfers), String_Constant (L"Total-Transfers"), StructureFieldInfo::NullFieldHandling::eOmit },
        });
        mapper.AddClass<VolumeInfo> (initializer_list<StructureFieldInfo> {
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (VolumeInfo, fFileSystemType), String_Constant (L"Filesystem-Type"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (VolumeInfo, fDeviceOrVolumeName), String_Constant (L"Device-Name"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (VolumeInfo, fVolumeID), String_Constant (L"Volume-ID"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (VolumeInfo, fMountedOnName), String_Constant (L"Mounted-On") },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (VolumeInfo, fDiskSizeInBytes), String_Constant (L"Disk-Size"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (VolumeInfo, fUsedSizeInBytes), String_Constant (L"Disk-Used-Size"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (VolumeInfo, fReadIOStats), String_Constant (L"Read-IO-Stats") },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (VolumeInfo, fWriteIOStats), String_Constant (L"Write-IO-Stats") },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (VolumeInfo, fIOStats), String_Constant (L"Combined-IO-Stats") },
        });
        DISABLE_COMPILER_GCC_WARNING_END("GCC diagnostic ignored \"-Winvalid-offsetof\"");
        DISABLE_COMPILER_CLANG_WARNING_END("clang diagnostic ignored \"-Winvalid-offsetof\"");
        mapper.AddCommonType<Collection<VolumeInfo>> ();
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
Instrument  SystemPerformance::Instruments::MountedFilesystemUsage::GetInstrument (Options options)
{
    CapturerWithContext_ useCaptureContext { options };  // capture context so copyable in mutable lambda
    static  Instrument  kInstrument_    = Instrument (
            InstrumentNameType (String_Constant (L"Mounted-Filesystem-Usage")),
    [useCaptureContext] () mutable -> MeasurementSet {
        MeasurementSet    results;
        DateTime    before = DateTime::Now ();
        Sequence<VolumeInfo> volumes   =   useCaptureContext.capture_ ();
        results.fMeasuredAt = DateTimeRange (before, DateTime::Now ());
        Measurement m;
        m.fValue = MountedFilesystemUsage::GetObjectVariantMapper ().FromObject (volumes);
        m.fType = kMountedVolumeUsage;
        results.fMeasurements.Add (m);
        return results;
    },
    {kMountedVolumeUsage},
    GetObjectVariantMapper ()
                                          );
    return kInstrument_;
}
