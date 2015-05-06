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
#include    "../../../Foundation/Characters/String2Int.h"
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
using   Characters::String2Int;
using   Time::DurationSecondsType;
using   IO::FileSystem::BinaryFileInputStream;
using   Streams::TextInputStreamBinaryAdapter;


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









namespace {
    struct  CapturerWithContext_COMMON_ {
    protected:
        Options                     fOptions_;
        DurationSecondsType         fMinimumAveragingInterval_;
        DurationSecondsType         fPostponeCaptureUntil_ { 0 };
        DateTime                    fLastCapturedAt;

    public:
        DateTime    GetLastCaptureAt () const { return fLastCapturedAt; }

    protected:
        CapturerWithContext_COMMON_ (const Options& options)
            : fOptions_ (options)
            , fMinimumAveragingInterval_ (options.fMinimumAveragingInterval)
        {
        }

    protected:
        void    _NoteCompletedCapture ()
        {
            fPostponeCaptureUntil_ = Time::GetTickCount () + fMinimumAveragingInterval_;
            fLastCapturedAt = DateTime::Now ();
        }
    };
}



#if     qPlatform_POSIX
namespace {
    struct  CapturerWithContext_POSIX_ : CapturerWithContext_COMMON_ {
    private:
        struct PerfStats_ {
            double  fSectorsRead;
            double  fTimeSpentReading;
            double  fReadsCompleted;
            double  fSectorsWritten;
            double  fTimeSpentWriting;
            double  fWritesCompleted;
        };
        Mapping<String, uint32_t>               fDeviceName2SectorSizeMap_;
        Optional<Mapping<String, PerfStats_>>   fContextStats_;
    public:
        CapturerWithContext_POSIX_ (Options options)
            : CapturerWithContext_COMMON_ (options)
        {
            capture ();        // for side-effect of setting fContextStats_
        }
        CapturerWithContext_POSIX_ (const CapturerWithContext_POSIX_&) = default;   // copy by value fine - no need to re-wait...
        Sequence<VolumeInfo> capture ()
        {
            Sequence<VolumeInfo>   results;
            Execution::SleepUntil (fPostponeCaptureUntil_);
            results = RunDF_ ();
            if (fOptions_.fIOStatistics) {
                ReadAndApplyProcFS_diskstats_ (&results);
            }
            ApplyDiskTypes_ (&results);
            _NoteCompletedCapture ();
            return results;
        }
    private:
        void    ApplyDiskTypes_ (Sequence<VolumeInfo>* volumes)
        {
            for (Iterator<VolumeInfo> i = volumes->begin (); i != volumes->end (); ++i) {
                // @todo - NOTE - this is NOT a reliable way to tell, but hopefully good enough for starters
                VolumeInfo vi = *i;
                if (vi.fFileSystemType == L"ext2" or vi.fFileSystemType == L"ext3" or vi.fFileSystemType == L"ext4") {
                    vi.fMountedDeviceType = MountedDeviceType::eLocalDisk;
                }
                else if (vi.fFileSystemType == L"devtmpfs") {
                    vi.fMountedDeviceType = MountedDeviceType::eSystemInformation;
                }
                else if (vi.fFileSystemType == L"tmpfs") {
                    vi.fMountedDeviceType = MountedDeviceType::eTemporaryFiles;
                }
                else if (vi.fFileSystemType == L"vboxsf") {
                    vi.fMountedDeviceType = MountedDeviceType::eNetworkDrive;
                }
                else if (vi.fFileSystemType == L"iso9660") {
                    vi.fMountedDeviceType = MountedDeviceType::eReadOnlyEjectable;
                }
                volumes->Update (i, vi);
            }
        }
    private:
        void    ReadAndApplyProcFS_diskstats_ (Sequence<VolumeInfo>* volumes)
        {
            try {
                Mapping<String, PerfStats_> diskStats = ReadProcFS_diskstats_ ();
                for (Iterator<VolumeInfo> i = volumes->begin (); i != volumes->end (); ++i) {
                    VolumeInfo vi = *i;
                    if (vi.fDeviceOrVolumeName.IsPresent ()) {
                        if (fContextStats_) {
                            String  devNameLessSlashes = *vi.fDeviceOrVolumeName;
                            size_t i = devNameLessSlashes.RFind ('/');
                            if (i != string::npos) {
                                devNameLessSlashes = devNameLessSlashes.SubString (i + 1);
                            }
                            Optional<PerfStats_>    oOld = fContextStats_->Lookup (devNameLessSlashes);
                            Optional<PerfStats_>    oNew = diskStats.Lookup (devNameLessSlashes);
                            if (oOld.IsPresent () and oNew.IsPresent ()) {
                                unsigned int sectorSizeTmpHack = GetSectorSize_ (devNameLessSlashes);
                                vi.fReadIOStats.fBytesTransfered = (oNew->fSectorsRead - oOld->fSectorsRead) * sectorSizeTmpHack;
                                vi.fReadIOStats.fTotalTransfers = oNew->fReadsCompleted - oOld->fReadsCompleted;
                                vi.fReadIOStats.fTimeTransfering = (oNew->fTimeSpentReading - oOld->fTimeSpentReading);
                                vi.fWriteIOStats.fBytesTransfered = (oNew->fSectorsWritten - oOld->fSectorsWritten) * sectorSizeTmpHack;
                                vi.fWriteIOStats.fTotalTransfers = oNew->fWritesCompleted - oOld->fWritesCompleted;
                                vi.fWriteIOStats.fTimeTransfering = oNew->fTimeSpentWriting - oOld->fTimeSpentWriting;

                                vi.fCombinedIOStats.fBytesTransfered = *vi.fReadIOStats.fBytesTransfered + *vi.fWriteIOStats.fBytesTransfered;
                                vi.fCombinedIOStats.fTotalTransfers = *vi.fReadIOStats.fTotalTransfers + *vi.fWriteIOStats.fTotalTransfers;
                                vi.fCombinedIOStats.fTimeTransfering = *vi.fReadIOStats.fTimeTransfering + *vi.fWriteIOStats.fTimeTransfering;
                            }
                        }
                    }
                    volumes->Update (i, vi);
                }
                fContextStats_ = diskStats;
            }
            catch (...) {
                DbgTrace ("Exception gathering procfs disk io stats");
            }
        }
    private:
        uint32_t    GetSectorSize_ (const String& deviceName)
        {
            auto    o   =   fDeviceName2SectorSizeMap_.Lookup (deviceName);
            if (o.IsMissing ()) {
                String  fn = Characters::Format (L"/sys/block/%s/queue/hw_sector_size", deviceName.c_str ());
                try {
                    o = String2Int<uint32_t> (TextInputStreamBinaryAdapter (BinaryFileInputStream::mk (fn, BinaryFileInputStream::eNotSeekable)).ReadAll ().Trim ());
                    fDeviceName2SectorSizeMap_.Add (deviceName, *o);
                }
                catch (...) {
                    DbgTrace (L"unknonwn error reading %s", fn.c_str ());
                    // ignore
                }
            }
            if (o.IsMissing ()) {
                o = 512;    // seems the typical answer on UNIX
            }
            return *o;
        }
    private:
        Sequence<VolumeInfo> RunDF_ (bool includeFSTypes)
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
        Sequence<VolumeInfo>    RunDF_ ()
        {
            try {
                return RunDF_ (true);
            }
            catch (...) {
                return RunDF_ (false);
            }
        }
    private:
        Mapping<String, PerfStats_> ReadProcFS_diskstats_ ()
        {
            using   Characters::String2Float;
            Mapping<String, PerfStats_>   result;
            DataExchange::CharacterDelimitedLines::Reader reader {{' ', '\t' }};
            const   String_Constant kProcMemInfoFileName_ { L"/proc/diskstats" };
            // Note - /procfs files always unseekable
            for (Sequence<String> line : reader.ReadMatrix (BinaryFileInputStream::mk (kProcMemInfoFileName_, BinaryFileInputStream::eNotSeekable))) {
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
                DbgTrace (L"***in Instruments::MountedFilesystemUsage::ReadProcFS_diskstats_ linesize=%d, line[0]=%s", line.size(), line.empty () ? L"" : line[0].c_str ());
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
    struct  CapturerWithContext_Windows_ : CapturerWithContext_COMMON_ {
#if   qUseWMICollectionSupport_
        WMICollector            fLogicalDiskWMICollector_;
#endif
        CapturerWithContext_Windows_ (Options options)
            : CapturerWithContext_COMMON_ (options)
#if     qUseWMICollectionSupport_
            , fLogicalDiskWMICollector_ { String_Constant { L"LogicalDisk" }, {},  {kDiskReadBytesPerSec_, kDiskWriteBytesPerSec_, kDiskReadsPerSec_, kDiskWritesPerSec_,  kPctDiskReadTime_, kPctDiskWriteTime_ } }
#endif
        {
#if   qUseWMICollectionSupport_
            if (fOptions_.fIOStatistics)
            {
                capture_Windows_GetVolumeInfo_ ();   // for side-effect of setting fLogicalDiskWMICollector_
            }
            _NoteCompletedCapture ();
#endif
        }
        CapturerWithContext_Windows_ (const CapturerWithContext_Windows_& from)
            : CapturerWithContext_COMMON_ (from)
#if   qUseWMICollectionSupport_
            , fLogicalDiskWMICollector_ (from.fLogicalDiskWMICollector_)
#endif
        {
#if   qUseWMICollectionSupport_
            if (fOptions_.fIOStatistics) {
                capture_Windows_GetVolumeInfo_ ();   // for side-effect of setting fLogicalDiskWMICollector_ (due to bug/misfeature in not being able  to copy query object - we choose to re-call here even if possibly not needed)
            }
            _NoteCompletedCapture ();
#endif
        }
        Sequence<VolumeInfo> capture ()
        {
            Sequence<VolumeInfo>   results;
            Execution::SleepUntil (fPostponeCaptureUntil_);
            results = capture_Windows_GetVolumeInfo_ ();
            _NoteCompletedCapture ();
            return results;
        }
    private:
        Sequence<VolumeInfo> capture_Windows_GetVolumeInfo_ ()
        {
#if     qUseWMICollectionSupport_
            Time::DurationSecondsType   timeOfPrevCollection = fLogicalDiskWMICollector_.GetTimeOfLastCollection ();
            if (fOptions_.fIOStatistics) {
                fLogicalDiskWMICollector_.Collect ();
            }
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
                    switch (::GetDriveType (volumeNameBuf)) {
                        case    DRIVE_REMOVABLE:
                            v.fMountedDeviceType = MountedDeviceType::eRemovableDisk;
                            break;
                        case    DRIVE_FIXED:
                            v.fMountedDeviceType = MountedDeviceType::eLocalDisk;
                            break;
                        case    DRIVE_REMOTE:
                            v.fMountedDeviceType = MountedDeviceType::eNetworkDrive;
                            break;
                        case    DRIVE_RAMDISK:
                            v.fMountedDeviceType = MountedDeviceType::eTemporaryFiles;
                            break;
                        case    DRIVE_CDROM:
                            v.fMountedDeviceType = MountedDeviceType::eReadOnlyEjectable;
                            break;
                        default:;   /*ignored - if it doesnt map or error - nevermind */
                    }
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
                                    if (fOptions_.fIOStatistics) {
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
                                            v.fCombinedIOStats.fBytesTransfered = v.fReadIOStats.fBytesTransfered.Value () + v.fWriteIOStats.fBytesTransfered.Value ();
                                        }
                                        if (v.fReadIOStats.fTotalTransfers or v.fWriteIOStats.fTotalTransfers) {
                                            v.fCombinedIOStats.fTotalTransfers = v.fReadIOStats.fTotalTransfers.Value ()  + v.fWriteIOStats.fTotalTransfers.Value () ;
                                        }
                                        if (v.fReadIOStats.fTimeTransfering or v.fWriteIOStats.fTimeTransfering) {
                                            v.fCombinedIOStats.fTimeTransfering = v.fReadIOStats.fTimeTransfering.Value ()  + v.fWriteIOStats.fTimeTransfering.Value () ;
                                        }
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
        Sequence<VolumeInfo> capture ()
        {
            lock_guard<const AssertExternallySynchronizedLock> critSec { *this };
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
            Debug::TraceContextBumper ctx ("Instruments::Memory::Info capture");
#endif
            return inherited::capture ();
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
        mapper.Add (mapper.MakeCommonSerializer_NamedEnumerations<MountedDeviceType> (Stroika_Enum_Names(MountedDeviceType)));
        mapper.AddCommonType<Optional<MountedDeviceType>> ();
        DISABLE_COMPILER_CLANG_WARNING_START("clang diagnostic ignored \"-Winvalid-offsetof\"");   // Really probably an issue, but not to debug here -- LGP 2014-01-04
        DISABLE_COMPILER_GCC_WARNING_START("GCC diagnostic ignored \"-Winvalid-offsetof\"");       // Really probably an issue, but not to debug here -- LGP 2014-01-04
        mapper.AddClass<VolumeInfo::IOStats> (initializer_list<StructureFieldInfo> {
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (VolumeInfo::IOStats, fBytesTransfered), String_Constant (L"Bytes"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (VolumeInfo::IOStats, fTimeTransfering), String_Constant (L"Time-Transfering"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (VolumeInfo::IOStats, fTotalTransfers), String_Constant (L"Total-Transfers"), StructureFieldInfo::NullFieldHandling::eOmit },
        });
        mapper.AddClass<VolumeInfo> (initializer_list<StructureFieldInfo> {
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (VolumeInfo, fMountedDeviceType), String_Constant (L"Mounted-Device-Type"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (VolumeInfo, fFileSystemType), String_Constant (L"Filesystem-Type"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (VolumeInfo, fDeviceOrVolumeName), String_Constant (L"Device-Name"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (VolumeInfo, fVolumeID), String_Constant (L"Volume-ID"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (VolumeInfo, fMountedOnName), String_Constant (L"Mounted-On") },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (VolumeInfo, fDiskSizeInBytes), String_Constant (L"Disk-Size"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (VolumeInfo, fUsedSizeInBytes), String_Constant (L"Disk-Used-Size"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (VolumeInfo, fReadIOStats), String_Constant (L"Read-IO-Stats") },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (VolumeInfo, fWriteIOStats), String_Constant (L"Write-IO-Stats") },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (VolumeInfo, fCombinedIOStats), String_Constant (L"Combined-IO-Stats") },
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
    static  const   MeasurementType kMountedVolumeUsage_  =   MeasurementType (String_Constant (L"Mounted-Volume-Usage"));
    CapturerWithContext_ useCaptureContext { options };  // capture context so copyable in mutable lambda
    return Instrument (
               InstrumentNameType (String_Constant (L"Mounted-Filesystem-Usage")),
    [useCaptureContext] () mutable -> MeasurementSet {
        MeasurementSet    results;
        DateTime    before = useCaptureContext.GetLastCaptureAt ();
        Sequence<VolumeInfo> volumes   =   useCaptureContext.capture ();
        results.fMeasuredAt = DateTimeRange (before, useCaptureContext.GetLastCaptureAt ());
        Measurement m;
        m.fValue = MountedFilesystemUsage::GetObjectVariantMapper ().FromObject (volumes);
        m.fType = kMountedVolumeUsage_;
        results.fMeasurements.Add (m);
        return results;
    },
    {kMountedVolumeUsage_},
    GetObjectVariantMapper ()
           );
}
