/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#if qPlatform_POSIX
#include <sys/stat.h>
#include <sys/statvfs.h>
#include <sys/types.h>
#include <unistd.h>
#elif qPlatform_Windows
#include <Windows.h>
#include <winioctl.h>
#endif

#include "../../../Foundation/Characters/CString/Utilities.h"
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

using Characters::String_Constant;
using Characters::String2Int;
using Time::DurationSecondsType;
using IO::FileSystem::FileInputStream;
using Streams::TextReader;

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1

/*
 *  I thought this might be useful. maybe at some point? But so far it doesn't appear super useful
 *  or needed.
 *  But we can use this to find out the disk kind for physical devices
 *  --LGP 2015-09-30
 */
#ifndef qCaptureDiskDeviceInfoWindows_
#define qCaptureDiskDeviceInfoWindows_ 0
#endif

#if qCaptureDiskDeviceInfoWindows_
#include <devguid.h>
#include <regstr.h>
#include <setupapi.h>
#pragma comment(lib, "Setupapi.lib")
DEFINE_GUID (GUID_DEVINTERFACE_DISK, 0x53f56307L, 0xb6bf, 0x11d0, 0x94, 0xf2, 0x00, 0xa0, 0xc9, 0x1e, 0xfb, 0x8b);
//#include <Ntddstor.h>
#endif

/*
 ********************************************************************************
 ********* Instruments::Filesystem::IOStatsType::EstimatedPercentInUse **********
 ********************************************************************************
 */
Optional<double> IOStatsType::EstimatedPercentInUse () const
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
    return Optional<double> ();
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
        Optional<Mapping<dev_t, PerfStats_>> fContextStats_;

    public:
        CapturerWithContext_Linux_ (Options options)
            : CapturerWithContext_COMMON_ (options)
        {
            // for side-effect of setting fContextStats_
            try {
                capture_ ();
            }
            catch (...) {
                DbgTrace ("bad sign that first pre-catpure failed."); // Dont propagate in case just listing collectors
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
#if 1
            for (IO::FileSystem::MountedFilesystemType mi : IO::FileSystem::GetMountedFilesystems ()) {
                MountedFilesystemInfoType vi;
                String                    deviceName = (mi.fDevicePaths.IsMissing () or mi.fDevicePaths->empty ()) ? String{} : mi.fDevicePaths->Nnth (0);
                if (not deviceName.empty () and deviceName != L"none") { // special name none often used when there is no name
                    vi.fDeviceOrVolumeName = deviceName;
                }
                vi.fFileSystemType = mi.fFileSystemType;
                UpdateVolumeInfo_statvfs_ (mi.fMountedOn, &vi);
                result.Add (mi.fMountedOn, vi);
            }
#else
            for (MountInfo_ mi : ReadMountInfo_ ()) {
                MountedFilesystemInfoType vi;
                if (not mi.fDeviceName.empty () and mi.fDeviceName != L"none") { // special name none often used when there is no name
                    vi.fDeviceOrVolumeName = mi.fDeviceName;
                }
                vi.fFileSystemType = mi.fFilesystemFormat;
                UpdateVolumeInfo_statvfs_ (mi.fMountedOn, &vi);
                result.Add (mi.fMountedOn, vi);
            }
#endif
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
#if 0
    private:
        struct MountInfo_ {
            String fDeviceName;
            String fMountedOn;
            String fFilesystemFormat;
        };
        Sequence<MountInfo_> ReadMountInfo_ ()
        {
            // first try procfs, but if that fails, fallback on mount command
            try {
                return ReadMountInfo_FromProcMounts_ ();
            }
            catch (...) {
                return Sequence<MountInfo_> ();
            }
        }
        Sequence<MountInfo_> ReadMountInfo_FromProcMounts_ ()
        {
            /*
             *  I haven't found this clearly documented yet, but it appears that a filesystem can be over-mounted.
             *  See https://www.kernel.org/doc/Documentation/filesystems/ramfs-rootfs-initramfs.txt
             *
             *  So the last one with a given mount point in the file wins.
             */
            Mapping<String, MountInfo_> result;
            DataExchange::Variant::CharacterDelimitedLines::Reader reader{{' ', '\t'}};
            // Note - /procfs files always unseekable
            static const String_Constant kProcMountsFileName_{L"/proc/mounts"};
            ;
            for (Sequence<String> line : reader.ReadMatrix (FileInputStream::mk (kProcMountsFileName_, FileInputStream::eNotSeekable))) {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                DbgTrace (L"***in Instruments::Filesystem::Read_proc_mounts_ linesize=%d, line[0]=%s", line.size (), line.empty () ? L"" : line[0].c_str ());
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
                    String mountedOn = line[1];
                    String fstype    = line[2];
                    result.Add (
                        mountedOn,
                        MountInfo_{
                            devName, mountedOn, fstype});
                }
            }
            return Sequence<MountInfo_> (result.MappedValues ());
        }
#endif

    private:
        void ReadAndApplyProcFS_diskstats_ (Mapping<MountedFilesystemNameType, MountedFilesystemInfoType>* volumes)
        {
            try {
                Mapping<dev_t, PerfStats_> diskStats = ReadProcFS_diskstats_ ();
                DurationSecondsType timeSinceLastMeasure = Time::GetTickCount () - GetLastCaptureAt ();
                for (KeyValuePair<MountedFilesystemNameType, MountedFilesystemInfoType> i : *volumes) {
                    MountedFilesystemInfoType vi = i.fValue;
                    if (vi.fDeviceOrVolumeName.IsPresent ()) {
                        if (fContextStats_) {
                            String devNameLessSlashes = *vi.fDeviceOrVolumeName;
                            size_t i                  = devNameLessSlashes.RFind ('/');
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
                            Optional<PerfStats_> oOld = fContextStats_->Lookup (useDevT);
                            Optional<PerfStats_> oNew = diskStats.Lookup (useDevT);
                            if (oOld.IsPresent () and oNew.IsPresent ()) {
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
        Optional<String> GetSysBlockDirPathForDevice_ (const String& deviceName)
        {
            Require (not deviceName.empty ());
            Require (not deviceName.Contains (L"/"));
            // Sometimes the /sys/block directory appears to have data for the each major/minor pair, and sometimes it appears
            // to only have it for the top level (minor=0) one without the digit after in the name.
            //
            // I dont understand this well yet, but this appears to temporarily allow us to limp along --LGP 2015-07-10
            //tmphack
            String tmp{L"/sys/block/" + deviceName + L"/"};
            if (IO::FileSystem::FileSystem::Default ().Access (tmp)) {
                return tmp;
            }
            //tmphack - try using one char less
            tmp = L"/sys/block/" + deviceName.CircularSubString (0, -1) + L"/";
            if (IO::FileSystem::FileSystem::Default ().Access (tmp)) {
                return tmp;
            }
            return Optional<String> ();
        }

    private:
        uint32_t GetSectorSize_ (const String& deviceName)
        {
            auto o = fDeviceName2SectorSizeMap_.Lookup (deviceName);
            if (o.IsMissing ()) {
                Optional<String> blockDeviceInfoPath = GetSysBlockDirPathForDevice_ (deviceName);
                if (blockDeviceInfoPath) {
                    String fn = *blockDeviceInfoPath + L"queue/hw_sector_size";
                    try {
                        o = String2Int<uint32_t> (TextReader (FileInputStream::mk (fn, FileInputStream::eNotSeekable)).ReadAll ().Trim ());
                        fDeviceName2SectorSizeMap_.Add (deviceName, *o);
                    }
                    catch (...) {
                        DbgTrace (L"Unknown error reading %s", fn.c_str ());
                        // ignore
                    }
                }
            }
            if (o.IsMissing ()) {
                o = 512; // seems the typical answer on UNIX
            }
            return *o;
        }

    private:
        Mapping<MountedFilesystemNameType, MountedFilesystemInfoType> RunDF_POSIX_ ()
        {
            Mapping<MountedFilesystemNameType, MountedFilesystemInfoType> result;
            ProcessRunner               pr{L"/bin/df -k -P"};
            Streams::MemoryStream<Byte> useStdOut;
            pr.SetStdOut (useStdOut);
            std::exception_ptr runException;
            try {
                pr.Run ();
            }
            catch (...) {
                runException = current_exception ();
            }
            String              out;
            Streams::TextReader stdOut        = Streams::TextReader (useStdOut);
            bool                skippedHeader = false;
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
            // Sometimes (with busy box df especailly) we get bogus error return. So only rethrow if we found no good data
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
            ProcessRunner               pr{includeFSTypes ? L"/bin/df -k -T" : L"/bin/df -k"};
            Streams::MemoryStream<Byte> useStdOut;
            pr.SetStdOut (useStdOut);
            std::exception_ptr runException;
            try {
                pr.Run ();
            }
            catch (...) {
                runException = current_exception ();
            }
            String              out;
            Streams::TextReader stdOut        = Streams::TextReader (useStdOut);
            bool                skippedHeader = false;
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
            // Sometimes (with busy box df especailly) we get bogus error return. So only rethrow if we found no good data
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
            Mapping<dev_t, PerfStats_> result;
            DataExchange::Variant::CharacterDelimitedLines::Reader reader{{' ', '\t'}};
            const String_Constant                                  kProcMemInfoFileName_{L"/proc/diskstats"};
            // Note - /procfs files always unseekable
            for (Sequence<String> line : reader.ReadMatrix (FileInputStream::mk (kProcMemInfoFileName_, FileInputStream::eNotSeekable))) {
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
                    Optional<double> weightedTimeInQSeconds;
                    if (kAlsoReadQLen_) {
                        Optional<String> sysBlockInfoPath = GetSysBlockDirPathForDevice_ (devName);
                        if (sysBlockInfoPath) {
                            for (Sequence<String> ll : reader.ReadMatrix (FileInputStream::mk (*sysBlockInfoPath + L"stat", FileInputStream::eNotSeekable))) {
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
                            weightedTimeInQSeconds.Value ()});
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
#if qCaptureDiskDeviceInfoWindows_
        list<wstring> GetPhysicalDiskDeviceInfo_ ()
        {
            HDEVINFO      hDeviceInfoSet;
            ULONG         ulMemberIndex;
            ULONG         ulErrorCode;
            BOOL          bFound = FALSE;
            BOOL          bOk;
            list<wstring> disks;

            // create a HDEVINFO with all present devices
            hDeviceInfoSet = ::SetupDiGetClassDevs (&GUID_DEVINTERFACE_DISK, NULL, NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
            if (hDeviceInfoSet == INVALID_HANDLE_VALUE) {
                _ASSERT (FALSE);
                return disks;
            }

            // enumerate through all devices in the set
            ulMemberIndex = 0;
            while (TRUE) {
                // get device info
                SP_DEVINFO_DATA deviceInfoData;
                deviceInfoData.cbSize = sizeof (SP_DEVINFO_DATA);
                if (!::SetupDiEnumDeviceInfo (hDeviceInfoSet, ulMemberIndex, &deviceInfoData)) {
                    if (::GetLastError () == ERROR_NO_MORE_ITEMS) {
                        // ok, reached end of the device enumeration
                        break;
                    }
                    else {
                        // error
                        _ASSERT (FALSE);
                        ::SetupDiDestroyDeviceInfoList (hDeviceInfoSet);
                        return disks;
                    }
                }

                // get device interfaces
                SP_DEVICE_INTERFACE_DATA deviceInterfaceData;
                deviceInterfaceData.cbSize = sizeof (SP_DEVICE_INTERFACE_DATA);
                if (!::SetupDiEnumDeviceInterfaces (hDeviceInfoSet, NULL, &GUID_DEVINTERFACE_DISK, ulMemberIndex, &deviceInterfaceData)) {
                    if (::GetLastError () == ERROR_NO_MORE_ITEMS) {
                        // ok, reached end of the device enumeration
                        break;
                    }
                    else {
                        // error
                        _ASSERT (FALSE);
                        ::SetupDiDestroyDeviceInfoList (hDeviceInfoSet);
                        return disks;
                    }
                }

                // process the next device next time
                ulMemberIndex++;

                // get hardware id of the device
                ULONG ulPropertyRegDataType = 0;
                ULONG ulRequiredSize        = 0;
                ULONG ulBufferSize          = 0;
                BYTE* pbyBuffer             = NULL;
                if (!::SetupDiGetDeviceRegistryProperty (hDeviceInfoSet, &deviceInfoData, SPDRP_HARDWAREID, &ulPropertyRegDataType, NULL, 0, &ulRequiredSize)) {
                    if (::GetLastError () == ERROR_INSUFFICIENT_BUFFER) {
                        pbyBuffer    = (BYTE*)::malloc (ulRequiredSize);
                        ulBufferSize = ulRequiredSize;
                        if (!::SetupDiGetDeviceRegistryProperty (hDeviceInfoSet, &deviceInfoData, SPDRP_HARDWAREID, &ulPropertyRegDataType, pbyBuffer, ulBufferSize, &ulRequiredSize)) {
                            // getting the hardware id failed
                            _ASSERT (FALSE);
                            ::SetupDiDestroyDeviceInfoList (hDeviceInfoSet);
                            ::free (pbyBuffer);
                            return disks;
                        }
                    }
                    else {
                        // getting device registry property failed
                        _ASSERT (FALSE);
                        ::SetupDiDestroyDeviceInfoList (hDeviceInfoSet);
                        return disks;
                    }
                }
                else {
                    // getting hardware id of the device succeeded unexpectedly
                    _ASSERT (FALSE);
                    ::SetupDiDestroyDeviceInfoList (hDeviceInfoSet);
                    return disks;
                }

                // pbyBuffer is initialized now!
                LPCWSTR pszHardwareId = (LPCWSTR)pbyBuffer;

                // retrieve detailed information about the device
                // (especially the device path which is needed to create the device object)
                SP_DEVICE_INTERFACE_DETAIL_DATA* pDeviceInterfaceDetailData      = NULL;
                ULONG                            ulDeviceInterfaceDetailDataSize = 0;
                ulRequiredSize                                                   = 0;
                bOk                                                              = ::SetupDiGetDeviceInterfaceDetail (hDeviceInfoSet, &deviceInterfaceData, pDeviceInterfaceDetailData, ulDeviceInterfaceDetailDataSize, &ulRequiredSize, NULL);
                if (!bOk) {
                    ulErrorCode = ::GetLastError ();
                    if (ulErrorCode == ERROR_INSUFFICIENT_BUFFER) {
                        // insufficient buffer space
                        // => that's ok, allocate enough space and try again
                        pDeviceInterfaceDetailData         = (SP_DEVICE_INTERFACE_DETAIL_DATA*)::malloc (ulRequiredSize);
                        pDeviceInterfaceDetailData->cbSize = sizeof (SP_DEVICE_INTERFACE_DETAIL_DATA);
                        ulDeviceInterfaceDetailDataSize    = ulRequiredSize;
                        deviceInfoData.cbSize              = sizeof (SP_DEVINFO_DATA);
                        bOk                                = ::SetupDiGetDeviceInterfaceDetail (hDeviceInfoSet, &deviceInterfaceData, pDeviceInterfaceDetailData, ulDeviceInterfaceDetailDataSize, &ulRequiredSize, &deviceInfoData);
                        ulErrorCode                        = ::GetLastError ();
                    }

                    if (!bOk) {
                        // retrieving detailed information about the device failed
                        _ASSERT (FALSE);
                        ::free (pbyBuffer);
                        ::free (pDeviceInterfaceDetailData);
                        ::SetupDiDestroyDeviceInfoList (hDeviceInfoSet);
                        return disks;
                    }
                }
                else {
                    // retrieving detailed information about the device succeeded unexpectedly
                    _ASSERT (FALSE);
                    ::free (pbyBuffer);
                    ::SetupDiDestroyDeviceInfoList (hDeviceInfoSet);
                    return disks;
                }

                disks.push_back (pDeviceInterfaceDetailData->DevicePath);

                // free buffer for device interface details
                ::free (pDeviceInterfaceDetailData);

                // free buffer
                ::free (pbyBuffer);
            }

            // destroy device info list
            ::SetupDiDestroyDeviceInfoList (hDeviceInfoSet);

            return disks;
        }
#endif
        static String GetPhysNameForDriveNumber_ (unsigned int i)
        {
            // This format is NOT super well documented, and was mostly derived from reading the remarks section
            // of https://msdn.microsoft.com/en-us/library/windows/desktop/aa363216%28v=vs.85%29.aspx?f=255&MSPPError=-2147217396
            // (DeviceIoControl function)
            return Characters::Format (L"\\\\.\\PhysicalDrive%d", i);
        }
        struct PhysicalDriveInfo_ {
            String   fDeviceName;
            uint64_t fDeviceSizeInBytes;
        };
        static Collection<PhysicalDriveInfo_> GetPhysDrives_ ()
        {
            // This is NOT super well documented, and was mostly derived from reading the remarks section
            // of https://msdn.microsoft.com/en-us/library/windows/desktop/aa363216%28v=vs.85%29.aspx?f=255&MSPPError=-2147217396
            // (DeviceIoControl function) and random fiddling (so unreliable)
            //  --LGP 2015-09-29
            Collection<PhysicalDriveInfo_> x;
            for (int i = 0; i < 100; i++) {
                HANDLE hHandle = CreateFileW (GetPhysNameForDriveNumber_ (i).c_str (), GENERIC_READ /*|GENERIC_WRITE*/, FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
                if (hHandle == INVALID_HANDLE_VALUE) {
                    break;
                }
                GET_LENGTH_INFORMATION li{};
                DWORD                  dwBytesReturned{};
                BOOL                   bResult = ::DeviceIoControl (hHandle, IOCTL_DISK_GET_LENGTH_INFO, NULL, 0, &li, sizeof (li), &dwBytesReturned, NULL);
                ::CloseHandle (hHandle);
                PhysicalDriveInfo_ di{GetPhysNameForDriveNumber_ (i), static_cast<uint64_t> (li.Length.QuadPart)};
                x.Add (di);
            }
            return x;
        }
        static Set<DynamicDiskIDType> GetDisksForVolume_ (String volumeName)
        {
            wchar_t volPathsBuf[10 * 1024] = {0};
            DWORD   retLen                 = 0;
            DWORD   x                      = ::GetVolumePathNamesForVolumeNameW (volumeName.c_str (), volPathsBuf, static_cast<DWORD> (NEltsOf (volPathsBuf)), &retLen);
            if (x == 0 or retLen <= 1) {
                return Set<String> ();
            }
            Assert (1 <= Characters::CString::Length (volPathsBuf) and Characters::CString::Length (volPathsBuf) < NEltsOf (volPathsBuf));
            ;
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
                _tprintf(_T("Disk number: %d\n"), pDiskExtent->DiskNumber);
                _tprintf(_T("DBR start sector: %I64d\n"), pDiskExtent->StartingOffset.QuadPart / 512);
#endif
                result.Add (GetPhysNameForDriveNumber_ (pDiskExtent->DiskNumber));
            }
            return result;
        }
        Optional<String> GetDeviceNameForVolumneName_ (const String& volumeName)
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
                return Optional<String> ();
            }
            tmp = tmp.CircularSubString (4, -1);

            WCHAR DeviceName[MAX_PATH] = L"";
            if (QueryDosDeviceW (tmp.c_str (), DeviceName, ARRAYSIZE (DeviceName)) != 0) {
                return String{DeviceName};
            }
            return Optional<String> ();
        }

        Info capture_Windows_GetVolumeInfo_ ()
        {
#if qCaptureDiskDeviceInfoWindows_ && 0
            for (auto s : GetPhysicalDiskDeviceInfo_ ()) {
                DbgTrace (L"s=%s", s.c_str ());
            }
#endif
            // Could probably usefully optimize to not capture if no drives because we can only get this when running as
            // Admin, and for now, we capture little useful information at the drive level. But - we will eventually...
            Collection<PhysicalDriveInfo_> physDrives         = GetPhysDrives_ ();
            bool                           driveInfoAvailable = physDrives.size () >= 1;

#if qUseWMICollectionSupport_
            Time::DurationSecondsType timeOfPrevCollection = fLogicalDiskWMICollector_.GetTimeOfLastCollection ();
            if (fOptions_.fIOStatistics) {
                fLogicalDiskWMICollector_.Collect ();
            }
            Time::DurationSecondsType timeCollecting{fLogicalDiskWMICollector_.GetTimeOfLastCollection () - timeOfPrevCollection};
#endif
            Info result;

            for (PhysicalDriveInfo_ pd : physDrives) {
                DiskInfoType di{};
                di.fSizeInBytes = pd.fDeviceSizeInBytes;
                result.fDisks.Add (pd.fDeviceName, di);
            }

            for (IO::FileSystem::MountedFilesystemType mfinfo : IO::FileSystem::GetMountedFilesystems ()) {
                MountedFilesystemInfoType v;
                v.fFileSystemType = mfinfo.fFileSystemType;
                v.fVolumeID       = mfinfo.fVolumeID;
                if (driveInfoAvailable) {
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
                        default:; /*ignored - if it doesnt map or error - nevermind */
                    }
                }

                {
                    ULARGE_INTEGER freeBytesAvailable{};
                    ULARGE_INTEGER totalNumberOfBytes{};
                    ULARGE_INTEGER totalNumberOfFreeBytes{};
                    DWORD          xxx      = ::GetDiskFreeSpaceEx (mfinfo.fMountedOn.AsSDKString ().c_str (), &freeBytesAvailable, &totalNumberOfBytes, &totalNumberOfFreeBytes);
                    v.fSizeInBytes          = totalNumberOfBytes.QuadPart;
                    v.fUsedSizeInBytes      = *v.fSizeInBytes - freeBytesAvailable.QuadPart;
                    v.fAvailableSizeInBytes = *v.fSizeInBytes - *v.fUsedSizeInBytes;
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
                        combinedStats.fBytesTransfered.AccumulateIf (writeStats.fBytesTransfered);
                        combinedStats.fTotalTransfers.AccumulateIf (writeStats.fTotalTransfers);
                        combinedStats.fQLength.AccumulateIf (writeStats.fQLength);
                        combinedStats.fInUsePercent.AccumulateIf (writeStats.fInUsePercent);
                        if (readStats.fInUsePercent and writeStats.fInUsePercent) {
                            combinedStats.fInUsePercent /= 2;
                        }

                        if (kUsePctIdleIimeForAveQLen_) {
                            if (auto o = fLogicalDiskWMICollector_.PeekCurrentValue (wmiInstanceName, kPctIdleTime_)) {
                                double aveCombinedQLen = safePctInUse2QL_ (100.0 - *o);
                                if (readStats.fQLength and writeStats.fQLength and *combinedStats.fQLength > 0) {
                                    // for some reason, the pct-idle-time #s combined are OK, but #s for aveQLen and disk read PCT/Write PCT wrong.
                                    // asusme ratio rate, and scale
                                    double correction = aveCombinedQLen / *combinedStats.fQLength;
                                    readStats.fQLength *= correction;
                                    writeStats.fQLength *= correction;
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
            Set<DynamicDiskIDType> disksForFS = i.fValue.fOnPhysicalDrive.Value ();
            if (disksForFS.size () > 0) {
                WeightingStat2UseType weightForFS = i.fValue.fCombinedIOStats.Value ().fBytesTransfered.Value ();
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
                Set<DynamicDiskIDType>    disksForFS = mfi.fOnPhysicalDrive.Value ();
                if (disksForFS.size () > 0) {
                    WeightingStat2UseType weightForFS = i.fValue.fCombinedIOStats.Value ().fBytesTransfered.Value ();
                    weightForFS /= disksForFS.size ();
                    IOStatsType cumStats          = mfi.fCombinedIOStats.Value ();
                    bool        computeInuse      = cumStats.fInUsePercent.IsMissing ();
                    bool        computeQLen       = cumStats.fQLength.IsMissing ();
                    bool        computeTotalXFers = cumStats.fTotalTransfers.IsMissing ();

                    for (DynamicDiskIDType di : disksForFS) {
                        IOStatsType diskIOStats = disks.LookupValue (di).fCombinedIOStats.Value ();
                        if (weightForFS > 0) {
                            double scaleFactor = weightForFS / totalWeights.LookupValue (di);
                            //Assert (0.0 <= scaleFactor and scaleFactor <= 1.0);
                            scaleFactor = Math::PinInRange (scaleFactor, 0.0, 1.0);
                            if (computeInuse and diskIOStats.fInUsePercent) {
                                cumStats.fInUsePercent += *diskIOStats.fInUsePercent * scaleFactor;
                            }
                            if (computeQLen and diskIOStats.fInUsePercent) {
                                cumStats.fQLength += *diskIOStats.fQLength * scaleFactor;
                            }
                            if (computeTotalXFers and diskIOStats.fTotalTransfers) {
                                cumStats.fTotalTransfers += *diskIOStats.fTotalTransfers * scaleFactor;
                            }
                        }
                    }
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                    if (computeInuse) {
                        DbgTrace (L"Adjusted fInUsePCT for filesystem '%s' is %f", i.fKey.c_str (), cumStats.fInUsePercent.Value ());
                    }
                    if (computeQLen) {
                        DbgTrace (L"Adjusted fQLength for filesystem '%s' is %f", i.fKey.c_str (), cumStats.fQLength.Value ());
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
        mapper.AddCommonType<Optional<double>> ();
        mapper.AddCommonType<Optional<uint64_t>> ();
        mapper.AddCommonType<Optional<String>> ();
        mapper.Add (mapper.MakeCommonSerializer_NamedEnumerations<BlockDeviceKind> ());
        mapper.AddCommonType<Optional<BlockDeviceKind>> ();
        DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Winvalid-offsetof\""); // Really probably an issue, but not to debug here -- LGP 2014-01-04
        mapper.AddClass<IOStatsType> (initializer_list<StructFieldInfo>{
            {L"Bytes", Stroika_Foundation_DataExchange_StructFieldMetaInfo (IOStatsType, fBytesTransfered), StructFieldInfo::NullFieldHandling::eOmit},
            {L"Q-Length", Stroika_Foundation_DataExchange_StructFieldMetaInfo (IOStatsType, fQLength), StructFieldInfo::NullFieldHandling::eOmit},
            {L"In-Use-%", Stroika_Foundation_DataExchange_StructFieldMetaInfo (IOStatsType, fInUsePercent), StructFieldInfo::NullFieldHandling::eOmit},
            {L"Total-Transfers", Stroika_Foundation_DataExchange_StructFieldMetaInfo (IOStatsType, fTotalTransfers), StructFieldInfo::NullFieldHandling::eOmit},
        });
        mapper.AddCommonType<Optional<IOStatsType>> ();
        mapper.AddClass<DiskInfoType> (initializer_list<StructFieldInfo>{
            {L"Persistence-Volume-ID", Stroika_Foundation_DataExchange_StructFieldMetaInfo (DiskInfoType, fPersistenceVolumeID), StructFieldInfo::NullFieldHandling::eOmit},
            {L"Device-Kind", Stroika_Foundation_DataExchange_StructFieldMetaInfo (DiskInfoType, fDeviceKind), StructFieldInfo::NullFieldHandling::eOmit},
            {L"Size", Stroika_Foundation_DataExchange_StructFieldMetaInfo (DiskInfoType, fSizeInBytes), StructFieldInfo::NullFieldHandling::eOmit},
            {L"Read-IO-Stats", Stroika_Foundation_DataExchange_StructFieldMetaInfo (DiskInfoType, fReadIOStats), StructFieldInfo::NullFieldHandling::eOmit},
            {L"Write-IO-Stats", Stroika_Foundation_DataExchange_StructFieldMetaInfo (DiskInfoType, fWriteIOStats), StructFieldInfo::NullFieldHandling::eOmit},
            {L"Combined-IO-Stats", Stroika_Foundation_DataExchange_StructFieldMetaInfo (DiskInfoType, fCombinedIOStats), StructFieldInfo::NullFieldHandling::eOmit},
        });
        mapper.AddCommonType<Set<String>> ();
        mapper.AddCommonType<Optional<Set<String>>> ();
        mapper.AddClass<MountedFilesystemInfoType> (initializer_list<StructFieldInfo>{
            {L"Device-Kind", Stroika_Foundation_DataExchange_StructFieldMetaInfo (MountedFilesystemInfoType, fDeviceKind), StructFieldInfo::NullFieldHandling::eOmit},
            {L"Filesystem-Type", Stroika_Foundation_DataExchange_StructFieldMetaInfo (MountedFilesystemInfoType, fFileSystemType), StructFieldInfo::NullFieldHandling::eOmit},
            {L"Device-Name", Stroika_Foundation_DataExchange_StructFieldMetaInfo (MountedFilesystemInfoType, fDeviceOrVolumeName), StructFieldInfo::NullFieldHandling::eOmit},
            {L"On-Physical-Drives", Stroika_Foundation_DataExchange_StructFieldMetaInfo (MountedFilesystemInfoType, fOnPhysicalDrive), StructFieldInfo::NullFieldHandling::eOmit},
            {L"Volume-ID", Stroika_Foundation_DataExchange_StructFieldMetaInfo (MountedFilesystemInfoType, fVolumeID), StructFieldInfo::NullFieldHandling::eOmit},
            {L"Total-Size", Stroika_Foundation_DataExchange_StructFieldMetaInfo (MountedFilesystemInfoType, fSizeInBytes), StructFieldInfo::NullFieldHandling::eOmit},
            {L"Available-Size", Stroika_Foundation_DataExchange_StructFieldMetaInfo (MountedFilesystemInfoType, fAvailableSizeInBytes), StructFieldInfo::NullFieldHandling::eOmit},
            {L"Used-Size", Stroika_Foundation_DataExchange_StructFieldMetaInfo (MountedFilesystemInfoType, fUsedSizeInBytes), StructFieldInfo::NullFieldHandling::eOmit},
            {L"Read-IO-Stats", Stroika_Foundation_DataExchange_StructFieldMetaInfo (MountedFilesystemInfoType, fReadIOStats), StructFieldInfo::NullFieldHandling::eOmit},
            {L"Write-IO-Stats", Stroika_Foundation_DataExchange_StructFieldMetaInfo (MountedFilesystemInfoType, fWriteIOStats), StructFieldInfo::NullFieldHandling::eOmit},
            {L"Combined-IO-Stats", Stroika_Foundation_DataExchange_StructFieldMetaInfo (MountedFilesystemInfoType, fCombinedIOStats), StructFieldInfo::NullFieldHandling::eOmit},
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
