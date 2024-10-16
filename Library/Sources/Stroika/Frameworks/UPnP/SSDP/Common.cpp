/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include "Common.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::IO::Network;

using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::UPnP::SSDP;

namespace {
    constexpr char     SSDP_MULTICAST_[] = "239.255.255.250";
    constexpr uint16_t SSDP_PORT_        = 1900;
}

const SocketAddress UPnP::SSDP::V4::kSocketAddress = SocketAddress{InternetAddress{SSDP_MULTICAST_, InternetAddress::AddressFamily::V4}, SSDP_PORT_};
const SocketAddress UPnP::SSDP::V6::kSocketAddress = SocketAddress{InternetAddress{"FF02::C"sv, InternetAddress::AddressFamily::V6}, SSDP_PORT_};

String UPnP::SSDP::MakeServerHeaderValue (const String& useProductTokenWithVersion, const String& usePlatformTokenAndVersion, const String& useUPNPVersion)
{
    Require (not useProductTokenWithVersion.empty ());
    Require (not usePlatformTokenAndVersion.empty ());
    Require (not useUPNPVersion.empty ());
    static const String kSpace_{" "sv};
    return usePlatformTokenAndVersion + kSpace_ + useUPNPVersion + kSpace_ + useProductTokenWithVersion;
}
