/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Framework_SystemPerformance_Instruments_NetworkInterfaces_h_
#define _Stroika_Framework_SystemPerformance_Instruments_NetworkInterfaces_h_ 1

#include    "../../StroikaPreComp.h"

#include    "../../../Foundation/Containers/Collection.h"
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


                    using   Containers::Collection;
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
                         *  Pretty display name to identify a particular interface.
                         */
                        String  fDisplayName {};


                        /**
                         *  bytes
                         *  The total number of bytes of data transmitted or received by the interface.
                         */
                        Optional<uint64_t>  fTotalBytesSent;
                        Optional<uint64_t>  fTotalBytesReceived;

                        /**
                         *  @todo TBD
                         */
                        Optional<double>  fBytesPerSecondSent;
                        Optional<double>  fBytesPerSecondReceived;

                        /**
                         *  @todo TBD
                         */
                        Optional<double>  fTCPRetransmittedSegmentsPerSecond;

                        /**
                         *  packets
                         *  The total number of packets of data transmitted or received by the interface.
                         */
                        Optional<uint64_t>  fTotalPacketsSent;
                        Optional<uint64_t>  fTotalPacketsReceived;

                        /**
                         *  @todo TBD
                         */
                        Optional<double>  fPacketsPerSecondSent;
                        Optional<double>  fPacketsPerSecondReceived;

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


                    struct  Info {
                        Collection<InterfaceInfo>   fInterfaceStatistics;
                    };


                    /**
                     *  For Info type.

                     }&&&todo lose
                     */
                    extern  const   MeasurementType kNetworkInterfacesMeasurement;

                    /**
                     *  For Info type.
                     */
                    ObjectVariantMapper GetObjectVariantMapper ();


                    /**
                     *  To control the behavior of the instrument.
                     */
                    struct  Options {
                        Time::DurationSecondsType   fMinimumAveragingInterval { 1.0 };
                    };


                    /**
                     *  Instrument returning Info measurements.
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

#endif  /*_Stroika_Framework_SystemPerformance_Instruments_NetworkInterfaces_h_*/
