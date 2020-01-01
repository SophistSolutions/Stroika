/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#ifndef _Stroika_Framework_SystemPerformance_Instruments_Network_h_
#define _Stroika_Framework_SystemPerformance_Instruments_Network_h_ 1

#include "../../StroikaPreComp.h"

#include <optional>

#include "../../../Foundation/Containers/Collection.h"
#include "../../../Foundation/Containers/Set.h"
#include "../../../Foundation/DataExchange/ObjectVariantMapper.h"
#include "../../../Foundation/IO/Network/Interface.h"

#include "../Instrument.h"

/*
 *  \file
 *
 *  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Late</a>
 *
 *  TODO:
 *      @todo   https://stroika.atlassian.net/browse/STK-479 - Add (elaborated) TCPStats - like we have in Process TCPStats
 */

namespace Stroika::Frameworks::SystemPerformance::Instruments::Network {

    using Containers::Collection;
    using DataExchange::ObjectVariantMapper;

    /**
     *  Note that the total values are 'total ever' while the OS has been running, and the rate values are
     *  averaged over the collection interval.
     */
    struct IOStatistics {
        /**
         *  bytes
         *  The total number of bytes of data transmitted or received by the interface.
         */
        optional<uint64_t> fTotalBytesSent;
        optional<uint64_t> fTotalBytesReceived;

        /**
         *  @todo TBD
         */
        optional<double> fBytesPerSecondSent;
        optional<double> fBytesPerSecondReceived;

        /**
         *  @todo TBD
         */
        optional<uint64_t> fTotalTCPSegments;
        optional<double>   fTCPSegmentsPerSecond;

        /**
         *  @todo TBD
         */
        optional<uint64_t> fTotalTCPRetransmittedSegments;
        optional<double>   fTCPRetransmittedSegmentsPerSecond;

        /**
         *  packets
         *  The total number of packets of data transmitted or received by the interface.
         */
        optional<uint64_t> fTotalPacketsSent;
        optional<uint64_t> fTotalPacketsReceived;

        /**
         *  @todo TBD
         */
        optional<double> fPacketsPerSecondSent;
        optional<double> fPacketsPerSecondReceived;

        /**
         *  errs
         *  The total number of transmit or receive errors detected by the device driver.
         */
        optional<uint64_t> fTotalErrors;

        /**
         *  drop
         *  The total number of packets dropped by the device driver.
         */
        optional<uint64_t> fTotalPacketsDropped;

        /**
         *  Utility to accomulate statistics. This simply sums each member, and if one side or the other was missing, it starts at zero.
         *  If both sides missing, the result stays missing.
         */
        nonvirtual IOStatistics& operator+= (const IOStatistics& rhs);

        /**
         *  @see Characters::ToString ();
         */
        nonvirtual String ToString () const;
    };

    /**
     */
    struct InterfaceInfo {

        /**
         */
        using Interface = Foundation::IO::Network::Interface;

        /**
         *  This sub-object contains most of the configuration information about the interface.
         *
         *      @todo replace most of whats below
         */
        Interface fInterface;

        /**
         *  Per interface I/O transfer statistics.
         */
        IOStatistics fIOStatistics;

        /**
         *  @see Characters::ToString ();
         */
        nonvirtual String ToString () const;
    };

    /**
     *  A single captured network status measurement.
     */
    struct Info {
        optional<Collection<InterfaceInfo>> fInterfaces;

        /**
         *  Conceptually fSummaryIOStatistics is just the sum of the stats for each fInterfaces member, but
         *  it maybe fetched via a different OS API, and may differ.
         */
        optional<IOStatistics> fSummaryIOStatistics;

        /**
         *  @see Characters::ToString ();
         */
        nonvirtual String ToString () const;
    };

    /**
     *  For Info type.
     */
    ObjectVariantMapper GetObjectVariantMapper ();

    /**
     *  To control the behavior of the instrument.
     *
     *      @todo add option controlling if we return details and if we return sumamry
     */
    struct Options {
        /**
         *  \req fMinimumAveragingInterval >= 0
         */
        Time::DurationSecondsType fMinimumAveragingInterval{1.0};
    };

    /**
     *  Instrument returning Info measurements.
     */
    Instrument GetInstrument (Options options = Options ());

}

namespace Stroika::Frameworks::SystemPerformance {

    /*
     *  Specialization to improve performance
     */
    template <>
    Instruments::Network::Info Instrument::CaptureOneMeasurement (Range<DurationSecondsType>* measurementTimeOut);

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#endif /*_Stroika_Framework_SystemPerformance_Instruments_Network_h_*/
