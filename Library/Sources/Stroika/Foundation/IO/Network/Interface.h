/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_Interface_h_
#define _Stroika_Foundation_IO_Network_Interface_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <optional>

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Common/Common.h"
#include "Stroika/Foundation/Common/Enumeration.h"
#include "Stroika/Foundation/Common/GUID.h"
#include "Stroika/Foundation/Containers/Collection.h"
#include "Stroika/Foundation/Containers/KeyedCollection.h"
#include "Stroika/Foundation/Containers/Set.h"
#include "Stroika/Foundation/IO/Network/CIDR.h"
#include "Stroika/Foundation/IO/Network/InternetAddress.h"
#include "Stroika/Foundation/IO/Network/SocketAddress.h"

/**
 *  \file
 *
 *  \note Code-Status:  <a href="Code-Status.md#Beta">Beta</a>
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
         *  The OS's own identity for the interface:
         *      POSIX:      its name - e.g. eth0, en0 (on Linux the device's name - not that of an IPv4 address's label, such as
         *                  eth0:1, whose address belongs to eth0)
         *      Windows:    IP_ADAPTER_ADDRESSES::AdapterName - its GUID (not very printable; @see fFriendlyName)
         *
         *  Unique among the interfaces there at any one moment - the OS ensures that (Linux: within the process's network
         *  namespace) - so it identifies an interface within one listing (@see SystemInterfacesMgr::GetAll).
         *
         *  \note But it is not a lasting identity. On POSIX an interface can be renamed, and once one goes away its name may be
         *        given to another (e.g. a VPN's utun3, or a replugged USB adapter's en5). On Windows the GUID lasts for a real
         *        adapter, but a re-created virtual one (a VPN's, a Hyper-V switch) gets a new one.
         *
         *        So key a collection of Interfaces by it only when they were all there at one moment (one listing). One kept
         *        across time - a history of the networks seen, say - can hold two different interfaces with the same ID, and
         *        the same interface under two IDs, so it needs some other notion of identity (its hardware address, perhaps,
         *        where it has one).
         */
        using SystemIDType = String;

        /**
         * @see SystemIDType
         */
        SystemIDType fInterfaceID;

#if qStroika_Platform_POSIX
        /**
         *  On unix, its the interface name, e.g. eth0, eth1, etc.
         *  On Windows, this is concept doesn't really exist.
         */
        nonvirtual String GetInterfaceName () const;
#endif

        /**
         *  Never a real interface's index - the OS numbers them from 1, and uses 0 for 'none'. Passed where an interface index
         *  is wanted, it means any interface - the OS picks one (as V4::kAddrAny and V6::kAddrAny do for an address).
         */
        static constexpr unsigned int kAnyIndex = 0;

        /**
         *  The operating system's number for the interface - how IPv6 names an interface (in its multicast APIs, and in a
         *  link-local address's scope). Called its index, but NOT its position in the list SystemInterfacesMgr::GetAll ()
         *  returns. POSIX: if_nametoindex (); Windows: IP_ADAPTER_ADDRESSES::IfIndex (Ipv6IfIndex when the interface has no
         *  IPv4). Never kAnyIndex.
         *
         *  \note RACE: an index looks like a lasting identity, but is not one - and unlike an address or a name changing, which
         *        is expected (other programs add and remove interfaces), this is easy to miss. An index names an interface only
         *        while that interface exists: no OS promises not to reuse it, so once an interface goes away, its index may be
         *        given to a DIFFERENT interface, and code still holding it silently acts on the wrong one. And an interface that
         *        goes away and comes back (an adapter disabled and re-enabled, a VPN reconnecting, a USB adapter replugged) may
         *        come back with a different index; none survives a reboot.
         *
         *        So rather than keep an index, pass the Interface itself where an API takes one (as ConnectionlessSocket's
         *        multicast calls do), which looks its index up - by fInterfaceID - at the call; else re-read the interface
         *        (SystemInterfacesMgr::GetById) just before using its index. In particular, to undo what was done by index
         *        (leave a multicast group, say), look the index up again rather than reuse the number it was done with. And redo
         *        whatever was done by index when the interfaces change (LinkMonitor) - a socket keeps the multicast interface it
         *        was given, say. (A multicast membership, though, is tied to the interface itself, so goes away with it.) For a
         *        longer-lasting name, use fInterfaceID.
         */
        optional<unsigned int> fIndex;

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
         *
         *  I can find no documentation on what this means, and all the network interfaces appear to have the same value (e.g. fake networks and real ones).
         *  VERGING on deprecating this. It doesn't appear to have any use. But leave for now... -- LGP 2018-12-16
         */
        optional<Common::GUID> fNetworkGUID;

        /**
         *
         *  \note   Common::DefaultNames<> supported
         *
         *  Most of these types are obvious, but eDeviceVirtualInternalNetwork is for special internal networks, like for virtualbox, Docker, or other purposes.
         *  They will generally (always) have private IP Addresses distinct from the main real external IP address of the machine in question. It may not always
         *  be possible to identify such networks.
         */
        enum class Type {
            eLoopback,
            eWiredEthernet,
            eWIFI,
            eTunnel,

            /**
             * Virtual adapter, like virtualbox, etc
             */
            eDeviceVirtualInternalNetwork,

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
         *  \note intentionally omitted fields 'Description' because included in parent object, connectionInfo state (cuz same as wireless adapater info state),
         *        and....
         */
        struct WirelessInfo {
            optional<String> fSSID;

            /**
             *
             *  \note   Common::DefaultNames<> supported
             */
            enum class State {
                eNotReady,           // WLAN_INTERFACE_STATE::wlan_interface_state_not_ready,
                eConnected,          // WLAN_INTERFACE_STATE::wlan_interface_state_connected,
                eAdHocNetworkFormed, // WLAN_INTERFACE_STATE::wlan_interface_state_ad_hoc_network_formed - First node in a ad hoc network
                eDisconnecting,      // WLAN_INTERFACE_STATE::wlan_interface_state_disconnecting,
                eDisconnected,       // WLAN_INTERFACE_STATE::wlan_interface_state_disconnected,
                eAssociating,        // WLAN_INTERFACE_STATE::wlan_interface_state_associating  - Attempting to associate with a network
                eDiscovering, // WLAN_INTERFACE_STATE::wlan_interface_state_discovering  - Auto configuration is discovering settings for the network
                eAuthenticating, // WLAN_INTERFACE_STATE::wlan_interface_state_authenticating
                eUnknown,

                Stroika_Define_Enum_Bounds (eNotReady, eUnknown)
            };
            optional<State> fState;

            /**
             *
             *  \note   Common::DefaultNames<> supported
             */
            enum class ConnectionMode {
                eProfile, // _WLAN_CONNECTION_MODE::wlan_connection_mode_profile- A profile is used to make the connection
                eTemporaryProfile, // _WLAN_CONNECTION_MODE::wlan_connection_mode_temporary_profile - A temporary profile is used to make the connection
                eDiscoverSecrure, // _WLAN_CONNECTION_MODE::wlan_connection_mode_discovery_secure - Secure discovery is used to make the connection
                eDiscoverInsecure, // _WLAN_CONNECTION_MODE::wlan_connection_mode_discovery_unsecure,
                eAuto, // _WLAN_CONNECTION_MODE::wlan_connection_mode_auto - connection initiated by wireless service automatically using a persistent profile
                eInvalid, // _WLAN_CONNECTION_MODE::wlan_connection_mode_invalid
                eUnknown,

                Stroika_Define_Enum_Bounds (eProfile, eUnknown)
            };
            optional<ConnectionMode> fConnectionMode;

            optional<String> fProfileName;

            /**
             *
             *  \note   Common::DefaultNames<> supported
             */
            enum class BSSType {
                eInfrastructure, // DOT11_BSS_TYPE::dot11_BSS_type_infrastructure
                eIndependent,    // DOT11_BSS_TYPE::dot11_BSS_type_independent
                eAny,            // DOT11_BSS_TYPE::dot11_BSS_type_any
                eUnknown,

                Stroika_Define_Enum_Bounds (eInfrastructure, eUnknown)
            };
            optional<BSSType> fBSSType;

            optional<String> fMACAddress;

            /**
             *
             *  \note   Common::DefaultNames<> supported
             */
            enum class PhysicalConnectionType {
                eFHSS,       // DOT11_PHY_TYPE::dot11_phy_type_fhss = 1,   Frequency-hopping spread-spectrum
                eDSSS,       // DOT11_PHY_TYPE:: dot11_phy_type_dsss = 2,   Direct sequence spread spectrum
                eIRBaseBand, // DOT11_PHY_TYPE::dot11_phy_type_irbaseband = 3,Infrared (IR) baseband
                e80211a,     // DOT11_PHY_TYPE::dot11_phy_type_ofdm = 4,                    // 11a
                e80211b,     // DOT11_PHY_TYPE::dot11_phy_type_hrdsss = 5,                  // 11b
                e80211g,     // DOT11_PHY_TYPE::dot11_phy_type_erp = 6,                     // 11g
                e80211n,     // DOT11_PHY_TYPE:: dot11_phy_type_ht = 7,                     // 11n
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
            optional<bool> f8021XEnabled; // 802.1X

            /**
             *
             *  \note   Common::DefaultNames<> supported
             */
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

        optional<WirelessInfo> fWirelessInfo; // has_value () if-and-only-if type fType == eWIFI

        /**
         */
        struct Bindings {
            /**
             *  until Stroika 2.1b9, this was called Network::fBindings
             *  These are the ranges of addresses (bindings?) associated with this network interface.
             *  Except for special cases (e.g. promiscuous mode) these are the only addreses expected to be one hop away on this
             *  network interface
             */
            Containers::Collection<CIDR> fAddressRanges; // can be IPv4 or IPv6

            /**
             *  Very similar to fAddressRanges, but these addresses are the ones we listen to, as targetted for this
             *  machine.
             */
            Containers::Collection<InternetAddress> fAddresses; // can be IPv4 or IPv6

            /**
             *  @see Characters::ToString ();
             */
            nonvirtual String ToString () const;
        };

        /**
         *  until Stroika 2.1b9, this was Containers::Collection<CIDR>: now fBindings.fAddressRanges
         */
        Bindings fBindings;

        /**
         *  Typically there will be zero or one, and if one, this is the default gateway. This maybe missing if it couldn't be retrieved from the operating system.
         */
        optional<Containers::Sequence<InternetAddress>> fGateways;

        /**
         *  The default set of (per adapter) dns servers. This maybe missing if it couldn't be retrieved from the operating system.
         */
        optional<Containers::Sequence<InternetAddress>> fDNSServers;

        /**
         *  \note   Common::DefaultNames<> supported
         */
        enum class Status {

            /**
             *  This tells if the low level network interface is powered on, and physically connected (e.g. cable plugged in).
             *
             *  This is always present if eRunning, but may also be present in the set of states if it can be determed that the interface is plugged in but disabled
             *  for reasons of software state (e.g. disabled interface, or testing mode).
             */
            eConnected,

            /**
             *  This tells if the network interface is ready to send and recieve packets.
             *
             *  \note see https://tools.ietf.org/html/rfc2020 and https://tools.ietf.org/html/rfc2863 - IfAdminStatus and IfOperStatus
             *        Corresponds to IfOperStatus==IfOperStatusUp
             */
            eRunning,

            Stroika_Define_Enum_Bounds (eConnected, eRunning)
        };

        /**
         *  The state of the interface (@see Status) if known.
         *
         *  \note not has_value () if IfOperStatusUnknown
         */
        optional<Containers::Set<Status>> fStatus;

        /**
         *  @see Characters::ToString ();
         */
        nonvirtual String ToString () const;

        /**
         *  An Interface's fInterfaceID - the key of InterfacesByID. (A struct, not a lambda: a lambda's type in a header
         *  would be an ODR violation.)
         */
        struct IDExtractor {
            SystemIDType operator() (const Interface& i) const noexcept
            {
                return i.fInterfaceID;
            }
        };
    };

    /**
     *  \brief Interfaces seen at one moment (e.g. one SystemInterfacesMgr::GetAll ()), keyed by fInterfaceID.
     *
     *  \note fInterfaceID is unique only among the interfaces there at one moment (@see Interface::SystemIDType) - so
     *        interfaces kept across time (a history of the networks seen, say) do not belong in one of these.
     */
    using InterfacesByID =
        Containers::KeyedCollection<Interface, Interface::SystemIDType, Containers::KeyedCollection_DefaultTraits<Interface, Interface::SystemIDType, Interface::IDExtractor>>;

    /**
     *  \todo   @todo https://github.com/SophistSolutions/Stroika/issues/844 (STK-710)
     *          add some level of caching to this object, possible with arguments to CTOR saying
     *          allowed latency for fetch from OS
     *
     *          IF I can do with netlink or something similar, probably at least optionally do this statically so that
     *          all instances can easily leverage the cache.
     */
    class SystemInterfacesMgr {
    public:
        /**
         *  Collect all the interfaces (and their status) from the operating system - keyed by their fInterfaceID.
         *
         *  \note An Interface's position in this list means nothing - its index is Interface::fIndex (@see its RACE note).
         */
        nonvirtual InterfacesByID GetAll ();

    public:
        /**
         *  Find the interface object with the given ID.
         *
         *  @see Interface::fInterfaceID
         */
        nonvirtual optional<Interface> GetById (const Interface::SystemIDType& interfaceID);

    public:
        /**
         *  A given address should be found in at most one interface.
         *  Note this doesnt check  fBoundAddresses, but fBoundAddressRanges, so what addresses are listened for, not
         *  actually associated with the interface
         */
        nonvirtual optional<Interface> GetContainingAddress (const InternetAddress& ia);
    };

    /**
     *  \brief This machine's address for reaching peer - the local address a datagram sent to peer would carry, so the address
     *         peer can reach this machine at, on the network between them.
     *
     *  Asks the routing table (via a UDP socket connected to peer - which sends nothing). peer's port does not matter. A
     *  link-local IPv6 peer must carry its scope id - as the sender's address from ConnectionlessSocket::Ptr::ReceiveFrom does.
     *
     *  Returns nullopt if there is no route to peer.
     *
     *  \note peer is a SocketAddress, not an InternetAddress, only because an InternetAddress cannot hold the scope id a
     *        link-local IPv6 peer needs. Once it can, this should take an InternetAddress -
     *        https://github.com/SophistSolutions/Stroika/issues/1201
     */
    optional<InternetAddress> GetLocalAddressToReach (const SocketAddress& peer);

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Interface.inl"

#endif /*_Stroika_Foundation_IO_Network_Interface_h_*/
