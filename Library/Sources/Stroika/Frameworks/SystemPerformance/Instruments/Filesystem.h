/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Framework_SystemPerformance_Instruments_Filesystem_h_
#define _Stroika_Framework_SystemPerformance_Instruments_Filesystem_h_ 1

#include "Stroika/Frameworks/StroikaPreComp.h"

#include <filesystem>
#include <optional>

#include "Stroika/Foundation/Containers/Mapping.h"
#include "Stroika/Foundation/DataExchange/ObjectVariantMapper.h"
#include "Stroika/Foundation/IO/FileSystem/Disk.h"
#include "Stroika/Foundation/IO/FileSystem/MountedFilesystem.h"
#include "Stroika/Foundation/Time/Realtime.h"
#include "Stroika/Frameworks/SystemPerformance/Instrument.h"

/*
 *  \file
 *
 *  \note Code-Status:  <a href="Code-Status.md#Beta">Beta</a>
 *
 *  TODO:
 *      @todo   add optional Options filter 'Set<String> fRestrictToVolumnesContainingPaths', or a regexp to match or
 *              a functional<>.
 *
 *      @todo   Support option to return information about unmounted filesystems (as on our AWS machine).
 *              Stuff like systempartition? Probs not important though.
 */

namespace Stroika::Frameworks::SystemPerformance::Instruments::Filesystem {

    using DataExchange::ObjectVariantMapper;

    using IO::FileSystem::BlockDeviceKind;

    /**
     *  IOStats represents the # of bytes (fBytesTransfered) and total number of transfers
     *  (fTotalTransfers) during the given capture interval. It is NOT cummulative.
     *
     *  Frequently you will have per read/write bytes transfered, but only Q-Length for the entire device (combined).
     */
    struct IOStatsType {
        optional<double> fBytesTransfered;
        optional<double> fTotalTransfers;
        optional<double> fQLength;
        optional<double> fInUsePercent;

        /**
         *  If InUse Percent is not known, it can be approximated from he Q-Length
         */
        nonvirtual optional<double> EstimatedPercentInUse () const;

        /**
         *  @see Characters::ToString ();
         */
        nonvirtual String ToString () const;
    };

    /**
     *  Disk names are filesystem::path (were strings until Stroika v2.1b2), but this type alias is used for documentation.
     */
    using DynamicDiskIDType = filesystem::path;

    /**
     *  Filesytem mount points are filesystem::paths (used to be strings), but this type alias is used for documentation.
     */
    using MountedFilesystemNameType = filesystem::path;

    /**
     *  \todo This is NYI, for Linux (http://stroika-bugs.sophists.com/browse/STK-733)
     */
    struct DiskInfoType {
        /*
         *  This is a UNIQUE ID scribbled onto the disk itself, like
         *  \\\\?\\Volume{e99304fe-4c5d-11e4-824c-806e6f6e6963}\\ This could be used to track when a disk is moved
         *  from one SATA or SCSI address to another.
         */
        optional<String> fPersistenceVolumeID;

        /*
         *  Is the 'disk' a 'remote' device (network),  CD-ROM, direct-attached hard disk (e.g. internal) or removable drive,
         */
        optional<BlockDeviceKind> fDeviceKind;

        /*
         *  This is the size of the physical block device. All the filesystems must fit in it.
         */
        optional<uint64_t> fSizeInBytes;

        /**
         *  The reason fCombinedIOStats is returned redundantly, is because some system may only be able
         *  to report totals, and not read/write breakdown. It is the same as Read + Write stats (if all available)
         *
         *  Frequently you will have per read/write bytes transfered, but only Q-Length for the entire device (combined).
         */
        optional<IOStatsType> fReadIOStats;
        optional<IOStatsType> fWriteIOStats;
        optional<IOStatsType> fCombinedIOStats;

        /**
         *  @see Characters::ToString ();
         */
        nonvirtual String ToString () const;
    };

    /**
     *  A volume is analagous to a Windows Volume (@see ) or a unix Filesystem (@see).
     *
     *  In UNIX, a filesystem has only a single point point, where as in windows, it CAN have multiple (we don't
     *  currently model that, but we could make mount point be a set).
     */
    struct MountedFilesystemInfoType {
        /*
         *  A volume is typically mounted on a single physical drive, but in some circumstances, on some operating
         *  systems, it can span multiple drives (e.g. RAID5).
         */
        optional<Set<DynamicDiskIDType>> fOnPhysicalDrive;

        /*
         *  @todo - sb in physical drive only? But for windows, we often (if not running as admin) cannot see physical drive info?
         *
         *  \note   this already is in the IO::FileSystem::DiskInfoType information, but there maybe some cases for which this value works better
         *          for the time being.
         */
        optional<BlockDeviceKind> fDeviceKind;

        /**
         *  This is an open enumeration indicating the format of the given filesystem:
         *
         *  Common values include:
         *         o    "ext2"
         *         o    "ext4"
         *         o    "jfs2"
         *         o    "nfs"
         *         o    "nfs3"
         *         o    "vboxsf"
         *         o    "NTFS"
         *         o    "procfs"
         */
        optional<String> fFileSystemType;

        /**
         *  @todo document
         */
        optional<String> fDeviceOrVolumeName;

        /**
         *  @todo document
         */
        optional<String> fVolumeID;

        /**
         */
        optional<uint64_t> fSizeInBytes;

        /**
         *  Available + used need not add up to sizeInBytes, as on some OSes, in some circumstances
         *  (like UNIX not running as root) keep some percentage 'reserved'.
         */
        optional<uint64_t> fAvailableSizeInBytes;

        /**
         */
        optional<uint64_t> fUsedSizeInBytes;

        /**
         *  The reason fCombinedIOStats is returned redundantly, is because some system may only be able
         *  to report totals, and not read/write breakdown. It is the same as Read + Write stats (if all available)
         */
        optional<IOStatsType> fReadIOStats;
        optional<IOStatsType> fWriteIOStats;
        optional<IOStatsType> fCombinedIOStats;

        /**
         *  @see Characters::ToString ();
         */
        nonvirtual String ToString () const;
    };

    /**
     *  Basic type returned by a capture() from the instrument.
     *
     *  \note   The relationship between filesystems and disks is many to many (a disk contains
     *          many filesystems, and a filesystem can span many disk). Though OFTEN you will
     *          find something more like one or two filesystems per disk: spanning is more rare.
     */
    struct Info {
        /**
         *  The key for the fDisks list is a UNIQUE (at a time) ID for the physical disk volume. It corresponds
         *  to something like the 'sda' for /dev/sda block device in UNIX.
         */
        Containers::Mapping<DynamicDiskIDType, DiskInfoType> fDisks;

        /**
         */
        Containers::Mapping<MountedFilesystemNameType, MountedFilesystemInfoType> fMountedFilesystems;

        /**
         *  @see Characters::ToString ();
         */
        nonvirtual String ToString () const;
    };

    /**
     *  To control the behavior of the instrument.
     */
    struct Options {
        /**
         *  To compute averages, the instrument may keep around some earlier snapshots of data. This time interval is regulated by how often
         *  the capture is called (typically the Captureset::'run interval'. However, this value can be used to override that partly, and provide
         *  a minimum time for averaging.
         * 
         *  If you call capture more frequently than this interval, some (averaged) items maybe missing from the result.
         *
         *  \req fMinimumAveragingInterval > 0
         */
        Time::DurationSeconds fMinimumAveragingInterval{1.0s};

        /**
         */
        bool fDiskspaceUsage{true};

        /**
         */
        bool fIOStatistics{true};

        /**
         *  Include 'ram disks' - that are intended to store temporary files a short period (BlockDeviceKind::eTemporaryFiles)
         */
        bool fIncludeTemporaryDevices{true};

        /**
         *  On unix, there are many 'fake' disks, like 'procfs'. For many purposes, these are uninteresting to list/query.
         */
        bool fIncludeSystemDevices{true};

        /**
         *  On some operating systems, we can gather performance statistics only for raw disks, and not for individual filesystems (I think AIX worked that way).
         *  Turning this true will apply the disk stats as a GUESS (proportional to size) to filesystem IO stats.
         */
        bool fEstimateFilesystemStatsFromDiskStatsIfHelpful{false};
    };

    /**
     *  This class is designed to be object-sliced into just the SystemPerformance::Instrument
     * 
     *  \note Constructing the instrument does no capturing (so sb quick/cheap) - capturing starts when you
     *        first call i.Capture()
     * 
     *  \note   On Windows, Physical Disk info is only captured if running as administrator (not dug into why, maybe peculiarity of system settings)
     */
    struct Instrument : SystemPerformance::Instrument {
    public:
        Instrument (const Options& options = Options{});

    public:
        /**
         *  For Instruments::Filesystem::Info, MountedFilesystemInfoType, etc types.
         */
        static const ObjectVariantMapper kObjectVariantMapper;
    };

}

namespace Stroika::Frameworks::SystemPerformance {
    /*
     *  Specialization to improve performance
     */
    template <>
    Instruments::Filesystem::Info Instrument::CaptureOneMeasurement (Range<TimePointSeconds>* measurementTimeOut);
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#endif /*_Stroika_Framework_SystemPerformance_Instruments_Filesystem_h_*/
