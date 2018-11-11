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

#include <Iphlpapi.h>
#include <WS2tcpip.h>
#include <netioapi.h>
#endif

#include "../../Characters/CString/Utilities.h"
#include "../../Characters/Format.h"
#include "../../Characters/StringBuilder.h"
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
#include "../../Memory/SmallStackBuffer.h"

#include "Socket.h"

#include "Interface.h"

using std::byte;

using namespace Stroika::Foundation;
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
Stroika_Foundation_Debug_ATTRIBUTE_NO_SANITIZE ("undefined")
    Traversal::Iterable<Interface> Network::GetInterfaces ()
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper ctx ("Network::GetInterfaces");
#endif
    // @todo - when we supported KeyedCollection - use KeyedCollection instead of mapping
    //Collection<Interface> result;
    Mapping<String, Interface> results;
#if qPlatform_POSIX
    auto getFlags = [](int sd, const char* name) {
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
                newInterface.fBindings.Add (sa.GetInternetAddress ());
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
#elif qPlatform_Windows
    ULONG                          flags  = GAA_FLAG_INCLUDE_PREFIX;
    ULONG                          family = AF_UNSPEC; // Both IPv4 and IPv6 addresses
    Memory::SmallStackBuffer<byte> buf;
Again:
    ULONG ulOutBufLen = static_cast<ULONG> (buf.GetSize ());
    PIP_ADAPTER_ADDRESSES pAddresses = reinterpret_cast<PIP_ADAPTER_ADDRESSES> (buf.begin ());
    // NB: we use GetAdapaterAddresses () instead of GetInterfaceInfo  () so we get non-ipv4 addresses
    DWORD dwRetVal = ::GetAdaptersAddresses (family, flags, nullptr, pAddresses, &ulOutBufLen);
    if (dwRetVal == NO_ERROR) {
        for (PIP_ADAPTER_ADDRESSES currAddresses = pAddresses; currAddresses != nullptr; currAddresses = currAddresses->Next) {
            String adapterName{String::FromNarrowSDKString (currAddresses->AdapterName)};
            Interface newInterface = results.LookupValue (adapterName);
            newInterface.fInternalInterfaceID = adapterName;
            newInterface.fFriendlyName = currAddresses->FriendlyName;
            newInterface.fDescription = currAddresses->Description;
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
                    newInterface.fBindings.Add (sa.GetInternetAddress ());
                }
            }
            for (PIP_ADAPTER_ANYCAST_ADDRESS pa = currAddresses->FirstAnycastAddress; pa != nullptr; pa = pa->Next) {
                SocketAddress sa{pa->Address};
                if (sa.IsInternetAddress ()) {
                    newInterface.fBindings.Add (sa.GetInternetAddress ());
                }
            }
            for (PIP_ADAPTER_MULTICAST_ADDRESS pm = currAddresses->FirstMulticastAddress; pm != nullptr; pm = pm->Next) {
                SocketAddress sa{pm->Address};
                if (sa.IsInternetAddress ()) {
                    newInterface.fBindings.Add (sa.GetInternetAddress ());
                }
            }
            if (currAddresses->PhysicalAddressLength == 6) {
                newInterface.fHardwareAddress = PrintMacAddr_ (currAddresses->PhysicalAddress, currAddresses->PhysicalAddress + 6);
            }

#if (NTDDI_VERSION >= NTDDI_WIN6)
            newInterface.fTransmitSpeedBaud = currAddresses->TransmitLinkSpeed;
            newInterface.fReceiveLinkSpeedBaud = currAddresses->ReceiveLinkSpeed;
#endif
#if USE_NOISY_TRACE_IN_THIS_MODULE_ && 0
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
        DbgTrace ("There are no network adapters with IPv4 enabled on the local system");
    }
    else {
        Execution::Platform::Windows::Exception::Throw (dwRetVal);
    }
#else
    AssertNotImplemented ();
#endif
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    DbgTrace (L"returning %s", Characters::ToString (results.MappedValues ()).c_str ());
#endif
    return results.MappedValues ();
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
