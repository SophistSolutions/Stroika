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


// for io stats
#ifndef qUseWMICollectionSupport_
#define qUseWMICollectionSupport_       qPlatform_Windows
#endif


#if     qUseWMICollectionSupport_
#include    <Pdh.h>
#include    <PdhMsg.h>
#endif


#if     defined (_MSC_VER) && qUseWMICollectionSupport_
// Use #pragma comment lib instead of explicit entry in the lib entry of the project file
#pragma comment (lib, "Pdh.lib")
#endif




#if     qUseWMICollectionSupport_
namespace {
    //
    //  @todo FIX THREADSAFTY OF THIS OBJECT!!!! (or add check to assure used extnerally Debug::AssertExtenrallySynchonized...)
    //
    //
    //  @todo GOOD CANDIATE TO MOVE TO ANOTHER SHARED FILE??
    //
    // Known good WMI object names:
    //  "Processor"
    //  "PhysicalDisk"
    //  "Memory"
    //  "System"
    //  "Network Interface"
    //  "LogicalDisk"
    //
    //  Example:
    //      WMIVarCollector_  tmp { L"Processor", L"_Total", Set<String> {L"% Processor Time"} };
    //      double x = tmp.getCurrentValue (L"% Processor Time");
    //
    //  Example:
    //      WMIVarCollector_  tmp { L"LogicalDisk", L"E:",  Set<String> {L"% Free Space"} };
    //      double x = tmp.getCurrentValue (L"% Free Space");
    //
    // Use the Windows Performance Monitor tool and click PerformanceMonitor and "Add Counters" to see more/list
    struct  WMIVarCollector_ {
        Time::DurationSecondsType       fTimeOfLastCollection {};
        String                          fObjectName_;
        String                          fInstanceIndex_;
        PDH_HQUERY                      fQuery {};              // @todo use Synchonized<> on this as a locker
        Mapping<String, PDH_HCOUNTER>   fCounters {};

        // Instance index is not numeric.. Often value is _Total
        WMIVarCollector_ (const String& objectName, const String& instanceIndex, const Iterable<String>& counterName)
            : fObjectName_ (objectName)
            , fInstanceIndex_ (instanceIndex)
        {
            PDH_STATUS  x = ::PdhOpenQuery (NULL, NULL, &fQuery);
            for (String i : counterName) {
                Add (i);
            }
            Collect ();
            {
                const Time::DurationSecondsType kUseIntervalIfNoBaseline_ { 1.0 };
                Execution::Sleep (kUseIntervalIfNoBaseline_);
            }
        }
        ~WMIVarCollector_ ()
        {
            ::PdhCloseQuery (fQuery);
        }
        WMIVarCollector_() = delete;
        WMIVarCollector_ (const WMIVarCollector_& from)
            : WMIVarCollector_ (from.fObjectName_, from.fInstanceIndex_, from.fCounters.Keys ())
        {
            // Note the above copy CTOR does a second collect, because we dont know how to clone collected data?
        }
        WMIVarCollector_& operator= (const WMIVarCollector_& rhs)
        {
            if (this != &rhs) {
                fObjectName_ = rhs.fObjectName_;
                fInstanceIndex_ = rhs.fInstanceIndex_;
            }
            PDH_STATUS  x = ::PdhOpenQuery (NULL, NULL, &fQuery);
            for (String i : rhs.fCounters.Keys ()) {
                Add (i);
            }
            Collect ();
            {
                const Time::DurationSecondsType kUseIntervalIfNoBaseline_ { 1.0 };
                Execution::Sleep (kUseIntervalIfNoBaseline_);
            }
            return *this;
        }
        void    Collect ()
        {
            PDH_STATUS  x = PdhCollectQueryData (fQuery);
            if (x != 0) {
                Execution::DoThrow (StringException (L"PdhCollectQueryData"));
            }
            fTimeOfLastCollection = Time::GetTickCount ();
        }
        void    Add (const String& counterName)
        {
            PDH_HCOUNTER newCounter = nullptr;
            PDH_STATUS  x = PdhAddCounter (fQuery, Characters::Format (L"\\%s(%s)\\%s", fObjectName_.c_str (), fInstanceIndex_.c_str (), counterName.c_str ()).c_str (), NULL, &newCounter);
            if (x != 0) {
                bool isPDH_CSTATUS_NO_OBJECT = (x == PDH_CSTATUS_NO_OBJECT);
                bool isPDH_CSTATUS_NO_COUNTER = (x == PDH_CSTATUS_NO_COUNTER);
                Execution::DoThrow (StringException (L"PdhAddCounter"));
            }
            fCounters.Add (counterName, newCounter);
        }
        double getCurrentValue (const String& name)
        {
            PDH_FMT_COUNTERVALUE counterVal;
            PDH_HCOUNTER    counter = *fCounters.Lookup (name);
            PDH_STATUS  x = ::PdhGetFormattedCounterValue (counter, PDH_FMT_DOUBLE, NULL, &counterVal);
            if (x != 0) {
                Execution::DoThrow (StringException (L"PdhGetFormattedCounterValue"));
            }
            return counterVal.doubleValue;
        }
    };
}
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





namespace {
    struct  CapturerWithContext_ {
#if     qPlatform_POSIX
        struct PerfStats_ {
            double  fSectorsRead;
            double  fTimeSpentReading;
            double  fReadsCompleted;
            double  fSectorsWritten;
            double  fTimeSpentWriting;
            double  fWritesCompleted;
        };
        Optional<Mapping<String, PerfStats_>>   fContextStats_;
#elif   qUseWMICollectionSupport_
        WMIVarCollector_    fLogicalDiskWMICollector_;
#endif
        CapturerWithContext_ ()
#if     qUseWMICollectionSupport_
            : fLogicalDiskWMICollector_ { L"LogicalDisk", L"_Total",  {kDiskReadBytesPerSec_, kDiskWriteBytesPerSec_, kDiskReadsPerSec_, kDiskWritesPerSec_,  kPctDiskReadTime_, kPctDiskWriteTime_ } }
#endif
        {
#if     qPlatform_POSIX
            capture_ ();        // for side-effect of setting fContextStats_
#endif
        }
        Sequence<VolumeInfo> capture_ ()
        {
            Sequence<VolumeInfo>   results;
#if     qPlatform_POSIX
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
#elif       qPlatform_Windows
            results = capture_Windows_GetVolumeInfo_ ();
#endif
            return results;
        }
#if     qPlatform_POSIX
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
#elif   qPlatform_Windows
        Sequence<VolumeInfo> capture_Windows_GetVolumeInfo_ ()
        {
#if     qUseWMICollectionSupport_
            Time::DurationSecondsType   timeOfPrevCollection = fLogicalDiskWMICollector_.fTimeOfLastCollection;
            fLogicalDiskWMICollector_.Collect ();
            Time::DurationSecondsType   timeCollecting { fLogicalDiskWMICollector_.fTimeOfLastCollection - timeOfPrevCollection };
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
                                    v.fReadIOStats.fBytesTransfered = fLogicalDiskWMICollector_.getCurrentValue (kDiskReadBytesPerSec_) * timeCollecting;
                                    v.fWriteIOStats.fBytesTransfered = fLogicalDiskWMICollector_.getCurrentValue (kDiskWriteBytesPerSec_) * timeCollecting;
                                    v.fReadIOStats.fTotalTransfers = fLogicalDiskWMICollector_.getCurrentValue (kDiskReadsPerSec_) * timeCollecting;
                                    v.fWriteIOStats.fTotalTransfers = fLogicalDiskWMICollector_.getCurrentValue (kDiskWritesPerSec_) * timeCollecting;
                                    v.fReadIOStats.fTimeTransfering = fLogicalDiskWMICollector_.getCurrentValue (kPctDiskReadTime_) * timeCollecting / 100;
                                    v.fWriteIOStats.fTimeTransfering = fLogicalDiskWMICollector_.getCurrentValue (kPctDiskWriteTime_) * timeCollecting / 100;

                                    v.fIOStats.fBytesTransfered = *v.fReadIOStats.fBytesTransfered + *v.fWriteIOStats.fBytesTransfered;
                                    v.fIOStats.fTotalTransfers = *v.fReadIOStats.fTotalTransfers + *v.fWriteIOStats.fTotalTransfers;
                                    v.fIOStats.fTimeTransfering = *v.fReadIOStats.fTimeTransfering + *v.fWriteIOStats.fTimeTransfering;
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
#endif
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
Instrument  SystemPerformance::Instruments::MountedFilesystemUsage::GetInstrument ()
{
    CapturerWithContext_ useCaptureContext;  // capture context so copyable in mutable lambda
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
