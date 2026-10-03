/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */

namespace Stroika::Foundation::IO::Network {

    /*
     ********************************************************************************
     ********************************** LinkMonitor *********************************
     ********************************************************************************
     */

    [[deprecated (
        "Since Stroika v3.0d25 - not well defined: use SystemInterfacesMgr{}.GetAll () or GetLocalAddressToReach ()")]] InternetAddress
    GetPrimaryInternetAddress ();

    // which interface's is arbitrary - on Linux and macOS it can even change with which network is connected
    [[deprecated ("Since Stroika v3.0d25 - not well defined: for an ID unique to this machine use Common::GetSystemConfiguration_MachineID "
                  "(); for an interface's hardware address, SystemInterfacesMgr{}.GetAll ()")]] String
    GetPrimaryNetworkDeviceMacAddress ();

}

namespace Stroika::Foundation::Common {
    template <>
    constexpr EnumNames<IO::Network::LinkMonitor::LinkChange> DefaultNames<IO::Network::LinkMonitor::LinkChange>::k{{{
        {IO::Network::LinkMonitor::LinkChange::eAdded, L"Added"},
        {IO::Network::LinkMonitor::LinkChange::eRemoved, L"Removed"},
    }}};
}
