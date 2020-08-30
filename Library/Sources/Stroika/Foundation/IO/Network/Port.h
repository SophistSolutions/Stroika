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

        enum class TCP : PortType {
            eSSH         = 22,
            eSMPT        = 25,
            eDNS         = 53,
            eHTTP        = 80,
            ePOP3        = 110,
            eNTP         = 123,
            eNETBIOS     = 137,
            eSMB         = 139, // NetBIOS Session Service
            eHTTPS       = 443,
            eMicrosoftDS = 445, // https://en.wikipedia.org/wiki/Active_Directory, https://en.wikipedia.org/wiki/Server_Message_Block
            eModbus      = 502,
            eLPD         = 515,  // Line Printer Daemon
            eIPP         = 631,  // internet printing protocol
            eDNSOverTLS  = 853,  // https://tools.ietf.org/html/rfc7858
            eMSSQL       = 1433, // https://en.wikipedia.org/wiki/Microsoft_SQL_Server
            eRDP         = 3389,
            eSIP         = 5060, // SIP (https://isc.sans.edu/diary/Cyber+Security+Awareness+Month+-+Day+20+-+Ports+5060+%26+5061+-+SIP+%28VoIP%29/7405
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