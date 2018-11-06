/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include <functional>

#if qPlatform_POSIX
#include <sys/stat.h>
#include <sys/statvfs.h>
#include <sys/types.h>
#include <unistd.h>
#elif qPlatform_Windows
#include <Windows.h>
#include <winioctl.h>
#endif

#if qPlatform_Linux
#include <sys/sysmacros.h>
#endif

#include "../../../Foundation/Characters/CString/Utilities.h"
#include "../../../Foundation/Characters/FloatConversion.h"
#include "../../../Foundation/Characters/String2Int.h"
#include "../../../Foundation/Characters/String_Constant.h"
#include "../../../Foundation/Configuration/SystemConfiguration.h"
#include "../../../Foundation/Containers/Mapping.h"
#include "../../../Foundation/Containers/Sequence.h"
#include "../../../Foundation/Containers/Set.h"
#include "../../../Foundation/DataExchange/Variant/CharacterDelimitedLines/Reader.h"
#include "../../../Foundation/DataExchange/Variant/JSON/Writer.h"
#include "../../../Foundation/Debug/Assertions.h"
#include "../../../Foundation/Execution/ProcessRunner.h"
#include "../../../Foundation/Execution/Sleep.h"
#include "../../../Foundation/Execution/StringException.h"
#include "../../../Foundation/IO/FileSystem/Disk.h"
#include "../../../Foundation/IO/FileSystem/FileInputStream.h"
#include "../../../Foundation/IO/FileSystem/FileSystem.h"
#include "../../../Foundation/Streams/MemoryStream.h"
#include "../../../Foundation/Streams/TextReader.h"

#include "Filesystem.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::DataExchange;
using namespace Stroika::Foundation::Execution;
using namespace Stroika::Foundation::Memory;

using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::SystemPerformance;
using namespace Stroika::Frameworks::SystemPerformance::Instruments::Filesystem;

using Characters::String2Int;
using Characters::String_Constant;
using IO::FileSystem::FileInputStream;
using Streams::TextReader;
using Time::DurationSecondsType;

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1

/*
 ********************************************************************************
 ********* Instruments::Filesystem::IOStatsType::EstimatedPercentInUse **********
 ********************************************************************************
 */
optional<double> IOStatsType::EstimatedPercentInUse () const
{
    if (fInUsePercent) {
        return fInUsePercent;
    }
    // %InUse = QL / (1 + QL).
    if (fQLength) {
        double QL = *fQLength;
        Require (0 <= QL);
        double pct{100.0 * (QL / (1 + QL))};
        Require (0 <= pct and pct <= 100.0);
        return pct;
    }
    return nullopt;
}

// for io stats
#ifndef qUseWMICollectionSupport_
#define qUseWMICollectionSupport_ qPlatform_Windows
#endif

#if qUseWMICollectionSupport_
#include "../Support/WMICollector.h"

using SystemPerformance::Support::WMICollector;
#endif

/*
 ********************************************************************************
 ***************** Instruments::Filesystem::MountedFilesystemInfoType ***********
 ********************************************************************************
 */
String Instruments::Filesystem::MountedFilesystemInfoType::ToString () const
{
    return DataExchange::Variant::JSON::Writer ().WriteAsString (GetObjectVariantMapper ().FromObject (*this));
}

/*
 ********************************************************************************
 ********************* Instruments::Filesystem::IOStatsType *********************
 ********************************************************************************
 */
String Instruments::Filesystem::IOStatsType::ToString () const
{
    return DataExchange::Variant::JSON::Writer ().WriteAsString (GetObjectVariantMapper ().FromObject (*this));
}

/*
 ********************************************************************************
 ******************** Instruments::Filesystem::DiskInfoType *********************
 ********************************************************************************
 */
String Instruments::Filesystem::DiskInfoType::ToString () const
{
    return DataExchange::Variant::JSON::Writer ().WriteAsString (GetObjectVariantMapper ().FromObject (*this));
}

/*
 ********************************************************************************
 ********************** Instruments::Filesystem::Info ***************************
 ********************************************************************************
 */
String Instruments::Filesystem::Info::ToString () const
{
    return DataExchange::Variant::JSON::Writer ().WriteAsString (GetObjectVariantMapper ().FromObject (*this));
}

namespace {
#if qUseWMICollectionSupport_
    const String_Constant kDiskReadBytesPerSec_{L"Disk Read Bytes/sec"};
    const String_Constant kDiskWriteBytesPerSec_{L"Disk Write Bytes/sec"};
    const String_Constant kDiskReadsPerSec_{L"Disk Reads/sec"};
    const String_Constant kDiskWritesPerSec_{L"Disk Writes/sec"};
    const String_Constant kPctDiskReadTime_{L"% Disk Read Time"};
    const String_Constant kPctDiskWriteTime_{L"% Disk Write Time"};
    const String_Constant kAveDiskReadQLen_{L"Avg. Disk Read Queue Length"};
    const String_Constant kAveDiskWriteQLen_{L"Avg. Disk Write Queue Length"};
    const String_Constant kPctIdleTime_{L"% Idle Time"};

    constexpr bool kUseDiskPercentReadTime_ElseAveQLen_ToComputeQLen_{false};
    /*
     *  No logical reason todo this. Its probably masking a real bug. But empirically it produces
     *  values closer to those reported by Windows Task Mgr.
     *      -- LGP 2015-07-20
     */
    constexpr bool kUsePctIdleIimeForAveQLen_{true};
#endif
}

namespace {
    struct CapturerWithContext_COMMON_ {
    protected:
        Options             fOptions_;
        DurationSecondsType fMinimumAveragingInterval_;
        DurationSecondsType fPostponeCaptureUntil_{0};
        DurationSecondsType fLastCapturedAt{};

    public:
        DurationSecondsType GetLastCaptureAt () const { return fLastCapturedAt; }

    protected:
        CapturerWithContext_COMMON_ (const Options& options)
            : fOptions_ (options)
            , fMinimumAveragingInterval_ (options.fMinimumAveragingInterval)
        {
        }

    protected:
        void _NoteCompletedCapture ()
        {
            auto now               = Time::GetTickCount ();
            fPostponeCaptureUntil_ = now + fMinimumAveragingInterval_;
            fLastCapturedAt        = now;
        }
    };
}

#if qPlatform_POSIX
namespace {
    void ApplyDiskTypes_ (Mapping<MountedFilesystemNameType, MountedFilesystemInfoType>* volumes)
    {
        RequireNotNull (volumes);
        static const Set<String> kRealDiskFS{
            String_Constant{L"ext2"},
            String_Constant{L"ext3"},
            String_Constant{L"ext4"},
            String_Constant{L"xfs"},
            String_Constant{L"jfs2"},
        };
        static const Set<String> kSysFSList_{
            String_Constant{L"autofs"},
            String_Constant{L"binfmt_misc"},
            String_Constant{L"cgroup"},
            String_Constant{L"configfs"},
            String_Constant{L"debugfs"},
            String_Constant{L"devpts"},
            String_Constant{L"devtmpfs"},
            String_Constant{L"fusectl"},
            String_Constant{L"fuse.gvfsd-fuse"},
            String_Constant{L"hugetlbfs"},
            String_Constant{L"mqueue"},
            String_Constant{L"nfsd"}, // not nfs filesystem, but special config fs - http://linux.die.net/man/7/nfsd
            String_Constant{L"pstore"},
            String_Constant{L"proc"},
            String_Constant{L"rpc_pipefs"},
            String_Constant{L"securityfs"},
            String_Constant{L"selinuxfs"},
            String_Constant{L"sunrpc"},
            String_Constant{L"sysfs"},
            String_Constant{L"usbfs"},
        };
        static const Set<String> kNetworkFS_{
            String_Constant{L"nfs"},
            String_Constant{L"nfs3"},
            String_Constant{L"vboxsf"},
        };
        for (auto i : *volumes) {
            // @todo - NOTE - this is NOT a reliable way to tell, but hopefully good enough for starters
            MountedFilesystemInfoType vi = i.fValue;
            if (vi.fFileSystemType) {
                String fstype = *vi.fFileSystemType;
                bool   changed{false};
                if (kRealDiskFS.Contains (fstype)) {
                    vi.fDeviceKind = BlockDeviceKind::eLocalDisk;
                    changed        = true;
                }
                else if (kNetworkFS_.Contains (fstype)) {
                    vi.fDeviceKind = BlockDeviceKind::eNetworkDrive;
                    changed        = true;
                }
                else if (fstype == L"tmpfs") {
                    vi.fDeviceKind = BlockDeviceKind::eTemporaryFiles;
                    changed        = true;
                }
                else if (fstype == L"iso9660") {
                    vi.fDeviceKind = BlockDeviceKind::eReadOnlyEjectable;
                    changed        = true;
                }
                else if (kSysFSList_.Contains (fstype)) {
                    vi.fDeviceKind = BlockDeviceKind::eSystemInformation;
                    changed        = true;
                }
                if (changed) {
                    volumes->Add (i.fKey, vi);
                }
            }
        }
    }
}
#endif

#if qPlatform_Linux
namespace {
    struct CapturerWithContext_Linux_ : CapturerWithContext_COMMON_ {
    private:
        struct PerfStats_ {
            double fSectorsRead;
            double fTimeSpentReading;
            double fReadsCompleted;
            double fSectorsWritten;
            double fTimeSpentWriting;
            double fWritesCompleted;
            double fWeightedTimeInQSeconds; // see https://www.kernel.org/doc/Documentation/block/stat.txt  time_in_queue (product of the number of milliseconds times the number of requests waiting)
        };
        Mapping<String, uint32_t>            fDeviceName2SectorSizeMap_;
        optional<Mapping<dev_t, PerfStats_>> fContextStats_;

    public:
        CapturerWithContext_Linux_ (Options options)
            : CapturerWithContext_COMMON_ (options)
        {
            // for side-effect of setting fContextStats_
            try {
                capture_ ();
            }
            catch (...) {
                DbgTrace ("bad sign that first pre-catpure failed."); // Don't propagate in case just listing collectors
            }
        }
        CapturerWithContext_Linux_ (const CapturerWithContext_Linux_&) = default; // copy by value fine - no need to re-wait...
        Info capture ()
        {
            Execution::SleepUntil (fPostponeCaptureUntil_);
            return capture_ ();
        }
        Info capture_ ()
        {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
            Debug::TraceContextBumper ctx ("Instruments::Filesystem...CapturerWithContext_Linux_::capture_");
#endif
            Info results;

            constexpr bool kUseProcFSForMounts_{true};
            if (kUseProcFSForMounts_) {
                results.fMountedFilesystems = ReadVolumesAndUsageFromProcMountsAndstatvfs_ ();
            }
            else {
                results.fMountedFilesystems = RunDF_ ();
            }
            ApplyDiskTypes_ (&results.fMountedFilesystems);
            if (not fOptions_.fIncludeTemporaryDevices or not fOptions_.fIncludeSystemDevices) {
                for (KeyValuePair<MountedFilesystemNameType, MountedFilesystemInfoType> i : results.fMountedFilesystems) {
                    if (not fOptions_.fIncludeTemporaryDevices and i.fValue.fDeviceKind == BlockDeviceKind::eTemporaryFiles) {
                        results.fMountedFilesystems.Remove (i.fKey);
                    }
                    else if (not not fOptions_.fIncludeSystemDevices and i.fValue.fDeviceKind == BlockDeviceKind::eSystemInformation) {
                        results.fMountedFilesystems.Remove (i.fKey);
                    }
                }
            }
            if (fOptions_.fIOStatistics) {
                ReadAndApplyProcFS_diskstats_ (&results.fMountedFilesystems);
            }
            _NoteCompletedCapture ();
            return results;
        }

    private:
        Mapping<MountedFilesystemNameType, MountedFilesystemInfoType> ReadVolumesAndUsageFromProcMountsAndstatvfs_ ()
        {
            Mapping<MountedFilesystemNameType, MountedFilesystemInfoType> result;
            for (IO::FileSystem::MountedFilesystemType mi : IO::FileSystem::GetMountedFilesystems ()) {
                MountedFilesystemInfoType vi;
                String                    deviceName = (not mi.fDevicePaths.has_value () or mi.fDevicePaths->empty ()) ? String{} : mi.fDevicePaths->Nth (0);
                if (not deviceName.empty ()) {
                    vi.fDeviceOrVolumeName = deviceName;
                }
                vi.fFileSystemType = mi.fFileSystemType;
                UpdateVolumeInfo_statvfs_ (mi.fMountedOn, &vi);
                result.Add (mi.fMountedOn, vi);
            }
            return result;
        }

    private:
        void UpdateVolumeInfo_statvfs_ (const String& mountedOnName, MountedFilesystemInfoType* v)
        {
            RequireNotNull (v);
            struct statvfs sbuf {
            };
            if (::statvfs (mountedOnName.AsNarrowSDKString ().c_str (), &sbuf) == 0) {
                uint64_t diskSize        = sbuf.f_bsize * sbuf.f_blocks;
                v->fSizeInBytes          = diskSize;
                v->fAvailableSizeInBytes = sbuf.f_bsize * sbuf.f_bavail;
                v->fUsedSizeInBytes      = diskSize - sbuf.f_bsize * sbuf.f_bfree;
            }
            else {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                DbgTrace (L"statvfs (%s) return error: errno=%d", mountedOnName.c_str (), errno);
#endif
            }
        }

    private:
        void ReadAndApplyProcFS_diskstats_ (Mapping<MountedFilesystemNameType, MountedFilesystemInfoType>* volumes)
        {
            try {
                Mapping<dev_t, PerfStats_> diskStats            = ReadProcFS_diskstats_ ();
                DurationSecondsType        timeSinceLastMeasure = Time::GetTickCount () - GetLastCaptureAt ();
                for (KeyValuePair<MountedFilesystemNameType, MountedFilesystemInfoType> i : *volumes) {
                    MountedFilesystemInfoType vi = i.fValue;
                    if (vi.fDeviceOrVolumeName.has_value ()) {
                        if (fContextStats_) {
                            String devNameLessSlashes = *vi.fDeviceOrVolumeName;
                            size_t i                  = devNameLessSlashes.rfind ('/');
                            if (i != string::npos) {
                                devNameLessSlashes = devNameLessSlashes.SubString (i + 1);
                            }
                            dev_t useDevT;
                            {
                                struct stat sbuf {
                                };
                                if (::stat (vi.fDeviceOrVolumeName->AsNarrowSDKString ().c_str (), &sbuf) == 0) {
                                    useDevT = sbuf.st_rdev;
                                }
                                else {
                                    continue;
                                }
                            }
                            optional<PerfStats_> oOld = fContextStats_->Lookup (useDevT);
                            optional<PerfStats_> oNew = diskStats.Lookup (useDevT);
                            if (oOld.has_value () and oNew.has_value ()) {
                                unsigned int sectorSizeTmpHack = GetSectorSize_ (devNameLessSlashes);
                                IOStatsType  readStats;
                                readStats.fBytesTransfered = (oNew->fSectorsRead - oOld->fSectorsRead) * sectorSizeTmpHack;
                                readStats.fTotalTransfers  = oNew->fReadsCompleted - oOld->fReadsCompleted;
                                readStats.fQLength         = (oNew->fTimeSpentReading - oOld->fTimeSpentReading) / timeSinceLastMeasure;

                                IOStatsType writeStats;
                                writeStats.fBytesTransfered = (oNew->fSectorsWritten - oOld->fSectorsWritten) * sectorSizeTmpHack;
                                writeStats.fTotalTransfers  = oNew->fWritesCompleted - oOld->fWritesCompleted;
                                writeStats.fQLength         = (oNew->fTimeSpentWriting - oOld->fTimeSpentWriting) / timeSinceLastMeasure;

                                IOStatsType combinedStats;
                                combinedStats.fBytesTransfered = *readStats.fBytesTransfered + *writeStats.fBytesTransfered;
                                combinedStats.fTotalTransfers  = *readStats.fTotalTransfers + *writeStats.fTotalTransfers;
                                combinedStats.fQLength         = *readStats.fQLength + *writeStats.fQLength;

                                vi.fReadIOStats  = readStats;
                                vi.fWriteIOStats = writeStats;
                                // @todo DESCRIBE divide by time between 2 and * 1000 - NYI
                                combinedStats.fQLength = ((oNew->fWeightedTimeInQSeconds - oOld->fWeightedTimeInQSeconds) / timeSinceLastMeasure);
                                vi.fCombinedIOStats    = combinedStats;
                            }
                        }
                    }
                    volumes->Add (i.fKey, vi);
                }
                fContextStats_ = diskStats;
            }
            catch (...) {
                DbgTrace ("Exception gathering procfs disk io stats");
            }
        }

    private:
        optional<String> GetSysBlockDirPathForDevice_ (const String& deviceName)
        {
            Require (not deviceName.empty ());
            Require (not deviceName.Contains (L"/"));
            // Sometimes the /sys/block directory appears to have data for the each major/minor pair, and sometimes it appears
            // to only have it for the top level (minor=0) one without the digit after in the name.
            //
            // I don't understand this well yet, but this appears to temporarily allow us to limp along --LGP 2015-07-10
            //tmphack
            String tmp{L"/sys/block/" + deviceName + L"/"};
            if (IO::FileSystem::Default ().Access (tmp)) {
                return tmp;
            }
            //tmphack - try using one char less
            tmp = L"/sys/block/" + deviceName.SubString (0, -1) + L"/";
            if (IO::FileSystem::Default ().Access (tmp)) {
                return tmp;
            }
            return nullopt;
        }

    private:
        uint32_t GetSectorSize_ (const String& deviceName)
        {
            auto o = fDeviceName2SectorSizeMap_.Lookup (deviceName);
            if (not o.has_value ()) {
                optional<String> blockDeviceInfoPath = GetSysBlockDirPathForDevice_ (deviceName);
                if (blockDeviceInfoPath) {
                    String fn = *blockDeviceInfoPath + L"queue/hw_sector_size";
                    try {
                        o = String2Int<uint32_t> (TextReader::New (FileInputStream::New (fn, FileInputStream::eNotSeekable)).ReadAll ().Trim ());
                        fDeviceName2SectorSizeMap_.Add (deviceName, *o);
                    }
                    catch (...) {
                        DbgTrace (L"Unknown error reading %s", fn.c_str ());
                        // ignore
                    }
                }
            }
            if (not o.has_value ()) {
                o = 512; // seems the typical answer on UNIX
            }
            return *o;
        }

    private:
        Mapping<MountedFilesystemNameType, MountedFilesystemInfoType> RunDF_POSIX_ ()
        {
            Mapping<MountedFilesystemNameType, MountedFilesystemInfoType> result;
            ProcessRunner                                                 pr{L"/bin/df -k -P"};
            Streams::MemoryStream<byte>::Ptr                              useStdOut = Streams::MemoryStream<byte>::New ();
            pr.SetStdOut (useStdOut);
            std::exception_ptr runException;
            try {
                pr.Run ();
            }
            catch (...) {
                runException = current_exception ();
            }
            String                   out;
            Streams::TextReader::Ptr stdOut        = Streams::TextReader::New (useStdOut);
            bool                     skippedHeader = false;
            for (String i = stdOut.ReadLine (); not i.empty (); i = stdOut.ReadLine ()) {
                if (not skippedHeader) {
                    skippedHeader = true;
                    continue;
                }
                Sequence<String> l = i.Tokenize (Set<Characters::Character>{' '});
                if (l.size () < 6) {
                    DbgTrace ("skipping line cuz len=%d", l.size ());
                    continue;
                }
                MountedFilesystemInfoType v;
                {
                    String d = l[0].Trim ();
                    if (not d.empty () and d != L"none") {
                        v.fDeviceOrVolumeName = d;
                    }
                }
                {
                    double szInBytes = Characters::String2Float<double> (l[1]) * 1024;
                    if (not std::isnan (szInBytes) and not std::isinf (szInBytes)) {
                        v.fSizeInBytes = szInBytes;
                    }
                }
                {
                    double usedSizeInBytes = Characters::String2Float<double> (l[2]) * 1024;
                    if (not std::isnan (usedSizeInBytes) and not std::isinf (usedSizeInBytes)) {
                        v.fUsedSizeInBytes = usedSizeInBytes;
                    }
                }
                if (v.fSizeInBytes and v.fUsedSizeInBytes) {
                    v.fAvailableSizeInBytes = *v.fSizeInBytes - *v.fUsedSizeInBytes;
                }
                result.Add (l[5].Trim (), v);
            }
            // Sometimes (with busy box df especially) we get bogus error return. So only rethrow if we found no good data
            if (runException and result.empty ()) {
                Execution::ReThrow (runException);
            }
            return result;
        }
        Mapping<MountedFilesystemNameType, MountedFilesystemInfoType> RunDF_ (bool includeFSTypes)
        {
            Mapping<MountedFilesystemNameType, MountedFilesystemInfoType> result;
            //
            // I looked through the /proc filesystem stuff and didnt see anything obvious to retrive this info...
            // run def with ProcessRunner
            //
            //  NEW NOTE - I THINK ITS IN THERE.... RE-EXAMINE proc/filesystems proc/partitions, and http://en.wikipedia.org/wiki/Procfs
            //      -- LGP 2014-08-01
            ProcessRunner                    pr{includeFSTypes ? L"/bin/df -k -T" : L"/bin/df -k"};
            Streams::MemoryStream<byte>::Ptr useStdOut = Streams::MemoryStream<byte>::New ();
            pr.SetStdOut (useStdOut);
            std::exception_ptr runException;
            try {
                pr.Run ();
            }
            catch (...) {
                runException = current_exception ();
            }
            String                   out;
            Streams::TextReader::Ptr stdOut        = Streams::TextReader::New (useStdOut);
            bool                     skippedHeader = false;
            for (String i = stdOut.ReadLine (); not i.empty (); i = stdOut.ReadLine ()) {
                if (not skippedHeader) {
                    skippedHeader = true;
                    continue;
                }
                Sequence<String> l = i.Tokenize (Set<Characters::Character>{' '});
                if (l.size () < (includeFSTypes ? 7 : 6)) {
                    DbgTrace ("skipping line cuz len=%d", l.size ());
                    continue;
                }
                MountedFilesystemInfoType v;
                if (includeFSTypes) {
                    v.fFileSystemType = l[1].Trim ();
                }
                {
                    String d = l[0].Trim ();
                    if (not d.empty () and d != L"none") {
                        v.fDeviceOrVolumeName = d;
                    }
                }
                v.fSizeInBytes          = Characters::String2Float<double> (l[includeFSTypes ? 2 : 1]) * 1024;
                v.fUsedSizeInBytes      = Characters::String2Float<double> (l[includeFSTypes ? 3 : 2]) * 1024;
                v.fAvailableSizeInBytes = *v.fSizeInBytes - *v.fUsedSizeInBytes;
                result.Add (l[includeFSTypes ? 6 : 5].Trim (), v);
            }
            // Sometimes (with busy box df especially) we get bogus error return. So only rethrow if we found no good data
            if (runException and result.empty ()) {
                Execution::ReThrow (runException);
            }
            return result;
        }
        Mapping<MountedFilesystemNameType, MountedFilesystemInfoType> RunDF_ ()
        {
            try {
                return RunDF_ (true);
            }
            catch (...) {
                return RunDF_ (false);
            }
        }

    private:
        Mapping<dev_t, PerfStats_> ReadProcFS_diskstats_ ()
        {
            using Characters::String2Float;
            Mapping<dev_t, PerfStats_>                             result;
            DataExchange::Variant::CharacterDelimitedLines::Reader reader{{' ', '\t'}};
            const String_Constant                                  kProcMemInfoFileName_{L"/proc/diskstats"};
            // Note - /procfs files always unseekable
            for (Sequence<String> line : reader.ReadMatrix (FileInputStream::New (kProcMemInfoFileName_, FileInputStream::eNotSeekable))) {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                DbgTrace (L"***in Instruments::Filesystem::ReadProcFS_diskstats_ linesize=%d, line[0]=%s", line.size (), line.empty () ? L"" : line[0].c_str ());
#endif
                //
                // https://www.kernel.org/doc/Documentation/ABI/testing/procfs-diskstats
                //
                //  1 - major number
                //  2 - minor mumber
                //  3 - device name
                //  4 - reads completed successfully
                //  6 - sectors read
                //  7 - time spent reading (ms)
                //  8 - writes completed
                //  10 - sectors written
                //  11 - time spent writing (ms)
                //
                if (line.size () >= 13) {
                    String           majorDevNumber     = line[1 - 1];
                    String           minorDevNumber     = line[2 - 1];
                    String           devName            = line[3 - 1];
                    String           readsCompleted     = line[4 - 1];
                    String           sectorsRead        = line[6 - 1];
                    String           timeSpentReadingMS = line[7 - 1];
                    String           writesCompleted    = line[8 - 1];
                    String           sectorsWritten     = line[10 - 1];
                    String           timeSpentWritingMS = line[11 - 1];
                    constexpr bool   kAlsoReadQLen_{true};
                    optional<double> weightedTimeInQSeconds;
                    if (kAlsoReadQLen_) {
                        optional<String> sysBlockInfoPath = GetSysBlockDirPathForDevice_ (devName);
                        if (sysBlockInfoPath) {
                            for (Sequence<String> ll : reader.ReadMatrix (FileInputStream::New (*sysBlockInfoPath + L"stat", FileInputStream::eNotSeekable))) {
                                if (ll.size () >= 11) {
                                    weightedTimeInQSeconds = String2Float (ll[11 - 1]) / 1000.0; // we record in seconds, but the value in file in milliseconds
                                    break;
                                }
                            }
                        }
                    }
                    result.Add (
                        makedev (String2Int<unsigned int> (majorDevNumber), String2Int<unsigned int> (minorDevNumber)),
                        PerfStats_{
                            String2Float (sectorsRead), String2Float (timeSpentReadingMS) / 1000, String2Float (readsCompleted),
                            String2Float (sectorsWritten), String2Float (timeSpentWritingMS) / 1000, String2Float (writesCompleted),
                            ValueOrDefault (weightedTimeInQSeconds)});
                }
            }
            return result;
        }
    };
}
#endif

#if qPlatform_Windows
namespace {
    struct CapturerWithContext_Windows_ : CapturerWithContext_COMMON_ {
#if qUseWMICollectionSupport_
        WMICollector fLogicalDiskWMICollector_;
#endif
        CapturerWithContext_Windows_ (Options options)
            : CapturerWithContext_COMMON_ (options)
#if qUseWMICollectionSupport_
            , fLogicalDiskWMICollector_
        {
            String_Constant{L"LogicalDisk"}, {},
            {
                kDiskReadBytesPerSec_, kDiskWriteBytesPerSec_, kDiskReadsPerSec_, kDiskWritesPerSec_,
                    (kUseDiskPercentReadTime_ElseAveQLen_ToComputeQLen_ ? kPctDiskReadTime_ : kAveDiskReadQLen_),
                    (kUseDiskPercentReadTime_ElseAveQLen_ToComputeQLen_ ? kPctDiskWriteTime_ : kAveDiskWriteQLen_),
                    kPctIdleTime_
            }
        }
#endif
        {
#if qUseWMICollectionSupport_
            if (fOptions_.fIOStatistics) {
                capture_Windows_GetVolumeInfo_ (); // for side-effect of setting fLogicalDiskWMICollector_
            }
            _NoteCompletedCapture ();
#endif
        }
        CapturerWithContext_Windows_ (const CapturerWithContext_Windows_& from)
            : CapturerWithContext_COMMON_ (from)
#if qUseWMICollectionSupport_
            , fLogicalDiskWMICollector_ (from.fLogicalDiskWMICollector_)
#endif
        {
#if qUseWMICollectionSupport_
            if (fOptions_.fIOStatistics) {
                capture_Windows_GetVolumeInfo_ (); // for side-effect of setting fLogicalDiskWMICollector_ (due to bug/misfeature in not being able  to copy query object - we choose to re-call here even if possibly not needed)
            }
            _NoteCompletedCapture ();
#endif
        }
        Info capture ()
        {
            Execution::SleepUntil (fPostponeCaptureUntil_);
            return capture_ ();
        }
        Info capture_ ()
        {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
            Debug::TraceContextBumper ctx ("Instruments::Filesystem...CapturerWithContext_Windows_::capture_");
#endif
            Info results = capture_Windows_GetVolumeInfo_ ();
            _NoteCompletedCapture ();
            return results;
        }

    private:
        optional<String> GetDeviceNameForVolumneName_ (const String& volumeName)
        {
            //  use
            //      http://msdn.microsoft.com/en-us/library/windows/desktop/cc542456(v=vs.85).aspx
            //      CharCount = QueryDosDeviceW(&VolumeName[4], DeviceName, ARRAYSIZE(DeviceName));
            //      to get DEVICENAME
            //
            String tmp = volumeName;
            //  Skip the \\?\ prefix and remove the trailing backslash.
            //  QueryDosDeviceW does not allow a trailing backslash,
            //  so temporarily remove it.
            if (tmp.length () < 5 or
                tmp[0] != L'\\' ||
                tmp[1] != L'\\' ||
                tmp[2] != L'?' ||
                tmp[3] != L'\\' ||
                tmp[tmp.length () - 1] != L'\\') {
                //Error = ERROR_BAD_PATHNAME;
                //wprintf(L"FindFirstVolumeW/FindNextVolumeW returned a bad path: %s\n", VolumeName);
                return nullopt;
            }
            tmp = tmp.SubString (4, -1);

            WCHAR deviceName[MAX_PATH] = L"";
            if (::QueryDosDeviceW (tmp.c_str (), deviceName, ARRAYSIZE (deviceName)) != 0) {
                return String{deviceName};
            }
            return nullopt;
        }

        Info capture_Windows_GetVolumeInfo_ ()
        {
            // Could probably usefully optimize to not capture if no drives because we can only get this when running as
            // Admin, and for now, we capture little useful information at the drive level. But - we may eventually capture more...
            Collection<IO::FileSystem::DiskInfoType> physDrives = IO::FileSystem::GetAvailableDisks ();

#if qUseWMICollectionSupport_
            Time::DurationSecondsType timeOfPrevCollection = fLogicalDiskWMICollector_.GetTimeOfLastCollection ();
            if (fOptions_.fIOStatistics) {
                fLogicalDiskWMICollector_.Collect ();
            }
            Time::DurationSecondsType timeCollecting{fLogicalDiskWMICollector_.GetTimeOfLastCollection () - timeOfPrevCollection};
#endif
            Info result;

            for (IO::FileSystem::DiskInfoType pd : physDrives) {
                DiskInfoType di{};
                di.fSizeInBytes = pd.fSizeInBytes;
                result.fDisks.Add (pd.fDeviceName, di);
            }

            for (IO::FileSystem::MountedFilesystemType mfinfo : IO::FileSystem::GetMountedFilesystems ()) {
                MountedFilesystemInfoType v;
                v.fFileSystemType = mfinfo.fFileSystemType;
                v.fVolumeID       = mfinfo.fVolumeID;
                if (not physDrives.empty ()) {
                    v.fOnPhysicalDrive = mfinfo.fDevicePaths;
                }

                /*
                 *  For now, we only capture the volume ID for a disk
                 */
                if (v.fVolumeID) {
                    switch (::GetDriveType (v.fVolumeID->AsSDKString ().c_str ())) {
                        case DRIVE_REMOVABLE:
                            v.fDeviceKind = BlockDeviceKind::eRemovableDisk;
                            break;
                        case DRIVE_FIXED:
                            v.fDeviceKind = BlockDeviceKind::eLocalDisk;
                            break;
                        case DRIVE_REMOTE:
                            v.fDeviceKind = BlockDeviceKind::eNetworkDrive;
                            break;
                        case DRIVE_RAMDISK:
                            v.fDeviceKind = BlockDeviceKind::eTemporaryFiles;
                            break;
                        case DRIVE_CDROM:
                            v.fDeviceKind = BlockDeviceKind::eReadOnlyEjectable;
                            break;
                        default:; /*ignored - if it doesn't map or error - nevermind */
                    }
                }

                {
                    ULARGE_INTEGER         freeBytesAvailable{};
                    ULARGE_INTEGER         totalNumberOfBytes{};
                    ULARGE_INTEGER         totalNumberOfFreeBytes{};
                    [[maybe_unused]] DWORD xxx = ::GetDiskFreeSpaceEx (mfinfo.fMountedOn.AsSDKString ().c_str (), &freeBytesAvailable, &totalNumberOfBytes, &totalNumberOfFreeBytes);
                    v.fSizeInBytes             = totalNumberOfBytes.QuadPart;
                    v.fUsedSizeInBytes         = *v.fSizeInBytes - freeBytesAvailable.QuadPart;
                    v.fAvailableSizeInBytes    = *v.fSizeInBytes - *v.fUsedSizeInBytes;
#if qUseWMICollectionSupport_
                    auto safePctInUse2QL_ = [](double pctInUse) {
                        // %InUse = QL / (1 + QL).
                        pctInUse /= 100;
                        pctInUse = Math::PinInRange<double> (pctInUse, 0, 1);
                        return pctInUse / (1 - pctInUse);
                    };
                    if (fOptions_.fIOStatistics) {
                        String wmiInstanceName = mfinfo.fMountedOn.RTrim ([](Characters::Character c) { return c == '\\'; });
                        fLogicalDiskWMICollector_.AddInstancesIf (wmiInstanceName);

                        IOStatsType readStats;
                        if (auto o = fLogicalDiskWMICollector_.PeekCurrentValue (wmiInstanceName, kDiskReadBytesPerSec_)) {
                            readStats.fBytesTransfered = *o * timeCollecting;
                        }
                        if (auto o = fLogicalDiskWMICollector_.PeekCurrentValue (wmiInstanceName, kDiskReadsPerSec_)) {
                            readStats.fTotalTransfers = *o * timeCollecting;
                        }
                        if (kUseDiskPercentReadTime_ElseAveQLen_ToComputeQLen_) {
                            if (auto o = fLogicalDiskWMICollector_.PeekCurrentValue (wmiInstanceName, kPctDiskReadTime_)) {
                                readStats.fInUsePercent = *o;
                                readStats.fQLength      = safePctInUse2QL_ (*o);
                            }
                        }
                        else {
                            if (auto o = fLogicalDiskWMICollector_.PeekCurrentValue (wmiInstanceName, kAveDiskReadQLen_)) {
                                readStats.fInUsePercent = *o;
                                readStats.fQLength      = *o;
                            }
                        }

                        IOStatsType writeStats;
                        if (auto o = fLogicalDiskWMICollector_.PeekCurrentValue (wmiInstanceName, kDiskWriteBytesPerSec_)) {
                            writeStats.fBytesTransfered = *o * timeCollecting;
                        }
                        if (auto o = fLogicalDiskWMICollector_.PeekCurrentValue (wmiInstanceName, kDiskWritesPerSec_)) {
                            writeStats.fTotalTransfers = *o * timeCollecting;
                        }
                        if (kUseDiskPercentReadTime_ElseAveQLen_ToComputeQLen_) {
                            if (auto o = fLogicalDiskWMICollector_.PeekCurrentValue (wmiInstanceName, kPctDiskWriteTime_)) {
                                writeStats.fInUsePercent = *o;
                                writeStats.fQLength      = safePctInUse2QL_ (*o);
                            }
                        }
                        else {
                            if (auto o = fLogicalDiskWMICollector_.PeekCurrentValue (wmiInstanceName, kAveDiskWriteQLen_)) {
                                writeStats.fInUsePercent = *o;
                                writeStats.fQLength      = *o;
                            }
                        }

                        IOStatsType combinedStats = readStats;
                        Memory::AccumulateIf (&combinedStats.fBytesTransfered, writeStats.fBytesTransfered);
                        Memory::AccumulateIf (&combinedStats.fTotalTransfers, writeStats.fTotalTransfers);
                        Memory::AccumulateIf (&combinedStats.fQLength, writeStats.fQLength);
                        Memory::AccumulateIf (&combinedStats.fInUsePercent, writeStats.fInUsePercent);
                        if (readStats.fInUsePercent and writeStats.fInUsePercent) {
                            combinedStats.fInUsePercent = *combinedStats.fInUsePercent / 2; // must be safe cuz above would have set combined stats
                        }

                        if (kUsePctIdleIimeForAveQLen_) {
                            if (auto o = fLogicalDiskWMICollector_.PeekCurrentValue (wmiInstanceName, kPctIdleTime_)) {
                                double aveCombinedQLen = safePctInUse2QL_ (100.0 - *o);
                                if (readStats.fQLength and writeStats.fQLength and *combinedStats.fQLength > 0) {
                                    // for some reason, the pct-idle-time #s combined are OK, but #s for aveQLen and disk read PCT/Write PCT wrong.
                                    // asusme ratio rate, and scale
                                    double correction = aveCombinedQLen / *combinedStats.fQLength;
                                    Memory::AccumulateIf (&readStats.fQLength, correction, std::multiplies{});
                                    Memory::AccumulateIf (&writeStats.fQLength, correction, std::multiplies{});
                                }
                                combinedStats.fQLength = aveCombinedQLen;
                            }
                        }

                        if (readStats.fBytesTransfered or readStats.fTotalTransfers or readStats.fQLength) {
                            v.fReadIOStats = readStats;
                        }
                        if (writeStats.fBytesTransfered or writeStats.fTotalTransfers or writeStats.fQLength) {
                            v.fWriteIOStats = writeStats;
                        }
                        if (combinedStats.fBytesTransfered or combinedStats.fTotalTransfers or combinedStats.fQLength) {
                            v.fCombinedIOStats = combinedStats;
                        }
                    }
#endif
                }
                result.fMountedFilesystems.Add (mfinfo.fMountedOn, v);
            }
            return result;
        }
    };
}
#endif

namespace {
    Mapping<MountedFilesystemNameType, MountedFilesystemInfoType> ApplyDiskStatsToMissingFileSystemStats_ (const Mapping<DynamicDiskIDType, DiskInfoType>& disks, const Mapping<MountedFilesystemNameType, MountedFilesystemInfoType>& fileSystems)
    {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
        Debug::TraceContextBumper ctx ("Instruments::Filesystem ... ApplyDiskStatsToMissingFileSystemStats_");
#endif
        // Each FS will have some stats about disk usage, and we want to use those to relatively weight the stats from the disk usage when
        // applied back to other FS stats.
        //
        // So first compute the total stat per disk
        using WeightingStat2UseType = double;
        Mapping<DynamicDiskIDType, WeightingStat2UseType> totalWeights;
        for (KeyValuePair<MountedFilesystemNameType, MountedFilesystemInfoType> i : fileSystems) {
            Set<DynamicDiskIDType> disksForFS = ValueOrDefault (i.fValue.fOnPhysicalDrive);
            if (disksForFS.size () > 0) {
                WeightingStat2UseType weightForFS = ValueOrDefault (ValueOrDefault (i.fValue.fCombinedIOStats).fBytesTransfered);
                weightForFS /= disksForFS.size ();
                for (DynamicDiskIDType di : disksForFS) {
                    totalWeights.Add (di, totalWeights.LookupValue (di) + weightForFS); // accumulate relative application to each disk
                }
            }
        }
#if USE_NOISY_TRACE_IN_THIS_MODULE_
        {
            Debug::TraceContextBumper ctx1 ("Weighted disk stats");
            for (auto i : totalWeights) {
                DbgTrace (L"Disk '%s' weight %f", i.fKey.c_str (), i.fValue);
            }
        }
#endif

        // At this point, for all disks with stats we can attribute back to a filesystem - we have the relative total of bytes xfered per disk

        /*
         *  Now walk the filesystem objects, and their stats, and replace the fInUsePCT and fQLength stats with weighted values from
         *  the disks, based on their relative number of byte IO.
         *
         *  This will not work well, but will work better than anything else I can think of, and have the NICE effect of at least not counting
         *  filesystems that are not doing any IO.
         */
        Mapping<MountedFilesystemNameType, MountedFilesystemInfoType> newFilessytems;
        if (totalWeights.size () >= 1) {
            for (KeyValuePair<MountedFilesystemNameType, MountedFilesystemInfoType> i : fileSystems) {
                MountedFilesystemInfoType mfi        = i.fValue;
                Set<DynamicDiskIDType>    disksForFS = ValueOrDefault (mfi.fOnPhysicalDrive);
                if (disksForFS.size () > 0) {
                    WeightingStat2UseType weightForFS = ValueOrDefault (ValueOrDefault (i.fValue.fCombinedIOStats).fBytesTransfered);
                    weightForFS /= disksForFS.size ();
                    IOStatsType cumStats          = ValueOrDefault (mfi.fCombinedIOStats);
                    bool        computeInuse      = not cumStats.fInUsePercent.has_value ();
                    bool        computeQLen       = not cumStats.fQLength.has_value ();
                    bool        computeTotalXFers = not cumStats.fTotalTransfers.has_value ();

                    for (DynamicDiskIDType di : disksForFS) {
                        IOStatsType diskIOStats = ValueOrDefault (disks.LookupValue (di).fCombinedIOStats);
                        if (weightForFS > 0) {
                            double scaleFactor = weightForFS / totalWeights.LookupValue (di);
                            //Assert (0.0 <= scaleFactor and scaleFactor <= 1.0);
                            scaleFactor = Math::PinInRange (scaleFactor, 0.0, 1.0);
                            if (computeInuse and diskIOStats.fInUsePercent) {
                                Memory::AccumulateIf<double> (&cumStats.fInUsePercent, *diskIOStats.fInUsePercent * scaleFactor);
                            }
                            if (computeQLen and diskIOStats.fInUsePercent) {
                                Memory::AccumulateIf<double> (&cumStats.fQLength, *diskIOStats.fQLength * scaleFactor);
                            }
                            if (computeTotalXFers and diskIOStats.fTotalTransfers) {
                                Memory::AccumulateIf<double> (&cumStats.fTotalTransfers, *diskIOStats.fTotalTransfers * scaleFactor);
                            }
                        }
                    }
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                    if (computeInuse) {
                        DbgTrace (L"Adjusted fInUsePCT for filesystem '%s' is %f", i.fKey.c_str (), ValueOrDefault (cumStats.fInUsePercent));
                    }
                    if (computeQLen) {
                        DbgTrace (L"Adjusted fQLength for filesystem '%s' is %f", i.fKey.c_str (), ValueOrDefault (cumStats.fQLength));
                    }
#endif
                    mfi.fCombinedIOStats = cumStats;
                }
                newFilessytems.Add (i.fKey, mfi);
            }
            return newFilessytems;
        }
        else {
            return fileSystems;
        }
    }
}

namespace {
    struct CapturerWithContext_
        : Debug::AssertExternallySynchronizedLock
#if qPlatform_Linux
        ,
          CapturerWithContext_Linux_
#elif qPlatform_Windows
        ,
          CapturerWithContext_Windows_
#else
        ,
          CapturerWithContext_COMMON_
#endif
    {
#if qPlatform_Linux
        using inherited = CapturerWithContext_Linux_;
#elif qPlatform_Windows
        using inherited = CapturerWithContext_Windows_;
#else
        using inherited = CapturerWithContext_COMMON_;
#endif
        CapturerWithContext_ (Options options)
            : inherited (options)
        {
        }
        Info capture ()
        {
            lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
            Debug::TraceContextBumper                          ctx ("Instruments::Filesystem capture");
#if qPlatform_Linux or qPlatform_Windows
            Info result = inherited::capture ();
#else
            Info result;
#endif
            if (fOptions_.fEstimateFilesystemStatsFromDiskStatsIfHelpful) {
                result.fMountedFilesystems = ApplyDiskStatsToMissingFileSystemStats_ (result.fDisks, result.fMountedFilesystems);
            }
            return result;
        }
    };
}

/*
 ********************************************************************************
 *************** Instruments::Filesystem::GetObjectVariantMapper ****************
 ********************************************************************************
 */
ObjectVariantMapper Instruments::Filesystem::GetObjectVariantMapper ()
{
    using StructFieldInfo                     = ObjectVariantMapper::StructFieldInfo;
    static const ObjectVariantMapper sMapper_ = []() -> ObjectVariantMapper {
        ObjectVariantMapper mapper;
        mapper.Add (mapper.MakeCommonSerializer_NamedEnumerations<BlockDeviceKind> ());
        mapper.AddCommonType<optional<BlockDeviceKind>> ();
        DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Winvalid-offsetof\""); // Really probably an issue, but not to debug here -- LGP 2014-01-04
        mapper.AddClass<IOStatsType> (initializer_list<StructFieldInfo>{
            {L"Bytes", Stroika_Foundation_DataExchange_StructFieldMetaInfo (IOStatsType, fBytesTransfered), StructFieldInfo::eOmitNullFields},
            {L"Q-Length", Stroika_Foundation_DataExchange_StructFieldMetaInfo (IOStatsType, fQLength), StructFieldInfo::eOmitNullFields},
            {L"In-Use-%", Stroika_Foundation_DataExchange_StructFieldMetaInfo (IOStatsType, fInUsePercent), StructFieldInfo::eOmitNullFields},
            {L"Total-Transfers", Stroika_Foundation_DataExchange_StructFieldMetaInfo (IOStatsType, fTotalTransfers), StructFieldInfo::eOmitNullFields},
        });
        mapper.AddCommonType<optional<IOStatsType>> ();
        mapper.AddClass<DiskInfoType> (initializer_list<StructFieldInfo>{
            {L"Persistence-Volume-ID", Stroika_Foundation_DataExchange_StructFieldMetaInfo (DiskInfoType, fPersistenceVolumeID), StructFieldInfo::eOmitNullFields},
            {L"Device-Kind", Stroika_Foundation_DataExchange_StructFieldMetaInfo (DiskInfoType, fDeviceKind), StructFieldInfo::eOmitNullFields},
            {L"Size", Stroika_Foundation_DataExchange_StructFieldMetaInfo (DiskInfoType, fSizeInBytes), StructFieldInfo::eOmitNullFields},
            {L"Read-IO-Stats", Stroika_Foundation_DataExchange_StructFieldMetaInfo (DiskInfoType, fReadIOStats), StructFieldInfo::eOmitNullFields},
            {L"Write-IO-Stats", Stroika_Foundation_DataExchange_StructFieldMetaInfo (DiskInfoType, fWriteIOStats), StructFieldInfo::eOmitNullFields},
            {L"Combined-IO-Stats", Stroika_Foundation_DataExchange_StructFieldMetaInfo (DiskInfoType, fCombinedIOStats), StructFieldInfo::eOmitNullFields},
        });
        mapper.AddCommonType<Set<String>> ();
        mapper.AddCommonType<optional<Set<String>>> ();
        mapper.AddClass<MountedFilesystemInfoType> (initializer_list<StructFieldInfo>{
            {L"Device-Kind", Stroika_Foundation_DataExchange_StructFieldMetaInfo (MountedFilesystemInfoType, fDeviceKind), StructFieldInfo::eOmitNullFields},
            {L"Filesystem-Type", Stroika_Foundation_DataExchange_StructFieldMetaInfo (MountedFilesystemInfoType, fFileSystemType), StructFieldInfo::eOmitNullFields},
            {L"Device-Name", Stroika_Foundation_DataExchange_StructFieldMetaInfo (MountedFilesystemInfoType, fDeviceOrVolumeName), StructFieldInfo::eOmitNullFields},
            {L"On-Physical-Drives", Stroika_Foundation_DataExchange_StructFieldMetaInfo (MountedFilesystemInfoType, fOnPhysicalDrive), StructFieldInfo::eOmitNullFields},
            {L"Volume-ID", Stroika_Foundation_DataExchange_StructFieldMetaInfo (MountedFilesystemInfoType, fVolumeID), StructFieldInfo::eOmitNullFields},
            {L"Total-Size", Stroika_Foundation_DataExchange_StructFieldMetaInfo (MountedFilesystemInfoType, fSizeInBytes), StructFieldInfo::eOmitNullFields},
            {L"Available-Size", Stroika_Foundation_DataExchange_StructFieldMetaInfo (MountedFilesystemInfoType, fAvailableSizeInBytes), StructFieldInfo::eOmitNullFields},
            {L"Used-Size", Stroika_Foundation_DataExchange_StructFieldMetaInfo (MountedFilesystemInfoType, fUsedSizeInBytes), StructFieldInfo::eOmitNullFields},
            {L"Read-IO-Stats", Stroika_Foundation_DataExchange_StructFieldMetaInfo (MountedFilesystemInfoType, fReadIOStats), StructFieldInfo::eOmitNullFields},
            {L"Write-IO-Stats", Stroika_Foundation_DataExchange_StructFieldMetaInfo (MountedFilesystemInfoType, fWriteIOStats), StructFieldInfo::eOmitNullFields},
            {L"Combined-IO-Stats", Stroika_Foundation_DataExchange_StructFieldMetaInfo (MountedFilesystemInfoType, fCombinedIOStats), StructFieldInfo::eOmitNullFields},
        });
        mapper.AddCommonType<Mapping<DynamicDiskIDType, DiskInfoType>> ();
        mapper.AddCommonType<Mapping<MountedFilesystemNameType, MountedFilesystemInfoType>> ();
        mapper.AddClass<Info> (initializer_list<StructFieldInfo>{
            {L"Disks", Stroika_Foundation_DataExchange_StructFieldMetaInfo (Info, fDisks)},
            {L"Mounted-Filesystems", Stroika_Foundation_DataExchange_StructFieldMetaInfo (Info, fMountedFilesystems)},
        });
        DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Winvalid-offsetof\"");
        return mapper;
    }();
    return sMapper_;
}

namespace {
    static const MeasurementType kMountedVolumeUsage_ = MeasurementType{String_Constant{L"Mounted-Filesystem-Usage"}};
}

namespace {
    class MyCapturer_ : public Instrument::ICapturer {
        CapturerWithContext_ fCaptureContext;

    public:
        MyCapturer_ (const CapturerWithContext_& ctx)
            : fCaptureContext (ctx)
        {
        }
        virtual MeasurementSet Capture () override
        {
            MeasurementSet results;
            Measurement    m{kMountedVolumeUsage_, GetObjectVariantMapper ().FromObject (Capture_Raw (&results.fMeasuredAt))};
            results.fMeasurements.Add (m);
            return results;
        }
        nonvirtual Info Capture_Raw (Range<DurationSecondsType>* outMeasuredAt)
        {
            DurationSecondsType before         = fCaptureContext.GetLastCaptureAt ();
            Info                rawMeasurement = fCaptureContext.capture ();
            if (outMeasuredAt != nullptr) {
                *outMeasuredAt = Range<DurationSecondsType> (before, fCaptureContext.GetLastCaptureAt ());
            }
            return rawMeasurement;
        }
        virtual unique_ptr<ICapturer> Clone () const override
        {
            return make_unique<MyCapturer_> (fCaptureContext);
        }
    };
}

/*
 ********************************************************************************
 ******************** Instruments::Filesystem::GetInstrument ********************
 ********************************************************************************
 */
Instrument SystemPerformance::Instruments::Filesystem::GetInstrument (Options options)
{
    return Instrument (
        InstrumentNameType{String_Constant{L"Filesystem"}},
        Instrument::SharedByValueCaptureRepType (make_unique<MyCapturer_> (CapturerWithContext_{options})),
        {kMountedVolumeUsage_},
        GetObjectVariantMapper ());
}

/*
 ********************************************************************************
 ********* SystemPerformance::Instrument::CaptureOneMeasurement *****************
 ********************************************************************************
 */
template <>
Instruments::Filesystem::Info SystemPerformance::Instrument::CaptureOneMeasurement (Range<DurationSecondsType>* measurementTimeOut)
{
    MyCapturer_* myCap = dynamic_cast<MyCapturer_*> (fCapFun_.get ());
    AssertNotNull (myCap);
    return myCap->Capture_Raw (measurementTimeOut);
}
