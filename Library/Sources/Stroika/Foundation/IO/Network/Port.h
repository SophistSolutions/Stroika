/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_Port_h_
#define _Stroika_Foundation_IO_Network_Port_h_ 1

#include "../../StroikaPreComp.h"

#if defined(__cpp_impl_three_way_comparison)
#include <compare>
#endif

#include "../../Characters/String.h"

#include "InternetAddressRange.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Alpha">Alphawi</a>
 *
 * TODO:
 */

namespace Stroika::Foundation::IO::Network {

    using Characters::String;

    using PortType = uint16_t;

    /**
     *  @see https://en.wikipedia.org/wiki/List_of_TCP_and_UDP_port_numbers
     */
    namespace WellKnownPorts {

        namespace TCP {
            constexpr PortType kSSH            = 22;
            constexpr PortType kSMPT = 25;
            constexpr PortType kDNS            = 53;
            constexpr PortType kHTTP           = 80;
            constexpr PortType kPOP3           = 110;
            constexpr PortType kNTP            = 123;
            constexpr PortType kNETBIOS        = 137;
            constexpr PortType kSMB            = 139; // NetBIOS Session Service
            constexpr PortType kHTTPS          = 443;
            constexpr PortType kMicrosoftDS    = 445; // https://en.wikipedia.org/wiki/Active_Directory, https://en.wikipedia.org/wiki/Server_Message_Block
            constexpr PortType kModbus         = 502;
            constexpr PortType kLPD            = 515;  // Line Printer Daemon
            constexpr PortType kIPP            = 631;  // internet printing protocol
            constexpr PortType kDNSOverTLS     = 853;  // https://tools.ietf.org/html/rfc7858
            constexpr PortType kMSSQL          = 1433; // https://en.wikipedia.org/wiki/Microsoft_SQL_Server
            constexpr PortType kRDP            = 3389;
            constexpr PortType kSIP            = 5060; // SIP (https://isc.sans.edu/diary/Cyber+Security+Awareness+Month+-+Day+20+-+Ports+5060+%26+5061+-+SIP+%28VoIP%29/7405
        };

    }

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
//#include "Port.inl"

#endif /*_Stroika_Foundation_IO_Network_Port_h_*/