/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Framework_SystemPerformance_Instruments_MountedFilesystemUsage_h_
#define _Stroika_Framework_SystemPerformance_Instruments_MountedFilesystemUsage_h_ 1

#include    "../../StroikaPreComp.h"

#include    "../../../Foundation/DataExchange/ObjectVariantMapper.h"
#include    "../../../Foundation/Memory/Optional.h"
#include    "../../../Foundation/Time/Realtime.h"

#include    "../Instrument.h"



/*
 *  \file
 *
 *  \version    <a href="code_status.html#Late-Alpha">Late-Alpha</a>
 *
 */



namespace   Stroika {
    namespace   Frameworks {
        namespace   SystemPerformance {
            namespace   Instruments {


                namespace   MountedFilesystemUsage {


                    using   Memory::Optional;
                    using   DataExchange::ObjectVariantMapper;


                    /**
                    */
                    enum    DriveType {
                        /**
                         *  On Windoze, corresponds to https://msdn.microsoft.com/en-us/library/aa394173%28v=vs.85%29.aspx?f=255&MSPPError=-2147217396 "Removable Disk"
                         */
                        eRemovableDisk,

                        /**
                         *  On Windoze, corresponds to https://msdn.microsoft.com/en-us/library/aa394173%28v=vs.85%29.aspx?f=255&MSPPError=-2147217396 "Local Disk"
                         */
                        eLocalDisk,

                        /**
                         *  On Windoze, corresponds to https://msdn.microsoft.com/en-us/library/aa394173%28v=vs.85%29.aspx?f=255&MSPPError=-2147217396 "Network Drive"
                         */
                        eNetworkDrive,

                        /**
                         *  On Windoze, corresponds to https://msdn.microsoft.com/en-us/library/aa394173%28v=vs.85%29.aspx?f=255&MSPPError=-2147217396 "RAM Disk"
                         *  On Linux, this is tmpfs
                         */
                        eTemporaryFiles,

                        /**
                         *  On Windoze, corresponds to https://msdn.microsoft.com/en-us/library/aa394173%28v=vs.85%29.aspx?f=255&MSPPError=-2147217396 "Compact Disc"
                         */
                        eReadOnlyEjectable,

                        /**
                         *  e.g. procfs
                         */
                        eSystemInformation,

                        Stroika_Define_Enum_Bounds(eRemovableDisk, eSystemInformation)
                    };


                    /**
                     *  \note   These print names are mostly for display and debugging purposes, and they are not gauranteed to be safe for
                     *          persistence (so be sure to version).
                     */
                    constexpr   Configuration::EnumNames<DriveType>    Stroika_Enum_Names(DriveType)
                    {
                        Configuration::EnumNames<DriveType>::BasicArrayInitializer {
                            {
                                { DriveType::eRemovableDisk, L"Removable-Disk" },
                                { DriveType::eLocalDisk, L"Local-Disk" },
                                { DriveType::eNetworkDrive, L"Network-Drive" },
                                { DriveType::eTemporaryFiles, L"Temporary-Files" },
                                { DriveType::eReadOnlyEjectable, L"Read-Only-Ejectable" },
                                { DriveType::eSystemInformation, L"System-Information" },
                            }
                        }
                    };


                    /**
                     *
                     */
                    struct  VolumeInfo {
                        Optional<DriveType> fDriveType;
                        Optional<String>    fFileSystemType;
                        Optional<String>    fDeviceOrVolumeName;
                        Optional<String>    fVolumeID;
                        String              fMountedOnName;
                        Optional<double>    fDiskSizeInBytes;
                        Optional<double>    fUsedSizeInBytes;

                        /**
                         *  IOStats represents the # of bytes (fBytesTransfered) and total number of transfers
                         *  (fTotalTransfers) during the given capture interval. It is NOT cummulative.
                         *
                         *  fTimeTransfering is the number of seconds spent doing the transfers.
                         *  From https://www.kernel.org/doc/Documentation/iostats.txt:
                         *      This is the total number of milliseconds spent by all [sic] (as
                         *      measured from __make_request() to end_that_request_last()).
                         *
                         *  The reason fIOStats is returned redundantly, is because some system may only be able
                         *  to report totals, and not read/write breakdown.
                         */
                        struct  IOStats {
                            Optional<double>    fBytesTransfered;
                            Optional<double>    fTotalTransfers;
                            Optional<double>    fTimeTransfering;
                        };
                        IOStats fReadIOStats;
                        IOStats fWriteIOStats;
                        IOStats fIOStats;
                    };


                    /**
                     *  For VolumeInfo, Collection<VolumeInfo>, and Sequence<VolumeInfo> types.
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
                    };


                    /**
                     *  Instrument returning Sequence<VolumeInfo> measurements (cross-platform).
                     */
                    Instrument          GetInstrument (Options options = Options ());


                }


            }
        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#endif  /*_Stroika_Framework_SystemPerformance_Instruments_MountedFilesystemUsage_h_*/
