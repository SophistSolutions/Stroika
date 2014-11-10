/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Framework_SystemPerformance_Instruments_NetworkInterfaces_h_
#define _Stroika_Framework_SystemPerformance_Instruments_NetworkInterfaces_h_ 1

#include    "../../StroikaPreComp.h"

#include    "../../../Foundation/DataExchange/ObjectVariantMapper.h"
#include    "../../../Foundation/Memory/Optional.h"

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


                namespace   NetworkInterfaces {


                    using   DataExchange::ObjectVariantMapper;
                    using   Foundation::Memory::Optional;


                    /**
                     */
                    struct  InterfaceInfo {
                        /**
                         *  For POSIX, this is the interface name (e.g. eth0)
                         *
                         *  For Windows, this is ???
                         */
                        String  fInterfaceID {};


                        /**
                         *  bytes
                         *  The total number of bytes of data transmitted or received by the interface.
                         */
                        Optional<uint64_t>  fTotalBytesSent;
                        Optional<uint64_t>  fTotalBytesRecieved;

                        /**
                         *  packets
                         *  The total number of packets of data transmitted or received by the interface.
                         */
                        Optional<uint64_t>  fTotalPacketsSent;
                        Optional<uint64_t>  fTotalPacketsRecieved;


                        /**
                         *  errs
                         *  The total number of transmit or receive errors detected by the device driver.
                         */
                        Optional<uint64_t>  fTotalErrors;

                        /**
                         *  drop
                         *  The total number of packets dropped by the device driver.
                         */
                        Optional<uint64_t>  fTotalPacketsDropped;
                    };


                    /**
                     *  For Info type.
                     */
                    extern  const   MeasurementType kNetworkInterfacesMeasurement;

                    /**
                     *  For Info type.
                     */
                    ObjectVariantMapper GetObjectVariantMapper ();


                    /**
                     *  Instrument returning Info measurements.
                     */
                    Instrument          GetInstrument ();


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

#endif  /*_Stroika_Framework_SystemPerformance_Instruments_NetworkInterfaces_h_*/
