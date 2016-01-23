/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#include    "../../StroikaPreComp.h"

#if     qPlatform_POSIX
#include    <sys/statvfs.h>
#include    <sys/stat.h>
#include    <sys/types.h>
#include    <unistd.h>
#if     qPlatform_AIX
#include    <sys/sysmacros.h>
#include    <libperfstat.h>
#endif
#elif   qPlatform_Windows
#include    <Windows.h>
#include    <winioctl.h>
#endif

#include    "../../../Foundation/Characters/CString/Utilities.h"
#include    "../../../Foundation/Characters/FloatConversion.h"
#include    "../../../Foundation/Characters/String_Constant.h"
#include    "../../../Foundation/Characters/String2Int.h"
#include    "../../../Foundation/Characters/CString/Utilities.h"
#include    "../../../Foundation/Containers/Mapping.h"
#include    "../../../Foundation/Containers/Set.h"
#include    "../../../Foundation/Containers/Sequence.h"
#include    "../../../Foundation/Configuration/SystemConfiguration.h"
#include    "../../../Foundation/DataExchange/CharacterDelimitedLines/Reader.h"
#include    "../../../Foundation/Debug/Assertions.h"
#include    "../../../Foundation/Execution/Sleep.h"
#include    "../../../Foundation/Execution/StringException.h"
#include    "../../../Foundation/Execution/ProcessRunner.h"
#include    "../../../Foundation/IO/FileSystem/FileInputStream.h"
#include    "../../../Foundation/IO/FileSystem/FileSystem.h"
#include    "../../../Foundation/Streams/MemoryStream.h"
#include    "../../../Foundation/Streams/TextReader.h"

#include    "Filesystem.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Containers;
using   namespace   Stroika::Foundation::DataExchange;
using   namespace   Stroika::Foundation::Execution;
using   namespace   Stroika::Foundation::Memory;

using   namespace   Stroika::Frameworks;
using   namespace   Stroika::Frameworks::SystemPerformance;
using   namespace   Stroika::Frameworks::SystemPerformance::Instruments::Filesystem;

using   Characters::String_Constant;
using   Characters::String2Int;
using   Time::DurationSecondsType;
using   IO::FileSystem::FileInputStream;
using   Streams::TextReader;





// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1






/*
 *  I thought this might be useful. maybe at some point? But so far it doesn't appear super useful
 *  or needed.
 *  But we can use this to find out the disk kind for physical devices
 *  --LGP 2015-09-30
 */
#ifndef qCaptureDiskDeviceInfoWindows_
#define qCaptureDiskDeviceInfoWindows_      0
#endif

#if     qCaptureDiskDeviceInfoWindows_
#include    <setupapi.h>
#include    <devguid.h>
#include    <regstr.h>
#pragma comment (lib, "Setupapi.lib")
DEFINE_GUID(GUID_DEVINTERFACE_DISK,                   0x53f56307L, 0xb6bf, 0x11d0, 0x94, 0xf2, 0x00, 0xa0, 0xc9, 0x1e, 0xfb, 0x8b);
//#include <Ntddstor.h>
#endif





/*
 ********************************************************************************
 ********* Instruments::Filesystem::IOStatsType::EstimatedPercentInUse **********
 ********************************************************************************
 */
Optional<double>    IOStatsType::EstimatedPercentInUse () const
{
    if (fInUsePercent) {
        return fInUsePercent;
    }
    // %InUse = QL / (1 + QL).
    if (fQLength) {
        double QL = *fQLength;
        Require (0 <= QL);
        double  pct     { 100.0 * (QL / (1 + QL)) };
        Require (0 <= pct and pct <= 100.0);
        return pct;
    }
    return Optional<double> ();
}







// for io stats
#ifndef qUseWMICollectionSupport_
#define qUseWMICollectionSupport_       qPlatform_Windows
#endif


#if     qUseWMICollectionSupport_
#include    "../Support/WMICollector.h"

using   SystemPerformance::Support::WMICollector;
#endif






/*
 ********************************************************************************
 **************************** Configuration::DefaultNames ***********************
 ********************************************************************************
 */
namespace   Stroika {
    namespace   Foundation {
        namespace   Configuration {
            template    <>
            const   EnumNames<Frameworks::SystemPerformance::Instruments::Filesystem::BlockDeviceKind> DefaultNames<Frameworks::SystemPerformance::Instruments::Filesystem::BlockDeviceKind>::k
#if     qCompilerAndStdLib_const_Array_Init_wo_UserDefined_Buggy
                =
#endif
            {
                EnumNames<Frameworks::SystemPerformance::Instruments::Filesystem::BlockDeviceKind>::BasicArrayInitializer {
                    {
                        { Frameworks::SystemPerformance::Instruments::Filesystem::BlockDeviceKind::eRemovableDisk, L"Removable-Disk" },
                        { Frameworks::SystemPerformance::Instruments::Filesystem::BlockDeviceKind::eLocalDisk, L"Local-Disk" },
                        { Frameworks::SystemPerformance::Instruments::Filesystem::BlockDeviceKind::eNetworkDrive, L"Network-Drive" },
                        { Frameworks::SystemPerformance::Instruments::Filesystem::BlockDeviceKind::eTemporaryFiles, L"Temporary-Files" },
                        { Frameworks::SystemPerformance::Instruments::Filesystem::BlockDeviceKind::eReadOnlyEjectable, L"Read-Only-Ejectable" },
                        { Frameworks::SystemPerformance::Instruments::Filesystem::BlockDeviceKind::eSystemInformation, L"System-Information" },
                    }
                }
            };
        }
    }
}






namespace {
#if     qUseWMICollectionSupport_
    const   String_Constant     kDiskReadBytesPerSec_   { L"Disk Read Bytes/sec" };
    const   String_Constant     kDiskWriteBytesPerSec_  { L"Disk Write Bytes/sec" };
    const   String_Constant     kDiskReadsPerSec_       { L"Disk Reads/sec" };
    const   String_Constant     kDiskWritesPerSec_      { L"Disk Writes/sec" };
    const   String_Constant     kPctDiskReadTime_       { L"% Disk Read Time" };
    const   String_Constant     kPctDiskWriteTime_      { L"% Disk Write Time" };
    const   String_Constant     kAveDiskReadQLen_       { L"Avg. Disk Read Queue Length" };
    const   String_Constant     kAveDiskWriteQLen_      { L"Avg. Disk Write Queue Length" };
    const   String_Constant     kPctIdleTime_           { L"% Idle Time" };

    constexpr   bool    kUseDiskPercentReadTime_ElseAveQLen_ToComputeQLen_  { false };
    /*
     *  No logical reason todo this. Its probably masking a real bug. But empirically it produces
     *  values closer to those reported by Windows Task Mgr.
     *      -- LGP 2015-07-20
     */
    constexpr   bool    kUsePctIdleIimeForAveQLen_                          { true };
#endif
}









namespace {
    struct  CapturerWithContext_COMMON_ {
    protected:
        Options                 fOptions_;
        DurationSecondsType     fMinimumAveragingInterval_;
        DurationSecondsType     fPostponeCaptureUntil_ { 0 };
        DurationSecondsType     fLastCapturedAt {};

    public:
        DurationSecondsType    GetLastCaptureAt () const { return fLastCapturedAt; }

    protected:
        CapturerWithContext_COMMON_ (const Options& options)
            : fOptions_ (options)
            , fMinimumAveragingInterval_ (options.fMinimumAveragingInterval)
        {
        }

    protected:
        void    _NoteCompletedCapture ()
        {
            auto now = Time::GetTickCount ();
            fPostponeCaptureUntil_ = now + fMinimumAveragingInterval_;
            fLastCapturedAt = now;
        }
    };
}






#if     qPlatform_POSIX
namespace {
    void    ApplyDiskTypes_ (Mapping<MountedFilesystemNameType, MountedFilesystemInfoType>* volumes)
    {
        RequireNotNull (volumes);
        static  const   Set<String> kRealDiskFS {
            String_Constant { L"ext2" },
            String_Constant { L"ext3" },
            String_Constant { L"ext4" },
            String_Constant { L"xfs" },
            String_Constant { L"jfs2" },
        };
        static  const   Set<String> kSysFSList_ {
            String_Constant { L"autofs" },
            String_Constant { L"binfmt_misc" },
            String_Constant { L"cgroup" },
            String_Constant { L"configfs" },
            String_Constant { L"debugfs" },
            String_Constant { L"devpts" },
            String_Constant { L"devtmpfs" },
            String_Constant { L"fusectl" },
            String_Constant { L"fuse.gvfsd-fuse" },
            String_Constant { L"hugetlbfs" },
            String_Constant { L"mqueue" },
            String_Constant { L"nfsd" },                // not nfs filesystem, but special config fs - http://linux.die.net/man/7/nfsd
            String_Constant { L"pstore" },
            String_Constant { L"proc" },
            String_Constant { L"rpc_pipefs" },
            String_Constant { L"securityfs" },
            String_Constant { L"selinuxfs" },
            String_Constant { L"sunrpc" },
            String_Constant { L"sysfs" },
            String_Constant { L"usbfs" },
        };
        static  const   Set<String> kNetworkFS_ {
            String_Constant { L"nfs" },
            String_Constant { L"nfs3" },
            String_Constant { L"vboxsf" },
        };
        for (auto i : *volumes) {
            // @todo - NOTE - this is NOT a reliable way to tell, but hopefully good enough for starters
            MountedFilesystemInfoType vi = i.fValue;
            if (vi.fFileSystemType) {
                String  fstype = *vi.fFileSystemType;
                bool    changed { false };
                if (kRealDiskFS.Contains (fstype)) {
                    vi.fDeviceKind = BlockDeviceKind::eLocalDisk;
                    changed = true;
                }
                else if (kNetworkFS_.Contains (fstype)) {
                    vi.fDeviceKind = BlockDeviceKind::eNetworkDrive;
                    changed = true;
                }
                else if (fstype == L"tmpfs") {
                    vi.fDeviceKind = BlockDeviceKind::eTemporaryFiles;
                    changed = true;
                }
                else if (fstype == L"iso9660") {
                    vi.fDeviceKind = BlockDeviceKind::eReadOnlyEjectable;
                    changed = true;
                }
                else if (kSysFSList_.Contains (fstype)) {
                    vi.fDeviceKind = BlockDeviceKind::eSystemInformation;
                    changed = true;
                }
                if (changed) {
                    volumes->Add (i.fKey, vi);
                }
            }
        }
    }
}
#endif








#if     qPlatform_AIX
namespace {
    struct  CapturerWithContext_AIX_ : CapturerWithContext_COMMON_ {
    private:
        struct DiskPerfStats_ {
            u_longlong_t  fDiskBlockSize;
            u_longlong_t  fTotalTransfers;
            u_longlong_t  fBlocksRead;
            u_longlong_t  fBlocksWritten;
            u_longlong_t  q_sampled;                // appears to be (subject to verification) weighted q ave len (so can divide by time elapsed to get qlen ave)
            u_longlong_t  time;                     // appears ave of percent time in use (accumulated) so divide by elapsed time and diff to get %time)
        };
        Mapping<DynamicDiskIDType, DiskPerfStats_>         fContextDiskName2DiskPerfStats_;
    private:
        struct FileSystemPerfStats_ {
            uint64_t  fReadKB;
            uint64_t  fWriteKB;
        };
        Mapping<MountedFilesystemNameType, FileSystemPerfStats_>   fContextDiskName2FileSystemPerfStats_;
    private:
        using   Disk2MountPointsMapType_ = Mapping<DynamicDiskIDType, Set<MountedFilesystemNameType>>;

    public:
        CapturerWithContext_AIX_ (Options options)
            : CapturerWithContext_COMMON_ (options)
        {
            // for side-effect of setting fContextDiskName2DiskPerfStats_
            try {
                capture_ ();
            }
            catch (...) {
                DbgTrace ("bad sign that first pre-catpure failed.");   // Dont propagate in case just listing collectors
            }
        }
        CapturerWithContext_AIX_ (const CapturerWithContext_AIX_&) = default;   // copy by value fine - no need to re-wait...
        Info    capture ()
        {
            Execution::SleepUntil (fPostponeCaptureUntil_);
            return capture_ ();
        }
        Info    capture_ ()
        {
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
            Debug::TraceContextBumper ctx ("Instruments::Filesystem...CapturerWithContext_AIX_::capture_");
#endif
            Info   results;
            results.fMountedFilesystems = ReadVolumesAndUsageFromProcMountsAndstatvfs_ ();
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

            // populate logical to physical disk map (and phys disk list)
            Disk2MountPointsMapType_        disk2MountPointMap = ReadDisk2MountPointsMap_ ();
            for (KeyValuePair<String, Set<String>> diskAndVols : disk2MountPointMap) {
                // must reverse teh mapping, so each
                for (String fsi : diskAndVols.fValue) {
                    if (Optional<MountedFilesystemInfoType> m2Update = results.fMountedFilesystems.Lookup (fsi)) {
                        MountedFilesystemInfoType   m = *m2Update;
                        Set<DynamicDiskIDType>  disks = m.fOnPhysicalDrive.Value ();
                        disks.Add (diskAndVols.fKey);
                        m.fOnPhysicalDrive = disks;
                        results.fMountedFilesystems.Add (fsi, m);
                    }
                    else {
                        DbgTrace (L"Shouldn't happen: missing filesystem '%s' from our returned list", fsi.c_str ());
                    }
                }
            }

            if (fOptions_.fIOStatistics) {
                ReadAndApplyProcFS_diskstats_ (disk2MountPointMap, &results.fDisks);
                ReadAndApply_iostat_dashF_stats_ (&results.fMountedFilesystems);
            }
            _NoteCompletedCapture ();
            return results;
        }
    private:
        Mapping<MountedFilesystemNameType, MountedFilesystemInfoType>    ReadVolumesAndUsageFromProcMountsAndstatvfs_ ()
        {
            Mapping<MountedFilesystemNameType, MountedFilesystemInfoType>    result;
            for (MountInfo_ mi : ReadMountInfo_ ()) {
                MountedFilesystemInfoType  vi;
                if (not mi.fDeviceName.empty () and mi.fDeviceName != L"none") {    // special name none often used when there is no name
                    vi.fDeviceOrVolumeName = mi.fDeviceName;
                }
                vi.fFileSystemType = mi.fFilesystemFormat;
                UpdateVolumneInfo_statvfs (mi.fMountedOn, &vi);
                result.Add (mi.fMountedOn, vi);
            }
            return result;
        }
    private:
        void    UpdateVolumneInfo_statvfs (const String& mountedOnName, MountedFilesystemInfoType* v)
        {
            RequireNotNull (v);
            if (v->fFileSystemType == L"procfs") {
                // on AIX, this returns a bogus disk size. Not sure how to tell otherwise to ignore
                return;
            }
            struct  statvfs64 sbuf {};
            if (::statvfs64 (mountedOnName.AsNarrowSDKString ().c_str (), &sbuf) == 0) {
                uint64_t    diskSize = sbuf.f_bsize * sbuf.f_blocks;
                v->fSizeInBytes = diskSize;
                v->fAvailableSizeInBytes = sbuf.f_bsize * sbuf.f_bavail;
                v->fUsedSizeInBytes = diskSize - sbuf.f_bsize * sbuf.f_bfree;
            }
            else {
                DbgTrace (L"statvfs (%s) return error: errno=%d", mountedOnName.c_str (), errno);
            }
        }
    private:
        void    ReadAndApplyProcFS_diskstats_ (const Disk2MountPointsMapType_& disk2MountPointMap, Mapping<DynamicDiskIDType, DiskInfoType>* disks)
        {
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
            Debug::TraceContextBumper ctx ("Instruments::Filesystem ReadAndApplyProcFS_diskstats_");
#endif
            RequireNotNull (disks);
            for (KeyValuePair<String, Set<String>> diskAndVols : disk2MountPointMap) {
                perfstat_id_t   name;
                Characters::CString::Copy (name.name, NEltsOf (name.name), diskAndVols.fKey.AsNarrowSDKString ().c_str ());
                perfstat_disk_t ds {};
                /*
                 *  From /usr/include/libperfstat.h
                 *      perfstat_disk_t:
                 *          char name[IDENTIFIER_LENGTH];   --  name of the disk
                 *          u_longlong_t bsize;             --  disk block size (in bytes)
                 *          u_longlong_t xfers;             --  number of transfers to/from disk
                 *          u_longlong_t rblks;             --  number of blocks read from disk
                 *          u_longlong_t wblks;             --  number of blocks written to disk
                 *          u_longlong_t qdepth;            --  instantaneous "service" queue depth (number of requests sent to disk and not completed yet)
                 *          u_longlong_t time;              --  amount of time disk is active
                 *          u_longlong_t wq_time;           --  accumulated wait queueing time
                 *          u_longlong_t wq_sampled;        --  accumulated sampled dk_wq_depth
                 *          u_longlong_t q_sampled;         -- accumulated sampled dk_q_depth
                 */
                int nDisksResult = ::perfstat_disk (&name, &ds, sizeof (ds), 1);
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
                DbgTrace ("perfstat_disk returned %d: name=%s; bsize=%lld; xfers=%lld; rblks=%lld; wblks=%lld; qdepth: %lld; time: %lld; q_sampled: %lld; wq_sampled: %lld; wq_time: %lld", disks, ds.name, ds.bsize, ds.xfers, ds.rblks, ds.wblks, ds.qdepth, ds.time, ds.q_sampled, ds.wq_sampled, ds.wq_time);
#endif
                if (nDisksResult == 1) {
                    // @todo see below Docs and examples quite unclear. Maybe use wq_sampled
                    DiskPerfStats_              ps              { ds.bsize, ds.xfers, ds.rblks, ds.wblks, ds.q_sampled, ds.time };
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
                    DbgTrace ("ps = {fDiskBlockSize: %lld; fTotalTransfers: %lld; fBlocksRead: %lld; fBlocksWritten: %lld; wq_sampled: %lld;}", ps.fDiskBlockSize, ps.fTotalTransfers, ps.fBlocksRead, ps.fBlocksWritten, ps.q_sampled);
#endif
                    Optional<DiskPerfStats_>    prevPerfStats   =   fContextDiskName2DiskPerfStats_.Lookup (diskAndVols.fKey);
                    if (prevPerfStats) {
                        IOStatsType readStats;
                        readStats.fBytesTransfered = (ps.fBlocksRead - prevPerfStats->fBlocksRead) *  ps.fDiskBlockSize;
                        IOStatsType writeStats;
                        writeStats.fBytesTransfered = (ps.fBlocksWritten - prevPerfStats->fBlocksWritten) *  ps.fDiskBlockSize;
                        IOStatsType combinedStats;
                        combinedStats.fBytesTransfered = *readStats.fBytesTransfered + *writeStats.fBytesTransfered;
                        combinedStats.fTotalTransfers = (ps.fTotalTransfers - prevPerfStats->fTotalTransfers);

#if     USE_NOISY_TRACE_IN_THIS_MODULE_
                        DbgTrace ("in compare for disk %s: time=%lld; prevPerfStats->time=%lld; (delta-time %lld); ps.q_sampled =%lld; prevPerfStats->q_sampled: %lld; (delta-q_sampled %lld) }", ds.name, ps.time, prevPerfStats->time, (ps.time - prevPerfStats->time), ps.q_sampled, prevPerfStats->q_sampled, (ps.q_sampled - prevPerfStats->q_sampled));
#endif
                        {
                            // Docs and examples quite unclear. Maybe use wq_sampled, or q_sampled. And unclear what to divide by.
                            double elapsed = Time::GetTickCount () - GetLastCaptureAt ();
                            static  uint32_t    kNumberLogicalCores_ = GetSystemConfiguration_CPU ().GetNumberOfLogicalCores ();
                            double a = (100.0 * (double)elapsed * (double)kNumberLogicalCores_);
                            combinedStats.fQLength = ((ps.q_sampled - prevPerfStats->q_sampled) / a);
                            combinedStats.fInUsePercent = ((ps.time - prevPerfStats->time) / elapsed);
                            //DbgTrace ("maybeaveq = %f", (double) (ps.wq_sampled - prevPerfStats->wq_sampled) / a);
                            //DbgTrace ("a=%f", a);
                            //DbgTrace ("a=%f", a);
                            //DbgTrace ("***ps.wq_time (%lld) - prevPerfStats->wq_time(%lld) = %lld  AND ps.wq_sampled (%lld) - prevPerfStats->wq_sampled(%lld) = %lld",
                            //          ps.wq_time, prevPerfStats->wq_time, ps.wq_time - prevPerfStats->wq_time,
                            //          ps.wq_sampled, prevPerfStats->wq_sampled, ps.wq_sampled - prevPerfStats->wq_sampled
                            //         );
                            //combinedStats.fQLength = (static_cast<double> (ps.wq_sampled - prevPerfStats->wq_sampled) / static_cast<double> (ps.wq_time - prevPerfStats->wq_time));
                        }
                        DiskInfoType di = disks->LookupValue (diskAndVols.fKey);
                        di.fCombinedIOStats = combinedStats;
                        disks->Add (diskAndVols.fKey, di);
                    }
                    fContextDiskName2DiskPerfStats_.Add (diskAndVols.fKey, ps);
                }
            }
        }

    private:
        void    ReadAndApply_iostat_dashF_stats_ (Mapping<MountedFilesystemNameType, MountedFilesystemInfoType>* volumes)
        {
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
            Debug::TraceContextBumper ctx ("Instruments::Filesystem ReadAndApply_iostat_dashF_stats_");
#endif
            RequireNotNull (volumes);
            using   Execution::ProcessRunner;

            // NYI
            {
                ProcessRunner   pr (String_Constant { L"/usr/bin/iostat -F" });
                Streams::MemoryStream<Byte>   useStdOut;
                pr.SetStdOut (useStdOut);
                pr.Run ();
                Streams::TextReader   stdOut  =   Streams::TextReader (useStdOut);
                bool    parsingHeader { true };
                for (String i = stdOut.ReadLine (); not i.empty (); i = stdOut.ReadLine ()) {
                    /*
                     *  $ /usr/bin/iostat -F
                     *      System configuration: lcpu=4 ent=0.20 fs=12
                     *
                     *      FS Name:                 % tm_act     Kbps      tps    Kb_read   Kb_wrtn
                     *      /                        -        0.0       0.1   591133       7314
                     *      /usr                     -       44.6      11.7 957992214          0
                     *      /var                     -        0.0       0.1    28987      33071
                     *      /tmp                     -        7.7       3.5 25617000   139441123
                     *      /home                    -        0.0       0.0    26741      25218
                     *      /admin                   -        0.0       0.0        0          0
                     *      /proc                    -        0.0       0.0    37956        155
                     *      /opt                     -        2.4       0.5 50484345      92038
                     *      /var/adm/ras/livedum     -        0.0       0.0        0          0
                     *      /resgrp156               -       33.1       8.1 650541100   61233948
                     *      /toolbox                 -        0.0       0.0        0          0
                     *      /mozilla                 -        0.0       0.0        0          0
                     */
                    Sequence<String>    tokens = i.Tokenize ();
                    if (parsingHeader) {
                        if (tokens.size () > 3 and tokens[0] == L"FS" and tokens[1] == L"Name:") {
                            parsingHeader = false;
                        }
                    }
                    else if (tokens.size () >= 6) {
                        String  fs = tokens[0].Trim ();
                        uint64_t    readKB = Characters::String2Int<uint64_t> (tokens[4]);
                        uint64_t    writeKB = Characters::String2Int<uint64_t> (tokens[5]);

                        Optional<FileSystemPerfStats_>    prevPerfStats   =   fContextDiskName2FileSystemPerfStats_.Lookup (fs);
                        if (prevPerfStats) {
                            if (Optional<MountedFilesystemInfoType> mfsi = volumes->Lookup (fs)) {
                                MountedFilesystemInfoType   m = *mfsi;

                                // wrong - must diff
                                IOStatsType readStats = m.fReadIOStats.Value ();
                                readStats.fBytesTransfered = (readKB - prevPerfStats->fReadKB) * 1024;
                                m.fReadIOStats = readStats;
                                IOStatsType writeStats = m.fWriteIOStats.Value ();
                                writeStats.fBytesTransfered = (writeKB - prevPerfStats->fWriteKB) * 1024;
                                m.fWriteIOStats = writeStats;
                                IOStatsType totalStats = m.fCombinedIOStats.Value ();
                                totalStats.fBytesTransfered = readStats.fBytesTransfered + writeStats.fBytesTransfered;
                                m.fCombinedIOStats = totalStats;

                                volumes->Add (fs, m);
                            }
                            else {
                                DbgTrace (L"Dropping unrecognized iostat result line on the floor (fs=%s)", fs.c_str ());
                            }
                        }
                        fContextDiskName2FileSystemPerfStats_.Add (fs, FileSystemPerfStats_ {readKB, writeKB});
                    }
                    else {
                        DbgTrace (L"Dropping unrecognized iostat result line on the floor (line=%s)", i.c_str ());
                    }
                }
            }
        }

    private:
        Disk2MountPointsMapType_    ReadDisk2MountPointsMap_ ()
        {
            Debug::TraceContextBumper ctx ("Instruments::Filesystem ReadDisk2MountPointsMap_");
            /*
             *  @todo See if there is a way to find this info on AIX without ProcessRunner...
             */
            using   Execution::ProcessRunner;

            Set<String> diskNames;
            {
                ProcessRunner   pr (String_Constant { L"/usr/sbin/lspv" });
                Streams::MemoryStream<Byte>   useStdOut;
                pr.SetStdOut (useStdOut);
                pr.Run ();
                Streams::TextReader   stdOut  =   Streams::TextReader (useStdOut);
                for (String i = stdOut.ReadLine (); not i.empty (); i = stdOut.ReadLine ()) {
                    /*
                     *  $ /usr/sbin/lspv
                     *      hdisk0          000193611d18c9a8                    rootvg          active
                     *      hdisk1          000193611d231719                    rootvg          active
                     *      hdisk2          000193611d2317d6                    rootvg          active
                     */
                    Sequence<String>    tokens = i.Tokenize ();
                    if (tokens.size () >= 4) {
                        diskNames.Add (tokens[0]);
                    }
                    else {
                        DbgTrace (L"Dropping unrecognized lspv result line on the floor (line=%s)", i.c_str ());
                    }
                }
            }
            Disk2MountPointsMapType_        result;
            for (String diskName : diskNames) {
                ProcessRunner   pr (Characters::Format (L"/usr/sbin/lspv -l %s" , diskName.c_str ()));
                Streams::MemoryStream<Byte>   useStdOut;
                pr.SetStdOut (useStdOut);
                pr.Run ();
                Streams::TextReader   stdOut  =   Streams::TextReader (useStdOut);
                uint skipCount = 2;
                Set<String> mountPoints;
                for (String i = stdOut.ReadLine (); not i.empty (); i = stdOut.ReadLine ()) {
                    if (skipCount > 0) {
                        skipCount--;
                        continue;
                    }
                    /*
                     *      $ /usr/sbin/lspv  -l hdisk2
                     *      hdisk2:
                     *      LV NAME               LPs     PPs     DISTRIBUTION          MOUNT POINT
                     *      hd10opt               16      16      00..00..00..16..00    /opt
                     *      paging02              60      60      00..00..00..04..56    N/A
                     *      paging01              30      30      00..00..00..30..00    N/A
                     *      paging00              24      24      00..00..00..24..00    N/A
                     *      resgrp156lv           564     564     160..160..159..85..00 /resgrp156
                     *      paging03              50      50      00..00..00..00..50    N/A
                     */
                    Sequence<String>    tokens = i.Tokenize ();
                    if (tokens.size () >= 5) {
                        String  mp = tokens[4];
                        if (mp != L"N/A") {
                            mountPoints.Add (mp);
                        }
                    }
                    else {
                        DbgTrace (L"Dropping unrecognized lspv -l result line on the floor (line=%s)", i.c_str ());
                    }
                }
                if (not mountPoints.empty ()) {
                    result.Add (diskName, mountPoints);
                }
            }
            return result;
        }
    private:
        struct MountInfo_ {
            String  fDeviceName;
            String  fMountedOn;
            String  fFilesystemFormat;
        };
        Sequence<MountInfo_>    ReadMountInfo_ ()
        {
            /*
             *  @todo See if there is a way to find this info on AIX without ProcessRunner...
             */
            using   Execution::ProcessRunner;
            Sequence<MountInfo_>        result;

            ProcessRunner   pr (String_Constant { L"/usr/sbin/mount" });
            Streams::MemoryStream<Byte>   useStdOut;
            pr.SetStdOut (useStdOut);
            pr.Run ();
            Streams::TextReader   stdOut  =   Streams::TextReader (useStdOut);
            unsigned int    lines2Skip = 2;
            for (String i = stdOut.ReadLine (); not i.empty (); i = stdOut.ReadLine ()) {
                /*
                 *   $ /usr/sbin/mount
                 *      node       mounted        mounted over    vfs       date        options
                 *    -------- ---------------  ---------------  ------ ------------ ---------------
                 *             /dev/hd4         /                jfs2   Aug 27 19:50 rw,log=/dev/hd8
                 *             /dev/hd2         /usr             jfs2   Aug 27 19:50 rw,log=/dev/hd8
                 *             /dev/hd9var      /var             jfs2   Aug 27 19:50 rw,log=/dev/hd8
                 *             /dev/hd3         /tmp             jfs2   Aug 27 19:50 rw,log=/dev/hd8
                 *             /dev/hd1         /home            jfs2   Aug 27 19:51 rw,log=/dev/hd8
                 *             /dev/hd11admin   /admin           jfs2   Aug 27 19:51 rw,log=/dev/hd8
                 *             /proc            /proc            procfs Aug 27 19:51 rw
                 *             /dev/hd10opt     /opt             jfs2   Aug 27 19:51 rw,log=/dev/hd8
                 *             /dev/livedump    /var/adm/ras/livedump jfs2   Aug 27 19:51 rw,log=/dev/hd8
                 *             /dev/resgrp156lv /resgrp156       jfs2   Aug 27 19:51 rw,log=/dev/resgrp156loglv
                 *    192.168.253.81 /usr/sys/inst.images/toolbox_20110809 /toolbox         nfs3   Aug 27 19:51 bg,soft,intr,sec=sys,rw
                 *    192.168.253.81 /usr/sys/inst.images/mozilla_3513 /mozilla         nfs3   Aug 27 19:51 bg,soft,intr,sec=sys,rw
                 */
                if (lines2Skip > 0) {
                    lines2Skip--;
                    continue;
                }
                Sequence<String>    tokens = i.Tokenize ();
                if (tokens.size () >= 3) {
                    // we seem to be able to tell node/nfs mounted fs from disk by if mountNode is spaces... Sigh...
                    bool    isNodeMounted = i[0] != ' ';
                    if (isNodeMounted) {
                        result.Append (MountInfo_ { tokens[0] + L":" + tokens[1], tokens[2], tokens[3] } );
                    }
                    else {
                        result.Append (MountInfo_ { tokens[0], tokens[1], tokens[2] } );
                    }
                }
                else {
                    DbgTrace (L"Dropping unrecognized mount result line on the floor (line=%s)", i.c_str ());
                }
            }
            return result;
        }
    };
}
#endif




#if     qPlatform_Linux
namespace {
    struct  CapturerWithContext_Linux_ : CapturerWithContext_COMMON_ {
    private:
        struct PerfStats_ {
            double  fSectorsRead;
            double  fTimeSpentReading;
            double  fReadsCompleted;
            double  fSectorsWritten;
            double  fTimeSpentWriting;
            double  fWritesCompleted;
            double  fWeightedTimeInQSeconds;       // see https://www.kernel.org/doc/Documentation/block/stat.txt  time_in_queue (product of the number of milliseconds times the number of requests waiting)
        };
        Mapping<String, uint32_t>               fDeviceName2SectorSizeMap_;
        Optional<Mapping<dev_t, PerfStats_>>    fContextStats_;
    public:
        CapturerWithContext_Linux_ (Options options)
            : CapturerWithContext_COMMON_ (options)
        {
            // for side-effect of setting fContextStats_
            try {
                capture_ ();
            }
            catch (...) {
                DbgTrace ("bad sign that first pre-catpure failed.");   // Dont propagate in case just listing collectors
            }
        }
        CapturerWithContext_Linux_ (const CapturerWithContext_Linux_&) = default;   // copy by value fine - no need to re-wait...
        Info capture ()
        {
            Execution::SleepUntil (fPostponeCaptureUntil_);
            return capture_ ();
        }
        Info    capture_ ()
        {
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
            Debug::TraceContextBumper ctx ("Instruments::Filesystem...CapturerWithContext_Linux_::capture_");
#endif
            Info   results;

            constexpr   bool    kUseProcFSForMounts_ { true };
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
        Mapping<MountedFilesystemNameType, MountedFilesystemInfoType>    ReadVolumesAndUsageFromProcMountsAndstatvfs_ ()
        {
            Mapping<MountedFilesystemNameType, MountedFilesystemInfoType>    result;
            for (MountInfo_ mi : ReadMountInfo_ ()) {
                MountedFilesystemInfoType  vi;
                if (not mi.fDeviceName.empty () and mi.fDeviceName != L"none") {    // special name none often used when there is no name
                    vi.fDeviceOrVolumeName = mi.fDeviceName;
                }
                vi.fFileSystemType = mi.fFilesystemFormat;
                UpdateVolumneInfo_statvfs (mi.fMountedOn, &vi);
                result.Add (mi.fMountedOn, vi);
            }
            return result;
        }
    private:
        void    UpdateVolumneInfo_statvfs (const String& mountedOnName, MountedFilesystemInfoType* v)
        {
            RequireNotNull (v);
            struct  statvfs sbuf {};
            if (::statvfs (mountedOnName.AsNarrowSDKString ().c_str (), &sbuf) == 0) {
                uint64_t    diskSize = sbuf.f_bsize * sbuf.f_blocks;
                v->fSizeInBytes = diskSize;
                v->fAvailableSizeInBytes = sbuf.f_bsize * sbuf.f_bavail;
                v->fUsedSizeInBytes = diskSize - sbuf.f_bsize * sbuf.f_bfree;
            }
            else {
                DbgTrace (L"statvfs (%s) return error: errno=%d", mountedOnName.c_str (), errno);
            }
        }
    private:
        struct MountInfo_ {
            String  fDeviceName;
            String  fMountedOn;
            String  fFilesystemFormat;
        };
        Sequence<MountInfo_>    ReadMountInfo_ ()
        {
            // first try procfs, but if that fails, fallback on mount command
            try {
                return ReadMountInfo_FromProcMounts_ ();
            }
            catch (...) {
                return Sequence<MountInfo_> ();
            }
        }
        Sequence<MountInfo_>    ReadMountInfo_FromProcMounts_ ()
        {
            /*
             *  I haven't found this clearly documented yet, but it appears that a filesystem can be over-mounted.
             *  See https://www.kernel.org/doc/Documentation/filesystems/ramfs-rootfs-initramfs.txt
             *
             *  So the last one with a given mount point in the file wins.
             */
            Mapping<String, MountInfo_>   result;
            DataExchange::CharacterDelimitedLines::Reader reader {{' ', '\t' }};
            // Note - /procfs files always unseekable
            static  const   String_Constant kProcMountsFileName_     { L"/proc/mounts" };;
            for (Sequence<String> line : reader.ReadMatrix (FileInputStream::mk (kProcMountsFileName_, FileInputStream::eNotSeekable))) {
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
                DbgTrace (L"***in Instruments::Filesystem::Read_proc_mounts_ linesize=%d, line[0]=%s", line.size(), line.empty () ? L"" : line[0].c_str ());
#endif
                //
                // https://www.centos.org/docs/5/html/5.2/Deployment_Guide/s2-proc-mounts.html
                //
                //  1 - device name
                //  2 - mounted on
                //  3 - fstype
                //
                if (line.size () >= 3) {
                    String  devName = line[0];
                    // procfs/mounts often contains symbolic links to device files
                    // e.g. /dev/disk/by-uuid/e1d70192-1bb0-461d-b89f-b054e45bfa00
                    if (devName.StartsWith (L"/")) {
                        IgnoreExceptionsExceptThreadAbortForCall (devName = IO::FileSystem::FileSystem::Default ().CanonicalizeName (devName));
                    }
                    String  mountedOn = line[1];
                    String  fstype = line[2];
                    result.Add (
                        mountedOn,
                    MountInfo_ {
                        devName, mountedOn, fstype
                    }
                    );
                }
            }
            return Sequence<MountInfo_> (result.Values ());
        }
    private:
        void    ReadAndApplyProcFS_diskstats_ (Mapping<MountedFilesystemNameType, MountedFilesystemInfoType>* volumes)
        {
            try {
                Mapping<dev_t, PerfStats_>  diskStats = ReadProcFS_diskstats_ ();
                DurationSecondsType         timeSinceLastMeasure = Time::GetTickCount () - GetLastCaptureAt ();
                for (KeyValuePair<MountedFilesystemNameType, MountedFilesystemInfoType> i : *volumes) {
                    MountedFilesystemInfoType vi = i.fValue;
                    if (vi.fDeviceOrVolumeName.IsPresent ()) {
                        if (fContextStats_) {
                            String  devNameLessSlashes = *vi.fDeviceOrVolumeName;
                            size_t i = devNameLessSlashes.RFind ('/');
                            if (i != string::npos) {
                                devNameLessSlashes = devNameLessSlashes.SubString (i + 1);
                            }
                            dev_t   useDevT;
                            {
                                struct stat sbuf {};
                                if (::stat (vi.fDeviceOrVolumeName->AsNarrowSDKString ().c_str (), &sbuf) == 0) {
                                    useDevT = sbuf.st_rdev;
                                }
                                else {
                                    continue;
                                }
                            }
                            Optional<PerfStats_>    oOld = fContextStats_->Lookup (useDevT);
                            Optional<PerfStats_>    oNew = diskStats.Lookup (useDevT);
                            if (oOld.IsPresent () and oNew.IsPresent ()) {
                                unsigned int sectorSizeTmpHack = GetSectorSize_ (devNameLessSlashes);
                                IOStatsType readStats;
                                readStats.fBytesTransfered = (oNew->fSectorsRead - oOld->fSectorsRead) * sectorSizeTmpHack;
                                readStats.fTotalTransfers = oNew->fReadsCompleted - oOld->fReadsCompleted;
                                readStats.fQLength = (oNew->fTimeSpentReading - oOld->fTimeSpentReading) / timeSinceLastMeasure;

                                IOStatsType writeStats;
                                writeStats.fBytesTransfered = (oNew->fSectorsWritten - oOld->fSectorsWritten) * sectorSizeTmpHack;
                                writeStats.fTotalTransfers = oNew->fWritesCompleted - oOld->fWritesCompleted;
                                writeStats.fQLength = (oNew->fTimeSpentWriting - oOld->fTimeSpentWriting) / timeSinceLastMeasure;

                                IOStatsType combinedStats;
                                combinedStats.fBytesTransfered = *readStats.fBytesTransfered + *writeStats.fBytesTransfered;
                                combinedStats.fTotalTransfers = *readStats.fTotalTransfers + *writeStats.fTotalTransfers;
                                combinedStats.fQLength = *readStats.fQLength + *writeStats.fQLength;

                                vi.fReadIOStats = readStats;
                                vi.fWriteIOStats = writeStats;
                                // @todo DESCRIBE divide by time between 2 and * 1000 - NYI
                                combinedStats.fQLength = ((oNew->fWeightedTimeInQSeconds - oOld->fWeightedTimeInQSeconds) / timeSinceLastMeasure);
                                vi.fCombinedIOStats = combinedStats;
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
        Optional<String>    GetSysBlockDirPathForDevice_ (const String& deviceName)
        {
            Require (not deviceName.empty ());
            Require (not deviceName.Contains (L"/"));
            // Sometimes the /sys/block directory appears to have data for the each major/minor pair, and sometimes it appears
            // to only have it for the top level (minor=0) one without the digit after in the name.
            //
            // I dont understand this well yet, but this appears to temporarily allow us to limp along --LGP 2015-07-10
            //tmphack
            String  tmp { L"/sys/block/" + deviceName + L"/" };
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
        uint32_t    GetSectorSize_ (const String& deviceName)
        {
            auto    o   =   fDeviceName2SectorSizeMap_.Lookup (deviceName);
            if (o.IsMissing ()) {
                Optional<String>    blockDeviceInfoPath = GetSysBlockDirPathForDevice_ (deviceName);
                if (blockDeviceInfoPath) {
                    String  fn = *blockDeviceInfoPath + L"queue/hw_sector_size";
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
                o = 512;    // seems the typical answer on UNIX
            }
            return *o;
        }
    private:
        Mapping<MountedFilesystemNameType, MountedFilesystemInfoType> RunDF_POSIX_ ()
        {
            Mapping<MountedFilesystemNameType, MountedFilesystemInfoType>   result;
            ProcessRunner pr { L"/bin/df -k -P" };
            Streams::MemoryStream<Byte>   useStdOut;
            pr.SetStdOut (useStdOut);
            std::exception_ptr runException;
            try {
                pr.Run ();
            }
            catch (...) {
                runException = current_exception ();
            }
            String out;
            Streams::TextReader   stdOut  =   Streams::TextReader (useStdOut);
            bool skippedHeader = false;
            for (String i = stdOut.ReadLine (); not i.empty (); i = stdOut.ReadLine ()) {
                if (not skippedHeader) {
                    skippedHeader = true;
                    continue;
                }
                Sequence<String>    l    =  i.Tokenize (Set<Characters::Character> { ' ' });
                if (l.size () <  6) {
                    DbgTrace ("skipping line cuz len=%d", l.size ());
                    continue;
                }
                MountedFilesystemInfoType v;
                {
                    String  d   =   l[0].Trim ();
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
            Mapping<MountedFilesystemNameType, MountedFilesystemInfoType>   result;
            //
            // I looked through the /proc filesystem stuff and didnt see anything obvious to retrive this info...
            // run def with ProcessRunner
            //
            //  NEW NOTE - I THINK ITS IN THERE.... RE-EXAMINE proc/filesystems proc/partitions, and http://en.wikipedia.org/wiki/Procfs
            //      -- LGP 2014-08-01
            ProcessRunner pr { includeFSTypes ? L"/bin/df -k -T" : L"/bin/df -k" };
            Streams::MemoryStream<Byte>   useStdOut;
            pr.SetStdOut (useStdOut);
            std::exception_ptr runException;
            try {
                pr.Run ();
            }
            catch (...) {
                runException = current_exception ();
            }
            String out;
            Streams::TextReader   stdOut  =   Streams::TextReader (useStdOut);
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
                MountedFilesystemInfoType v;
                if (includeFSTypes) {
                    v.fFileSystemType = l[1].Trim ();
                }
                {
                    String  d   =   l[0].Trim ();
                    if (not d.empty () and d != L"none") {
                        v.fDeviceOrVolumeName = d;
                    }
                }
                v.fSizeInBytes = Characters::String2Float<double> (l[includeFSTypes ? 2 : 1]) * 1024;
                v.fUsedSizeInBytes = Characters::String2Float<double> (l[includeFSTypes ? 3 : 2]) * 1024;
                v.fAvailableSizeInBytes = *v.fSizeInBytes - *v.fUsedSizeInBytes;
                result.Add ( l[includeFSTypes ? 6 : 5].Trim (), v);
            }
            // Sometimes (with busy box df especailly) we get bogus error return. So only rethrow if we found no good data
            if (runException and result.empty ()) {
                Execution::ReThrow (runException);
            }
            return result;
        }
        Mapping<MountedFilesystemNameType, MountedFilesystemInfoType>    RunDF_ ()
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
            using   Characters::String2Float;
            Mapping<dev_t, PerfStats_>                      result;
            DataExchange::CharacterDelimitedLines::Reader   reader {{' ', '\t' }};
            const   String_Constant kProcMemInfoFileName_ { L"/proc/diskstats" };
            // Note - /procfs files always unseekable
            for (Sequence<String> line : reader.ReadMatrix (FileInputStream::mk (kProcMemInfoFileName_, FileInputStream::eNotSeekable))) {
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
                DbgTrace (L"***in Instruments::Filesystem::ReadProcFS_diskstats_ linesize=%d, line[0]=%s", line.size(), line.empty () ? L"" : line[0].c_str ());
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
                    String  majorDevNumber = line[1 - 1];
                    String  minorDevNumber = line[2 - 1];
                    String  devName = line[3 - 1];
                    String  readsCompleted = line[4 - 1];
                    String  sectorsRead = line[6 - 1];
                    String  timeSpentReadingMS = line[7 - 1];
                    String  writesCompleted = line[8 - 1];
                    String  sectorsWritten = line[10 - 1];
                    String  timeSpentWritingMS = line[11 - 1];
                    constexpr bool kAlsoReadQLen_ { true };
                    Optional<double>    weightedTimeInQSeconds;
                    if (kAlsoReadQLen_) {
                        Optional<String>    sysBlockInfoPath = GetSysBlockDirPathForDevice_ (devName);
                        if (sysBlockInfoPath) {
                            for (Sequence<String> ll : reader.ReadMatrix (FileInputStream::mk (*sysBlockInfoPath + L"stat", FileInputStream::eNotSeekable))) {
                                if (ll.size () >= 11) {
                                    weightedTimeInQSeconds = String2Float (ll[11 - 1]) / 1000.0;    // we record in seconds, but the value in file in milliseconds
                                    break;
                                }
                            }
                        }
                    }
                    result.Add (
                        makedev (String2Int<unsigned int> (majorDevNumber), String2Int<unsigned int> (minorDevNumber)),
                    PerfStats_ {
                        String2Float (sectorsRead), String2Float (timeSpentReadingMS) / 1000,  String2Float (readsCompleted),
                        String2Float (sectorsWritten),  String2Float (timeSpentWritingMS) / 1000, String2Float (writesCompleted),
                        weightedTimeInQSeconds.Value ()
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
            , fLogicalDiskWMICollector_ {
            String_Constant { L"LogicalDisk" }, {},  {
                kDiskReadBytesPerSec_, kDiskWriteBytesPerSec_, kDiskReadsPerSec_, kDiskWritesPerSec_,
                (kUseDiskPercentReadTime_ElseAveQLen_ToComputeQLen_ ? kPctDiskReadTime_ : kAveDiskReadQLen_),
                (kUseDiskPercentReadTime_ElseAveQLen_ToComputeQLen_ ? kPctDiskWriteTime_ : kAveDiskWriteQLen_),
                kPctIdleTime_
            }
        }
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
        Info capture ()
        {
            Execution::SleepUntil (fPostponeCaptureUntil_);
            return capture_ ();
        }
        Info capture_ ()
        {
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
            Debug::TraceContextBumper ctx ("Instruments::Filesystem...CapturerWithContext_Windows_::capture_");
#endif
            Info   results  = capture_Windows_GetVolumeInfo_ ();
            _NoteCompletedCapture ();
            return results;
        }
    private:
#if     qCaptureDiskDeviceInfoWindows_
        list<wstring> GetPhysicalDiskDeviceInfo_()
        {
            HDEVINFO hDeviceInfoSet;
            ULONG ulMemberIndex;
            ULONG ulErrorCode;
            BOOL bFound = FALSE;
            BOOL bOk;
            list<wstring> disks;

            // create a HDEVINFO with all present devices
            hDeviceInfoSet = ::SetupDiGetClassDevs(&GUID_DEVINTERFACE_DISK, NULL, NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
            if (hDeviceInfoSet == INVALID_HANDLE_VALUE) {
                _ASSERT(FALSE);
                return disks;
            }

            // enumerate through all devices in the set
            ulMemberIndex = 0;
            while (TRUE) {
                // get device info
                SP_DEVINFO_DATA deviceInfoData;
                deviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
                if (!::SetupDiEnumDeviceInfo(hDeviceInfoSet, ulMemberIndex, &deviceInfoData)) {
                    if (::GetLastError() == ERROR_NO_MORE_ITEMS) {
                        // ok, reached end of the device enumeration
                        break;
                    }
                    else {
                        // error
                        _ASSERT(FALSE);
                        ::SetupDiDestroyDeviceInfoList(hDeviceInfoSet);
                        return disks;
                    }
                }

                // get device interfaces
                SP_DEVICE_INTERFACE_DATA deviceInterfaceData;
                deviceInterfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
                if (!::SetupDiEnumDeviceInterfaces(hDeviceInfoSet, NULL, &GUID_DEVINTERFACE_DISK, ulMemberIndex, &deviceInterfaceData)) {
                    if (::GetLastError() == ERROR_NO_MORE_ITEMS) {
                        // ok, reached end of the device enumeration
                        break;
                    }
                    else {
                        // error
                        _ASSERT(FALSE);
                        ::SetupDiDestroyDeviceInfoList(hDeviceInfoSet);
                        return disks;
                    }
                }

                // process the next device next time
                ulMemberIndex++;

                // get hardware id of the device
                ULONG ulPropertyRegDataType = 0;
                ULONG ulRequiredSize = 0;
                ULONG ulBufferSize = 0;
                BYTE* pbyBuffer = NULL;
                if (!::SetupDiGetDeviceRegistryProperty(hDeviceInfoSet, &deviceInfoData, SPDRP_HARDWAREID, &ulPropertyRegDataType, NULL, 0, &ulRequiredSize)) {
                    if (::GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
                        pbyBuffer = (BYTE*)::malloc(ulRequiredSize);
                        ulBufferSize = ulRequiredSize;
                        if (!::SetupDiGetDeviceRegistryProperty(hDeviceInfoSet, &deviceInfoData, SPDRP_HARDWAREID, &ulPropertyRegDataType, pbyBuffer, ulBufferSize, &ulRequiredSize)) {
                            // getting the hardware id failed
                            _ASSERT(FALSE);
                            ::SetupDiDestroyDeviceInfoList(hDeviceInfoSet);
                            ::free(pbyBuffer);
                            return disks;
                        }
                    }
                    else {
                        // getting device registry property failed
                        _ASSERT(FALSE);
                        ::SetupDiDestroyDeviceInfoList(hDeviceInfoSet);
                        return disks;
                    }
                }
                else {
                    // getting hardware id of the device succeeded unexpectedly
                    _ASSERT(FALSE);
                    ::SetupDiDestroyDeviceInfoList(hDeviceInfoSet);
                    return disks;
                }

                // pbyBuffer is initialized now!
                LPCWSTR pszHardwareId = (LPCWSTR )pbyBuffer;

                // retrieve detailed information about the device
                // (especially the device path which is needed to create the device object)
                SP_DEVICE_INTERFACE_DETAIL_DATA* pDeviceInterfaceDetailData = NULL;
                ULONG ulDeviceInterfaceDetailDataSize = 0;
                ulRequiredSize = 0;
                bOk = ::SetupDiGetDeviceInterfaceDetail(hDeviceInfoSet, &deviceInterfaceData, pDeviceInterfaceDetailData, ulDeviceInterfaceDetailDataSize, &ulRequiredSize, NULL);
                if (!bOk) {
                    ulErrorCode = ::GetLastError();
                    if (ulErrorCode == ERROR_INSUFFICIENT_BUFFER) {
                        // insufficient buffer space
                        // => that's ok, allocate enough space and try again
                        pDeviceInterfaceDetailData = (SP_DEVICE_INTERFACE_DETAIL_DATA*)::malloc(ulRequiredSize);
                        pDeviceInterfaceDetailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
                        ulDeviceInterfaceDetailDataSize = ulRequiredSize;
                        deviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
                        bOk = ::SetupDiGetDeviceInterfaceDetail(hDeviceInfoSet, &deviceInterfaceData, pDeviceInterfaceDetailData, ulDeviceInterfaceDetailDataSize, &ulRequiredSize, &deviceInfoData);
                        ulErrorCode = ::GetLastError();
                    }

                    if (!bOk) {
                        // retrieving detailed information about the device failed
                        _ASSERT(FALSE);
                        ::free(pbyBuffer);
                        ::free(pDeviceInterfaceDetailData);
                        ::SetupDiDestroyDeviceInfoList(hDeviceInfoSet);
                        return disks;
                    }
                }
                else {
                    // retrieving detailed information about the device succeeded unexpectedly
                    _ASSERT(FALSE);
                    ::free(pbyBuffer);
                    ::SetupDiDestroyDeviceInfoList(hDeviceInfoSet);
                    return disks;
                }

                disks.push_back(pDeviceInterfaceDetailData->DevicePath);

                // free buffer for device interface details
                ::free(pDeviceInterfaceDetailData);

                // free buffer
                ::free(pbyBuffer);
            }

            // destroy device info list
            ::SetupDiDestroyDeviceInfoList(hDeviceInfoSet);

            return disks;
        }
#endif
        static  String  GetPhysNameForDriveNumber_ (unsigned int i)
        {
            // This format is NOT super well documented, and was mostly derived from reading the remarks section
            // of https://msdn.microsoft.com/en-us/library/windows/desktop/aa363216%28v=vs.85%29.aspx?f=255&MSPPError=-2147217396
            // (DeviceIoControl function)
            return Characters::Format (L"\\\\.\\PhysicalDrive%d", i);
        }
        struct  PhysicalDriveInfo_ {
            String      fDeviceName;
            uint64_t    fDeviceSizeInBytes;
        };
        static  Collection<PhysicalDriveInfo_> GetPhysDrives_ ()
        {
            // This is NOT super well documented, and was mostly derived from reading the remarks section
            // of https://msdn.microsoft.com/en-us/library/windows/desktop/aa363216%28v=vs.85%29.aspx?f=255&MSPPError=-2147217396
            // (DeviceIoControl function) and random fiddling (so unreliable)
            //  --LGP 2015-09-29
            Collection<PhysicalDriveInfo_> x;
            for (int i = 0; i < 100; i++) {
                HANDLE hHandle = CreateFileW (GetPhysNameForDriveNumber_ (i).c_str (), GENERIC_READ/*|GENERIC_WRITE*/, FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
                if (hHandle == INVALID_HANDLE_VALUE) {
                    break;
                }
                GET_LENGTH_INFORMATION  li {};
                DWORD dwBytesReturned {};
                BOOL bResult = ::DeviceIoControl (hHandle, IOCTL_DISK_GET_LENGTH_INFO, NULL, 0, &li, sizeof(li), &dwBytesReturned, NULL);
                ::CloseHandle (hHandle);
                PhysicalDriveInfo_  di { GetPhysNameForDriveNumber_ (i), static_cast<uint64_t> (li.Length.QuadPart) };
                x.Add (di);
            }
            return x;
        }
        static  Set<DynamicDiskIDType> GetDisksForVolume_ (String volumeName)
        {
            wchar_t volPathsBuf[10 * 1024] = {0};
            DWORD   retLen  =   0;
            DWORD   x       =   ::GetVolumePathNamesForVolumeNameW (volumeName.c_str (), volPathsBuf, static_cast<DWORD> (NEltsOf (volPathsBuf)), &retLen);
            if (x == 0 or retLen <= 1) {
                return Set<String> ();
            }
            Assert (1 <= Characters::CString::Length (volPathsBuf) and Characters::CString::Length (volPathsBuf) < NEltsOf (volPathsBuf));;
            volumeName = L"\\\\.\\" + String::FromSDKString (volPathsBuf).CircularSubString (0, -1);

            // @todo - rewrite this - must somehow otherwise callocate this to be large enuf (dynamic alloc) - if we want more disk exents, but not sure when that happens...
            VOLUME_DISK_EXTENTS volumeDiskExtents;
            {
                HANDLE hHandle = ::CreateFileW (volumeName.c_str () , GENERIC_READ/*|GENERIC_WRITE*/ , FILE_SHARE_WRITE | FILE_SHARE_READ , NULL , OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
                if (hHandle == INVALID_HANDLE_VALUE) {
                    return Set<String> ();
                }
                DWORD               dwBytesReturned = 0;
                BOOL                bResult = ::DeviceIoControl (hHandle, IOCTL_VOLUME_GET_VOLUME_DISK_EXTENTS, NULL, 0, &volumeDiskExtents, sizeof(volumeDiskExtents), &dwBytesReturned, NULL);
                ::CloseHandle (hHandle);
                if (not bResult) {
                    return Set<String> ();
                }
            }
            Set<DynamicDiskIDType> result;
            for (DWORD n = 0; n < volumeDiskExtents.NumberOfDiskExtents;  ++n) {
                PDISK_EXTENT pDiskExtent = &volumeDiskExtents.Extents[n];
#if 0
                _tprintf(_T("Disk number: %d\n") , pDiskExtent->DiskNumber);
                _tprintf(_T("DBR start sector: %I64d\n") , pDiskExtent->StartingOffset.QuadPart / 512);
#endif
                result.Add (GetPhysNameForDriveNumber_ (pDiskExtent->DiskNumber));
            }
            return result;
        }
        Optional<String>    GetDeviceNameForVolumneName_ (const String& volumeName)
        {
            //  use
            //      http://msdn.microsoft.com/en-us/library/windows/desktop/cc542456(v=vs.85).aspx
            //      CharCount = QueryDosDeviceW(&VolumeName[4], DeviceName, ARRAYSIZE(DeviceName));
            //      to get DEVICENAME
            //
            String  tmp =   volumeName;
            //  Skip the \\?\ prefix and remove the trailing backslash.
            //  QueryDosDeviceW does not allow a trailing backslash,
            //  so temporarily remove it.
            if (tmp.length () < 5 or
                    tmp[0]     != L'\\' ||
                    tmp[1]     != L'\\' ||
                    tmp[2]     != L'?'  ||
                    tmp[3]     != L'\\' ||
                    tmp[tmp.length () - 1] != L'\\') {
                //Error = ERROR_BAD_PATHNAME;
                //wprintf(L"FindFirstVolumeW/FindNextVolumeW returned a bad path: %s\n", VolumeName);
                return Optional<String> ();
            }
            tmp = tmp.CircularSubString (4, -1);

            WCHAR  DeviceName[MAX_PATH] = L"";
            if (QueryDosDeviceW(tmp.c_str (), DeviceName, ARRAYSIZE(DeviceName)) !=  0) {
                return DeviceName;
            }
            return Optional<String> ();
        }

        Info    capture_Windows_GetVolumeInfo_ ()
        {
#if     qCaptureDiskDeviceInfoWindows_ && 0
            for (auto s : GetPhysicalDiskDeviceInfo_()) {
                DbgTrace (L"s=%s", s.c_str ());
            }
#endif
            // Could probably usefully optimize to not capture if no drives because we can only get this when running as
            // Admin, and for now, we capture little useful information at the drive level. But - we will eventually...
            Collection<PhysicalDriveInfo_> physDrives = GetPhysDrives_ ();
            bool    driveInfoAvailable = physDrives.size () >= 1;

#if     qUseWMICollectionSupport_
            Time::DurationSecondsType   timeOfPrevCollection = fLogicalDiskWMICollector_.GetTimeOfLastCollection ();
            if (fOptions_.fIOStatistics) {
                fLogicalDiskWMICollector_.Collect ();
            }
            Time::DurationSecondsType   timeCollecting { fLogicalDiskWMICollector_.GetTimeOfLastCollection () - timeOfPrevCollection };
#endif
            Info    result;

            for (PhysicalDriveInfo_ pd : physDrives) {
                DiskInfoType    di { };
                di.fSizeInBytes = pd.fDeviceSizeInBytes;
                result.fDisks.Add (pd.fDeviceName, di);
            }

            TCHAR   volumeNameBuf[1024];
            for (HANDLE hVol = ::FindFirstVolume (volumeNameBuf, static_cast<DWORD> (NEltsOf (volumeNameBuf))); hVol != INVALID_HANDLE_VALUE; ) {
                DWORD lpMaximumComponentLength;
                DWORD dwSysFlags;
                TCHAR FileSysNameBuf[1024];
                if (::GetVolumeInformation (volumeNameBuf, nullptr, static_cast<DWORD> (NEltsOf (volumeNameBuf)), nullptr, &lpMaximumComponentLength, &dwSysFlags, FileSysNameBuf, static_cast<DWORD> (NEltsOf (FileSysNameBuf)))) {
                    MountedFilesystemInfoType v;
                    v.fFileSystemType = String::FromSDKString (FileSysNameBuf);
                    v.fVolumeID = String::FromSDKString (volumeNameBuf);

                    if (driveInfoAvailable) {
                        v.fOnPhysicalDrive = GetDisksForVolume_ (volumeNameBuf);
                    }

                    /*
                     *  For now, we only capture the volume ID for a disk
                     */
                    switch (::GetDriveType (volumeNameBuf)) {
                        case    DRIVE_REMOVABLE:
                            v.fDeviceKind = BlockDeviceKind::eRemovableDisk;
                            break;
                        case    DRIVE_FIXED:
                            v.fDeviceKind = BlockDeviceKind::eLocalDisk;
                            break;
                        case    DRIVE_REMOTE:
                            v.fDeviceKind = BlockDeviceKind::eNetworkDrive;
                            break;
                        case    DRIVE_RAMDISK:
                            v.fDeviceKind = BlockDeviceKind::eTemporaryFiles;
                            break;
                        case    DRIVE_CDROM:
                            v.fDeviceKind = BlockDeviceKind::eReadOnlyEjectable;
                            break;
                        default:;   /*ignored - if it doesnt map or error - nevermind */
                    }
                    {
                        /*
                         *  @todo NOT sure if this comment is right. Must do research??? - Could just be multiple mount points?
                         *
                         *  On Windoze, each volume object can have multiple sub-volumes (logical volumes) and we return the size of each).
                         */

                        ///
                        /// @todo FIX TO USE get extended error information, call GetLastError. If the buffer is not large enough to hold the complete list,
                        /// the error code is ERROR_MORE_DATA a
                        /// so resize buffer accordingly...
                        ///
                        TCHAR volPathsBuf[10 * 1024];
                        DWORD retLen = 0;
                        DWORD x = ::GetVolumePathNamesForVolumeName (volumeNameBuf, volPathsBuf, static_cast<DWORD> (NEltsOf (volPathsBuf)), &retLen);
                        if (x == 0) {
                            DbgTrace (SDKSTR ("Ignoring error getting paths (volume='%s')"), volumeNameBuf);
                        }
                        else if (volPathsBuf[0] == 0) {
                            // Ignore - unmounted!
                            DbgTrace (SDKSTR ("Ignoring unmounted filesystem (volume='%s')"), volumeNameBuf);
                        }
                        else {
                            auto safePctInUse2QL_ = [] (double pctInUse) {
                                // %InUse = QL / (1 + QL).
                                pctInUse /= 100;
                                pctInUse = Math::PinInRange<double> (pctInUse, 0, 1);
                                return pctInUse / (1 - pctInUse);
                            };
                            for (const TCHAR* NameIdx = volPathsBuf; NameIdx[0] != L'\0'; NameIdx += Characters::CString::Length (NameIdx) + 1) {
                                String  mountedOnName = String::FromSDKString (NameIdx);
                                {
                                    ULARGE_INTEGER freeBytesAvailable {};
                                    ULARGE_INTEGER totalNumberOfBytes {};
                                    ULARGE_INTEGER totalNumberOfFreeBytes {};
                                    DWORD xxx = ::GetDiskFreeSpaceEx (mountedOnName.AsSDKString ().c_str (), &freeBytesAvailable, &totalNumberOfBytes, &totalNumberOfFreeBytes);
                                    v.fSizeInBytes = totalNumberOfBytes.QuadPart;
                                    v.fUsedSizeInBytes = *v.fSizeInBytes  - freeBytesAvailable.QuadPart;
                                    v.fAvailableSizeInBytes = *v.fSizeInBytes - *v.fUsedSizeInBytes;
#if     qUseWMICollectionSupport_
                                    if (fOptions_.fIOStatistics) {
                                        String wmiInstanceName = mountedOnName.RTrim ([] (Characters::Character c) { return c == '\\'; });
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
                                                readStats.fQLength = safePctInUse2QL_ (*o);
                                            }
                                        }
                                        else {
                                            if (auto o = fLogicalDiskWMICollector_.PeekCurrentValue (wmiInstanceName, kAveDiskReadQLen_)) {
                                                readStats.fInUsePercent = *o;
                                                readStats.fQLength = *o;
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
                                                writeStats.fQLength = safePctInUse2QL_ (*o);
                                            }
                                        }
                                        else {
                                            if (auto o = fLogicalDiskWMICollector_.PeekCurrentValue (wmiInstanceName, kAveDiskWriteQLen_)) {
                                                writeStats.fInUsePercent = *o;
                                                writeStats.fQLength = *o;
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
                                                double  aveCombinedQLen = safePctInUse2QL_ (100.0 - *o);
                                                if (readStats.fQLength and writeStats.fQLength and * combinedStats.fQLength > 0) {
                                                    // for some reason, the pct-idle-time #s combined are OK, but #s for aveQLen and disk read PCT/Write PCT wrong.
                                                    // asusme ratio rate, and scale
                                                    double  correction = aveCombinedQLen / *combinedStats.fQLength;
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
                                result.fMountedFilesystems.Add (mountedOnName, v);
                            }
                        }
                    }
                }
                else {
                    // warning...
                }

                // find next
                if (not ::FindNextVolume (hVol, volumeNameBuf, static_cast<DWORD> (NEltsOf(volumeNameBuf)))) {
                    ::FindVolumeClose (hVol);
                    hVol = INVALID_HANDLE_VALUE;
                }
            }
            return result;
        }
    };
}
#endif








namespace {
    Mapping<MountedFilesystemNameType, MountedFilesystemInfoType>   ApplyDiskStatsToMissingFileSystemStats_ (const Mapping<DynamicDiskIDType, DiskInfoType>& disks, const Mapping<MountedFilesystemNameType, MountedFilesystemInfoType>& fileSystems)
    {
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
        Debug::TraceContextBumper ctx ("Instruments::Filesystem ... ApplyDiskStatsToMissingFileSystemStats_");
#endif
        // Each FS will have some stats about disk usage, and we want to use those to relatively weight the stats from the disk usage when
        // applied back to other FS stats.
        //
        // So first compute the total stat per disk
        using   WeightingStat2UseType = double;
        Mapping<DynamicDiskIDType, WeightingStat2UseType>   totalWeights;
        for (KeyValuePair<MountedFilesystemNameType, MountedFilesystemInfoType> i :  fileSystems) {
            Set<DynamicDiskIDType>  disksForFS  =   i.fValue.fOnPhysicalDrive.Value ();
            if (disksForFS.size () > 0) {
                WeightingStat2UseType   weightForFS =   i.fValue.fCombinedIOStats.Value ().fBytesTransfered.Value ();
                weightForFS /= disksForFS.size ();
                for (DynamicDiskIDType di : disksForFS) {
                    totalWeights.Add (di, totalWeights.LookupValue (di) + weightForFS); // accumulate relative application to each disk
                }
            }
        }
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
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
        Mapping<MountedFilesystemNameType, MountedFilesystemInfoType>   newFilessytems;
        if (totalWeights.size () >= 1) {
            for (KeyValuePair<MountedFilesystemNameType, MountedFilesystemInfoType> i :  fileSystems) {
                MountedFilesystemInfoType   mfi         =   i.fValue;
                Set<DynamicDiskIDType>      disksForFS  =   mfi.fOnPhysicalDrive.Value ();
                if (disksForFS.size () > 0) {
                    WeightingStat2UseType   weightForFS =   i.fValue.fCombinedIOStats.Value ().fBytesTransfered.Value ();
                    weightForFS /= disksForFS.size ();
                    IOStatsType     cumStats = mfi.fCombinedIOStats.Value ();
                    bool    computeInuse        =   cumStats.fInUsePercent.IsMissing ();
                    bool    computeQLen         =   cumStats.fQLength.IsMissing ();
                    bool    computeTotalXFers   =   cumStats.fTotalTransfers.IsMissing ();

                    for (DynamicDiskIDType di : disksForFS) {
                        IOStatsType     diskIOStats = disks.LookupValue (di).fCombinedIOStats.Value ();
                        if (weightForFS > 0) {
                            double  scaleFactor =   weightForFS / totalWeights.LookupValue (di);
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
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
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
    struct  CapturerWithContext_
            : Debug::AssertExternallySynchronizedLock
#if     qPlatform_AIX
            , CapturerWithContext_AIX_
#elif   qPlatform_Linux
            , CapturerWithContext_Linux_
#elif   qPlatform_Windows
            , CapturerWithContext_Windows_
#endif
    {
#if     qPlatform_AIX
        using inherited = CapturerWithContext_AIX_;
#elif   qPlatform_Linux
        using inherited = CapturerWithContext_Linux_;
#elif   qPlatform_Windows
        using inherited = CapturerWithContext_Windows_;
#endif
        CapturerWithContext_ (Options options)
            : inherited (options)
        {
        }
        Info capture ()
        {
            lock_guard<const AssertExternallySynchronizedLock> critSec { *this };
            Debug::TraceContextBumper ctx ("Instruments::Filesystem capture");
            Info    result = inherited::capture ();
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
    using   StructureFieldInfo = ObjectVariantMapper::StructFieldInfo;
    static  const   ObjectVariantMapper sMapper_ = [] () -> ObjectVariantMapper {
        ObjectVariantMapper mapper;
        mapper.AddCommonType<Optional<double>> ();
        mapper.AddCommonType<Optional<uint64_t>> ();
        mapper.AddCommonType<Optional<String>> ();
#if     qCompilerAndStdLib_DefaultArgOfStaticTemplateMember_Buggy
        mapper.Add (mapper.MakeCommonSerializer_NamedEnumerations<BlockDeviceKind> (Configuration::DefaultNames<BlockDeviceKind>::k));
#else
        mapper.Add (mapper.MakeCommonSerializer_NamedEnumerations<BlockDeviceKind> ());
#endif
        mapper.AddCommonType<Optional<BlockDeviceKind>> ();
        DISABLE_COMPILER_GCC_WARNING_START("GCC diagnostic ignored \"-Winvalid-offsetof\"");       // Really probably an issue, but not to debug here -- LGP 2014-01-04
        mapper.AddClass<IOStatsType> (initializer_list<StructureFieldInfo> {
            { Stroika_Foundation_DataExchange_StructFieldMetaInfo (IOStatsType, fBytesTransfered), String_Constant (L"Bytes"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_StructFieldMetaInfo (IOStatsType, fQLength), String_Constant (L"Q-Length"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_StructFieldMetaInfo (IOStatsType, fInUsePercent), String_Constant (L"In-Use-%"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_StructFieldMetaInfo (IOStatsType, fTotalTransfers), String_Constant (L"Total-Transfers"), StructureFieldInfo::NullFieldHandling::eOmit },
        });
        mapper.AddCommonType<Optional<IOStatsType>> ();
        mapper.AddClass<DiskInfoType> (initializer_list<StructureFieldInfo> {
            { Stroika_Foundation_DataExchange_StructFieldMetaInfo (DiskInfoType, fPersistenceVolumeID), String_Constant (L"Persistence-Volume-ID"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_StructFieldMetaInfo (DiskInfoType, fDeviceKind), String_Constant (L"Device-Kind"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_StructFieldMetaInfo (DiskInfoType, fSizeInBytes), String_Constant (L"Size"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_StructFieldMetaInfo (DiskInfoType, fReadIOStats), String_Constant (L"Read-IO-Stats"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_StructFieldMetaInfo (DiskInfoType, fWriteIOStats), String_Constant (L"Write-IO-Stats"), StructureFieldInfo::NullFieldHandling::eOmit  },
            { Stroika_Foundation_DataExchange_StructFieldMetaInfo (DiskInfoType, fCombinedIOStats), String_Constant (L"Combined-IO-Stats"), StructureFieldInfo::NullFieldHandling::eOmit  },
        });
        mapper.AddCommonType<Set<String>> ();
        mapper.AddCommonType<Optional<Set<String>>> ();
        mapper.AddClass<MountedFilesystemInfoType> (initializer_list<StructureFieldInfo> {
            { Stroika_Foundation_DataExchange_StructFieldMetaInfo (MountedFilesystemInfoType, fDeviceKind), String_Constant (L"Device-Kind"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_StructFieldMetaInfo (MountedFilesystemInfoType, fFileSystemType), String_Constant (L"Filesystem-Type"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_StructFieldMetaInfo (MountedFilesystemInfoType, fDeviceOrVolumeName), String_Constant (L"Device-Name"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_StructFieldMetaInfo (MountedFilesystemInfoType, fOnPhysicalDrive), String_Constant (L"On-Physical-Drives"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_StructFieldMetaInfo (MountedFilesystemInfoType, fVolumeID), String_Constant (L"Volume-ID"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_StructFieldMetaInfo (MountedFilesystemInfoType, fSizeInBytes), String_Constant (L"Total-Size"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_StructFieldMetaInfo (MountedFilesystemInfoType, fAvailableSizeInBytes), String_Constant (L"Available-Size"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_StructFieldMetaInfo (MountedFilesystemInfoType, fUsedSizeInBytes), String_Constant (L"Used-Size"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_StructFieldMetaInfo (MountedFilesystemInfoType, fReadIOStats), String_Constant (L"Read-IO-Stats"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_StructFieldMetaInfo (MountedFilesystemInfoType, fWriteIOStats), String_Constant (L"Write-IO-Stats"), StructureFieldInfo::NullFieldHandling::eOmit  },
            { Stroika_Foundation_DataExchange_StructFieldMetaInfo (MountedFilesystemInfoType, fCombinedIOStats), String_Constant (L"Combined-IO-Stats"), StructureFieldInfo::NullFieldHandling::eOmit  },
        });
        mapper.Add (mapper.MakeCommonSerializer_ContainerWithStringishKey<Mapping<DynamicDiskIDType, DiskInfoType>> ());
        mapper.Add (mapper.MakeCommonSerializer_ContainerWithStringishKey<Mapping<MountedFilesystemNameType, MountedFilesystemInfoType>> ());
        mapper.AddClass<Info> (initializer_list<StructureFieldInfo> {
            { Stroika_Foundation_DataExchange_StructFieldMetaInfo (Info, fDisks), String_Constant (L"Disks") },
            { Stroika_Foundation_DataExchange_StructFieldMetaInfo (Info, fMountedFilesystems), String_Constant (L"Mounted-Filesystems") },
        });
        DISABLE_COMPILER_GCC_WARNING_END("GCC diagnostic ignored \"-Winvalid-offsetof\"");
        return mapper;
    } ();
    return sMapper_;
}


namespace {
    static  const   MeasurementType kMountedVolumeUsage_  =   MeasurementType { String_Constant { L"Mounted-Filesystem-Usage"} };
}




namespace {
    class   MyCapturer_ : public Instrument::ICapturer {
        CapturerWithContext_ fCaptureContext;
    public:
        MyCapturer_ (const CapturerWithContext_& ctx)
            : fCaptureContext (ctx)
        {
        }
        virtual MeasurementSet  Capture () override
        {
            MeasurementSet  results;
            Measurement     m { kMountedVolumeUsage_, GetObjectVariantMapper ().FromObject (Capture_Raw (&results.fMeasuredAt))};
            results.fMeasurements.Add (m);
            return results;
        }
        nonvirtual Info  Capture_Raw (Range<DurationSecondsType>* outMeasuredAt)
        {
            DurationSecondsType    before = fCaptureContext.GetLastCaptureAt ();
            Info rawMeasurement = fCaptureContext.capture ();
            if (outMeasuredAt != nullptr) {
                *outMeasuredAt = Range<DurationSecondsType> (before, fCaptureContext.GetLastCaptureAt ());
            }
            return rawMeasurement;
        }
        virtual unique_ptr<ICapturer>   Clone () const override
        {
#if     qCompilerAndStdLib_make_unique_Buggy
            return unique_ptr<ICapturer> (new MyCapturer_ (fCaptureContext));
#else
            return make_unique<MyCapturer_> (fCaptureContext);
#endif
        }
    };
}






/*
 ********************************************************************************
 ******************** Instruments::Filesystem::GetInstrument ********************
 ********************************************************************************
 */
Instrument  SystemPerformance::Instruments::Filesystem::GetInstrument (Options options)
{
    return Instrument (
               InstrumentNameType { String_Constant {L"Filesystem"} },
#if     qCompilerAndStdLib_make_unique_Buggy
               Instrument::SharedByValueCaptureRepType (unique_ptr<MyCapturer_> (new MyCapturer_ (CapturerWithContext_ { options }))),
#else
               Instrument::SharedByValueCaptureRepType (make_unique<MyCapturer_> (CapturerWithContext_ { options })),
#endif
    { kMountedVolumeUsage_ },
    GetObjectVariantMapper ()
           );
}








/*
 ********************************************************************************
 ********* SystemPerformance::Instrument::CaptureOneMeasurement *****************
 ********************************************************************************
 */
template    <>
Instruments::Filesystem::Info   SystemPerformance::Instrument::CaptureOneMeasurement (Range<DurationSecondsType>* measurementTimeOut)
{
    MyCapturer_*    myCap = dynamic_cast<MyCapturer_*> (fCapFun_.get ());
    AssertNotNull (myCap);
    return myCap->Capture_Raw (measurementTimeOut);
}

