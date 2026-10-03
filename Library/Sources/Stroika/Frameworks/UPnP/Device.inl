/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */

namespace Stroika::Frameworks::UPnP {

    // built on the deprecated IO::Network::GetPrimaryNetworkDeviceMacAddress ()
    [[deprecated ("Since Stroika v3.0d25 - for a device ID unique to this machine use Common::GetSystemConfiguration_MachineID "
                  "(applicationKey) - as the SSDPServer sample does")]] String
    MungePrimaryMacAddrIntoBaseDeviceID (String baseDeviceID);

}
