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
 *
 *  TODO:
 *      @todo   Fix handling of fMinimumAveragingInterval to do on second capture - and do that by setting a 'wait until', rather than
 *              waiting initially
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
                     *
                     */
                    struct  VolumeInfo {
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
                         *      if == 0, skip initial capture, and if > 0, do initial capture and wait that long before second capture
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
