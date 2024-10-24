/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Frameworks_UPnP_SSDP_Common_h_
#define _Stroika_Frameworks_UPnP_SSDP_Common_h_ 1

#include "Stroika/Frameworks/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Configuration/Common.h"
#include "Stroika/Foundation/Configuration/SystemConfiguration.h"
#include "Stroika/Foundation/IO/Network/SocketAddress.h"

/**
 *  \file
 *
 *  See http://quimby.gnus.org/internet-drafts/draft-cai-ssdp-v1-03.txt
 *  for details on the SSDP specification.
 *
 *  And http://www.upnp-hacks.org/upnp.html for more hints.
 *
 *  Also - http://upnp.org/specs/arch/UPnP-arch-DeviceArchitecture-v1.0.pdf
 *  Also - http://upnp.org/specs/arch/UPnP-arch-DeviceArchitecture-v1.1.pdf
 */

namespace Stroika::Frameworks::UPnP::SSDP {

    namespace V4 {
        extern const Foundation::IO::Network::SocketAddress kSocketAddress;
    }
    namespace V6 {
        extern const Foundation::IO::Network::SocketAddress kSocketAddress;
    }

    using Foundation::Characters::String;

    /**
     *  MakeServerHeaderValue
     */
    String MakeServerHeaderValue (const String& useProductTokenWithVersion,
                                  const String& usePlatformTokenAndVersion =
                                      Foundation::Configuration::GetSystemConfiguration_ActualOperatingSystem ().fRFC1945CompatProductTokenWithVersion,
                                  const String& useUPNPVersion = "UPnP/1.0"sv);

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Common.inl"

#endif /*_Stroika_Frameworks_UPnP_SSDP_Common_h_*/
