/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_Interface_h_
#define _Stroika_Foundation_IO_Network_Interface_h_ 1

#include "../../StroikaPreComp.h"

#include <optional>

#include "../../Characters/String.h"
#include "../../Common/GUID.h"
#include "../../Configuration/Common.h"
#include "../../Configuration/Enumeration.h"
#include "../../Containers/Collection.h"
#include "../../Containers/Set.h"

#include "InternetAddress.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Late</a>
 *
 * TODO:
 *      @todo   Use http://stackoverflow.com/questions/14264371/how-to-get-nic-details-from-a-c-program to grab ethernet speed
 *              for inteface (bandwidth of network interface)
 *
 *      @todo   Handle case of multiple address bindings to a single interface (groupby).
 *
 *      @todo   Incomplete POSIX IMPL
 *              IPV6 compat for POSIX (already done for windows)
 *
 *      @todo   Fix use of assert - SB exceptions mostly...
 */

namespace Stroika::Foundation::IO::Network {

    using Characters::String;

    /**
     *  Capture details describing a network interface.
     */
    struct Interface {
        /**
         *      This is a somewhat artificial concept - which is introduced in Stroika. This is only guaranteed
         *   unique or the life of one program lifetime (@todo - not even sure we can do that much).
         *
         *      UNIX:
         *          interface name - e.g. eth0 - in the first column reported in ifconfig.
         *
         *      WINDOWS:
         *          IP_ADAPTER_ADDRESSES::fAdapterName
         *
         *          This is interface AdapterName, which is not particularly printable (usualy a GUID)
         */
        String fInternalInterfaceID;

#if qPlatform_POSIX
        /**
         *  On unix, its the interface name, e.g. eth0, eth1, etc.
         *  On Windows, this is concept doesn't really exist.
         */
        nonvirtual String GetInterfaceName () const;
#endif

        /**
         *  This is a generally good display name to describe a network interface.
         *
         *      WINDOWS:
         *          IP_ADAPTER_ADDRESSES::FriendlyName
         *
         *          A user-friendly name for the adapter. For example: "Local Area Connection 1." This name appears
         *          in contexts such as the ipconfig command line program and the Connection folder.
         *
         *          Note this name is often user-editable
         */
        String fFriendlyName;

        /**
         *  This description of the adpapter is typically a short string describing the hardware, such as
         *      "Intel(R) Dual Band Wireless-AC 7260"
         */
        optional<String> fDescription;

        /**
         *  Network GUID
         *      @see https://docs.microsoft.com/en-us/windows/desktop/api/iptypes/ns-iptypes-_ip_adapter_addresses_lh "NetworkGuid"
         */
        optional<Common::GUID> fNetworkGUID;

        /**
         *
         *  \note   Configuration::DefaultNames<> supported
         */
        enum class Type {
            eLoopback,
            eWiredEthernet,
            eWIFI,
            eTunnel,
            eOther,

            Stroika_Define_Enum_Bounds (eLoopback, eOther)
        };

        /**
         */
        optional<Type> fType;

        /**
         *  This - if present - is typically an ethernet macaddr (6 bytes in hex separated by :)
         */
        optional<String> fHardwareAddress;

        /**
         *  bits per second
         */
        optional<uint64_t> fTransmitSpeedBaud;

        /**
         *  bits per second
         */
        optional<uint64_t> fReceiveLinkSpeedBaud;

        /**
         */
        struct Binding {
            InternetAddress        fInternetAddress;
            optional<unsigned int> fOnLinkPrefixLength;

            /**
             *  @see Characters::ToString ();
             */
            nonvirtual String ToString () const;
        };

        /**
         *  \note intentionally omitted fields 'Description' because included in parent object, connectionInfo state (cuz same as wireless adapater info state),
         *        and....
         */
        struct WirelessInfo {
            optional<String> fSSID;

            enum class State {
                eNotReady,           // WLAN_INTERFACE_STATE::wlan_interface_state_not_ready,
                eConnected,          // WLAN_INTERFACE_STATE::wlan_interface_state_connected,
                eAdHocNetworkFormed, // WLAN_INTERFACE_STATE::wlan_interface_state_ad_hoc_network_formed - First node in a ad hoc network
                eDisconnecting,      // WLAN_INTERFACE_STATE::wlan_interface_state_disconnecting,
                eDisconnected,       // WLAN_INTERFACE_STATE::wlan_interface_state_disconnected,
                eAssociating,        // WLAN_INTERFACE_STATE::wlan_interface_state_associating  - Attempting to associate with a network
                eDiscovering,        // WLAN_INTERFACE_STATE::wlan_interface_state_discovering  - Auto configuration is discovering settings for the network
                eAuthenticating,     // WLAN_INTERFACE_STATE::wlan_interface_state_authenticating
                eUnknown,

                Stroika_Define_Enum_Bounds (eNotReady, eUnknown)
            };
            optional<State> fState;

            enum class ConnectionMode {
                eProfile,          // _WLAN_CONNECTION_MODE::wlan_connection_mode_profile- A profile is used to make the connection
                eTemporaryProfile, // _WLAN_CONNECTION_MODE::wlan_connection_mode_temporary_profile - A temporary profile is used to make the connection
                eDiscoverSecrure,  // _WLAN_CONNECTION_MODE::wlan_connection_mode_discovery_secure - Secure discovery is used to make the connection
                eDiscoverInsecure, // _WLAN_CONNECTION_MODE::wlan_connection_mode_discovery_unsecure,
                eAuto,             // _WLAN_CONNECTION_MODE::wlan_connection_mode_auto - connection initiated by wireless service automatically using a persistent profile
                eInvalid,          // _WLAN_CONNECTION_MODE::wlan_connection_mode_invalid
                eUnknown,

                Stroika_Define_Enum_Bounds (eProfile, eUnknown)
            };
            optional<ConnectionMode> fConnectionMode;

            optional<String> fProfileName;

            enum class BSSType {
                eInfrastructure, // DOT11_BSS_TYPE::dot11_BSS_type_infrastructure
                eIndependent,    // DOT11_BSS_TYPE::dot11_BSS_type_independent
                eAny,            // DOT11_BSS_TYPE::dot11_BSS_type_any
                eUnknown,

                Stroika_Define_Enum_Bounds (eInfrastructure, eUnknown)
            };
            optional<BSSType> fBSSType;

            optional<String> fMACAddress;

            enum class PhysicalConnectionType {
                eFHSS,       // DOT11_PHY_TYPE::dot11_phy_type_fhss = 1,   Frequency-hopping spread-spectrum
                eDSSS,       // DOT11_PHY_TYPE:: dot11_phy_type_dsss = 2,   Direct sequence spread spectrum
                eIRBaseBand, // DOT11_PHY_TYPE::dot11_phy_type_irbaseband = 3,Infrared (IR) baseband
                e80211a,     // DOT11_PHY_TYPE::dot11_phy_type_ofdm = 4,                    // 11a
                e80211b,     // DOT11_PHY_TYPE::dot11_phy_type_hrdsss = 5,                  // 11b
                e80211g,     // DOT11_PHY_TYPE::dot11_phy_type_erp = 6,                     // 11g
                e80211n,     // DOT11_PHY_TYPE:: dot11_phy_type_ht = 7,                      // 11n
                e80211ac,    // DOT11_PHY_TYPE::dot11_phy_type_vht = 8,                     // 11ac
                e80211ad,    // DOT11_PHY_TYPE::dot11_phy_type_dmg = 9,                     // 11ad
                e80211ax,    // DOT11_PHY_TYPE::dot11_phy_type_he = 10,                     // 11ax
                eUnknown,    // DOT11_PHY_TYPE::dot11_phy_type_unknown = 0,

                Stroika_Define_Enum_Bounds (eFHSS, eUnknown)
            };
            optional<PhysicalConnectionType> fPhysicalConnectionType;

            /**
             * A percentage value that represents the signal quality of the network. 
             * This member contains a value between 0 and 100. A value of 0 implies an actual RSSI signal strength of -100 dbm.
             * A value of 100 implies an actual RSSI signal strength of -50 dbm. You can calculate the RSSI signal 
             * strength value for wlanSignalQuality values between 1 and 99 using linear interpolation.
             *
             * https://stackoverflow.com/questions/15797920/how-to-convert-wifi-signal-strength-from-quality-percent-to-rssi-dbm
             */
            optional<double> fSignalQuality;

            optional<bool> fSecurityEnabled; // any wireless security enabled (versus open)  // NOT SURE HOW DIFFERS FROM _DOT11_AUTH_ALGORITHM::DOT11_AUTH_ALGO_80211_OPEN
            optional<bool> f8021XEnabled;    // 802.1X

            enum class AuthAlgorithm {
                eOpen,         // DOT11_AUTH_ALGORITHM::DOT11_AUTH_ALGO_80211_OPEN = 1,
                ePresharedKey, // DOT11_AUTH_ALGORITHM::DOT11_AUTH_ALGO_80211_SHARED_KEY = 2,
                eWPA,          // DOT11_AUTH_ALGORITHM:: DOT11_AUTH_ALGO_WPA = 3,
                eWPA_PSK,      // DOT11_AUTH_ALGORITHM::DOT11_AUTH_ALGO_WPA_PSK = 4,
                eWPA_NONE,     // DOT11_AUTH_ALGORITHM::DOT11_AUTH_ALGO_WPA_NONE = 5,               // used in NatSTA only
                eRSNA,         // DOT11_AUTH_ALGORITHM::DOT11_AUTH_ALGO_RSNA = 6,
                eRSNA_PSK,     // DOT11_AUTH_ALGORITHM::DOT11_AUTH_ALGO_RSNA_PSK = 7,
                eUnknown,

                Stroika_Define_Enum_Bounds (eOpen, eUnknown)
            };
            optional<AuthAlgorithm> fAuthAlgorithm;

            optional<String> fCipher;

            /**
             *  @see Characters::ToString ();
             */
            nonvirtual String ToString () const;
        };

        optional<WirelessInfo> fWirelessInfo; // IFF type fType == eWIFI

        /**
         */
        Containers::Collection<Binding> fBindings; // can be IPv4 or IPv6

        /**
         * @todo document these - 'eRunning' == LINUX RUNNING
         *
         *  \note   Configuration::DefaultNames<> supported
         */
        enum class Status {
            eConnected,
            eRunning,

            Stroika_Define_Enum_Bounds (eConnected, eRunning)
        };

        /**
         */
        optional<Containers::Set<Status>> fStatus;

        /**
         *  @see Characters::ToString ();
         */
        nonvirtual String ToString () const;
    };

    /**
     *  Collect all the interfaces (and their status) from the operating system.
     */
    Traversal::Iterable<Interface> GetInterfaces ();

    /**
     *  Find the interface object with the given ID.
     *
     *  @see Interface::fInternalInterfaceID
     */
    optional<Interface> GetInterfaceById (const String& internalInterfaceID);

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Interface.inl"

#endif /*_Stroika_Foundation_IO_Network_Interface_h_*/
