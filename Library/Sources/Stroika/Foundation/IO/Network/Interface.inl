/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */

namespace Stroika::Foundation::IO::Network {

    /*
     ********************************************************************************
     ************************************* Interface ********************************
     ********************************************************************************
     */
#if qStroika_Platform_POSIX
    /**
     *  On unix, its the interface name, e.g. eth0, eth1, etc.
     *  On Windows, this is concept doesn't really exist.
     */
    inline String Interface::GetInterfaceName () const
    {
        return fInterfaceID;
    }
#endif

}
namespace Stroika::Foundation::IO::Network {
    Common::StroikaNaming          EnumNamingRule (Interface::WirelessInfo::State);
    Common::StroikaNaming          EnumNamingRule (Interface::WirelessInfo::ConnectionMode);
    Common::StroikaNaming          EnumNamingRule (Interface::WirelessInfo::BSSType);
    Common::StroikaNaming          EnumNamingRule (Interface::WirelessInfo::PhysicalConnectionType);
    constexpr Common::EnumNameText EnumeratorName (Interface::WirelessInfo::PhysicalConnectionType e, Common::EnumNameText byRule)
    {
        switch (e) {
            case Interface::WirelessInfo::PhysicalConnectionType::e80211a:
                return "802.11a";
            case Interface::WirelessInfo::PhysicalConnectionType::e80211b:
                return "802.11b";
            case Interface::WirelessInfo::PhysicalConnectionType::e80211g:
                return "802.11g";
            case Interface::WirelessInfo::PhysicalConnectionType::e80211n:
                return "802.11n";
            case Interface::WirelessInfo::PhysicalConnectionType::e80211ac:
                return "802.11ac";
            case Interface::WirelessInfo::PhysicalConnectionType::e80211ad:
                return "802.11ad";
            case Interface::WirelessInfo::PhysicalConnectionType::e80211ax:
                return "802.11ax";
            default:
                return byRule;
        }
    }
    Common::StroikaNaming          EnumNamingRule (Interface::WirelessInfo::AuthAlgorithm);
    constexpr Common::EnumNameText EnumeratorName (Interface::WirelessInfo::AuthAlgorithm e, Common::EnumNameText byRule)
    {
        switch (e) {
            case Interface::WirelessInfo::AuthAlgorithm::eWPA_PSK:
                return "WPA-PSK";
            case Interface::WirelessInfo::AuthAlgorithm::eWPA_NONE:
                return "WPA-NONE";
            case Interface::WirelessInfo::AuthAlgorithm::eRSNA_PSK:
                return "RSNA-PSK";
            default:
                return byRule;
        }
    }
    Common::StroikaNaming EnumNamingRule (Interface::Status);
    Common::StroikaNaming EnumNamingRule (Interface::Type);
}
