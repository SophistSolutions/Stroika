/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include <cstdio>
#include <random>

#if qPlatform_POSIX
//@todo see how many of these includes are needed
#include <arpa/inet.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>
#if qPlatform_Linux
#include <linux/ethtool.h>
#include <linux/sockios.h>
#include <linux/types.h> // needed on RedHat5
#include <linux/wireless.h>
#endif
#elif qPlatform_Windows
#include <WinSock2.h>

#include <Windot11.h> // for DOT11_SSID struct
#include <wlanapi.h>

#include <Iphlpapi.h>
#include <WS2tcpip.h>
#include <netioapi.h>
#endif

#include "../../Characters/CString/Utilities.h"
#include "../../Characters/Format.h"
#include "../../Characters/StringBuilder.h"
#include "../../Characters/String_Constant.h"
#include "../../Characters/ToString.h"
#include "../../Containers/Collection.h"
#include "../../Containers/Mapping.h"
#include "../../Debug/Sanitizer.h"
#include "../../Execution/ErrNoException.h"
#include "../../Execution/Finally.h"
#if qPlatform_Windows
#include "../../../Foundation/Execution/Platform/Windows/Exception.h"
#endif
#include "../../Execution/Synchronized.h"
#include "../../Memory/Optional.h"
#include "../../Memory/SmallStackBuffer.h"

#include "Socket.h"

#include "Interface.h"

using std::byte;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::Execution;
using namespace Stroika::Foundation::Memory;
using namespace Stroika::Foundation::IO;
using namespace Stroika::Foundation::IO::Network;

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define USE_NOISY_TRACE_IN_THIS_MODULE_ 1

#if defined(_MSC_VER)
// support use of Iphlpapi - but better to reference here than in lib entry of project file cuz
// easiser to see/modularize (and only pulled in if this module is referenced)
#pragma comment(lib, "Iphlpapi.lib")

#pragma comment(lib, "wlanapi.lib")
#endif

#if qPlatform_Linux
// Hack for centos5 support:
//      Overload with linux version so other one wins, but this gets called if other doesn't exist
//      TRY --LGP 2015-05-19
template <typename HACK = int>
static __inline__ __u32 ethtool_cmd_speed (const struct ethtool_cmd* ep, HACK i = 0)
{
    //return (ep->speed_hi << 16) | ep->speed;
    return ep->speed;
}
#endif

namespace {
    // Windows uses '-' as separator, and linux ':'. Pick arbitrarily (more linux machines
    // than windows, or soon will be)
    auto PrintMacAddr_ (const uint8_t* macaddrBytes, [[maybe_unused]] const uint8_t* macaddrBytesEnd) -> String
    {
        Require (macaddrBytesEnd - macaddrBytes == 6);
        char buf[100]{};
        (void)snprintf (buf, sizeof (buf),
                        "%02x:%02x:%02x:%02x:%02x:%02x",
                        macaddrBytes[0], macaddrBytes[1],
                        macaddrBytes[2], macaddrBytes[3],
                        macaddrBytes[4], macaddrBytes[5]);
        return String::FromASCII (buf);
    };
}

#if qCompiler_cpp17InlineStaticMemberOfTemplateLinkerUndefined_Buggy
/*
 ********************************************************************************
 **************************** Configuration::DefaultNames ***********************
 ********************************************************************************
 */
namespace Stroika::Foundation::Configuration {
    constexpr EnumNames<IO::Network::Interface::Status> DefaultNames<IO::Network::Interface::Status>::k;
    constexpr EnumNames<IO::Network::Interface::Type>   DefaultNames<IO::Network::Interface::Type>::k;
}
#endif

/*
 ********************************************************************************
 ************************** Interface::Binding **********************************
 ********************************************************************************
 */
String Interface::Binding::ToString () const
{
    Characters::StringBuilder sb;
    sb += L"{";
    sb += L"Internet-Address: " + Characters::ToString (fInternetAddress) + L", ";
    if (fOnLinkPrefixLength) {
        sb += L"On-Link-Prefix-Length: " + Characters::ToString (*fOnLinkPrefixLength) + L", ";
    }
    sb += L"}";
    return sb.str ();
}

/*
 ********************************************************************************
 ********************** Interface::WirelessInfo *********************************
 ********************************************************************************
 */
String Interface::WirelessInfo::ToString () const
{
    Characters::StringBuilder sb;
    sb += L"{";
    sb += L"SSID: " + Characters::ToString (fSSID) + L", ";
    // todo add more
    sb += L"}";
    return sb.str ();
}

/*
 ********************************************************************************
 *********************************** Interface **********************************
 ********************************************************************************
 */
String Interface::ToString () const
{
    Characters::StringBuilder sb;
    sb += L"{";
    sb += L"Internal-Interface-ID: " + Characters::ToString (fInternalInterfaceID) + L", ";
    sb += L"Friendly-Name: " + Characters::ToString (fFriendlyName) + L", ";
    if (fDescription) {
        sb += L"Description: " + Characters::ToString (*fDescription) + L", ";
    }
    if (fNetworkGUID) {
        sb += L"Network-GUID: " + Characters::ToString (*fNetworkGUID) + L", ";
    }
    if (fType) {
        sb += L"Type: " + Characters::ToString (*fType) + L", ";
    }
    if (fHardwareAddress) {
        sb += L"Hardware-Address: " + Characters::ToString (*fHardwareAddress) + L", ";
    }
    if (fTransmitSpeedBaud) {
        sb += L"Transmit-Speed-Baud: " + Characters::ToString (*fTransmitSpeedBaud) + L", ";
    }
    if (fReceiveLinkSpeedBaud) {
        sb += L"Receive-Link-Speed-Baud: " + Characters::ToString (*fReceiveLinkSpeedBaud) + L", ";
    }
    if (fWirelessInfo) {
        sb += L"Wireless-Info: " + Characters::ToString (fWirelessInfo) + L", ";
    }
    sb += L"Bindings: " + Characters::ToString (fBindings) + L", ";
    if (fStatus) {
        sb += L"Status: " + Characters::ToString (*fStatus) + L", ";
    }
    sb += L"}";
    return sb.str ();
}

/*
 ********************************************************************************
 ************************** Network::GetInterfaces ******************************
 ********************************************************************************
 */
#if qPlatform_POSIX
namespace {
    Stroika_Foundation_Debug_ATTRIBUTE_NO_SANITIZE ("undefined") Traversal::Iterable<Interface> GetInterfaces_POSIX_ ()
    {
        using Binding = Interface::Binding;
        // @todo - when we supported KeyedCollection - use KeyedCollection instead of mapping
        //Collection<Interface> result;
        Mapping<String, Interface> results;
        auto                       getFlags = [](int sd, const char* name) {
            struct ifreq ifreq {
            };
            Characters::CString::Copy (ifreq.ifr_name, NEltsOf (ifreq.ifr_name), name);

            int r = ::ioctl (sd, SIOCGIFFLAGS, (char*)&ifreq);
            Assert (r == 0 or errno == ENXIO); // ENXIO happens on MacOS sometimes, but never seen on linux
            return r == 0 ? ifreq.ifr_flags : 0;
        };
#if qPlatform_Linux
        auto getWirelessFlag = [](int sd, const char* name) {
            struct iwreq pwrq {
            };
            Characters::CString::Copy (pwrq.ifr_name, NEltsOf (pwrq.ifr_name), name);

            int r = ::ioctl (sd, SIOCGIWNAME, (char*)&pwrq);
            return r == 0;
        };
#endif

        struct ifreq  ifreqs[128]{};
        struct ifconf ifconf {
            sizeof (ifreqs), { reinterpret_cast<char*> (ifreqs) }
        };

        int sd = ::socket (PF_INET, SOCK_STREAM, 0);
        Assert (sd >= 0);
        [[maybe_unused]] auto&& cleanup = Execution::Finally ([sd]() noexcept { ::close (sd); });

        int r = ::ioctl (sd, SIOCGIFCONF, (char*)&ifconf);
        Assert (r == 0);

        for (const ifreq* i = std::begin (ifreqs); reinterpret_cast<const char*> (i) - reinterpret_cast<const char*> (std::begin (ifreqs)) < ifconf.ifc_len;) {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
            DbgTrace ("interface: ifr_name=%s; ifr_addr.sa_family = %d", i->ifr_name, i->ifr_addr.sa_family);
#endif
            String    interfaceName{String::FromSDKString (i->ifr_name)};
            Interface newInterface            = results.LookupValue (interfaceName);
            newInterface.fInternalInterfaceID = interfaceName;
            newInterface.fFriendlyName        = interfaceName; // not great - maybe find better name - but this will do for now...
            int flags                         = getFlags (sd, i->ifr_name);

            if (flags & IFF_LOOPBACK) {
                newInterface.fType = Interface::Type::eLoopback;
            }
#if qPlatform_Linux
            else if (getWirelessFlag (sd, i->ifr_name)) {
                newInterface.fType = Interface::Type::eWIFI;
            }
#endif
            else {
                // NYI
                newInterface.fType = Interface::Type::eWiredEthernet; // WAG - not the right way to tell!
            }

#if qPlatform_Linux
            {
                ifreq tmp = *i;
                if (::ioctl (sd, SIOCGIFHWADDR, &tmp) == 0 and tmp.ifr_hwaddr.sa_family == ARPHRD_ETHER) {
                    newInterface.fHardwareAddress = PrintMacAddr_ (reinterpret_cast<const uint8_t*> (tmp.ifr_hwaddr.sa_data), reinterpret_cast<const uint8_t*> (tmp.ifr_hwaddr.sa_data) + 6);
                }
            }
#endif

#if qPlatform_Linux
            {
                auto getSpeed = [](int sd, const char* name) -> optional<uint64_t> {
                    struct ifreq ifreq {
                    };
                    Characters::CString::Copy (ifreq.ifr_name, NEltsOf (ifreq.ifr_name), name);
                    struct ethtool_cmd edata {
                    };
                    ifreq.ifr_data = reinterpret_cast<caddr_t> (&edata);
                    edata.cmd      = ETHTOOL_GSET;
                    int r          = ioctl (sd, SIOCETHTOOL, &ifreq);
                    if (r != 0) {
                        DbgTrace ("No speed for interface %s, errno=%d", name, errno);
                        return nullopt;
                    }
                    constexpr uint64_t kMegabit_ = 1000 * 1000;
                    DbgTrace ("ethtool_cmd_speed (&edata)=%d", ethtool_cmd_speed (&edata));
                    switch (ethtool_cmd_speed (&edata)) {
                        case SPEED_10:
                            return 10 * kMegabit_;
                        case SPEED_100:
                            return 100 * kMegabit_;
                        case SPEED_1000:
                            return 1000 * kMegabit_;
                        case SPEED_2500:
                            return 2500 * kMegabit_;
                        case SPEED_10000:
                            return 10000 * kMegabit_;
                        default:
                            return nullopt;
                    }
                };
                newInterface.fTransmitSpeedBaud    = getSpeed (sd, i->ifr_name);
                newInterface.fReceiveLinkSpeedBaud = newInterface.fTransmitSpeedBaud;
            }
#endif

            {
                Containers::Set<Interface::Status> status = Memory::ValueOrDefault (newInterface.fStatus);
                if (flags & IFF_RUNNING) {
                    // not right!!! But a start...
                    status.Add (Interface::Status::eConnected);
                    status.Add (Interface::Status::eRunning);
                }
                newInterface.fStatus = status;
            }
            {
                SocketAddress sa{i->ifr_addr};
                if (sa.IsInternetAddress ()) {
                    newInterface.fBindings.Add (Binding{sa.GetInternetAddress ()});
                }
            }
            results.Add (newInterface.fInternalInterfaceID, newInterface);

            // On MacOS (at least) I needed to use IFNAMESIZ + addr.size - as suggested
            // in https://gist.githubusercontent.com/OrangeTide/909204/raw/ed097cf0fc73eb0c44de1b26118f041a36424e3f/showif.c
            //
            // https://linux.die.net/man/7/netdevice strongly suggests ("array of structures" to treat as array - fixed offset per element
            //
            // We'll have to see what other OSes require... --LGP 2018-09-27
#if qPlatform_Linux
            size_t len = sizeof (*i);
#else
            size_t len = IFNAMSIZ + i->ifr_addr.sa_len;
#endif
            i = reinterpret_cast<const ifreq*> (reinterpret_cast<const byte*> (i) + len);
        }
        return results.MappedValues ();
    }
}
#endif

#if qPlatform_Windows
namespace {
    struct WirelessInfoPlus_ : Interface::WirelessInfo {
        // extra info so can be patched into Interface
        optional<uint64_t> fTransmitSpeedBaud;
        optional<uint64_t> fReceiveLinkSpeedBaud;
    };
    Mapping<Common::GUID, WirelessInfoPlus_> GetInterfaces_Windows_WirelessInfo_ ()
    {
        using WirelessInfo = Interface::WirelessInfo;
        Mapping<Common::GUID, WirelessInfoPlus_> results;

        HANDLE hClient = nullptr;
        {
            DWORD dwCurVersion = 0;
            Execution::Platform::Windows::ThrowIfNotERROR_SUCCESS (::WlanOpenHandle (2, NULL, &dwCurVersion, &hClient));
        }
        [[maybe_unused]] auto&& cleanup1 = Execution::Finally ([&]() noexcept { if (hClient !=nullptr) {::WlanCloseHandle (hClient, nullptr);} });

        PWLAN_INTERFACE_INFO_LIST pIfList = nullptr;
        Execution::Platform::Windows::ThrowIfNotERROR_SUCCESS (::WlanEnumInterfaces (hClient, nullptr, &pIfList));
        [[maybe_unused]] auto&& cleanup2 = Execution::Finally ([&]() noexcept { if (pIfList !=nullptr) {::WlanFreeMemory (pIfList);} });

        for (DWORD i = 0; i < pIfList->dwNumberOfItems; i++) {
            PWLAN_INTERFACE_INFO pIfInfo = (WLAN_INTERFACE_INFO*)&pIfList->InterfaceInfo[i];
            WirelessInfoPlus_    wInfo;
            Common::GUID         interfaceGUID{pIfInfo->InterfaceGuid};

            auto mapState = [](WLAN_INTERFACE_STATE s) -> WirelessInfo::State {
                switch (s) {
                    case wlan_interface_state_not_ready:
                        return WirelessInfo::State::eNotReady;
                    case wlan_interface_state_connected:
                        return WirelessInfo::State::eConnected;
                    case wlan_interface_state_ad_hoc_network_formed:
                        return WirelessInfo::State::eAdHocNetworkFormed;
                    case wlan_interface_state_disconnecting:
                        return WirelessInfo::State::eDisconnecting;
                    case wlan_interface_state_disconnected:
                        return WirelessInfo::State::eDisconnected;
                    case wlan_interface_state_associating:
                        return WirelessInfo::State::eAssociating;
                    case wlan_interface_state_discovering:
                        return WirelessInfo::State::eDiscovering;
                    case wlan_interface_state_authenticating:
                        return WirelessInfo::State::eAuthenticating;
                    default:
                        DbgTrace (L"Unknown state %ld\n", s);
                        return WirelessInfo::State::eUnknown;
                }
            };

            wInfo.fState = mapState (pIfInfo->isState);

            // If interface state is connected, call WlanQueryInterface
            // to get current connection attributes
            if (pIfInfo->isState == wlan_interface_state_connected) {
                PWLAN_CONNECTION_ATTRIBUTES pConnectInfo{};
                {
                    DWORD                  connectInfoSize = sizeof (WLAN_CONNECTION_ATTRIBUTES);
                    WLAN_OPCODE_VALUE_TYPE opCode          = wlan_opcode_value_type_invalid;
                    Execution::Platform::Windows::ThrowIfNotERROR_SUCCESS (
                        ::WlanQueryInterface (hClient, &pIfInfo->InterfaceGuid, wlan_intf_opcode_current_connection,
                                              nullptr, &connectInfoSize, (PVOID*)&pConnectInfo, &opCode));
                }
                [[maybe_unused]] auto&& cleanup3 = Execution::Finally ([&]() noexcept { if (pConnectInfo !=nullptr) {::WlanFreeMemory (pConnectInfo);} });

                if (pConnectInfo->isState != pIfInfo->isState) {
                    DbgTrace (L"Not sure how these can differ (except for race condition) - but if they do, maybe worth looking into");
                }

                auto mapConnectionMode = [](WLAN_CONNECTION_MODE s) -> WirelessInfo::ConnectionMode {
                    switch (s) {
                        case wlan_connection_mode_profile:
                            return WirelessInfo::ConnectionMode::eProfile;
                        case wlan_connection_mode_temporary_profile:
                            return WirelessInfo::ConnectionMode::eTemporaryProfile;
                        case wlan_connection_mode_discovery_secure:
                            return WirelessInfo::ConnectionMode::eDiscoverSecrure;
                        case wlan_connection_mode_discovery_unsecure:
                            return WirelessInfo::ConnectionMode::eDiscoverInsecure;
                        case wlan_connection_mode_auto:
                            return WirelessInfo::ConnectionMode::eAuto;
                        case wlan_connection_mode_invalid:
                            return WirelessInfo::ConnectionMode::eInvalid;
                        default:
                            DbgTrace (L"Unknown connection mode %d\n", s);
                            return WirelessInfo::ConnectionMode::eUnknown;
                    }
                };
                wInfo.fConnectionMode = mapConnectionMode (pConnectInfo->wlanConnectionMode);
                wInfo.fProfileName    = pConnectInfo->strProfileName;

                //Association Attributes for this connection
                if (pConnectInfo->wlanAssociationAttributes.dot11Ssid.uSSIDLength != 0) {
                    wInfo.fSSID = String::FromNarrowSDKString (reinterpret_cast<const char*> (pConnectInfo->wlanAssociationAttributes.dot11Ssid.ucSSID), reinterpret_cast<const char*> (pConnectInfo->wlanAssociationAttributes.dot11Ssid.ucSSID + pConnectInfo->wlanAssociationAttributes.dot11Ssid.uSSIDLength));
                }

                auto mapBSSType = [](DOT11_BSS_TYPE s) -> WirelessInfo::BSSType {
                    switch (s) {
                        case dot11_BSS_type_infrastructure:
                            return WirelessInfo::BSSType::eInfrastructure;
                        case dot11_BSS_type_independent:
                            return WirelessInfo::BSSType::eIndependent;
                        case dot11_BSS_type_any:
                            return WirelessInfo::BSSType::eAny;
                        default:
                            DbgTrace (L"Unknown connection mode %d\n", s);
                            return WirelessInfo::BSSType::eUnknown;
                    }
                };
                wInfo.fBSSType = mapBSSType (pConnectInfo->wlanAssociationAttributes.dot11BssType);

                wInfo.fMACAddress = PrintMacAddr_ (std::begin (pConnectInfo->wlanAssociationAttributes.dot11Bssid), std::end (pConnectInfo->wlanAssociationAttributes.dot11Bssid));

                auto mapPhysicalConnectionType = [](DOT11_PHY_TYPE s) -> WirelessInfo::PhysicalConnectionType {
                    switch (s) {
                        case dot11_phy_type_unknown:
                            return WirelessInfo::PhysicalConnectionType::eUnknown;
                        case dot11_phy_type_fhss:
                            return WirelessInfo::PhysicalConnectionType::eFHSS;
                        case dot11_phy_type_dsss:
                            return WirelessInfo::PhysicalConnectionType::eDSSS;
                        case dot11_phy_type_irbaseband:
                            return WirelessInfo::PhysicalConnectionType::eIRBaseBand;
                        case dot11_phy_type_ofdm:
                            return WirelessInfo::PhysicalConnectionType::e80211a;
                        case dot11_phy_type_hrdsss:
                            return WirelessInfo::PhysicalConnectionType::e80211b;
                        case dot11_phy_type_erp:
                            return WirelessInfo::PhysicalConnectionType::e80211g;
                        case dot11_phy_type_ht:
                            return WirelessInfo::PhysicalConnectionType::e80211n;
                        case dot11_phy_type_vht:
                            return WirelessInfo::PhysicalConnectionType::e80211ac;
                        case dot11_phy_type_dmg:
                            return WirelessInfo::PhysicalConnectionType::e80211ad;
                        case dot11_phy_type_he:
                            return WirelessInfo::PhysicalConnectionType::e80211ax;
                        default:
                            DbgTrace (L"Unknown DOT11_PHY_TYPE %d\n", s);
                            return WirelessInfo::PhysicalConnectionType::eUnknown;
                    }
                };
                wInfo.fPhysicalConnectionType = mapPhysicalConnectionType (pConnectInfo->wlanAssociationAttributes.dot11PhyType);

                wInfo.fSignalQuality = pConnectInfo->wlanAssociationAttributes.wlanSignalQuality;

                wInfo.fTransmitSpeedBaud    = pConnectInfo->wlanAssociationAttributes.ulTxRate;
                wInfo.fReceiveLinkSpeedBaud = pConnectInfo->wlanAssociationAttributes.ulRxRate;

                wInfo.fSecurityEnabled = pConnectInfo->wlanSecurityAttributes.bSecurityEnabled;
                wInfo.f8021XEnabled    = pConnectInfo->wlanSecurityAttributes.bOneXEnabled;
                auto mapAuthAlgorithm  = [](DOT11_AUTH_ALGORITHM s) -> WirelessInfo::AuthAlgorithm {
                    switch (s) {
                        case DOT11_AUTH_ALGO_80211_OPEN:
                            return WirelessInfo::AuthAlgorithm::eOpen;
                        case DOT11_AUTH_ALGO_80211_SHARED_KEY:
                            return WirelessInfo::AuthAlgorithm::ePresharedKey;
                        case DOT11_AUTH_ALGO_WPA:
                            return WirelessInfo::AuthAlgorithm::eWPA;
                        case DOT11_AUTH_ALGO_WPA_PSK:
                            return WirelessInfo::AuthAlgorithm::eWPA_PSK;
                        case DOT11_AUTH_ALGO_WPA_NONE:
                            return WirelessInfo::AuthAlgorithm::eWPA_NONE;
                        case DOT11_AUTH_ALGO_RSNA:
                            return WirelessInfo::AuthAlgorithm::eRSNA;
                        case DOT11_AUTH_ALGO_RSNA_PSK:
                            return WirelessInfo::AuthAlgorithm::eRSNA_PSK;
                        default:
                            DbgTrace (L"Unknown AuthAlgorithm %d\n", s);
                            return WirelessInfo::AuthAlgorithm::eUnknown;
                    }
                };
                wInfo.fAuthAlgorithm = mapAuthAlgorithm (pConnectInfo->wlanSecurityAttributes.dot11AuthAlgorithm);

                auto mapCipher = [](DOT11_CIPHER_ALGORITHM s) -> String {
                    switch (s) {
                        case DOT11_CIPHER_ALGO_NONE:
                            return String_Constant{L"None"};
                        case DOT11_CIPHER_ALGO_WEP40:
                            return String_Constant{L"WEP-40"};
                        case DOT11_CIPHER_ALGO_TKIP:
                            return String_Constant{L"TKIP"};
                        case DOT11_CIPHER_ALGO_CCMP:
                            return String_Constant{L"CCMP"};
                        case DOT11_CIPHER_ALGO_WEP104:
                            return String_Constant{L"WEP-104"};
                        case DOT11_CIPHER_ALGO_WEP:
                            return String_Constant{L"WEP"};
                        default:
                            return Characters::Format (L"%d", s);
                    }
                };
                wInfo.fCipher = mapCipher (pConnectInfo->wlanSecurityAttributes.dot11CipherAlgorithm);
                results.Add (interfaceGUID, wInfo);
            }
        }
        return results;
    }

    Traversal::Iterable<Interface> GetInterfaces_Windows_ ()
    {
        Mapping<Common::GUID, WirelessInfoPlus_> wirelessInfo2Merge;
        
        try {
            wirelessInfo2Merge = GetInterfaces_Windows_WirelessInfo_ ();
        }
        catch (const Execution::Platform::Windows::Exception& e) {
            if (e == ERROR_SERVICE_NOT_ACTIVE) {
                // this just means no wireless services active, so return empty iterable
                return Traversal::Iterable<Interface> {};
            }
            Execution::ReThrow ();
        }
        using Binding                                               = Interface::Binding;
        // @todo - when we supported KeyedCollection - use KeyedCollection instead of mapping
        //Collection<Interface> result;
        Mapping<String, Interface>     results;
        ULONG                          flags  = GAA_FLAG_INCLUDE_PREFIX;
        ULONG                          family = AF_UNSPEC; // Both IPv4 and IPv6 addresses
        Memory::SmallStackBuffer<byte> buf;
    Again:
        ULONG                 ulOutBufLen = static_cast<ULONG> (buf.GetSize ());
        PIP_ADAPTER_ADDRESSES pAddresses  = reinterpret_cast<PIP_ADAPTER_ADDRESSES> (buf.begin ());
        // NB: we use GetAdapaterAddresses () instead of GetInterfaceInfo  () so we get non-ipv4 addresses
        DWORD dwRetVal = ::GetAdaptersAddresses (family, flags, nullptr, pAddresses, &ulOutBufLen);
        if (dwRetVal == NO_ERROR) {
            for (PIP_ADAPTER_ADDRESSES currAddresses = pAddresses; currAddresses != nullptr; currAddresses = currAddresses->Next) {
                String    adapterName{String::FromNarrowSDKString (currAddresses->AdapterName)};
                Interface newInterface            = results.LookupValue (adapterName);
                newInterface.fInternalInterfaceID = adapterName;
                newInterface.fFriendlyName        = currAddresses->FriendlyName;
                newInterface.fDescription         = currAddresses->Description;
                static const GUID kZeroGUID_{};
                if (memcmp (&currAddresses->NetworkGuid, &kZeroGUID_, sizeof (kZeroGUID_)) != 0) {
                    newInterface.fNetworkGUID = currAddresses->NetworkGuid;
                }
                switch (currAddresses->IfType) {
                    case IF_TYPE_SOFTWARE_LOOPBACK:
                        newInterface.fType = Interface::Type::eLoopback;
                        break;
                    case IF_TYPE_IEEE80211:
                        newInterface.fType = Interface::Type::eWIFI;
                        break;
                    case IF_TYPE_ETHERNET_CSMACD:
                        newInterface.fType = Interface::Type::eWiredEthernet;
                        break;
                    default:
                        newInterface.fType = Interface::Type::eOther;
                        break;
                }
                if (currAddresses->TunnelType != TUNNEL_TYPE_NONE) {
                    newInterface.fType = Interface::Type::eTunnel;
                }
                switch (currAddresses->OperStatus) {
                    case IfOperStatusUp:
                        newInterface.fStatus = Memory::ValueOrDefault (newInterface.fStatus) + Set<Interface::Status> ({Interface::Status::eConnected, Interface::Status::eRunning});
                        break;
                    case IfOperStatusDown:
                        newInterface.fStatus = Memory::ValueOrDefault (newInterface.fStatus); // keep any existing status values, but don't leave unknown
                        break;
                    default:
                        // Don't know how to interpret the other status states
                        break;
                }
                for (PIP_ADAPTER_UNICAST_ADDRESS pu = currAddresses->FirstUnicastAddress; pu != nullptr; pu = pu->Next) {
                    SocketAddress sa{pu->Address};
                    if (sa.IsInternetAddress ()) {
                        newInterface.fBindings.Add (Binding{sa.GetInternetAddress (), pu->OnLinkPrefixLength == 255 ? optional<uint8_t>{} : pu->OnLinkPrefixLength});
                    }
                }
                for (PIP_ADAPTER_ANYCAST_ADDRESS pa = currAddresses->FirstAnycastAddress; pa != nullptr; pa = pa->Next) {
                    SocketAddress sa{pa->Address};
                    if (sa.IsInternetAddress ()) {
                        newInterface.fBindings.Add (Binding{sa.GetInternetAddress ()});
                    }
                }
                for (PIP_ADAPTER_MULTICAST_ADDRESS pm = currAddresses->FirstMulticastAddress; pm != nullptr; pm = pm->Next) {
                    SocketAddress sa{pm->Address};
                    if (sa.IsInternetAddress ()) {
                        newInterface.fBindings.Add (Binding{sa.GetInternetAddress ()});
                    }
                }
                if (currAddresses->PhysicalAddressLength == 6) {
                    newInterface.fHardwareAddress = PrintMacAddr_ (currAddresses->PhysicalAddress, currAddresses->PhysicalAddress + 6);
                }

#if (NTDDI_VERSION >= NTDDI_WIN6)
                newInterface.fTransmitSpeedBaud    = currAddresses->TransmitLinkSpeed;
                newInterface.fReceiveLinkSpeedBaud = currAddresses->ReceiveLinkSpeed;
#endif

                if (newInterface.fType == Interface::Type::eWIFI) {
                    if (auto owinfo = wirelessInfo2Merge.Lookup (newInterface.fInternalInterfaceID)) {
                        newInterface.fWirelessInfo = *owinfo;
                        WeakAssert (not newInterface.fTransmitSpeedBaud.has_value () or newInterface.fTransmitSpeedBaud == owinfo->fTransmitSpeedBaud);
                        WeakAssert (not newInterface.fReceiveLinkSpeedBaud.has_value () or newInterface.fReceiveLinkSpeedBaud == owinfo->fReceiveLinkSpeedBaud);
                        newInterface.fTransmitSpeedBaud    = Memory::OptionalValue (newInterface.fTransmitSpeedBaud, owinfo->fTransmitSpeedBaud);
                        newInterface.fReceiveLinkSpeedBaud = Memory::OptionalValue (newInterface.fReceiveLinkSpeedBaud, owinfo->fReceiveLinkSpeedBaud);
                    }
                    else {
                        // This happens for down/wifi-direct interfaces
                        // no biggie.
                        // DbgTrace (L"Oops - didn't find wireless interface we should have: %s, avail-keys=%s", Characters::ToString (newInterface.fInternalInterfaceID).c_str (), Characters::ToString (wirelessInfo2Merge.Keys ()).c_str ());
                    }
                }
                else {
                    WeakAssert (not wirelessInfo2Merge.ContainsKey (newInterface.fInternalInterfaceID));
                }
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                DbgTrace (L"newInterface=%s", Characters::ToString (newInterface).c_str ());
#endif
                results.Add (newInterface.fInternalInterfaceID, newInterface);
            }
        }
        else if (dwRetVal == ERROR_BUFFER_OVERFLOW) {
            buf.GrowToSize_uninitialized (ulOutBufLen);
            goto Again;
        }
        else if (dwRetVal == ERROR_NO_DATA) {
            DbgTrace ("There are no network adapters enabled on the local system");
        }
        else {
            Execution::Platform::Windows::Exception::Throw (dwRetVal);
        }

#if USE_NOISY_TRACE_IN_THIS_MODULE_
        DbgTrace (L"returning %s", Characters::ToString (results.MappedValues ()).c_str ());
#endif
        return results.MappedValues ();
    }
}
#endif

Traversal::Iterable<Interface> Network::GetInterfaces ()
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper ctx ("Network::GetInterfaces");
#endif
    // @todo - when we supported KeyedCollection - use KeyedCollection instead of mapping
    //Collection<Interface> result;
#if qPlatform_POSIX
    Traversal::Iterable<Interface> results = GetInterfaces_POSIX_ ();
#elif qPlatform_Windows
    Traversal::Iterable<Interface> results = GetInterfaces_Windows_ ();
#else
    AssertNotImplemented ();
#endif
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    DbgTrace (L"returning %s", Characters::ToString (results).c_str ());
#endif
    return results;
}

/*
 ********************************************************************************
 ************************** Network::GetInterfaceById ***************************
 ********************************************************************************
 */
optional<Interface> Network::GetInterfaceById (const String& internalInterfaceID)
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper ctx ("Network::GetInterfaceById");
#endif
    // @todo - a much more efficent implementation - maybe good enuf to use caller staleness cache with a few seconds staleness
    for (Interface i : Network::GetInterfaces ()) {
        if (i.fInternalInterfaceID == internalInterfaceID) {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
            DbgTrace (L"found interface %s", internalInterfaceID.c_str ());
#endif
            return i;
        }
    }
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    DbgTrace (L"interface %s not found", internalInterfaceID.c_str ());
#endif
    return nullopt;
}
