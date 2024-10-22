/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */

namespace Stroika::Foundation::IO::Network {

    /*
     ********************************************************************************
     ************************************* Interface ********************************
     ********************************************************************************
     */
#if qStroika_Foundation_Common_Platform_POSIX
    /**
     *  On unix, its the interface name, e.g. eth0, eth1, etc.
     *  On Windows, this is concept doesn't really exist.
     */
    inline String Interface::GetInterfaceName () const
    {
        return fInternalInterfaceID;
    }
#endif

}
namespace Stroika::Foundation::Common {
    template <>
    constexpr EnumNames<IO::Network::Interface::WirelessInfo::State> DefaultNames<IO::Network::Interface::WirelessInfo::State>::k{{{
        {IO::Network::Interface::WirelessInfo::State::eNotReady, L"Not-Ready"},
        {IO::Network::Interface::WirelessInfo::State::eConnected, L"Connected"},
        {IO::Network::Interface::WirelessInfo::State::eAdHocNetworkFormed, L"Ad-Hoc-Network-Formed"},
        {IO::Network::Interface::WirelessInfo::State::eDisconnecting, L"Disconnecting"},
        {IO::Network::Interface::WirelessInfo::State::eDisconnected, L"Disconnected"},
        {IO::Network::Interface::WirelessInfo::State::eAssociating, L"Associating"},
        {IO::Network::Interface::WirelessInfo::State::eDiscovering, L"Discovering"},
        {IO::Network::Interface::WirelessInfo::State::eAuthenticating, L"Authenticating"},
        {IO::Network::Interface::WirelessInfo::State::eUnknown, L"Unknown"},
    }}};
    template <>
    constexpr EnumNames<IO::Network::Interface::WirelessInfo::ConnectionMode> DefaultNames<IO::Network::Interface::WirelessInfo::ConnectionMode>::k{{{
        {IO::Network::Interface::WirelessInfo::ConnectionMode::eProfile, L"Profile"},
        {IO::Network::Interface::WirelessInfo::ConnectionMode::eTemporaryProfile, L"Temporary-Profile"},
        {IO::Network::Interface::WirelessInfo::ConnectionMode::eDiscoverSecrure, L"Discover-Secrure"},
        {IO::Network::Interface::WirelessInfo::ConnectionMode::eDiscoverInsecure, L"Discover-Insecure"},
        {IO::Network::Interface::WirelessInfo::ConnectionMode::eAuto, L"Auto"},
        {IO::Network::Interface::WirelessInfo::ConnectionMode::eInvalid, L"Invalid"},
        {IO::Network::Interface::WirelessInfo::ConnectionMode::eUnknown, L"Unknown"},
    }}};
    template <>
    constexpr EnumNames<IO::Network::Interface::WirelessInfo::BSSType> DefaultNames<IO::Network::Interface::WirelessInfo::BSSType>::k{{{
        {IO::Network::Interface::WirelessInfo::BSSType::eInfrastructure, L"Infrastructure"},
        {IO::Network::Interface::WirelessInfo::BSSType::eIndependent, L"Independent"},
        {IO::Network::Interface::WirelessInfo::BSSType::eAny, L"Any"},
        {IO::Network::Interface::WirelessInfo::BSSType::eUnknown, L"Unknown"},
    }}};
    template <>
    constexpr EnumNames<IO::Network::Interface::WirelessInfo::PhysicalConnectionType> DefaultNames<IO::Network::Interface::WirelessInfo::PhysicalConnectionType>::k{{{
        {IO::Network::Interface::WirelessInfo::PhysicalConnectionType::eFHSS, L"FHSS"},
        {IO::Network::Interface::WirelessInfo::PhysicalConnectionType::eDSSS, L"DSSS"},
        {IO::Network::Interface::WirelessInfo::PhysicalConnectionType::eIRBaseBand, L"IRBaseBand"},
        {IO::Network::Interface::WirelessInfo::PhysicalConnectionType::e80211a, L"802.11a"},
        {IO::Network::Interface::WirelessInfo::PhysicalConnectionType::e80211b, L"802.11b"},
        {IO::Network::Interface::WirelessInfo::PhysicalConnectionType::e80211g, L"802.11g"},
        {IO::Network::Interface::WirelessInfo::PhysicalConnectionType::e80211n, L"802.11n"},
        {IO::Network::Interface::WirelessInfo::PhysicalConnectionType::e80211ac, L"802.11ac"},
        {IO::Network::Interface::WirelessInfo::PhysicalConnectionType::e80211ad, L"802.11ad"},
        {IO::Network::Interface::WirelessInfo::PhysicalConnectionType::e80211ax, L"802.11ax"},
        {IO::Network::Interface::WirelessInfo::PhysicalConnectionType::eUnknown, L"Unknown"},
    }}};
    template <>
    constexpr EnumNames<IO::Network::Interface::WirelessInfo::AuthAlgorithm> DefaultNames<IO::Network::Interface::WirelessInfo::AuthAlgorithm>::k{{{
        {IO::Network::Interface::WirelessInfo::AuthAlgorithm::eOpen, L"Open"},
        {IO::Network::Interface::WirelessInfo::AuthAlgorithm::ePresharedKey, L"Preshared-Key"},
        {IO::Network::Interface::WirelessInfo::AuthAlgorithm::eWPA, L"WPA"},
        {IO::Network::Interface::WirelessInfo::AuthAlgorithm::eWPA_PSK, L"WPA-PSK"},
        {IO::Network::Interface::WirelessInfo::AuthAlgorithm::eWPA_NONE, L"WPA-NONE"},
        {IO::Network::Interface::WirelessInfo::AuthAlgorithm::eRSNA, L"RSNA"},
        {IO::Network::Interface::WirelessInfo::AuthAlgorithm::eRSNA_PSK, L"RSNA-PSK"},
        {IO::Network::Interface::WirelessInfo::AuthAlgorithm::eUnknown, L"Unknown"},
    }}};
    template <>
    constexpr EnumNames<IO::Network::Interface::Status> DefaultNames<IO::Network::Interface::Status>::k{{{
        {IO::Network::Interface::Status::eConnected, L"Connected"},
        {IO::Network::Interface::Status::eRunning, L"Running"},
    }}};
    template <>
    constexpr EnumNames<IO::Network::Interface::Type> DefaultNames<IO::Network::Interface::Type>::k{{{
        {IO::Network::Interface::Type::eLoopback, L"Loopback"},
        {IO::Network::Interface::Type::eWiredEthernet, L"Wired-Ethernet"},
        {IO::Network::Interface::Type::eWIFI, L"WIFI"},
        {IO::Network::Interface::Type::eTunnel, L"Tunnel"},
        {IO::Network::Interface::Type::eDeviceVirtualInternalNetwork, L"Device-Virtual-Internal-Network"},
        {IO::Network::Interface::Type::eOther, L"Other"},
    }}};

}
