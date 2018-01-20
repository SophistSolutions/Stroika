/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Framework_SystemPerformance_Instruments_Filesystem_h_
#define _Stroika_Framework_SystemPerformance_Instruments_Filesystem_h_ 1

#include "../../StroikaPreComp.h"

#include "../../../Foundation/Containers/Mapping.h"
#include "../../../Foundation/DataExchange/ObjectVariantMapper.h"
#include "../../../Foundation/IO/FileSystem/Disk.h"
#include "../../../Foundation/IO/FileSystem/MountedFilesystem.h"
#include "../../../Foundation/Memory/Optional.h"
#include "../../../Foundation/Time/Realtime.h"

#include "../Instrument.h"

/*
 *  \file
 *
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 *
 *  TODO:
 *      @todo   add optional Options filter 'Set<String> fRestrictToVolumnesContainingPaths', or a regexp to match or
 *              a functional<>.
 *
 *      @todo   Support option to return information about unmounted filesystems (as on our AWS machine).
 *              Stuff like systempartition? Probs not important though.
 */

namespace Stroika {
    namespace Frameworks {
        namespace SystemPerformance {
            namespace Instruments {
                namespace Filesystem {

                    using DataExchange::ObjectVariantMapper;
                    using Memory::Optional;

                    using IO::FileSystem::BlockDeviceKind;

                    /**
                     *  IOStats represents the # of bytes (fBytesTransfered) and total number of transfers
                     *  (fTotalTransfers) during the given capture interval. It is NOT cummulative.
                     *
                     *  Frequently you will have per read/write bytes transfered, but only Q-Length for the entire device (combined).
                     */
                    struct IOStatsType {
                        Optional<double> fBytesTransfered;
                        Optional<double> fTotalTransfers;
                        Optional<double> fQLength;
                        Optional<double> fInUsePercent;

                        /**
                         *  If InUse Percent is not known, it can be approximated from he Q-Length
                         */
                        nonvirtual Optional<double> EstimatedPercentInUse () const;

                        /**
                         *  @see Characters::ToString ();
                         */
                        nonvirtual String ToString () const;
                    };

                    /**
                     *  Disk names are strings, but this type alias is used for documentation.
                     */
                    using DynamicDiskIDType = String;

                    /**
                     *  Filesytem mount points are strings, but this type alias is used for documentation.
                     */
                    using MountedFilesystemNameType = String;

                    /**
                     *
                     */
                    struct DiskInfoType {
                        /*
                         *  This is a UNIQUE ID scribbled onto the disk itself, like
                         *  \\\\?\\Volume{e99304fe-4c5d-11e4-824c-806e6f6e6963}\\ This could be used to track when a disk is moved
                         *  from one SATA or SCSI address to another.
                         */
                        Optional<String> fPersistenceVolumeID;

                        /*
                         *  Is the 'disk' a 'remote' device (network),  CD-ROM, direct-attached hard disk (e.g. internal) or removable drive,
                         */
                        Optional<BlockDeviceKind> fDeviceKind;

                        /*
                         *  This is the size of the physical block device. All the filesystems must fit in it.
                         */
                        Optional<uint64_t> fSizeInBytes;

                        /**
                         *  The reason fCombinedIOStats is returned redundantly, is because some system may only be able
                         *  to report totals, and not read/write breakdown. It is the same as Read + Write stats (if all available)
                         *
                         *  Frequently you will have per read/write bytes transfered, but only Q-Length for the entire device (combined).
                         */
                        Optional<IOStatsType> fReadIOStats;
                        Optional<IOStatsType> fWriteIOStats;
                        Optional<IOStatsType> fCombinedIOStats;

                        /**
                         *  @see Characters::ToString ();
                         */
                        nonvirtual String ToString () const;
                    };

                    /**
                     *  A volume is analagous to a Windows Volume (@see ) or a unix Filesystem (@see).
                     *
                     *  In UNIX, a filesystem has only a single point point, where as in windows, it CAN have multiple (we dont
                     *  currently model that, but we could make mount point be a set).
                     */
                    struct MountedFilesystemInfoType {
                        /*
                         *  A volume is typically mounted on a single physical drive, but in some circumstances, on some operating
                         *  systems, it can span multiple drives (e.g. RAID5).
                         */
                        Optional<Set<DynamicDiskIDType>> fOnPhysicalDrive;

                        /*
                         *  @todo - sb in physical drive only? But for windows, we often (if not running as admin) cannot see physical drive info?
                         *
                         *  \note   this already is in the IO::FileSystem::DiskInfoType information, but there maybe some cases for which this value works better
                         *          for the time being.
                         */
                        Optional<BlockDeviceKind> fDeviceKind;

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
                        Optional<String> fFileSystemType;

                        /**
                         *  @todo document
                         */
                        Optional<String> fDeviceOrVolumeName;

                        /**
                         *  @todo document
                         */
                        Optional<String> fVolumeID;

                        /**
                         */
                        Optional<uint64_t> fSizeInBytes;

                        /**
                         *  Available + used need not add up to sizeInBytes, as on some OSes, in some circumstances
                         *  (like UNIX not running as root) keep some percentage 'reserved'.
                         */
                        Optional<uint64_t> fAvailableSizeInBytes;

                        /**
                         */
                        Optional<uint64_t> fUsedSizeInBytes;

                        /**
                         *  The reason fCombinedIOStats is returned redundantly, is because some system may only be able
                         *  to report totals, and not read/write breakdown. It is the same as Read + Write stats (if all available)
                         */
                        Optional<IOStatsType> fReadIOStats;
                        Optional<IOStatsType> fWriteIOStats;
                        Optional<IOStatsType> fCombinedIOStats;

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
                     *  For MountedFilesystemInfoType, etc types.
                     */
                    ObjectVariantMapper GetObjectVariantMapper ();

                    /**
                     *  To control the behavior of the instrument.
                     */
                    struct Options {
                        /**
                         *  \req fMinimumAveragingInterval >= 0
                         */
                        Time::DurationSecondsType fMinimumAveragingInterval{1.0};

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
                         *  On unix, there are many 'fake' disks, like 'procfs'. For many purposes, this are uninteresting to list/query.
                         */
                        bool fIncludeSystemDevices{true};

                        /**
                         *
                         */
                        bool fEstimateFilesystemStatsFromDiskStatsIfHelpful{false};
                    };

                    /**
                     *  Instrument returning Info object (cross-platform).
                     */
                    Instrument GetInstrument (Options options = Options ());
                }
            }

            /*
             *  Specialization to improve performance
             */
            template <>
            Instruments::Filesystem::Info Instrument::CaptureOneMeasurement (Range<DurationSecondsType>* measurementTimeOut);
        }
    }
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#endif /*_Stroika_Framework_SystemPerformance_Instruments_Filesystem_h_*/
