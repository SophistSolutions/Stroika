/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
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
 ********************************************************************************
 ********** Instruments::Filesystem::VolumeInfo::EstimatedPercentInUse **********
 ********************************************************************************
 */
Optional<double>    VolumeInfo::EstimatedPercentInUse () const
{
    // %InUse = QL / (1 + QL).
    if (fCombinedIOStats and fCombinedIOStats->fQLength) {
        double QL = *fCombinedIOStats->fQLength;
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
    void    ApplyDiskTypes_ (Sequence<VolumeInfo>* volumes)
    {
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
        for (Iterator<VolumeInfo> i = volumes->begin (); i != volumes->end (); ++i) {
            // @todo - NOTE - this is NOT a reliable way to tell, but hopefully good enough for starters
            VolumeInfo vi = *i;
            if (vi.fFileSystemType) {
                String  fstype = *vi.fFileSystemType;
                bool    changed { false };
                if (kRealDiskFS.Contains (fstype)) {
                    vi.fMountedDeviceType = MountedDeviceType::eLocalDisk;
                    changed = true;
                }
                else if (kNetworkFS_.Contains (fstype)) {
                    vi.fMountedDeviceType = MountedDeviceType::eNetworkDrive;
                    changed = true;
                }
                else if (fstype == L"tmpfs") {
                    vi.fMountedDeviceType = MountedDeviceType::eTemporaryFiles;
                    changed = true;
                }
                else if (fstype == L"iso9660") {
                    vi.fMountedDeviceType = MountedDeviceType::eReadOnlyEjectable;
                    changed = true;
                }
                else if (kSysFSList_.Contains (fstype)) {
                    vi.fMountedDeviceType = MountedDeviceType::eSystemInformation;
                    changed = true;
                }
                if (changed) {
                    volumes->Update (i, vi);
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
        struct PerfStats_ {
            u_longlong_t  fDiskBlockSize;
            u_longlong_t  fTotalTransfers;
            u_longlong_t  fBlocksRead;
            u_longlong_t  fBlocksWritten;
            u_longlong_t  wq_sampled;               // appears to be (subject to verification) weighted q ave len (so can divide by time elapsed to get qlen ave)
            u_longlong_t  time;                     // appears ave of percent time in use (accumulated) so divide by elapsed time and diff to get %time)
        };
        Mapping<String, PerfStats_>     fContextDiskName2PerfStats_;
    public:
        CapturerWithContext_AIX_ (Options options)
            : CapturerWithContext_COMMON_ (options)
        {
            // for side-effect of setting fContextDiskName2PerfStats_
            try {
                capture_ ();
            }
            catch (...) {
                DbgTrace ("bad sign that first pre-catpure failed.");   // Dont propagate in case just listing collectors
            }
        }
        CapturerWithContext_AIX_ (const CapturerWithContext_AIX_&) = default;   // copy by value fine - no need to re-wait...
        Sequence<VolumeInfo> capture ()
        {
            Execution::SleepUntil (fPostponeCaptureUntil_);
            return capture_ ();
        }
        Sequence<VolumeInfo> capture_ ()
        {
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
            Debug::TraceContextBumper ctx ("Instruments::Filesystem...CapturerWithContext_AIX_::capture_");
#endif
            Sequence<VolumeInfo>   results;
            results = ReadVolumesAndUsageFromProcMountsAndstatvfs_ ();
            ApplyDiskTypes_ (&results);
            if (not fOptions_.fIncludeTemporaryDevices) {
                for (Iterator<VolumeInfo> i = results.begin (); i != results.end (); ++i) {
                    if (i->fMountedDeviceType == MountedDeviceType::eTemporaryFiles) {
                        results.Remove (i);
                    }
                }
            }
            if (not fOptions_.fIncludeSystemDevices) {
                for (Iterator<VolumeInfo> i = results.begin (); i != results.end (); ++i) {
                    if (i->fMountedDeviceType == MountedDeviceType::eSystemInformation) {
                        results.Remove (i);
                    }
                }
            }
            if (fOptions_.fIOStatistics) {
                ReadAndApplyProcFS_diskstats_ (&results);
            }
            _NoteCompletedCapture ();
            return results;
        }
    private:
        Sequence<VolumeInfo>    ReadVolumesAndUsageFromProcMountsAndstatvfs_ ()
        {
            Sequence<VolumeInfo>    result;
            for (MountInfo_ mi : ReadMountInfo_ ()) {
                VolumeInfo  vi;
                vi.fMountedOnName = mi.fMountedOn;
                if (not mi.fDeviceName.empty () and mi.fDeviceName != L"none") {    // special name none often used when there is no name
                    vi.fDeviceOrVolumeName = mi.fDeviceName;
                }
                vi.fFileSystemType = mi.fFilesystemFormat;
                UpdateVolumneInfo_statvfs (&vi);
                result.Append (vi);
            }
            return result;
        }
    private:
        void    UpdateVolumneInfo_statvfs (VolumeInfo* v)
        {
            RequireNotNull (v);
            if (v->fFileSystemType == L"procfs") {
                // on AIX, this returns a bogus disk size. Not sure how to tell otherwise to ignore
                return;
            }
            struct  statvfs64 sbuf;
            (void)::memset (&sbuf, 0, sizeof (sbuf));
            if (::statvfs64 (v->fMountedOnName.AsNarrowSDKString ().c_str (), &sbuf) == 0) {
                uint64_t    diskSize = sbuf.f_bsize * sbuf.f_blocks;
                v->fSizeInBytes = diskSize;
                v->fAvailableSizeInBytes = sbuf.f_bsize * sbuf.f_bavail;
                v->fUsedSizeInBytes = diskSize - sbuf.f_bsize * sbuf.f_bfree;
            }
            else {
                DbgTrace (L"statvfs (%s) return error: errno=%d", v->fMountedOnName.c_str (), errno);
            }
        }
    private:
        void    ReadAndApplyProcFS_diskstats_ (Sequence<VolumeInfo>* volumes)
        {
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
            Debug::TraceContextBumper ctx ("Instruments::Filesystem ReadAndApplyProcFS_diskstats_");
#endif
            RequireNotNull (volumes);
            Disk2MountPointsMapType_        disk2MountPointMap = ReadDisk2MountPointsMap_ ();
            Mapping<String, VolumeInfo>     volMap;
            volumes->Apply ([&volMap] (const VolumeInfo & vi) {volMap.Add (vi.fMountedOnName, vi); });
            for (KeyValuePair<String, Set<String>> diskAndVols : disk2MountPointMap) {
                perfstat_id_t   name;
                Characters::CString::Copy (name.name, NEltsOf (name.name), diskAndVols.fKey.AsNarrowSDKString ().c_str ());
                perfstat_disk_t ds;
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
                 */
                (void)::memset (&ds, 0, sizeof (ds));
                int disks = ::perfstat_disk (&name, &ds, sizeof (ds), 1);
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
                DbgTrace ("perfstat_disk returned %d: name=%s; bsize=%lld; xfers=%lld; rblks=%lld; wblks=%lld; qdepth: %lld; time: %lld; wq_sampled: %lld; wq_time: %lld", disks, ds.name, ds.bsize, ds.xfers, ds.rblks, ds.wblks, ds.qdepth, ds.time, ds.wq_sampled, ds.wq_time);
#endif
                if (disks == 1) {
                    // @todo see below Docs and examples quite unclear. Maybe use wq_sampled
                    PerfStats_              ps              { ds.bsize, ds.xfers, ds.rblks, ds.wblks, ds.wq_sampled, ds.time };
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
                    DbgTrace ("ps = {fDiskBlockSize: %lld; fTotalTransfers: %lld; fBlocksRead: %lld; fBlocksWritten: %lld; wq_sampled: %lld;}", ps.fDiskBlockSize, ps.fTotalTransfers, ps.fBlocksRead, ps.fBlocksWritten, ps.wq_sampled);
#endif
                    Optional<PerfStats_>    prevPerfStats   =   fContextDiskName2PerfStats_.Lookup (diskAndVols.fKey);
                    if (prevPerfStats) {
                        Assert (diskAndVols.fValue.size () >= 1);   // else wouldn't be in this list
                        /*
                         *  Since we know total disk stats, and not how divided over the mounted filesystems on that disk,
                         *  divide activity evently.
                         */
                        double  scaleResultsBy  =   1.0 / diskAndVols.fValue.size ();
                        VolumeInfo::IOStats readStats;
                        readStats.fBytesTransfered = (ps.fBlocksRead - prevPerfStats->fBlocksRead) *  ps.fDiskBlockSize * scaleResultsBy;
                        VolumeInfo::IOStats writeStats;
                        writeStats.fBytesTransfered = (ps.fBlocksWritten - prevPerfStats->fBlocksWritten) *  ps.fDiskBlockSize * scaleResultsBy;
                        VolumeInfo::IOStats combinedStats;
                        combinedStats.fBytesTransfered = *readStats.fBytesTransfered + *writeStats.fBytesTransfered;
                        combinedStats.fTotalTransfers = (ps.fTotalTransfers - prevPerfStats->fTotalTransfers) * scaleResultsBy;

#if     USE_NOISY_TRACE_IN_THIS_MODULE_
                        DbgTrace ("in compare for disk %s: wq_time=%lld; prevPerfStats->wq_time=%lld; ps.wq_sampled =%lld; prevPerfStats->wq_sampled: %lld }", ds.name, ps.wq_time, prevPerfStats->wq_time, ps.wq_sampled, prevPerfStats->wq_sampled);
#endif
                        {
                            // Docs and examples quite unclear. Maybe use wq_sampled, or q_sampled. And unclear what to divide by.
                            double elapsed = Time::GetTickCount () - GetLastCaptureAt ();
                            static  uint32_t    kNumberLogicalCores_ = GetSystemConfiguration_CPU ().GetNumberOfLogicalCores ();
                            double a = (100.0 * (double)elapsed * (double)kNumberLogicalCores_);
                            combinedStats.fQLength = ((ps.wq_sampled - prevPerfStats->wq_sampled) / a) * scaleResultsBy;
                            combinedStats.fInUsePercent = ((ps.time - prevPerfStats->time) / elapsed) * scaleResultsBy;
                            //DbgTrace ("maybeaveq = %f", (double) (ps.wq_sampled - prevPerfStats->wq_sampled) / a);
                            //DbgTrace ("a=%f", a);
                            //DbgTrace ("a=%f", a);
                            //DbgTrace ("***ps.wq_time (%lld) - prevPerfStats->wq_time(%lld) = %lld  AND ps.wq_sampled (%lld) - prevPerfStats->wq_sampled(%lld) = %lld",
                            //          ps.wq_time, prevPerfStats->wq_time, ps.wq_time - prevPerfStats->wq_time,
                            //          ps.wq_sampled, prevPerfStats->wq_sampled, ps.wq_sampled - prevPerfStats->wq_sampled
                            //         );
                            //combinedStats.fQLength = (static_cast<double> (ps.wq_sampled - prevPerfStats->wq_sampled) / static_cast<double> (ps.wq_time - prevPerfStats->wq_time)) * scaleResultsBy;
                        }
                        for (String mountPt : diskAndVols.fValue) {
                            if (Optional<VolumeInfo> vi = volMap.Lookup (mountPt)) {
                                VolumeInfo tmp = *vi;
                                tmp.fReadIOStats = readStats;
                                tmp.fWriteIOStats = writeStats;
                                tmp.fCombinedIOStats = combinedStats;
                                volMap.Add (mountPt, tmp);
                            }
                        }
                    }
                    fContextDiskName2PerfStats_.Add (diskAndVols.fKey, ps);
                }
            }
            *volumes = Sequence<VolumeInfo> { volMap.Values () };
        }

    private:
        using   Disk2MountPointsMapType_ = Mapping<String, Set<String>>;
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
        Sequence<VolumeInfo> capture ()
        {
            Execution::SleepUntil (fPostponeCaptureUntil_);
            return capture_ ();
        }
        Sequence<VolumeInfo> capture_ ()
        {
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
            Debug::TraceContextBumper ctx ("Instruments::Filesystem...CapturerWithContext_Linux_::capture_");
#endif
            Sequence<VolumeInfo>   results;

            constexpr   bool    kUseProcFSForMounts_ { true };
            if (kUseProcFSForMounts_) {
                results = ReadVolumesAndUsageFromProcMountsAndstatvfs_ ();
            }
            else {
                results = RunDF_ ();
            }
            ApplyDiskTypes_ (&results);
            if (not fOptions_.fIncludeTemporaryDevices) {
                for (Iterator<VolumeInfo> i = results.begin (); i != results.end (); ++i) {
                    if (i->fMountedDeviceType == MountedDeviceType::eTemporaryFiles) {
                        results.Remove (i);
                    }
                }
            }
            if (not fOptions_.fIncludeSystemDevices) {
                for (Iterator<VolumeInfo> i = results.begin (); i != results.end (); ++i) {
                    if (i->fMountedDeviceType == MountedDeviceType::eSystemInformation) {
                        results.Remove (i);
                    }
                }
            }
            if (fOptions_.fIOStatistics) {
                ReadAndApplyProcFS_diskstats_ (&results);
            }
            _NoteCompletedCapture ();
            return results;
        }
    private:
        Sequence<VolumeInfo>    ReadVolumesAndUsageFromProcMountsAndstatvfs_ ()
        {
            Sequence<VolumeInfo>    result;
            for (MountInfo_ mi : ReadMountInfo_ ()) {
                VolumeInfo  vi;
                vi.fMountedOnName = mi.fMountedOn;
                if (not mi.fDeviceName.empty () and mi.fDeviceName != L"none") {    // special name none often used when there is no name
                    vi.fDeviceOrVolumeName = mi.fDeviceName;
                }
                vi.fFileSystemType = mi.fFilesystemFormat;
                UpdateVolumneInfo_statvfs (&vi);
                result.Append (vi);
            }
            return result;
        }
    private:
        void    UpdateVolumneInfo_statvfs (VolumeInfo* v)
        {
            RequireNotNull (v);
            struct  statvfs sbuf;
            memset (&sbuf, 0, sizeof (sbuf));
            if (::statvfs (v->fMountedOnName.AsNarrowSDKString ().c_str (), &sbuf) == 0) {
                uint64_t    diskSize = sbuf.f_bsize * sbuf.f_blocks;
                v->fSizeInBytes = diskSize;
                v->fAvailableSizeInBytes = sbuf.f_bsize * sbuf.f_bavail;
                v->fUsedSizeInBytes = diskSize - sbuf.f_bsize * sbuf.f_bfree;
            }
            else {
                DbgTrace (L"statvfs (%s) return error: errno=%d", v->fMountedOnName.c_str (), errno);
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
        void    ReadAndApplyProcFS_diskstats_ (Sequence<VolumeInfo>* volumes)
        {
            try {
                Mapping<dev_t, PerfStats_>  diskStats = ReadProcFS_diskstats_ ();
                DurationSecondsType         timeSinceLastMeasure = Time::GetTickCount () - GetLastCaptureAt ();
                for (Iterator<VolumeInfo> i = volumes->begin (); i != volumes->end (); ++i) {
                    VolumeInfo vi = *i;
                    if (vi.fDeviceOrVolumeName.IsPresent ()) {
                        if (fContextStats_) {
                            String  devNameLessSlashes = *vi.fDeviceOrVolumeName;
                            size_t i = devNameLessSlashes.RFind ('/');
                            if (i != string::npos) {
                                devNameLessSlashes = devNameLessSlashes.SubString (i + 1);
                            }
                            dev_t   useDevT;
                            {
                                struct stat sbuf;
                                memset (&sbuf, 0, sizeof (sbuf));
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
                                VolumeInfo::IOStats readStats;
                                readStats.fBytesTransfered = (oNew->fSectorsRead - oOld->fSectorsRead) * sectorSizeTmpHack;
                                readStats.fTotalTransfers = oNew->fReadsCompleted - oOld->fReadsCompleted;
                                readStats.fQLength = (oNew->fTimeSpentReading - oOld->fTimeSpentReading) / timeSinceLastMeasure;

                                VolumeInfo::IOStats writeStats;
                                writeStats.fBytesTransfered = (oNew->fSectorsWritten - oOld->fSectorsWritten) * sectorSizeTmpHack;
                                writeStats.fTotalTransfers = oNew->fWritesCompleted - oOld->fWritesCompleted;
                                writeStats.fQLength = (oNew->fTimeSpentWriting - oOld->fTimeSpentWriting) / timeSinceLastMeasure;

                                VolumeInfo::IOStats combinedStats;
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
                    volumes->Update (i, vi);
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
        Sequence<VolumeInfo> RunDF_POSIX_ ()
        {
            Sequence<VolumeInfo>   result;
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
                VolumeInfo v;
                v.fMountedOnName = l[5].Trim ();
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
                result.Append (v);
            }
            // Sometimes (with busy box df especailly) we get bogus error return. So only rethrow if we found no good data
            if (runException and result.empty ()) {
                Execution::DoReThrow (runException);
            }
            return result;
        }
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
                v.fSizeInBytes = Characters::String2Float<double> (l[includeFSTypes ? 2 : 1]) * 1024;
                v.fUsedSizeInBytes = Characters::String2Float<double> (l[includeFSTypes ? 3 : 2]) * 1024;
                v.fAvailableSizeInBytes = *v.fSizeInBytes - *v.fUsedSizeInBytes;
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
        Sequence<VolumeInfo> capture ()
        {
            Execution::SleepUntil (fPostponeCaptureUntil_);
            return capture_ ();
        }
        Sequence<VolumeInfo> capture_ ()
        {
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
            Debug::TraceContextBumper ctx ("Instruments::Filesystem...CapturerWithContext_Windows_::capture_");
#endif
            Sequence<VolumeInfo>   results;
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
                            auto safePctInUse2QL_ = [] (double pctInUse) {
                                // %InUse = QL / (1 + QL).
                                pctInUse /= 100;
                                pctInUse = Math::PinInRange<double> (pctInUse, 0, 1);
                                return pctInUse / (1 - pctInUse);
                            };
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
                                    v.fSizeInBytes = static_cast<double> (totalNumberOfBytes.QuadPart);
                                    v.fUsedSizeInBytes = *v.fSizeInBytes  - freeBytesAvailable.QuadPart;
                                    v.fAvailableSizeInBytes = *v.fSizeInBytes - *v.fUsedSizeInBytes;
#if     qUseWMICollectionSupport_
                                    if (fOptions_.fIOStatistics) {
                                        String wmiInstanceName = v.fMountedOnName.RTrim ([] (Characters::Character c) { return c == '\\'; });
                                        fLogicalDiskWMICollector_.AddInstancesIf (wmiInstanceName);

                                        VolumeInfo::IOStats readStats;
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

                                        VolumeInfo::IOStats writeStats;
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

                                        VolumeInfo::IOStats combinedStats = readStats;
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
        Sequence<VolumeInfo> capture ()
        {
            lock_guard<const AssertExternallySynchronizedLock> critSec { *this };
            Debug::TraceContextBumper ctx ("Instruments::Filesystem capture");
            return inherited::capture ();
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
    using   StructureFieldInfo = ObjectVariantMapper::StructureFieldInfo;
    static  const   ObjectVariantMapper sMapper_ = [] () -> ObjectVariantMapper {
        ObjectVariantMapper mapper;
        mapper.AddCommonType<Optional<double>> ();
        mapper.AddCommonType<Optional<String>> ();
        mapper.Add (mapper.MakeCommonSerializer_NamedEnumerations<MountedDeviceType> (Stroika_Enum_Names(MountedDeviceType)));
        mapper.AddCommonType<Optional<MountedDeviceType>> ();
        DISABLE_COMPILER_CLANG_WARNING_START("clang diagnostic ignored \"-Winvalid-offsetof\"");   // Really probably an issue, but not to debug here -- LGP 2014-01-04
        DISABLE_COMPILER_GCC_WARNING_START("GCC diagnostic ignored \"-Winvalid-offsetof\"");       // Really probably an issue, but not to debug here -- LGP 2014-01-04
        mapper.AddClass<VolumeInfo::IOStats> (initializer_list<StructureFieldInfo> {
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (VolumeInfo::IOStats, fBytesTransfered), String_Constant (L"Bytes"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (VolumeInfo::IOStats, fQLength), String_Constant (L"Q-Length"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (VolumeInfo::IOStats, fInUsePercent), String_Constant (L"In-Use-%"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (VolumeInfo::IOStats, fTotalTransfers), String_Constant (L"Total-Transfers"), StructureFieldInfo::NullFieldHandling::eOmit },
        });
        mapper.AddCommonType<Optional<VolumeInfo::IOStats>> ();
        mapper.AddClass<VolumeInfo> (initializer_list<StructureFieldInfo> {
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (VolumeInfo, fMountedDeviceType), String_Constant (L"Mounted-Device-Type"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (VolumeInfo, fFileSystemType), String_Constant (L"Filesystem-Type"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (VolumeInfo, fDeviceOrVolumeName), String_Constant (L"Device-Name"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (VolumeInfo, fVolumeID), String_Constant (L"Volume-ID"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (VolumeInfo, fMountedOnName), String_Constant (L"Mounted-On") },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (VolumeInfo, fSizeInBytes), String_Constant (L"Volume-Total-Size"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (VolumeInfo, fAvailableSizeInBytes), String_Constant (L"Volume-Available-Size"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (VolumeInfo, fUsedSizeInBytes), String_Constant (L"Volume-Used-Size"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (VolumeInfo, fReadIOStats), String_Constant (L"Read-IO-Stats"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (VolumeInfo, fWriteIOStats), String_Constant (L"Write-IO-Stats"), StructureFieldInfo::NullFieldHandling::eOmit  },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (VolumeInfo, fCombinedIOStats), String_Constant (L"Combined-IO-Stats"), StructureFieldInfo::NullFieldHandling::eOmit  },

        });
        DISABLE_COMPILER_GCC_WARNING_END("GCC diagnostic ignored \"-Winvalid-offsetof\"");
        DISABLE_COMPILER_CLANG_WARNING_END("clang diagnostic ignored \"-Winvalid-offsetof\"");
        mapper.AddCommonType<Collection<VolumeInfo>> ();
        mapper.AddCommonType<Sequence<VolumeInfo>> ();
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
        virtual MeasurementSet  Capture ()
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

