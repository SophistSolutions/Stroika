/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#include    "../../StroikaPreComp.h"

#include    <cstdio>
#include    <random>

#if     qPlatform_POSIX
//@todo see how many of these includes are needed
#include    <unistd.h>
#include    <arpa/inet.h>
#include    <net/if.h>
#include    <netinet/in.h>
#include    <netdb.h>
#include    <sys/socket.h>
#include    <sys/ioctl.h>
#if     qPlatform_Linux
#include    <linux/types.h>     // needed on RedHat5
#include    <linux/ethtool.h>
#include    <linux/sockios.h>
#endif
#elif   qPlatform_Windows
#include    <WinSock2.h>
#include    <WS2tcpip.h>
#include    <Iphlpapi.h>
#include    <netioapi.h>
#endif

#include    "../../Characters/CString/Utilities.h"
#include    "../../Characters/Format.h"
#include    "../../Containers/Collection.h"
#include    "../../Containers/Mapping.h"
#include    "../../Execution/ErrNoException.h"
#include    "../../Execution/Finally.h"
#if     qPlatform_Windows
#include    "../../../Foundation/Execution/Platform/Windows/Exception.h"
#endif
#include    "../../Execution/Synchronized.h"
#include    "../../Memory/SmallStackBuffer.h"

#include    "Socket.h"

#include    "Interface.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Containers;
using   namespace   Stroika::Foundation::Execution;
using   namespace   Stroika::Foundation::Memory;
using   namespace   Stroika::Foundation::IO;
using   namespace   Stroika::Foundation::IO::Network;





// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1








#if     defined (_MSC_VER)
// support use of Iphlpapi - but better to reference here than in lib entry of project file cuz
// easiser to see/modularize (and only pulled in if this module is referenced)
#pragma comment (lib, "Iphlpapi.lib")
#endif





/*
 ********************************************************************************
 **************************** Network::Interface ********************************
 ********************************************************************************
 */
const Configuration::EnumNames<Interface::Status>   Interface::Stroika_Enum_Names(Status)
{
    { Interface::Status::eConnected, L"Connected" },
    { Interface::Status::eRunning, L"Running" },
};

const Configuration::EnumNames<Interface::Type>   Interface::Stroika_Enum_Names(Type)
{
    { Interface::Type::eLoopback, L"Loopback" },
    { Interface::Type::eWiredEthernet, L"WiredEthernet" },
    { Interface::Type::eWIFI, L"WIFI" },
    { Interface::Type::eTunnel, L"Tunnel" },
    { Interface::Type::eOther, L"Other" },
};



#if     qPlatform_Linux
// Hack for centos5 support:
//      Overload with linux version so other one wins, but this gets called if other doesnt exist
//      TRY --LGP 2015-05-19
template    <typename HACK = int>
static __inline__ __u32 ethtool_cmd_speed (const struct ethtool_cmd* ep, HACK i = 0)
{
    //return (ep->speed_hi << 16) | ep->speed;
    return ep->speed;
}
#endif



namespace {
    // Windows uses '-' as separator, and linux ':'. Pick arbitrarily (more linux machines
    // than windows, or soon will be)
    auto     PrintMacAddr_ (const uint8_t* macaddrBytes, const uint8_t* macaddrBytesEnd) -> String {
        Require (macaddrBytesEnd - macaddrBytes == 6);
        char     buf[100] {};
        (void)snprintf (buf, sizeof (buf), "%02x:%02x:%02x:%02x:%02x:%02x",
                        macaddrBytes[0], macaddrBytes[1],
                        macaddrBytes[2], macaddrBytes[3],
                        macaddrBytes[4], macaddrBytes[5]
                       );
        return String::FromAscii (buf);
    };
}

/*
 ********************************************************************************
 ************************** Network::GetInterfaces ******************************
 ********************************************************************************
 */
Traversal::Iterable<Interface>  Network::GetInterfaces ()
{
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper ctx ("Network::GetInterfaces");
#endif
    Collection<Interface>   result;
#if     qPlatform_POSIX
    auto getFlags = [] (int sd, const char* name) {
        struct ifreq ifreq {};
        Characters::CString::Copy (ifreq.ifr_name, NEltsOf (ifreq.ifr_name), name);

        int r = ::ioctl (sd, SIOCGIFFLAGS, (char*)&ifreq);
        Assert (r == 0);
        return ifreq.ifr_flags;
    };

    struct ifreq    ifreqs[128] {};
    struct ifconf   ifconf {};
    ifconf.ifc_req = ifreqs;
    ifconf.ifc_len = sizeof(ifreqs);

    int sd = ::socket (PF_INET, SOCK_STREAM, 0);
    Assert (sd >= 0);
    Execution::Finally cleanup ([sd] () {
        ::close (sd);
    });

    int r = ::ioctl (sd, SIOCGIFCONF, (char*)&ifconf);
    Assert (r == 0);

    for (int i = 0; i < ifconf.ifc_len / sizeof(struct ifreq); ++i) {
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
        DbgTrace ("interface: ifr_name=%s; ifr_addr.sa_family = %d", ifreqs[i].ifr_name, ifreqs[i].ifr_addr.sa_family);
#endif
#if     qPlatform_AIX
        // I don't understand the logic behind this, but without this, we get errors
        // in getFlags(). We could check there - and handle that with an extra return value, but this
        // is simpler.
        //
        // And - its somewhat prescribed in https://www.ibm.com/developerworks/community/forums/html/topic?id=77777777-0000-0000-0000-000014698597
        //
        if (ifreqs[i].ifr_addr.sa_family != AF_INET and ifreqs[i].ifr_addr.sa_family != AF_INET6 and ifreqs[i].ifr_addr.sa_family != AF_LINK) {
            // Skip interfaces not bound to and IPv4 or IPv6 address, or AF_LINK (not sure what later is used for)
            // this list of exceptions arrived at experimentally on the one AIX machine I tested (so not good)
            continue;
        }
#endif
        Interface   newInterface;
        String interfaceName { String::FromSDKString (ifreqs[i].ifr_name) };
        newInterface.fInternalInterfaceID = interfaceName;
        newInterface.fFriendlyName = interfaceName;         // not great - maybe find better name - but this will do for now...
        int flags = getFlags (sd, ifreqs[i].ifr_name);

        if (flags & IFF_LOOPBACK) {
            newInterface.fType = Interface::Type::eLoopback;
        }
        else {
            // NYI
            newInterface.fType = Interface::Type::eWiredEthernet;    // WAY - not the right way to tell!
        }

        if (::ioctl (sd, SIOCGIFHWADDR, &ifr) == 0 and ifr.ifr_hwaddr.sa_family != ARPHRD_ETHER) {
            newInterface.fHwardwareAddress = PrintMacAddr_ (reinterpret_cast<const uint8_t*> (ifr.ifr_hwaddr.sa_data), reinterpret_cast<const uint8_t*> (ifr.ifr_hwaddr.sa_data) + 6);
        }


#if     qPlatform_AIX
        {
            auto getSpeed = [] (int sd, const char* name) -> Optional<uint64_t> {
                struct ifreq ifreq;
                (void)::memset (&ifreq, 0, sizeof (ifreq));
                Characters::CString::Copy (ifreq.ifr_name, NEltsOf (ifreq.ifr_name), name);
                int r = ioctl (sd, SIOCGIFBAUDRATE, &ifreq);
                if (r != 0)
                {
                    DbgTrace ("No speed for interface %s, errno=%d", name, errno);
                    return Optional<uint64_t> ();
                }
                return ifreq.ifr_baudrate;
            };
            newInterface.fTransmitSpeedBaud = getSpeed (sd, ifreqs[i].ifr_name);
            newInterface.fReceiveLinkSpeedBaud = newInterface.fTransmitSpeedBaud;
        }
#elif   qPlatform_Linux
        {
            auto getSpeed = [] (int sd, const char* name) -> Optional<uint64_t> {
                struct ifreq ifreq {};
                Characters::CString::Copy (ifreq.ifr_name, NEltsOf (ifreq.ifr_name), name);
                struct ethtool_cmd edata {};
                ifreq.ifr_data = reinterpret_cast<caddr_t> (&edata);
                edata.cmd = ETHTOOL_GSET;
                int r = ioctl(sd, SIOCETHTOOL, &ifreq);
                if (r != 0)
                {
                    DbgTrace ("No speed for interface %s, errno=%d", name, errno);
                    return Optional<uint64_t> ();
                }
                constexpr uint64_t kMegabit_ = 1000 * 1000;
                DbgTrace ("ethtool_cmd_speed (&edata)=%d", ethtool_cmd_speed (&edata));
                switch (ethtool_cmd_speed (&edata))
                {
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
                        return Optional<uint64_t> ();
                }
            };
            newInterface.fTransmitSpeedBaud = getSpeed (sd, ifreqs[i].ifr_name);
            newInterface.fReceiveLinkSpeedBaud = newInterface.fTransmitSpeedBaud;
        }
#endif

        {
            Containers::Set<Interface::Status>  status;
            if (flags & IFF_RUNNING) {
                // not right!!! But a start...
                status.Add (Interface::Status::eConnected);
                status.Add (Interface::Status::eRunning);
            }
            newInterface.fStatus = status;
        }
        newInterface.fBindings.Add (InternetAddress (((struct sockaddr_in*)&ifreqs[i].ifr_addr)->sin_addr));    // @todo fix so works for ipv6 addresses as well!
        result.Add (newInterface);
    }
#elif   qPlatform_Windows
    ULONG flags = GAA_FLAG_INCLUDE_PREFIX;
    ULONG family = AF_UNSPEC;       // Both IPv4 and IPv6 addresses
    Memory::SmallStackBuffer<Byte>  buf(0);
Again:
    ULONG ulOutBufLen = static_cast<ULONG> (buf.GetSize ());
    PIP_ADAPTER_ADDRESSES   pAddresses = reinterpret_cast<PIP_ADAPTER_ADDRESSES> (buf.begin ());
    // NB: we use GetAdapaterAddresses () instead of GetInterfaceInfo  () so we get non-ipv4 addresses
    DWORD dwRetVal = ::GetAdaptersAddresses (family, flags, nullptr, pAddresses, &ulOutBufLen);
    if (dwRetVal == NO_ERROR) {
        for (PIP_ADAPTER_ADDRESSES currAddresses  = pAddresses; currAddresses != nullptr; currAddresses = currAddresses->Next) {
            Interface   newInterface;
            String      adapterName     { String::FromNarrowSDKString (currAddresses->AdapterName) };
            newInterface.fInternalInterfaceID = adapterName;
            newInterface.fFriendlyName = currAddresses->FriendlyName;
            newInterface.fDescription = currAddresses->Description;
            switch (currAddresses->IfType) {
                case    IF_TYPE_SOFTWARE_LOOPBACK:
                    newInterface.fType = Interface::Type::eLoopback;
                    break;
                case    IF_TYPE_IEEE80211:
                    newInterface.fType = Interface::Type::eWIFI;
                    break;
                case    IF_TYPE_ETHERNET_CSMACD:
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
                case    IfOperStatusUp:
                    newInterface.fStatus = Set<Interface::Status> ({Interface::Status::eConnected, Interface::Status::eRunning});
                    break;
                case    IfOperStatusDown:
                    newInterface.fStatus = Set<Interface::Status> ();
                    break;
                default:
                    // Dont know how to interpret the other status states
                    break;
            }
            for (PIP_ADAPTER_UNICAST_ADDRESS pu = currAddresses->FirstUnicastAddress; pu != nullptr; pu = pu->Next) {
                SocketAddress sa { pu->Address };
                if (sa.IsInternetAddress ()) {
                    newInterface.fBindings.Add (sa.GetInternetAddress ());
                }
            }
            for (PIP_ADAPTER_ANYCAST_ADDRESS pa = currAddresses->FirstAnycastAddress; pa != nullptr; pa = pa->Next) {
                SocketAddress sa { pa->Address };
                if (sa.IsInternetAddress ()) {
                    newInterface.fBindings.Add (sa.GetInternetAddress ());
                }
            }
            for (PIP_ADAPTER_MULTICAST_ADDRESS  pm = currAddresses->FirstMulticastAddress; pm != nullptr; pm = pm->Next) {
                SocketAddress sa { pm->Address };
                if (sa.IsInternetAddress ()) {
                    newInterface.fBindings.Add (sa.GetInternetAddress ());
                }
            }

            if (currAddresses->PhysicalAddressLength == 6) {
                newInterface.fHwardwareAddress = PrintMacAddr_ (currAddresses->PhysicalAddress, currAddresses->PhysicalAddress + 6);
            }

#if     (NTDDI_VERSION >= NTDDI_WIN6)
            newInterface.fTransmitSpeedBaud = currAddresses->TransmitLinkSpeed;
            newInterface.fReceiveLinkSpeedBaud = currAddresses->ReceiveLinkSpeed;
#endif
            result.Add (newInterface);
        }
    }
    else if (dwRetVal == ERROR_BUFFER_OVERFLOW) {
        buf.GrowToSize (ulOutBufLen);
        goto Again;
    }
    else if (dwRetVal == ERROR_NO_DATA) {
        DbgTrace ("There are no network adapters with IPv4 enabled on the local system");
    }
    else {
        Execution::Platform::Windows::Exception::DoThrow (dwRetVal);
    }
#else
    AssertNotImplemented ();
#endif
    return result;
}




/*
 ********************************************************************************
 ************************** Network::GetInterfaceById ***************************
 ********************************************************************************
 */
Optional<Interface>  Network::GetInterfaceById (const String& internalInterfaceID)
{
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper ctx ("Network::GetInterfaceById");
#endif
    // @todo - a much more efficent implemenation - maybe good enuf to use caller staleness cache with a few seconds staleness
    for (Interface i : Network::GetInterfaces ()) {
        if (i.fInternalInterfaceID == internalInterfaceID) {
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
            DbgTrace (L"found interface %s", internalInterfaceID.c_str ());
#endif
            return i;
        }
    }
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
    DbgTrace (L"interface %s not found", internalInterfaceID.c_str ());
#endif
    return Optional<Interface> ();
}
