/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Framework_SystemPerformance_Instruments_Filesystem_h_
#define _Stroika_Framework_SystemPerformance_Instruments_Filesystem_h_ 1

#include    "../../StroikaPreComp.h"

#include    "../../../Foundation/Containers/Mapping.h"
#include    "../../../Foundation/DataExchange/ObjectVariantMapper.h"
#include    "../../../Foundation/Memory/Optional.h"
#include    "../../../Foundation/Time/Realtime.h"

#include    "../Instrument.h"



/*
 *  \file
 *
 *  \version    <a href="code_status.html#Late-Alpha">Late-Alpha</a>
 *
 *  TODO:
 *      @todo   add optional Options filter 'Set<String> fRestrictToVolumnesContainingPaths', or a regexp to match or
 *              a functional<>.
 *
 */



namespace   Stroika {
    namespace   Frameworks {
        namespace   SystemPerformance {
            namespace   Instruments {
                namespace   Filesystem {


                    using   Memory::Optional;
                    using   DataExchange::ObjectVariantMapper;


                    /**
                    */
                    enum    BlockDeviceKind {
                        /**
                         *  On Windoze, corresponds to https://msdn.microsoft.com/en-us/library/aa394173%28v=vs.85%29.aspx?f=255&MSPPError=-2147217396 "Removable Disk" or
                         *  https://msdn.microsoft.com/en-us/library/windows/desktop/aa364939%28v=vs.85%29.aspx DRIVE_REMOVABLE
                         */
                        eRemovableDisk,

                        /**
                         *  On Windoze, corresponds to https://msdn.microsoft.com/en-us/library/aa394173%28v=vs.85%29.aspx?f=255&MSPPError=-2147217396 "Local Disk" or
                         *  https://msdn.microsoft.com/en-us/library/windows/desktop/aa364939%28v=vs.85%29.aspx DRIVE_FIXED
                         */
                        eLocalDisk,

                        /**
                         *  On Windoze, corresponds to https://msdn.microsoft.com/en-us/library/aa394173%28v=vs.85%29.aspx?f=255&MSPPError=-2147217396 "Network Drive" or
                         *  https://msdn.microsoft.com/en-us/library/windows/desktop/aa364939%28v=vs.85%29.aspx DRIVE_REMOTE
                         */
                        eNetworkDrive,

                        /**
                         *  On Windoze, corresponds to https://msdn.microsoft.com/en-us/library/aa394173%28v=vs.85%29.aspx?f=255&MSPPError=-2147217396 "RAM Disk" or
                         *  https://msdn.microsoft.com/en-us/library/windows/desktop/aa364939%28v=vs.85%29.aspx DRIVE_RAMDISK
                         *  On Linux, this is tmpfs
                         */
                        eTemporaryFiles,

                        /**
                         *  On Windoze, corresponds to https://msdn.microsoft.com/en-us/library/aa394173%28v=vs.85%29.aspx?f=255&MSPPError=-2147217396 "Compact Disc" or
                         *  https://msdn.microsoft.com/en-us/library/windows/desktop/aa364939%28v=vs.85%29.aspx DRIVE_CDROM
                         */
                        eReadOnlyEjectable,

                        /**
                         *  e.g. Linux procfs
                         */
                        eSystemInformation,

                        Stroika_Define_Enum_Bounds(eRemovableDisk, eSystemInformation)
                    };


                    /**
                     *  \note   These print names are mostly for display and debugging purposes, and they are not gauranteed to be safe for
                     *          persistence (so be sure to version).
                     */
                    constexpr   Configuration::EnumNames<BlockDeviceKind>    Stroika_Enum_Names(BlockDeviceKind)
                    {
                        Configuration::EnumNames<BlockDeviceKind>::BasicArrayInitializer {
                            {
                                { BlockDeviceKind::eRemovableDisk, L"Removable-Disk" },
                                { BlockDeviceKind::eLocalDisk, L"Local-Disk" },
                                { BlockDeviceKind::eNetworkDrive, L"Network-Drive" },
                                { BlockDeviceKind::eTemporaryFiles, L"Temporary-Files" },
                                { BlockDeviceKind::eReadOnlyEjectable, L"Read-Only-Ejectable" },
                                { BlockDeviceKind::eSystemInformation, L"System-Information" },
                            }
                        }
                    };


                    /**
                     *  IOStats represents the # of bytes (fBytesTransfered) and total number of transfers
                     *  (fTotalTransfers) during the given capture interval. It is NOT cummulative.
                     *
                     *  Frequently you will have per read/write bytes transfered, but only Q-Length for the entire device (combined).
                     */
                    struct  IOStatsType {
                        Optional<double>    fBytesTransfered;
                        Optional<double>    fTotalTransfers;
                        Optional<double>    fQLength;
                        Optional<double>    fInUsePercent;

                        /**
                         *  If InUse Percent is not known, it can be approximated from he Q-Length
                         */
                        nonvirtual  Optional<double>    EstimatedPercentInUse () const;
                    };


                    /**
                     *
                     */
                    using   DynamicDiskIDType   =   String;


                    /**
                     *
                     */
                    using   MountedFilesystemNameType   =   String;

                    /**
                     *
                     */
                    struct  DiskInfoType {
                        /*
                         *  This is a UNIQUE ID scribbled onto the disk itself, like
                         *  \\\\?\\Volume{e99304fe-4c5d-11e4-824c-806e6f6e6963}\\ This could be used to track when a disk is moved
                         *  from one SATA or SCSI address to another.
                         */
                        Optional<String>            fPersistenceVolumeID;

                        /*
                         *  Is the 'disk' a 'remote' device (network),  CD-ROM, direct-attached hard disk (e.g. internal) or removable drive,
                         */
                        Optional<BlockDeviceKind>   fDeviceKind;

                        /*
                         *  This is the size of the physical block device. All the filesystems must fit in it.
                         */
                        Optional<uint64_t>          fSizeInBytes;

                        /**
                         *  The reason fCombinedIOStats is returned redundantly, is because some system may only be able
                         *  to report totals, and not read/write breakdown. It is the same as Read + Write stats (if all available)
                         *
                         *  Frequently you will have per read/write bytes transfered, but only Q-Length for the entire device (combined).
                         */
                        Optional<IOStatsType>   fReadIOStats;
                        Optional<IOStatsType>   fWriteIOStats;
                        Optional<IOStatsType>   fCombinedIOStats;
                    };


                    /**
                     *  A volume is analagous to a Windows Volume (@see ) or a unix Filesystem (@see).
                     *
                     *  In UNIX, a filesystem has only a single point point, where as in windows, it CAN have multiple (we dont
                     *  currently model that, but we could make mount point be a set).
                     */
                    struct  MountedFilesystemInfoType {
                        /*
                         *  A volume is typically mounted on a single physical drive, but in some circumstances, on some operating
                         *  systems, it can span multiple drives (e.g. RAID5).
                         */
                        Optional<Set<DynamicDiskIDType>>    fOnPhysicalDrive;

                        /*
                         *  @todo - sb in physical drive only? But for windows, we often (if not running as admin) cannot see physical drive info?
                         */
                        Optional<BlockDeviceKind>   fDeviceKind;

                        Optional<String>            fFileSystemType;
                        Optional<String>            fDeviceOrVolumeName;
                        Optional<String>            fVolumeID;
                        Optional<uint64_t>          fSizeInBytes;
                        Optional<uint64_t>          fAvailableSizeInBytes;
                        Optional<uint64_t>          fUsedSizeInBytes;

                        /**
                         *  The reason fCombinedIOStats is returned redundantly, is because some system may only be able
                         *  to report totals, and not read/write breakdown. It is the same as Read + Write stats (if all available)
                         */
                        Optional<IOStatsType>   fReadIOStats;
                        Optional<IOStatsType>   fWriteIOStats;
                        Optional<IOStatsType>   fCombinedIOStats;
                    };


                    /**
                     *  Basic type returned by a capture() from the instrument.
                     */
                    struct  Info {
                        /**
                         *  The key for the fDisks list is a UNIQUE (at a time) ID for the physical disk volume. It corresponds
                         *  to something like the 'sda' for /dev/sda block device in UNIX.
                         */
                        Containers::Mapping<DynamicDiskIDType, DiskInfoType>    fDisks;

                        /**
                         */
                        Containers::Mapping<MountedFilesystemNameType, MountedFilesystemInfoType>       fMountedFilesystems;

                        /**
                         *  Use the most specific information we have available, but if needed, go to the disk level and
                         *  return the appropriate estimated usage.
                         */
                        Optional<IOStatsType>   GetCombinedIOStats (const MountedFilesystemInfoType& volumeInfo) const;
                    };


                    /**
                     *  For MountedFilesystemInfoType, etc types.
                     */
                    ObjectVariantMapper GetObjectVariantMapper ();


                    /**
                     *  To control the behavior of the instrument.
                     */
                    struct  Options {
                        /**
                         *  \req fMinimumAveragingInterval >= 0
                         */
                        Time::DurationSecondsType   fMinimumAveragingInterval { 1.0 };

                        /**
                         */
                        bool    fDiskspaceUsage { true };

                        /**
                         */
                        bool    fIOStatistics { true };

                        /**
                         *  Include 'ram disks' - that are intended to store temporary files a short period (BlockDeviceKind::eTemporaryFiles)
                         */
                        bool    fIncludeTemporaryDevices { true };

                        /**
                         *  On unix, there are many 'fake' disks, like 'procfs'. For many purposes, this are uninteresting to list/query.
                         */
                        bool    fIncludeSystemDevices { true };
                    };


                    /**
                     *  Instrument returning Info object (cross-platform).
                     */
                    Instrument          GetInstrument (Options options = Options ());


                }
            }


            /*
             *  Specialization to improve performance
             */
            template    <>
            Instruments::Filesystem::Info   Instrument::CaptureOneMeasurement (Range<DurationSecondsType>* measurementTimeOut);


        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#endif  /*_Stroika_Framework_SystemPerformance_Instruments_Filesystem_h_*/
