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



/*
 ********************************************************************************
 ************************** Network::GetInterfaces ******************************
 ********************************************************************************
 */
Traversal::Iterable<Interface>  Network::GetInterfaces ()
{
    Collection<Interface>   result;
#if     qPlatform_POSIX
    auto getFlags = [] (int sd, const char* name) {
        char buf[1024];

        struct ifreq ifreq;
        memset(&ifreq, 0, sizeof (ifreq));
        strcpy (ifreq.ifr_name, name);

        int r = ioctl (sd, SIOCGIFFLAGS, (char*)&ifreq);
        Assert (r == 0);
        return ifreq.ifr_flags;
    };

    struct ifreq ifreqs[64];
    struct ifconf ifconf;
    memset (&ifconf, 0, sizeof(ifconf));
    ifconf.ifc_req = ifreqs;
    ifconf.ifc_len = sizeof(ifreqs);

    int sd = socket (PF_INET, SOCK_STREAM, 0);
    Assert (sd >= 0);
    Execution::Finally cleanup ([sd] () {
        close (sd);
    });

    int r = ioctl (sd, SIOCGIFCONF, (char*)&ifconf);
    Assert (r == 0);

    for (int i = 0; i < ifconf.ifc_len / sizeof(struct ifreq); ++i) {
#if     qPlatform_AIX
        // I don't understand the logic behind this, but without this, we get errors
        // in getFlags(). We could check there - and handle that with an extra return value, but this
        // is simpler.
        //
        // And - its somewhat prescribed in https://www.ibm.com/developerworks/community/forums/html/topic?id=77777777-0000-0000-0000-000014698597
        //
        if (ifreqs[i].ifr_addr.sa_family != AF_INET) {
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

#if     qPlatform_Linux
        {
            auto getSpeed = [] (int sd, const char* name) -> Optional<uint64_t> {
                struct ifreq ifreq;
                memset(&ifreq, 0, sizeof (ifreq));
                strcpy (ifreq.ifr_name, name);
                struct ethtool_cmd edata;
                memset (&edata, 0, sizeof (edata));
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
    // @todo - a much more efficent implemenation
    for (Interface i : Network::GetInterfaces ()) {
        if (i.fInternalInterfaceID == internalInterfaceID) {
            return i;
        }
    }
    return Optional<Interface> ();
}
