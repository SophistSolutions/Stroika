/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
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
                     *
                     */
                    struct  VolumeInfo {
                        Optional<String>    fFileSystemType;
                        Optional<String>    fDeviceOrVolumeName;
                        Optional<String>    fVolumeID;
                        String              fMountedOnName;
                        Optional<double>    fDiskSizeInBytes;
                        Optional<double>    fUsedSizeInBytes;
                    };


                    /**
                     *  For VolumeInfo, Collection<VolumeInfo>, and Sequence<VolumeInfo> types.
                     */
                    ObjectVariantMapper GetObjectVariantMapper ();


                    /**
                     *  Instrument returning Sequence<VolumeInfo> measurements (cross-platform).
                     */
                    Instrument          GetInstrument ();


                }


                _DeprecatedFunction_ (Instrument  GetMountedFilesystemUsage (), "Deprecated in v2.0a36; instead use MountedFilesystemUsage::GetInstrument ()");


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
