/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include <filesystem>
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

#include "Stroika/Foundation/Characters/CString/Utilities.h"
#include "Stroika/Foundation/Characters/FloatConversion.h"
#include "Stroika/Foundation/Characters/String2Int.h"
#include "Stroika/Foundation/Common/SystemConfiguration.h"
#include "Stroika/Foundation/Containers/Mapping.h"
#include "Stroika/Foundation/Containers/Sequence.h"
#include "Stroika/Foundation/Containers/Set.h"
#include "Stroika/Foundation/DataExchange/Variant/CharacterDelimitedLines/Reader.h"
#include "Stroika/Foundation/DataExchange/Variant/JSON/Writer.h"
#include "Stroika/Foundation/Debug/AssertExternallySynchronizedMutex.h"
#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Execution/Exceptions.h"
#include "Stroika/Foundation/Execution/ProcessRunner.h"
#include "Stroika/Foundation/Execution/Synchronized.h"
#include "Stroika/Foundation/IO/FileSystem/Disk.h"
#include "Stroika/Foundation/IO/FileSystem/FileInputStream.h"
#include "Stroika/Foundation/IO/FileSystem/FileSystem.h"
#include "Stroika/Foundation/IO/FileSystem/PathName.h"
#include "Stroika/Foundation/Streams/MemoryStream.h"
#include "Stroika/Foundation/Streams/TextReader.h"

#include "Stroika/Frameworks/SystemPerformance/Support/InstrumentHelpers.h"

#include "Filesystem.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::DataExchange;
using namespace Stroika::Foundation::Execution;
using namespace Stroika::Foundation::Memory;
using namespace Stroika::Foundation::Streams;

using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::SystemPerformance;

using Characters::String2Int;

using Instruments::Filesystem::BlockDeviceKind;
using Instruments::Filesystem::DiskInfoType;
using Instruments::Filesystem::DynamicDiskIDType;
using Instruments::Filesystem::Info;
using Instruments::Filesystem::IOStatsType;
using Instruments::Filesystem::MountedFilesystemInfoType;
using Instruments::Filesystem::MountedFilesystemNameType;
using Instruments::Filesystem::Options;

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
    // %InUse = QL / (1 + QL)
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
#include "Stroika/Frameworks/SystemPerformance/Support/WMICollector.h"

using SystemPerformance::Support::WMICollector;
#endif

/*
 ********************************************************************************
 ***************** Instruments::Filesystem::MountedFilesystemInfoType ***********
 ********************************************************************************
 */
String Instruments::Filesystem::MountedFilesystemInfoType::ToString () const
{
    return DataExchange::Variant::JSON::Writer{}.WriteAsString (Instrument::kObjectVariantMapper.FromObject (*this));
}

/*
 ********************************************************************************
 ********************* Instruments::Filesystem::IOStatsType *********************
 ********************************************************************************
 */
String Instruments::Filesystem::IOStatsType::ToString () const
{
    return DataExchange::Variant::JSON::Writer{}.WriteAsString (Instrument::kObjectVariantMapper.FromObject (*this));
}

/*
 ********************************************************************************
 ******************** Instruments::Filesystem::DiskInfoType *********************
 ********************************************************************************
 */
String Instruments::Filesystem::DiskInfoType::ToString () const
{
    return DataExchange::Variant::JSON::Writer{}.WriteAsString (Instrument::kObjectVariantMapper.FromObject (*this));
}

/*
 ********************************************************************************
 ********************** Instruments::Filesystem::Info ***************************
 ********************************************************************************
 */
String Instruments::Filesystem::Info::ToString () const
{
    return DataExchange::Variant::JSON::Writer{}.WriteAsString (Instrument::kObjectVariantMapper.FromObject (*this));
}

namespace {
#if qUseWMICollectionSupport_
    const String kDiskReadBytesPerSec_{"Disk Read Bytes/sec"sv};
    const String kDiskWriteBytesPerSec_{"Disk Write Bytes/sec"sv};
    const String kDiskReadsPerSec_{"Disk Reads/sec"sv};
    const String kDiskWritesPerSec_{"Disk Writes/sec"sv};
    const String kPctDiskReadTime_{"% Disk Read Time"sv};
    const String kPctDiskWriteTime_{"% Disk Write Time"sv};
    const String kAveDiskReadQLen_{"Avg. Disk Read Queue Length"sv};
    const String kAveDiskWriteQLen_{"Avg. Disk Write Queue Length"sv};
    const String kPctIdleTime_{"% Idle Time"sv};

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
    template <typename CONTEXT>
    using InstrumentRepBase_ = SystemPerformance::Support::InstrumentRep_COMMON<Options, CONTEXT>;
}

#if qPlatform_POSIX
namespace {
    void ApplyDiskTypes_ (Mapping<MountedFilesystemNameType, MountedFilesystemInfoType>* volumes)
    {
        RequireNotNull (volumes);
        static const Set<String> kRealDiskFS{
            "ext2"sv, "ext3"sv, "ext4"sv, "xfs"sv, "jfs2"sv,
        };
        static const Set<String> kSysFSList_{
            "autofs"sv,   "binfmt_misc"sv, "cgroup"sv,          "configfs"sv,   "debugfs"sv,   "devpts"sv,
            "devtmpfs"sv, "fusectl"sv,     "fuse.gvfsd-fuse"sv, "hugetlbfs"sv,  "mqueue"sv,
            "nfsd"sv, // not nfs filesystem, but special config fs - http://linux.die.net/man/7/nfsd
            "pstore"sv,   "proc"sv,        "rpc_pipefs"sv,      "securityfs"sv, "selinuxfs"sv, "sunrpc"sv,
            "sysfs"sv,    "usbfs"sv,
        };
        static const Set<String> kNetworkFS_{
            "nfs"sv,
            "nfs3"sv,
            "vboxsf"sv,
        };
        for (auto vi = volumes->begin (); vi != volumes->end (); ++vi) {
            // @todo - NOTE - this is NOT a reliable way to tell, but hopefully good enough for starters
            MountedFilesystemInfoType val2Update = vi->fValue;
            if (val2Update.fFileSystemType) {
                String fstype = *val2Update.fFileSystemType;
                bool   changed{false};
                if (kRealDiskFS.Contains (fstype)) {
                    val2Update.fDeviceKind = BlockDeviceKind::eLocalDisk;
                    changed                = true;
                }
                else if (kNetworkFS_.Contains (fstype)) {
                    val2Update.fDeviceKind = BlockDeviceKind::eNetworkDrive;
                    changed                = true;
                }
                else if (fstype == "tmpfs"_k) {
                    val2Update.fDeviceKind = BlockDeviceKind::eTemporaryFiles;
                    changed                = true;
                }
                else if (fstype == "iso9660"_k) {
                    val2Update.fDeviceKind = BlockDeviceKind::eReadOnlyEjectable;
                    changed                = true;
                }
                else if (kSysFSList_.Contains (fstype)) {
                    val2Update.fDeviceKind = BlockDeviceKind::eSystemInformation;
                    changed                = true;
                }
                if (changed) {
                    volumes->Update (vi, val2Update, &vi);
                }
            }
        }
    }
}
#endif
#if qPlatform_Linux
namespace {
    struct PerfStats_ {
        double fSectorsRead;
        double fTimeSpentReading;
        double fReadsCompleted;
        double fSectorsWritten;
        double fTimeSpentWriting;
        double fWritesCompleted;
        double fWeightedTimeInQSeconds; // see https://www.kernel.org/doc/Documentation/block/stat.txt  time_in_queue (product of the number of milliseconds times the number of requests waiting)
    };
    struct _Context : SystemPerformance::Support::Context {
        Mapping<String, uint32_t>            fDeviceName2SectorSizeMap_;
        optional<Mapping<dev_t, PerfStats_>> fDiskPerfStats_;
    };

    struct InstrumentRep_Linux_ : InstrumentRepBase_<_Context> {
    public:
        using InstrumentRepBase_<_Context>::InstrumentRepBase_;

        nonvirtual Info _InternalCapture ()
        {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
            Debug::TraceContextBumper ctx{"Instruments::Filesystem...InstrumentRep_Linux_::capture"};
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
            if (not _fOptions.fIncludeTemporaryDevices or not _fOptions.fIncludeSystemDevices) {
                for (const KeyValuePair<MountedFilesystemNameType, MountedFilesystemInfoType>& i : results.fMountedFilesystems) {
                    if (not _fOptions.fIncludeTemporaryDevices and i.fValue.fDeviceKind == BlockDeviceKind::eTemporaryFiles) {
                        results.fMountedFilesystems.Remove (i.fKey);
                    }
                    else if (not not _fOptions.fIncludeSystemDevices and i.fValue.fDeviceKind == BlockDeviceKind::eSystemInformation) {
                        results.fMountedFilesystems.Remove (i.fKey);
                    }
                }
            }
            if (_fOptions.fIOStatistics) {
                ReadAndApplyProcFS_diskstats_ (&results.fMountedFilesystems);
            }
            _NoteCompletedCapture ();
            return results;
        }

    private:
        static Mapping<MountedFilesystemNameType, MountedFilesystemInfoType> ReadVolumesAndUsageFromProcMountsAndstatvfs_ ()
        {
            Mapping<MountedFilesystemNameType, MountedFilesystemInfoType> result;
            for (const IO::FileSystem::MountedFilesystemType& mi : IO::FileSystem::GetMountedFilesystems ()) {
                MountedFilesystemInfoType vi;
                String                    deviceName = (not mi.fDevicePaths.has_value () or mi.fDevicePaths->empty ())
                                                           ? String{}
                                                           : IO::FileSystem::FromPath (mi.fDevicePaths->Nth (0));
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
        static void UpdateVolumeInfo_statvfs_ (const filesystem::path& mountedOnName, MountedFilesystemInfoType* v)
        {
            RequireNotNull (v);
            struct statvfs sbuf {};
            if (::statvfs (mountedOnName.c_str (), &sbuf) == 0) {
                uint64_t diskSize        = sbuf.f_bsize * sbuf.f_blocks;
                v->fSizeInBytes          = diskSize;
                v->fAvailableSizeInBytes = sbuf.f_bsize * sbuf.f_bavail;
                v->fUsedSizeInBytes      = diskSize - sbuf.f_bsize * sbuf.f_bfree;
            }
            else {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                DbgTrace ("statvfs ({}) return error: errno={}"_f, mountedOnName, errno);
#endif
            }
        }

    private:
        nonvirtual void ReadAndApplyProcFS_diskstats_ (Mapping<MountedFilesystemNameType, MountedFilesystemInfoType>* volumes)
        {
            try {
                Mapping<dev_t, PerfStats_> diskStats            = ReadProcFS_diskstats_ ();
                Time::DurationSeconds      timeSinceLastMeasure = Time::GetTickCount () - _GetCaptureContextTime ();
                for (Iterator<KeyValuePair<MountedFilesystemNameType, MountedFilesystemInfoType>> i = volumes->begin (); i != volumes->end (); ++i) {
                    MountedFilesystemInfoType vi = i->fValue;
                    if (vi.fDeviceOrVolumeName.has_value ()) {
                        if (_fContext.load ()->fDiskPerfStats_) {
                            String devNameLessSlashes = *vi.fDeviceOrVolumeName;
                            size_t i                  = devNameLessSlashes.rfind ('/');
                            if (i != string::npos) {
                                devNameLessSlashes = devNameLessSlashes.SubString (i + 1);
                            }
                            dev_t useDevT;
                            {
                                struct stat sbuf {};
                                if (::stat (vi.fDeviceOrVolumeName->AsNarrowSDKString ().c_str (), &sbuf) == 0) {
                                    useDevT = sbuf.st_rdev;
                                }
                                else {
                                    continue;
                                }
                            }
                            optional<PerfStats_> oOld = _fContext.load ()->fDiskPerfStats_->Lookup (useDevT);
                            optional<PerfStats_> oNew = diskStats.Lookup (useDevT);
                            if (oOld.has_value () and oNew.has_value ()) {
                                unsigned int sectorSizeTmpHack = GetSectorSize_ (devNameLessSlashes);
                                IOStatsType  readStats;
                                readStats.fBytesTransfered = (oNew->fSectorsRead - oOld->fSectorsRead) * sectorSizeTmpHack;
                                readStats.fTotalTransfers  = oNew->fReadsCompleted - oOld->fReadsCompleted;
                                if (timeSinceLastMeasure > _fOptions.fMinimumAveragingInterval) {
                                    readStats.fQLength = (oNew->fTimeSpentReading - oOld->fTimeSpentReading) / timeSinceLastMeasure.count ();
                                }

                                IOStatsType writeStats;
                                writeStats.fBytesTransfered = (oNew->fSectorsWritten - oOld->fSectorsWritten) * sectorSizeTmpHack;
                                writeStats.fTotalTransfers  = oNew->fWritesCompleted - oOld->fWritesCompleted;
                                if (timeSinceLastMeasure > _fOptions.fMinimumAveragingInterval) {
                                    writeStats.fQLength = (oNew->fTimeSpentWriting - oOld->fTimeSpentWriting) / timeSinceLastMeasure.count ();
                                }

                                IOStatsType combinedStats;
                                combinedStats.fBytesTransfered = *readStats.fBytesTransfered + *writeStats.fBytesTransfered;
                                combinedStats.fTotalTransfers  = *readStats.fTotalTransfers + *writeStats.fTotalTransfers;
                                combinedStats.fQLength         = *readStats.fQLength + *writeStats.fQLength;

                                vi.fReadIOStats  = readStats;
                                vi.fWriteIOStats = writeStats;
                                // @todo DESCRIBE divide by time between 2 and * 1000 - NYI
                                if (timeSinceLastMeasure > _fOptions.fMinimumAveragingInterval) {
                                    combinedStats.fQLength =
                                        ((oNew->fWeightedTimeInQSeconds - oOld->fWeightedTimeInQSeconds) / timeSinceLastMeasure.count ());
                                }
                                vi.fCombinedIOStats = combinedStats;
                            }
                        }
                    }
                    volumes->Update (i, vi, &i);
                }
                _fContext.rwget ().rwref ()->fDiskPerfStats_ = diskStats;
            }
            catch (...) {
                DbgTrace ("Exception gathering procfs disk io stats"_f);
            }
        }

    private:
        static optional<filesystem::path> GetSysBlockDirPathForDevice_ (const String& deviceName)
        {
            Require (not deviceName.empty ());
            Require (not deviceName.Contains ("/"sv));
            // Sometimes the /sys/block directory appears to have data for the each major/minor pair, and sometimes it appears
            // to only have it for the top level (minor=0) one without the digit after in the name.
            //
            // I don't understand this well yet, but this appears to temporarily allow us to limp along --LGP 2015-07-10
            //tmphack
            static const filesystem::path kSysBlock_{"/sys/block"sv};
            filesystem::path              tmp{kSysBlock_ / IO::FileSystem::ToPath (deviceName)};
            if (IO::FileSystem::Default ().Access (tmp)) {
                return tmp;
            }
            //tmphack - try using one char less
            tmp = kSysBlock_ / IO::FileSystem::ToPath (deviceName.SubString (0, -1));
            if (IO::FileSystem::Default ().Access (tmp)) {
                return tmp;
            }
            return nullopt;
        }

    private:
        nonvirtual uint32_t GetSectorSize_ (const String& deviceName)
        {
            auto o = _fContext.load ()->fDeviceName2SectorSizeMap_.Lookup (deviceName);
            if (not o.has_value ()) {
                if (optional<filesystem::path> blockDeviceInfoPath = GetSysBlockDirPathForDevice_ (deviceName)) {
                    filesystem::path fn = *blockDeviceInfoPath / "queue/hw_sector_size";
                    try {
                        o = String2Int<uint32_t> (TextReader::New (IO::FileSystem::FileInputStream::New (fn, IO::FileSystem::FileInputStream::eNotSeekable))
                                                      .ReadAll ()
                                                      .Trim ());
                        _fContext.rwget ().rwref ()->fDeviceName2SectorSizeMap_.Add (deviceName, *o);
                    }
                    catch (...) {
                        DbgTrace ("Unknown error reading {}"_f, fn);
                        // ignore
                    }
                }
            }
            return o.value_or (512); // seems the typical answer on UNIX
        }

    private:
        // CURRENTLY UNUSED, but COULD be used on MacOS to gather some stats... -- LGP 2021-04-21
        static Mapping<MountedFilesystemNameType, MountedFilesystemInfoType> RunDF_POSIX_ ()
        {
            Mapping<MountedFilesystemNameType, MountedFilesystemInfoType> result;
            ProcessRunner                                                 pr{"/bin/df -k -P"sv};
            Streams::MemoryStream::Ptr<byte>                              useStdOut = Streams::MemoryStream::New<byte> ();
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
                    DbgTrace ("skipping line cuz len={}"_f, l.size ());
                    continue;
                }
                MountedFilesystemInfoType v;
                {
                    String d = l[0].Trim ();
                    if (not d.empty () and d != "none"sv) {
                        v.fDeviceOrVolumeName = d;
                    }
                }
                {
                    double szInBytes = Characters::FloatConversion::ToFloat<double> (l[1]) * 1024;
                    if (not std::isnan (szInBytes) and not std::isinf (szInBytes)) {
                        v.fSizeInBytes = szInBytes;
                    }
                }
                {
                    double usedSizeInBytes = FloatConversion::ToFloat<double> (l[2]) * 1024;
                    if (not std::isnan (usedSizeInBytes) and not std::isinf (usedSizeInBytes)) {
                        v.fUsedSizeInBytes = usedSizeInBytes;
                    }
                }
                if (v.fSizeInBytes and v.fUsedSizeInBytes) {
                    v.fAvailableSizeInBytes = *v.fSizeInBytes - *v.fUsedSizeInBytes;
                }
                result.Add (IO::FileSystem::ToPath (l[5].Trim ()), v);
            }
            // Sometimes (with busy box df especially) we get bogus error return. So only rethrow if we found no good data
            if (runException and result.empty ()) {
                Execution::ReThrow (runException);
            }
            return result;
        }
        static Mapping<MountedFilesystemNameType, MountedFilesystemInfoType> RunDF_ (bool includeFSTypes)
        {
            Mapping<MountedFilesystemNameType, MountedFilesystemInfoType> result;
            //
            // I looked through the /proc filesystem stuff and didnt see anything obvious to retrive this info...
            // run def with ProcessRunner
            //
            //  NEW NOTE - I THINK ITS IN THERE.... RE-EXAMINE proc/filesystems proc/partitions, and http://en.wikipedia.org/wiki/Procfs
            //      -- LGP 2014-08-01
            ProcessRunner                    pr{includeFSTypes ? "/bin/df -k -T"sv : "/bin/df -k"sv};
            Streams::MemoryStream::Ptr<byte> useStdOut = Streams::MemoryStream::New<byte> ();
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
                    DbgTrace ("skipping line cuz len={}"_f, l.size ());
                    continue;
                }
                MountedFilesystemInfoType v;
                if (includeFSTypes) {
                    v.fFileSystemType = l[1].Trim ();
                }
                {
                    String d = l[0].Trim ();
                    if (not d.empty () and d != "none"_k) {
                        v.fDeviceOrVolumeName = d;
                    }
                }
                v.fSizeInBytes          = FloatConversion::ToFloat<double> (l[includeFSTypes ? 2 : 1]) * 1024;
                v.fUsedSizeInBytes      = FloatConversion::ToFloat<double> (l[includeFSTypes ? 3 : 2]) * 1024;
                v.fAvailableSizeInBytes = *v.fSizeInBytes - *v.fUsedSizeInBytes;
                result.Add (IO::FileSystem::ToPath (l[includeFSTypes ? 6 : 5].Trim ()), v);
            }
            // Sometimes (with busy box df especially) we get bogus error return. So only rethrow if we found no good data
            if (runException and result.empty ()) {
                Execution::ReThrow (runException);
            }
            return result;
        }
        static Mapping<MountedFilesystemNameType, MountedFilesystemInfoType> RunDF_ ()
        {
            try {
                return RunDF_ (true);
            }
            catch (...) {
                return RunDF_ (false);
            }
        }

    private:
        static Mapping<dev_t, PerfStats_> ReadProcFS_diskstats_ ()
        {
            Mapping<dev_t, PerfStats_>                             result;
            DataExchange::Variant::CharacterDelimitedLines::Reader reader{{' ', '\t'}};
            static const filesystem::path                          kProcMemInfoFileName_{"/proc/diskstats"sv};
            // Note - /procfs files always unseekable
            for (const Sequence<String>& line :
                 reader.ReadMatrix (IO::FileSystem::FileInputStream::New (kProcMemInfoFileName_, IO::FileSystem::FileInputStream::eNotSeekable))) {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                DbgTrace (L"***in Instruments::Filesystem::ReadProcFS_diskstats_ linesize=%d, line[0]=%s", line.size (),
                          line.empty () ? L"" : line[0].c_str ());
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
                        optional<filesystem::path> sysBlockInfoPath = GetSysBlockDirPathForDevice_ (devName);
                        if (sysBlockInfoPath) {
                            for (const Sequence<String>& ll : reader.ReadMatrix (IO::FileSystem::FileInputStream::New (
                                     *sysBlockInfoPath / "stat", IO::FileSystem::FileInputStream::eNotSeekable))) {
                                if (ll.size () >= 11) {
                                    weightedTimeInQSeconds = FloatConversion::ToFloat (ll[11 - 1]) / 1000.0; // we record in seconds, but the value in file in milliseconds
                                    break;
                                }
                            }
                        }
                    }
                    result.Add (makedev (String2Int<unsigned int> (majorDevNumber), String2Int<unsigned int> (minorDevNumber)),
                                PerfStats_{FloatConversion::ToFloat (sectorsRead), FloatConversion::ToFloat (timeSpentReadingMS) / 1000,
                                           FloatConversion::ToFloat (readsCompleted), FloatConversion::ToFloat (sectorsWritten),
                                           FloatConversion::ToFloat (timeSpentWritingMS) / 1000, FloatConversion::ToFloat (writesCompleted),
                                           NullCoalesce (weightedTimeInQSeconds)});
                }
            }
            return result;
        }
    };
}
#endif

#if qPlatform_Windows
namespace {

    struct _Context : SystemPerformance::Support::Context {
#if qUseWMICollectionSupport_
        // for collecting IO statistics
        WMICollector fLogicalDiskWMICollector_{"LogicalDisk"sv,
                                               {},
                                               { kDiskReadBytesPerSec_,
                                                 kDiskWriteBytesPerSec_,
                                                 kDiskReadsPerSec_,
                                                 kDiskWritesPerSec_,
                                                 (kUseDiskPercentReadTime_ElseAveQLen_ToComputeQLen_ ? kPctDiskReadTime_ : kAveDiskReadQLen_),
                                                 (kUseDiskPercentReadTime_ElseAveQLen_ToComputeQLen_ ? kPctDiskWriteTime_ : kAveDiskWriteQLen_),
                                                 kPctIdleTime_ }};
#endif
    };

    struct InstrumentRep_Windows_ : InstrumentRepBase_<_Context> {
        using InstrumentRepBase_<_Context>::InstrumentRepBase_;

        nonvirtual Info _InternalCapture ()
        {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
            Debug::TraceContextBumper ctx{"Instruments::Filesystem...InstrumentRep_Windows_::_InternalCapture"};
#endif
            Info result;

            // Could probably usefully optimize to not capture if no drives because we can only get this when running as
            // Admin, and for now, we capture little useful information at the drive level. But - we may eventually capture more...
            Collection<IO::FileSystem::DiskInfoType> physDrives = IO::FileSystem::GetAvailableDisks ();
            for (const IO::FileSystem::DiskInfoType& pd : physDrives) {
                DiskInfoType di{};
                di.fSizeInBytes = pd.fSizeInBytes;
                result.fDisks.Add (pd.fDeviceName, di);
            }

#if qUseWMICollectionSupport_
            auto                            contextLock = scoped_lock{_fContext};
            auto                            contextPtr  = _fContext.rwget ().rwref ();
            optional<Time::DurationSeconds> timeCollecting;
            {
                optional<Time::TimePointSeconds> timeOfPrevCollection = contextPtr->fLogicalDiskWMICollector_.GetTimeOfLastCollection ();
                if (_fOptions.fIOStatistics) {
                    contextPtr->fLogicalDiskWMICollector_.Collect ();
                }
                if (timeOfPrevCollection) {
                    // time of lastCollection - once set, cannot become unset
                    timeCollecting = *contextPtr->fLogicalDiskWMICollector_.GetTimeOfLastCollection () - *timeOfPrevCollection;
                }
            }
#endif

            for (const IO::FileSystem::MountedFilesystemType& mfinfo : IO::FileSystem::GetMountedFilesystems ()) {
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
                    [[maybe_unused]] DWORD ignored =
                        ::GetDiskFreeSpaceEx (mfinfo.fMountedOn.c_str (), &freeBytesAvailable, &totalNumberOfBytes, &totalNumberOfFreeBytes);
                    v.fSizeInBytes          = totalNumberOfBytes.QuadPart;
                    v.fUsedSizeInBytes      = *v.fSizeInBytes - freeBytesAvailable.QuadPart;
                    v.fAvailableSizeInBytes = *v.fSizeInBytes - *v.fUsedSizeInBytes;
#if qUseWMICollectionSupport_
                    auto safePctInUse2QL_ = [] (double pctInUse) {
                        // %InUse = QL / (1 + QL)
                        pctInUse /= 100;
                        pctInUse = Math::PinInRange<double> (pctInUse, 0, 1);
                        return pctInUse / (1 - pctInUse);
                    };
                    if (_fOptions.fIOStatistics) {
                        String wmiInstanceName =
                            IO::FileSystem::FromPath (mfinfo.fMountedOn).RTrim ([] (Characters::Character c) { return c == '\\'; });
                        contextPtr->fLogicalDiskWMICollector_.AddInstancesIf (wmiInstanceName);

                        IOStatsType readStats;
                        if (timeCollecting) {
                            if (auto o = contextPtr->fLogicalDiskWMICollector_.PeekCurrentValue (wmiInstanceName, kDiskReadBytesPerSec_)) {
                                readStats.fBytesTransfered = *o * timeCollecting->count ();
                            }
                            if (auto o = contextPtr->fLogicalDiskWMICollector_.PeekCurrentValue (wmiInstanceName, kDiskReadsPerSec_)) {
                                readStats.fTotalTransfers = *o * timeCollecting->count ();
                            }
                        }
                        if (kUseDiskPercentReadTime_ElseAveQLen_ToComputeQLen_) {
                            if (auto o = contextPtr->fLogicalDiskWMICollector_.PeekCurrentValue (wmiInstanceName, kPctDiskReadTime_)) {
                                readStats.fInUsePercent = *o;
                                readStats.fQLength      = safePctInUse2QL_ (*o);
                            }
                        }
                        else {
                            if (auto o = contextPtr->fLogicalDiskWMICollector_.PeekCurrentValue (wmiInstanceName, kAveDiskReadQLen_)) {
                                readStats.fInUsePercent = *o;
                                readStats.fQLength      = *o;
                            }
                        }

                        IOStatsType writeStats;
                        if (timeCollecting) {
                            if (auto o = contextPtr->fLogicalDiskWMICollector_.PeekCurrentValue (wmiInstanceName, kDiskWriteBytesPerSec_)) {
                                writeStats.fBytesTransfered = *o * timeCollecting->count ();
                            }
                            if (auto o = contextPtr->fLogicalDiskWMICollector_.PeekCurrentValue (wmiInstanceName, kDiskWritesPerSec_)) {
                                writeStats.fTotalTransfers = *o * timeCollecting->count ();
                            }
                        }
                        if (kUseDiskPercentReadTime_ElseAveQLen_ToComputeQLen_) {
                            if (auto o = contextPtr->fLogicalDiskWMICollector_.PeekCurrentValue (wmiInstanceName, kPctDiskWriteTime_)) {
                                writeStats.fInUsePercent = *o;
                                writeStats.fQLength      = safePctInUse2QL_ (*o);
                            }
                        }
                        else {
                            if (auto o = contextPtr->fLogicalDiskWMICollector_.PeekCurrentValue (wmiInstanceName, kAveDiskWriteQLen_)) {
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
                            if (auto o = contextPtr->fLogicalDiskWMICollector_.PeekCurrentValue (wmiInstanceName, kPctIdleTime_)) {
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
#if qUseWMICollectionSupport_
            _NoteCompletedCapture (contextPtr->fLogicalDiskWMICollector_.GetTimeOfLastCollection ().value_or (Time::GetTickCount ()));
#else
            _NoteCompletedCapture ();
#endif
            return result;
        }

#if 0
    private:
// As of 2021-04-21, this is UNUSED. Not sure what it was for. Leave around for a bit longer, and then delete...
        static optional<String> GetDeviceNameForVolumneName_ (const String& volumeName)
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
#endif
    };
}
#endif

namespace {
    static const MeasurementType kMountedVolumeUsage_ = MeasurementType{"Mounted-Filesystem-Usage"sv};
}

namespace {
    struct FilesystemInstrumentRep_
#if qPlatform_Linux
        : InstrumentRep_Linux_
#elif qPlatform_Windows
        : InstrumentRep_Windows_
#else
        : InstrumentRepBase_<SystemPerformance::Support::Context>
#endif
    {
#if qPlatform_Linux
        using inherited = InstrumentRep_Linux_;
#elif qPlatform_Windows
        using inherited = InstrumentRep_Windows_;
#else
        using inherited = InstrumentRepBase_<SystemPerformance::Support::Context>;
#endif
        FilesystemInstrumentRep_ (const Options& options, const shared_ptr<_Context>& context = make_shared<_Context> ())
            : inherited{options, context}
        {
            Require (_fOptions.fMinimumAveragingInterval > 0s);
        }
        virtual MeasurementSet Capture () override
        {
            MeasurementSet results;
            Measurement    m{kMountedVolumeUsage_,
                          Instruments::Filesystem::Instrument::kObjectVariantMapper.FromObject (Capture_Raw (&results.fMeasuredAt))};
            results.fMeasurements.Add (m);
            return results;
        }
        nonvirtual Info Capture_Raw (Range<TimePointSeconds>* outMeasuredAt)
        {
            return Do_Capture_Raw<Info> ([this] () { return _InternalCapture (); }, outMeasuredAt);
        }
        virtual unique_ptr<IRep> Clone () const override
        {
            return make_unique<FilesystemInstrumentRep_> (_fOptions, _fContext.load ());
        }
        nonvirtual Info _InternalCapture ()
        {
            AssertExternallySynchronizedMutex::WriteContext declareContext{*this};
            Debug::TraceContextBumper                       ctx{"Instruments::Filesystem _InternalCapture"};
#if qPlatform_Linux or qPlatform_Windows
            Info result = inherited::_InternalCapture ();
#else
            Info result;
#endif
            if (_fOptions.fEstimateFilesystemStatsFromDiskStatsIfHelpful) {
                result.fMountedFilesystems = ApplyDiskStatsToMissingFileSystemStats_ (result.fDisks, result.fMountedFilesystems);
            }
            return result;
        }
        // rarely used - see fEstimateFilesystemStatsFromDiskStatsIfHelpful in filesystem options
        static Mapping<MountedFilesystemNameType, MountedFilesystemInfoType>
        ApplyDiskStatsToMissingFileSystemStats_ (const Mapping<DynamicDiskIDType, DiskInfoType>&                      disks,
                                                 const Mapping<MountedFilesystemNameType, MountedFilesystemInfoType>& fileSystems)
        {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
            Debug::TraceContextBumper ctx{"Instruments::Filesystem ... ApplyDiskStatsToMissingFileSystemStats_"};
#endif
            // Each FS will have some stats about disk usage, and we want to use those to relatively weight the stats from the disk usage when
            // applied back to other FS stats.
            //
            // So first compute the total stat per disk
            using WeightingStat2UseType = double;
            Mapping<DynamicDiskIDType, WeightingStat2UseType> totalWeights;
            for (const KeyValuePair<MountedFilesystemNameType, MountedFilesystemInfoType>& i : fileSystems) {
                Set<DynamicDiskIDType> disksForFS = NullCoalesce (i.fValue.fOnPhysicalDrive);
                if (disksForFS.size () > 0) {
                    WeightingStat2UseType weightForFS = NullCoalesce (NullCoalesce (i.fValue.fCombinedIOStats).fBytesTransfered);
                    weightForFS /= disksForFS.size ();
                    for (const DynamicDiskIDType& di : disksForFS) {
                        totalWeights.Add (di, totalWeights.LookupValue (di) + weightForFS); // accumulate relative application to each disk
                    }
                }
            }
#if USE_NOISY_TRACE_IN_THIS_MODULE_
            {
                Debug::TraceContextBumper ctx1{"Weighted disk stats"};
                for (const auto& i : totalWeights) {
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
                for (const KeyValuePair<MountedFilesystemNameType, MountedFilesystemInfoType>& i : fileSystems) {
                    MountedFilesystemInfoType mfi        = i.fValue;
                    Set<DynamicDiskIDType>    disksForFS = NullCoalesce (mfi.fOnPhysicalDrive);
                    if (disksForFS.size () > 0) {
                        WeightingStat2UseType weightForFS = NullCoalesce (NullCoalesce (i.fValue.fCombinedIOStats).fBytesTransfered);
                        weightForFS /= disksForFS.size ();
                        IOStatsType cumStats          = NullCoalesce (mfi.fCombinedIOStats);
                        bool        computeInuse      = not cumStats.fInUsePercent.has_value ();
                        bool        computeQLen       = not cumStats.fQLength.has_value ();
                        bool        computeTotalXFers = not cumStats.fTotalTransfers.has_value ();

                        for (const DynamicDiskIDType& di : disksForFS) {
                            IOStatsType diskIOStats = NullCoalesce (disks.LookupValue (di).fCombinedIOStats);
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
                            DbgTrace (L"Adjusted fInUsePCT for filesystem '%s' is %f", i.fKey.c_str (), NullCoalesce (cumStats.fInUsePercent));
                        }
                        if (computeQLen) {
                            DbgTrace (L"Adjusted fQLength for filesystem '%s' is %f", i.fKey.c_str (), NullCoalesce (cumStats.fQLength));
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
    };
}

/*
 ********************************************************************************
 ******************** Instruments::Filesystem::Instrument ***********************
 ********************************************************************************
 */
const ObjectVariantMapper Instruments::Filesystem::Instrument::kObjectVariantMapper = [] () -> ObjectVariantMapper {
    ObjectVariantMapper mapper;
    mapper.Add (mapper.MakeCommonSerializer_NamedEnumerations<BlockDeviceKind> ());
    mapper.AddCommonType<optional<BlockDeviceKind>> ();
    mapper.AddClass<IOStatsType> ({
        {"Bytes"sv, &IOStatsType::fBytesTransfered},
        {"Q-Length"sv, &IOStatsType::fQLength},
        {"In-Use-%"sv, &IOStatsType::fInUsePercent},
        {"Total-Transfers"sv, &IOStatsType::fTotalTransfers},
    });
    mapper.AddCommonType<optional<IOStatsType>> ();
    mapper.AddClass<DiskInfoType> ({
        {"Persistence-Volume-ID"sv, &DiskInfoType::fPersistenceVolumeID},
        {"Device-Kind"sv, &DiskInfoType::fDeviceKind},
        {"Size"sv, &DiskInfoType::fSizeInBytes},
        {"Read-IO-Stats"sv, &DiskInfoType::fReadIOStats},
        {"Write-IO-Stats"sv, &DiskInfoType::fWriteIOStats},
        {"Combined-IO-Stats"sv, &DiskInfoType::fCombinedIOStats},
    });
    mapper.AddCommonType<Set<String>> ();
    mapper.AddCommonType<optional<Set<String>>> ();
    mapper.AddCommonType<Set<filesystem::path>> ();
    mapper.AddCommonType<optional<Set<filesystem::path>>> ();
    mapper.AddClass<MountedFilesystemInfoType> ({
        {"Device-Kind"_k, &MountedFilesystemInfoType::fDeviceKind},
        {"Filesystem-Type"_k, &MountedFilesystemInfoType::fFileSystemType},
        {"Device-Name"_k, &MountedFilesystemInfoType::fDeviceOrVolumeName},
        {"On-Physical-Drives"_k, &MountedFilesystemInfoType::fOnPhysicalDrive},
        {"Volume-ID"_k, &MountedFilesystemInfoType::fVolumeID},
        {"Total-Size"_k, &MountedFilesystemInfoType::fSizeInBytes},
        {"Available-Size"_k, &MountedFilesystemInfoType::fAvailableSizeInBytes},
        {"Used-Size"_k, &MountedFilesystemInfoType::fUsedSizeInBytes},
        {"Read-IO-Stats"_k, &MountedFilesystemInfoType::fReadIOStats},
        {"Write-IO-Stats"_k, &MountedFilesystemInfoType::fWriteIOStats},
        {"Combined-IO-Stats"_k, &MountedFilesystemInfoType::fCombinedIOStats},
    });
    mapper.AddCommonType<Mapping<DynamicDiskIDType, DiskInfoType>> ();
    mapper.AddCommonType<Mapping<MountedFilesystemNameType, MountedFilesystemInfoType>> ();
    mapper.AddClass<Info> ({
        {"Disks"_k, &Info::fDisks},
        {"Mounted-Filesystems"_k, &Info::fMountedFilesystems},
    });
    return mapper;
}();

Instruments::Filesystem::Instrument::Instrument (const Options& options)
    : SystemPerformance::Instrument{InstrumentNameType{"Filesystem"sv},
                                    make_unique<FilesystemInstrumentRep_> (options),
                                    {kMountedVolumeUsage_},
                                    {KeyValuePair<type_index, MeasurementType>{typeid (Info), kMountedVolumeUsage_}},
                                    Instrument::kObjectVariantMapper}
{
}

/*
 ********************************************************************************
 ********* SystemPerformance::Instrument::CaptureOneMeasurement *****************
 ********************************************************************************
 */
template <>
Instruments::Filesystem::Info SystemPerformance::Instrument::CaptureOneMeasurement (Range<TimePointSeconds>* measurementTimeOut)
{
    Debug::TraceContextBumper ctx{"SystemPerformance::Instrument::CaptureOneMeasurement"};
    FilesystemInstrumentRep_* myCap = dynamic_cast<FilesystemInstrumentRep_*> (fCaptureRep_.get ());
    AssertNotNull (myCap);
    return myCap->Capture_Raw (measurementTimeOut);
}
