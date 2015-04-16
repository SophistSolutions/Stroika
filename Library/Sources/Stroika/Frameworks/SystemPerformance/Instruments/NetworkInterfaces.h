/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Framework_SystemPerformance_Instruments_NetworkInterfaces_h_
#define _Stroika_Framework_SystemPerformance_Instruments_NetworkInterfaces_h_ 1

#include    "../../StroikaPreComp.h"

#include    "../../../Foundation/DataExchange/ObjectVariantMapper.h"
#include    "../../../Foundation/Memory/Optional.h"
#include    "../../../Foundation/IO/Network/Interface.h"

#include    "../Instrument.h"



/*
 *  \file
 *
 *  \version    <a href="code_status.html#Alpha">Alpha</a>
 *
 * TODO:
 *      @todo   Windows code is just a very rough draft and I'm not sure i have the underlying API to measure
 *              by interface.
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
                         *  Stroika internal interface ID.
                         *  This cannot be persisted, but can be used within a  given process lifetime to check for object identity.
                         *
                         *  \note   This corresponds to the IO::Network::Interface::fInternalInterfaceID name and can be used to join.
                         */
                        String  fInternalInterfaceID;


                        /**
                         *  For POSIX, this is the interface name (e.g. eth0).
                         *
                         *  For Windows, this concept doesn't appear to exist.
                         *
                         *      @todo I think we can lose this!!! Just have instead of getIntfaces - GetInterfaceByInterfalInterfaceID in IO::Network::Interaface code
                         */
                        Optional<String>  fInterfaceID {};


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
