/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include <cstddef>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <random>

#if qStroika_Platform_POSIX
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <net/if_arp.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>
#if qStroika_Platform_Linux
#include <linux/ethtool.h>
#include <linux/sockios.h>
#include <linux/wireless.h>
#elif qStroika_Platform_MacOS
#include <net/if.h>
#include <net/if_dl.h>
#endif
#elif qStroika_Platform_Windows
#include <WinSock2.h>

#include <Windot11.h> // for DOT11_SSID struct
#include <wlanapi.h>

#include <Iphlpapi.h>
#include <WS2tcpip.h>
#include <netioapi.h>
#endif

#include "Stroika/Foundation/Characters/CString/Utilities.h"
#include "Stroika/Foundation/Characters/Format.h"
#include "Stroika/Foundation/Characters/StringBuilder.h"
#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/Containers/Collection.h"
#include "Stroika/Foundation/Containers/KeyedCollection.h"
#include "Stroika/Foundation/Containers/Mapping.h"
#include "Stroika/Foundation/DataExchange/Variant/CharacterDelimitedLines/Reader.h"
#include "Stroika/Foundation/Debug/Sanitizer.h"
#include "Stroika/Foundation/Execution/Exceptions.h"
#include "Stroika/Foundation/Execution/Finally.h"
#if qStroika_Platform_Windows
#include "Stroika/Foundation/../Foundation/Execution/Platform/Windows/Exception.h"
#endif
#include "Stroika/Foundation/Execution/ProcessRunner.h"
#include "Stroika/Foundation/Execution/Synchronized.h"
#include "Stroika/Foundation/IO/FileSystem/FileInputStream.h"
#include "Stroika/Foundation/IO/Network/DNS.h"
#include "Stroika/Foundation/Memory/Optional.h"
#include "Stroika/Foundation/Streams/MemoryStream.h"

#include "Socket.h"

#include "Interface.h"

using std::byte;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::Execution;
using namespace Stroika::Foundation::Memory;
using namespace Stroika::Foundation::IO;
using namespace Stroika::Foundation::IO::FileSystem;
using namespace Stroika::Foundation::IO::Network;

// Comment this in to turn on aggressive noisy DbgTrace in this module
// #define USE_NOISY_TRACE_IN_THIS_MODULE_ 1

#if defined(_MSC_VER)
// support use of Iphlpapi - but better to reference here than in lib entry of project file cuz
// easier to see/modularize (and only pulled in if this module is referenced)
#pragma comment(lib, "Iphlpapi.lib")
#endif

namespace {
    // Windows uses '-' as separator, and linux ':'. Pick arbitrarily (more linux machines
    // than windows, or soon will be)
    auto PrintMacAddr_ (const uint8_t* macaddrBytes, [[maybe_unused]] const uint8_t* macaddrBytesEnd) -> String
    {
        Require (macaddrBytesEnd - macaddrBytes == 6);
        Stroika_ATTRIBUTE_INDETERMINATE char buf[100];
        (void)snprintf (buf, sizeof (buf), "%02x:%02x:%02x:%02x:%02x:%02x", macaddrBytes[0], macaddrBytes[1], macaddrBytes[2],
                        macaddrBytes[3], macaddrBytes[4], macaddrBytes[5]);
        Assert (::strlen (buf) < std::size (buf)); // else we must patch in '\0' but I think snprintf always works here
        return String{buf};
    };
}

/*
 ********************************************************************************
 ********************** Interface::WirelessInfo *********************************
 ********************************************************************************
 */
String Interface::WirelessInfo::ToString () const
{
    StringBuilder sb;
    sb << "{"sv;
    sb << "SSID: "sv << fSSID;
    sb << ", state: "sv << fState;
    sb << ", connectionMode: "sv << fConnectionMode;
    sb << ", profileName: "sv << fProfileName;
    sb << ", BSSType: "sv << fBSSType;
    sb << ", MACAddress: "sv << fMACAddress;
    sb << ", physicalConnectionType: "sv << fPhysicalConnectionType;
    sb << ", signalQuality: "sv << fSignalQuality;
    sb << ", securityEnabled: "sv << fSecurityEnabled;
    sb << ", 8021XEnabled: "sv << f8021XEnabled;
    sb << ", authAlgorithm: "sv << fAuthAlgorithm;
    sb << ", cipher: "sv << fCipher;
    sb << "}"sv;
    return sb;
}

/*
 ********************************************************************************
 ******************************* Interface::Bindings ****************************
 ********************************************************************************
 */
String Interface::Bindings::ToString () const
{
    StringBuilder sb;
    sb << "{"sv;
    sb << "boundAddressRanges: "sv << fAddressRanges;
    sb << ", boundAddresses: "sv << fAddresses;
    sb << "}"sv;
    return sb;
}
/*
 ********************************************************************************
 *********************************** Interface **********************************
 ********************************************************************************
 */
String Interface::ToString () const
{
    StringBuilder sb;
    sb << "{"sv;
    sb << "internalInterfaceID: "sv << fInternalInterfaceID;
#if qStroika_Platform_POSIX
    sb << ", interfaceName: "sv << GetInterfaceName ();
#endif
    sb << ", friendlyName: "sv << fFriendlyName;
    if (fDescription) {
        sb << ", description: "sv << *fDescription;
    }
    if (fNetworkGUID) {
        sb << ", networkGUID: "sv << *fNetworkGUID;
    }
    if (fType) {
        sb << ", type: "sv << *fType;
    }
    if (fHardwareAddress) {
        sb << ", hardwareAddress: "sv << *fHardwareAddress;
    }
    if (fTransmitSpeedBaud) {
        sb << ", transmitSpeedBaud: "sv << *fTransmitSpeedBaud;
    }
    if (fReceiveLinkSpeedBaud) {
        sb << ", receiveLinkSpeedBaud: "sv << *fReceiveLinkSpeedBaud;
    }
    if (fWirelessInfo) {
        sb << ", wirelessInfo: "sv << fWirelessInfo;
    }
    sb << ", bindings: "sv << fBindings;

    sb << ", gateways: "sv << fGateways;
    sb << ", DNSServers: "sv << fDNSServers;
    if (fStatus) {
        sb << ", status: "sv << *fStatus;
    }
    sb << "}"sv;
    return sb;
}

/*
 ********************************************************************************
 ************************** Network::GetInterfaces ******************************
 ********************************************************************************
 */
#if qStroika_Platform_POSIX
namespace {

    // a getifaddrs () address, if it is an IPv4 or IPv6 one - copied into the right sockaddr type, so an IPv6 address is read whole
    optional<InternetAddress> GetInternetAddress_ (const sockaddr* sa)
    {
        if (sa != nullptr and sa->sa_family == AF_INET) {
            sockaddr_in a{};
            ::memcpy (&a, sa, sizeof (a));
            return InternetAddress{a.sin_addr};
        }
        if (sa != nullptr and sa->sa_family == AF_INET6) {
            sockaddr_in6 a{};
            ::memcpy (&a, sa, sizeof (a));
            return InternetAddress{a.sin6_addr};
        }
        return nullopt;
    }

    // the prefix length a getifaddrs () netmask encodes - read as the ADDRESS's family: macOS can leave the netmask's own
    // sa_family 0, and its sa_len short of the whole sockaddr (the trailing zero bytes left off)
    optional<unsigned int> GetPrefixLength_ (const sockaddr* netmask, int family)
    {
        if (netmask == nullptr) {
            return nullopt;
        }
        sockaddr_storage ss{};
#if qStroika_Platform_MacOS
        ::memcpy (&ss, netmask, min<size_t> (netmask->sa_len, sizeof (ss)));
#else
        ::memcpy (&ss, netmask, family == AF_INET ? sizeof (sockaddr_in) : sizeof (sockaddr_in6));
#endif
        ss.ss_family                   = static_cast<sa_family_t> (family);
        optional<InternetAddress> mask = GetInternetAddress_ (reinterpret_cast<const sockaddr*> (&ss));
        if (not mask) {
            return nullopt;
        }
        unsigned int prefixLen{};
        for (bool b : mask->As<vector<bool>> ()) {
            if (not b) {
                break;
            }
            ++prefixLen;
        }
        return prefixLen;
    }

#if qStroika_Platform_MacOS
    // a getifaddrs () AF_LINK entry's 6-byte link-layer (MAC) address, if it has one. A sockaddr_dl is often longer than the
    // struct (name and address can exceed sdl_data), so it is copied by sa_len into a buffer that holds all of it
    optional<String> GetHardwareAddress_ (const sockaddr* sa)
    {
        if (sa == nullptr or sa->sa_family != AF_LINK) {
            return nullopt;
        }
        alignas (sockaddr_dl) uint8_t buf[sizeof (sockaddr_storage)]{};
        ::memcpy (buf, sa, min<size_t> (sa->sa_len, sizeof (buf)));
        const sockaddr_dl* sdl = reinterpret_cast<const sockaddr_dl*> (buf);
        if (sdl->sdl_alen != 6 or offsetof (sockaddr_dl, sdl_data) + sdl->sdl_nlen + 6 > sizeof (buf)) {
            return nullopt;
        }
        const uint8_t* mac = reinterpret_cast<const uint8_t*> (LLADDR (sdl));
        return PrintMacAddr_ (mac, mac + 6);
    }
#endif

    // everything about one interface except its addresses - GetInterfaces_POSIX_ adds those, one per getifaddrs () entry
    Interface GetInterfaces_POSIX_mkInterface_ (int sd, const char* name, unsigned int flags)
    {
        Interface newInterface;
        newInterface.fInternalInterfaceID = String::FromSDKString (name);
        newInterface.fFriendlyName = newInterface.fInternalInterfaceID; // not great - maybe find better name - but this will do for now...
#if qStroika_Platform_Linux
        auto getWirelessFlag = [] (int sd, const char* name) -> bool {
#if defined(SIOCGIWNAME)
            iwreq pwrq{};
            CString::Copy (pwrq.ifr_name, std::size (pwrq.ifr_name), name);
            int r = ::ioctl (sd, SIOCGIWNAME, (char*)&pwrq);
            return r == 0;
#else
            return false;
#endif
        };
#endif
        if (flags & IFF_LOOPBACK) {
            newInterface.fType = Interface::Type::eLoopback;
        }
#if qStroika_Platform_Linux
        else if (getWirelessFlag (sd, name)) {
            newInterface.fType = Interface::Type::eWIFI;
        }
#endif
        else {
            // NYI
            newInterface.fType = Interface::Type::eWiredEthernet; // WAG - not the right way to tell!
        }

#if qStroika_Platform_Linux
        {
            ifreq tmp{};
            CString::Copy (tmp.ifr_name, std::size (tmp.ifr_name), name);
            if (::ioctl (sd, SIOCGIFHWADDR, &tmp) == 0 and tmp.ifr_hwaddr.sa_family == ARPHRD_ETHER) {
                newInterface.fHardwareAddress = PrintMacAddr_ (reinterpret_cast<const uint8_t*> (tmp.ifr_hwaddr.sa_data),
                                                               reinterpret_cast<const uint8_t*> (tmp.ifr_hwaddr.sa_data) + 6);
            }
        }
#endif

#if qStroika_Platform_Linux || qStroika_Platform_MacOS
        auto getDefaultGateway = [] (const char* name) -> optional<InternetAddress> {
            try {
#if qStroika_Platform_Linux
                DataExchange::Variant::CharacterDelimitedLines::Reader reader{{' ', '\t'}};
                static const filesystem::path                          kFileName_{"/proc/net/route"};
                /*
                 * EXAMPLE OUTPUT:
                 *        cat /proc/net/route
                 *        Iface   Destination     Gateway         Flags   RefCnt  Use     Metric  Mask            MTU     Window  IRTT
                 *        eth0    00000000        010011AC        0003    0       0       0       00000000        0       0       0
                 *        eth0    000011AC        00000000        0001    0       0       0       0000FFFF        0       0       0
                 */
                // Note - /procfs files always unseekable
                for (const Sequence<String>& line :
                     reader.ReadMatrix (FileInputStream::New (kFileName_, IO::FileSystem::FileInputStream::eNotSeekable))) {
                    if (line.size () >= 3 and line[0] == String::FromNarrowSDKString (name) and line[1] == "00000000"sv) {
                        //
                        int tmp[4]{};
                        swscanf (line[2].As<wstring> ().c_str (), L"%02x%02x%02x%02x", &tmp[3], &tmp[2], &tmp[1], &tmp[0]);
                        return InternetAddress{static_cast<byte> (tmp[0]), static_cast<byte> (tmp[1]), static_cast<byte> (tmp[2]),
                                               static_cast<byte> (tmp[3])};
                    }
                }
#elif qStroika_Platform_MacOS
                /*
                 *  NOTE: Could ALSO use netstat -nr   - https://unh.edu/it/kb/article/how-to-route-print-mac-os-x.html
                 *
                 * EXAMPLE OUTPUT:
                 *      >route get default
                 *         route to: default
                 *      destination: default
                 *             mask: default
                 *          gateway: router.asus.com
                 *        interface: en0
                 *            flags: <UP,GATEWAY,DONE,STATIC,PRCLONING>
                 *       recvpipe  sendpipe  ssthresh  rtt,msec    rttvar  hopcount      mtu     expire
                 *             0         0         0         0         0         0      1500         0
                 */
                ProcessRunner                    pr{"route get default"sv};
                Streams::MemoryStream::Ptr<byte> useStdOut = Streams::MemoryStream::New<byte> ();
                pr.Run (nullptr, useStdOut);
                DataExchange::Variant::CharacterDelimitedLines::Reader reader{{':'}};
                optional<String>                                       forInterface;
                optional<String>                                       gateway;
                for (const Sequence<String>& line : reader.ReadMatrix (useStdOut)) {
                    if (line.size () == 2 and line[0] == "interface"sv) {
                        forInterface = line[1];
                    }
                    else if (line.size () == 2 and line[0] == "gateway"sv) {
                        gateway = line[1];
                    }
                }
                if (forInterface == String::FromNarrowSDKString (name) and gateway) {
                    try {
                        return InternetAddress{*gateway};
                    }
                    catch (...) {
                        // frequently fails - cuz its a dns name
                    }
                    try {
                        auto s = IO::Network::DNS::kThe.GetHostAddresses (*gateway);
                        if (not s.empty ()) {
                            return InternetAddress{s.Nth (0)};
                        }
                    }
                    catch (...) {
                        DbgTrace ("got exception converting gateway to address (dns): {}"_f, current_exception ()); // should work...
                    }
                }
#endif
            }
            catch (...) {
                // lot's of reasons this could fail, including running WSL on Windows (2018-12-03)
            }
            return nullopt;
        };
        if (auto gw = getDefaultGateway (name)) {
            newInterface.fGateways = Containers::Sequence<InternetAddress>{*gw};
        }
#endif

#if qStroika_Platform_Linux
        auto getSpeed = [] (int sd, const char* name) -> optional<uint64_t> {
            ifreq ifreq{};
            CString::Copy (ifreq.ifr_name, std::size (ifreq.ifr_name), name);
            ethtool_cmd edata{};
            ifreq.ifr_data = reinterpret_cast<caddr_t> (&edata);
            edata.cmd      = ETHTOOL_GSET;
            int r          = ioctl (sd, SIOCETHTOOL, &ifreq);
            if (r != 0) {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                DbgTrace ("No speed for interface {}, errno={}"_f, name, errno); // typicall errno=22{EINVAL} on linux
#endif
                return nullopt;
            }
            constexpr uint64_t kMegabit_ = 1000 * 1000;
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
        newInterface.fTransmitSpeedBaud    = getSpeed (sd, name);
        newInterface.fReceiveLinkSpeedBaud = newInterface.fTransmitSpeedBaud;
#endif

        {
            Containers::Set<Interface::Status> status;
            if (flags & IFF_RUNNING) {
                // see https://stackoverflow.com/questions/11679514/what-is-the-difference-between-iff-up-and-iff-running for difference between IFF_UP and IFF_RUNNING
                status.Add (Interface::Status::eConnected);
                status.Add (Interface::Status::eRunning);
            }
            else {
                // see if we can check if physical cable plugged in - https://stackoverflow.com/questions/808560/how-to-detect-the-physical-connected-state-of-a-network-cable-connector
#if qStroika_Platform_Linux
                auto checkCarrierKnownSet = [] (const char* id) -> bool {
#if !USE_NOISY_TRACE_IN_THIS_MODULE_
                    Debug::TraceContextSuppressor suppressTraceInThisBlock; // needlessly noisy on linux systems (due to frequent throw), and more heat than light
#endif
                    try {
                        auto fs = FileInputStream::New (filesystem::path{"/sys/class/net"} / id, IO::FileSystem::FileInputStream::eNotSeekable);
                        Memory::BLOB b = fs.ReadAll ();
                        if (b.size () >= 1 and b[0] == static_cast<byte> ('1')) {
                            return true;
                        }
                    }
                    catch (...) {
                    }
                    return false; // unknown if this fails
                };
                if (checkCarrierKnownSet (name)) {
                    status.Add (Interface::Status::eConnected);
                }
#endif
            }
            newInterface.fStatus = status;
        }
#if USE_NOISY_TRACE_IN_THIS_MODULE_
        DbgTrace ("GetInterfaces_POSIX_mkInterface_ returns {}"_f, newInterface);
#endif
        return newInterface;
    }

    Traversal::Iterable<Interface> GetInterfaces_POSIX_ ()
    {
        ifaddrs* ifa = nullptr;
        if (::getifaddrs (&ifa) != 0) {
            ThrowPOSIXErrNo ();
        }
        [[maybe_unused]] auto&& cleanupIfa = Execution::Finally ([ifa] () noexcept { ::freeifaddrs (ifa); });

        int sd = ::socket (PF_INET, SOCK_STREAM, 0); // for the per-interface ioctls
        Assert (sd >= 0);
        [[maybe_unused]] auto&& cleanup = Execution::Finally ([sd] () noexcept { ::close (sd); });

        /*
         *  getifaddrs () has one entry per (interface, address) - and for each interface one more, with no internet address
         *  (AF_PACKET on Linux, AF_LINK on macOS) - so an interface with no IP address is listed too.
         *
         *  Not SIOCGIFCONF, which this used until v3.0d25: on Linux it reports IPv4 addresses only (so interfaces with
         *  no IPv4 address went missing); on macOS its records are packed (misaligned), each holds just a struct sockaddr
         *  (too small for an IPv6 address), and link-local addresses come with the scope id embedded in them.
         */
        KeyedCollection<Interface, String> results{[] (const Interface& i) { return i.fInternalInterfaceID; }};
        for (const ifaddrs* p = ifa; p != nullptr; p = p->ifa_next) {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
            DbgTrace ("interface: ifa_name={}; ifa_addr.sa_family = {}"_f, p->ifa_name, p->ifa_addr == nullptr ? -1 : p->ifa_addr->sa_family);
#endif
            String              interfaceName = String::FromSDKString (p->ifa_name);
            optional<Interface> prev          = results.Lookup (interfaceName);
            Interface           newInterface  = prev ? *prev : GetInterfaces_POSIX_mkInterface_ (sd, p->ifa_name, p->ifa_flags);
            if (optional<InternetAddress> ia = GetInternetAddress_ (p->ifa_addr)) {
                DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Wfree-nonheap-object\"");
                newInterface.fBindings.fAddressRanges.Add (CIDR{*ia, GetPrefixLength_ (p->ifa_netmask, p->ifa_addr->sa_family)});
                DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Wfree-nonheap-object\"");
                newInterface.fBindings.fAddresses.Add (*ia);
            }
#if qStroika_Platform_MacOS
            // macOS has no SIOCGIFHWADDR (as Linux uses above): the hardware address is the interface's AF_LINK entry
            if (optional<String> mac = GetHardwareAddress_ (p->ifa_addr)) {
                newInterface.fHardwareAddress = mac;
            }
#endif
            results.Add (newInterface);
        }
        return move (results);
    }
}
#endif

#if qStroika_Platform_Windows
namespace {
    /* 
     * This DLL is apparently only available if the Wireless Lan Service feature is installed
     *  (see https://forum.kodi.tv/showthread.php?tid=333721&pid=2751888#pid2751888)
     * It is also missing from the docker image mcr.microsoft.com/windows/servercore:ltsc2019.
     * So don't statically link since that caused the whole app to not load. Instead, load the DLL conditionally.
     *
     *      \note switched to this helper class from #pragma comment(lib, "wlanapi.lib") in Stroika v2.1a6, to avoid dependency on
     *            that DLL (not met by simple docker containers).
     */
    struct WLANAPI_ {
        WLANAPI_ ()
            : fDLL{::LoadLibrary (_T ("wlanapi.dll"))}
        {
            Execution::ThrowIfNull (fDLL);
            fWlanOpenHandle = (DWORD (WINAPI*) (_In_ DWORD dwClientVersion, _Reserved_ PVOID pReserved, _Out_ PDWORD pdwNegotiatedVersion,
                                                _Out_ PHANDLE phClientHandle)) (::GetProcAddress (fDLL, "WlanOpenHandle"));
            fWlanCloseHandle =
                (DWORD (WINAPI*) (_In_ HANDLE hClientHandle, _Reserved_ PVOID pReserved)) (::GetProcAddress (fDLL, "WlanCloseHandle"));
            fWlanFreeMemory = (DWORD (WINAPI*) (_In_ PVOID pMemory)) (::GetProcAddress (fDLL, "WlanFreeMemory"));
            fWlanEnumInterfaces =
                (DWORD (WINAPI*) (_In_ HANDLE hClientHandle, _Reserved_ PVOID pReserved,
                                  _Outptr_ PWLAN_INTERFACE_INFO_LIST * ppInterfaceList)) (::GetProcAddress (fDLL, "WlanEnumInterfaces"));
            fWlanQueryInterface =
                (DWORD (WINAPI*) (_In_ HANDLE hClientHandle, _In_ CONST ::GUID * pInterfaceGuid, _In_ WLAN_INTF_OPCODE OpCode,
                                  _Reserved_ PVOID pReserved, _Out_ PDWORD pdwDataSize, _Outptr_result_bytebuffer_ (*pdwDataSize) PVOID * ppData,
                                  _Out_opt_ PWLAN_OPCODE_VALUE_TYPE pWlanOpcodeValueType)) (::GetProcAddress (fDLL, "WlanQueryInterface"));
        }
        WLANAPI_ (const WLANAPI_&) = delete;
        ~WLANAPI_ ()
        {
            if (fDLL != nullptr) {
                Verify (::FreeLibrary (fDLL));
            }
        }
        HINSTANCE fDLL;
        DWORD (WINAPI* fWlanOpenHandle)
        (_In_ DWORD dwClientVersion, _Reserved_ PVOID pReserved, _Out_ PDWORD pdwNegotiatedVersion, _Out_ PHANDLE phClientHandle) = nullptr;
        DWORD (WINAPI* fWlanCloseHandle)
        (_In_ HANDLE hClientHandle, _Reserved_ PVOID pReserved) = nullptr;
        DWORD (WINAPI* fWlanFreeMemory)
        (_In_ PVOID pMemory) = nullptr;
        DWORD (WINAPI* fWlanEnumInterfaces)
        (_In_ HANDLE hClientHandle, _Reserved_ PVOID pReserved, _Outptr_ PWLAN_INTERFACE_INFO_LIST* ppInterfaceList) = nullptr;
        DWORD (WINAPI* fWlanQueryInterface)
        (_In_ HANDLE hClientHandle, _In_ CONST ::GUID* pInterfaceGuid, _In_ WLAN_INTF_OPCODE OpCode, _Reserved_ PVOID pReserved, _Out_ PDWORD pdwDataSize,
         _Outptr_result_bytebuffer_ (*pdwDataSize) PVOID* ppData, _Out_opt_ PWLAN_OPCODE_VALUE_TYPE pWlanOpcodeValueType) = nullptr;
    };
    struct WirelessInfoPlus_ : Interface::WirelessInfo {
        // extra info so can be patched into Interface
        optional<uint64_t> fTransmitSpeedBaud;
        optional<uint64_t> fReceiveLinkSpeedBaud;
    };
    Mapping<Common::GUID, WirelessInfoPlus_> GetInterfaces_Windows_WirelessInfo_ ()
    {
        using WirelessInfo = Interface::WirelessInfo;
        Mapping<Common::GUID, WirelessInfoPlus_> results;

        static unique_ptr<WLANAPI_> sWlanAPI_;
        static once_flag            sOnceFlag_;
        call_once (sOnceFlag_, [&] () { IgnoreExceptionsForCall (sWlanAPI_ = make_unique<WLANAPI_> ()) });

        if (sWlanAPI_) {
            HANDLE hClient = nullptr;
            {
                DWORD dwCurVersion = 0;
                Execution::Platform::Windows::ThrowIfNotERROR_SUCCESS (sWlanAPI_->fWlanOpenHandle (2, NULL, &dwCurVersion, &hClient));
            }
            [[maybe_unused]] auto&& cleanup1 = Execution::Finally ([&] () noexcept {
                if (hClient != nullptr) {
                    sWlanAPI_->fWlanCloseHandle (hClient, nullptr);
                }
            });

            PWLAN_INTERFACE_INFO_LIST pIfList = nullptr;
            Execution::Platform::Windows::ThrowIfNotERROR_SUCCESS (sWlanAPI_->fWlanEnumInterfaces (hClient, nullptr, &pIfList));
            [[maybe_unused]] auto&& cleanup2 = Execution::Finally ([&] () noexcept {
                if (pIfList != nullptr) {
                    sWlanAPI_->fWlanFreeMemory (pIfList);
                }
            });

            //
            // makes more sense for pConnectionInfo to be scoped inside loop, but the example docs in:
            //      https://docs.microsoft.com/en-us/windows/win32/api/wlanapi/nf-wlanapi-wlanqueryinterface
            // keep it out here (so gets re-used in each QueryInterface call) and just deleted once
            // at the end.
            //
            // Scoped inside the loop, we get ASAN errors under windows, so presume the docs / example was right
            // any my intuitions wrong.
            //
            PWLAN_CONNECTION_ATTRIBUTES pConnectInfo{};
            [[maybe_unused]] auto&&     cleanup3 = Execution::Finally ([&] () noexcept {
                if (pConnectInfo != nullptr) {
                    sWlanAPI_->fWlanFreeMemory (pConnectInfo);
                }
            });

            for (DWORD i = 0; i < pIfList->dwNumberOfItems; ++i) {
                PWLAN_INTERFACE_INFO pIfInfo = (WLAN_INTERFACE_INFO*)&pIfList->InterfaceInfo[i];
                WirelessInfoPlus_    wInfo;
                Common::GUID         interfaceGUID{pIfInfo->InterfaceGuid};

                auto mapState = [] (WLAN_INTERFACE_STATE s) -> WirelessInfo::State {
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
                            DbgTrace ("Unknown state {}"_f, static_cast<int> (s));
                            return WirelessInfo::State::eUnknown;
                    }
                };

                wInfo.fState = mapState (pIfInfo->isState);

                // If interface state is connected, call WlanQueryInterface
                // to get current connection attributes
                if (pIfInfo->isState == wlan_interface_state_connected) {
                    {
                        DWORD                  connectInfoSize = sizeof (WLAN_CONNECTION_ATTRIBUTES);
                        WLAN_OPCODE_VALUE_TYPE opCode          = wlan_opcode_value_type_invalid;
                        Execution::Platform::Windows::ThrowIfNotERROR_SUCCESS (
                            sWlanAPI_->fWlanQueryInterface (hClient, &pIfInfo->InterfaceGuid, wlan_intf_opcode_current_connection, nullptr,
                                                            &connectInfoSize, (PVOID*)&pConnectInfo, &opCode));
                    }

                    if (pConnectInfo->isState != pIfInfo->isState) {
                        DbgTrace ("Not sure how these can differ (except for race condition) - but if they do, maybe worth looking into"_f);
                    }

                    auto mapConnectionMode = [] (WLAN_CONNECTION_MODE s) -> WirelessInfo::ConnectionMode {
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
                                DbgTrace ("Unknown connection mode {}"_f, static_cast<int> (s));
                                return WirelessInfo::ConnectionMode::eUnknown;
                        }
                    };
                    wInfo.fConnectionMode = mapConnectionMode (pConnectInfo->wlanConnectionMode);
                    wInfo.fProfileName    = pConnectInfo->strProfileName;

                    //Association Attributes for this connection
                    if (pConnectInfo->wlanAssociationAttributes.dot11Ssid.uSSIDLength != 0) {
                        wInfo.fSSID = String::FromNarrowSDKString (
                            span{reinterpret_cast<const char*> (pConnectInfo->wlanAssociationAttributes.dot11Ssid.ucSSID),
                                 pConnectInfo->wlanAssociationAttributes.dot11Ssid.uSSIDLength});
                    }

                    auto mapBSSType = [] (DOT11_BSS_TYPE s) -> WirelessInfo::BSSType {
                        switch (s) {
                            case dot11_BSS_type_infrastructure:
                                return WirelessInfo::BSSType::eInfrastructure;
                            case dot11_BSS_type_independent:
                                return WirelessInfo::BSSType::eIndependent;
                            case dot11_BSS_type_any:
                                return WirelessInfo::BSSType::eAny;
                            default:
                                DbgTrace ("Unknown BSS mode {}"_f, static_cast<int> (s));
                                return WirelessInfo::BSSType::eUnknown;
                        }
                    };
                    wInfo.fBSSType = mapBSSType (pConnectInfo->wlanAssociationAttributes.dot11BssType);

                    wInfo.fMACAddress = PrintMacAddr_ (std::begin (pConnectInfo->wlanAssociationAttributes.dot11Bssid),
                                                       std::end (pConnectInfo->wlanAssociationAttributes.dot11Bssid));

                    auto mapPhysicalConnectionType = [] (DOT11_PHY_TYPE s) -> WirelessInfo::PhysicalConnectionType {
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
                                DbgTrace (L"Unknown DOT11_PHY_TYPE {}"_f, static_cast<int> (s));
                                return WirelessInfo::PhysicalConnectionType::eUnknown;
                        }
                    };
                    wInfo.fPhysicalConnectionType = mapPhysicalConnectionType (pConnectInfo->wlanAssociationAttributes.dot11PhyType);

                    wInfo.fSignalQuality = pConnectInfo->wlanAssociationAttributes.wlanSignalQuality;

                    {
                        const ULONG kMagicFactor_{1000}; // empirical, so we get same answers from IP_ADAPTER_ADDRESSES_LH::TransmitLinkSpeed
                        wInfo.fTransmitSpeedBaud    = pConnectInfo->wlanAssociationAttributes.ulTxRate * kMagicFactor_;
                        wInfo.fReceiveLinkSpeedBaud = pConnectInfo->wlanAssociationAttributes.ulRxRate * kMagicFactor_;
                    }

                    wInfo.fSecurityEnabled = pConnectInfo->wlanSecurityAttributes.bSecurityEnabled;
                    wInfo.f8021XEnabled    = pConnectInfo->wlanSecurityAttributes.bOneXEnabled;
                    auto mapAuthAlgorithm  = [] (DOT11_AUTH_ALGORITHM s) -> WirelessInfo::AuthAlgorithm {
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
                                DbgTrace (L"Unknown AuthAlgorithm {}"_f, static_cast<int> (s));
                                return WirelessInfo::AuthAlgorithm::eUnknown;
                        }
                    };
                    wInfo.fAuthAlgorithm = mapAuthAlgorithm (pConnectInfo->wlanSecurityAttributes.dot11AuthAlgorithm);

                    auto mapCipher = [] (DOT11_CIPHER_ALGORITHM s) -> String {
                        switch (s) {
                            case DOT11_CIPHER_ALGO_NONE:
                                return "None"sv;
                            case DOT11_CIPHER_ALGO_WEP40:
                                return "WEP-40"sv;
                            case DOT11_CIPHER_ALGO_TKIP:
                                return "TKIP"sv;
                            case DOT11_CIPHER_ALGO_CCMP:
                                return "CCMP"sv;
                            case DOT11_CIPHER_ALGO_WEP104:
                                return "WEP-104"sv;
                            case DOT11_CIPHER_ALGO_WEP:
                                return "WEP"sv;
                            default:
                                return "{}"_f(static_cast<int> (s));
                        }
                    };
                    wInfo.fCipher = mapCipher (pConnectInfo->wlanSecurityAttributes.dot11CipherAlgorithm);
                    results.Add (interfaceGUID, wInfo);
                }
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
        catch (const std::system_error& e) {
            if (Execution::Platform::Windows::IsWin32Error (e.code (), ERROR_SERVICE_NOT_ACTIVE)) {
                // this just means no wireless services active, so leave wirelessInfo2Merge empty
            }
            else {
                // but other errors it makes sense to propagate
                Execution::ReThrow ();
            }
        }
        KeyedCollection<Interface, String> results{[] (const Interface& i) { return i.fInternalInterfaceID; }};
        ULONG                              flags  = GAA_FLAG_INCLUDE_PREFIX | GAA_FLAG_INCLUDE_GATEWAYS;
        ULONG                              family = AF_UNSPEC; // Both IPv4 and IPv6 addresses
        Memory::StackBuffer<byte>          buf;
    Again:
        ULONG                 ulOutBufLen = static_cast<ULONG> (buf.GetSize ());
        PIP_ADAPTER_ADDRESSES pAddresses  = reinterpret_cast<PIP_ADAPTER_ADDRESSES> (buf.begin ());
        // NB: we use GetAdapaterAddresses () instead of GetInterfaceInfo  () so we get non-ipv4 addresses
        //
        // Note also: calling GetAdaptersAddresses () produces about 10 "Invalid parameter passed to C runtime function" lines in the debugger.
        //      @see https://developercommunity.visualstudio.com/content/problem/363323/getadaptersaddresses-invalid-parameter-passed-to-c.html
        //
        DWORD dwRetVal = ::GetAdaptersAddresses (family, flags, nullptr, pAddresses, &ulOutBufLen);
        if (dwRetVal == NO_ERROR) {
            for (PIP_ADAPTER_ADDRESSES currAddresses = pAddresses; currAddresses != nullptr; currAddresses = currAddresses->Next) {
                String    adapterName{String::FromNarrowSDKString (currAddresses->AdapterName)};
                Interface newInterface            = results.LookupValue (adapterName);
                newInterface.fInternalInterfaceID = adapterName;
                newInterface.fFriendlyName        = currAddresses->FriendlyName;
                newInterface.fDescription         = currAddresses->Description;

                static constexpr Common::GUID kZeroGUID_{};
                if (memcmp (&currAddresses->NetworkGuid, &kZeroGUID_, sizeof (kZeroGUID_)) != 0) {
                    newInterface.fNetworkGUID = currAddresses->NetworkGuid;
                }
                switch (currAddresses->IfType) {
                    case IF_TYPE_SOFTWARE_LOOPBACK:
                        newInterface.fType = Interface::Type::eLoopback;
                        break;
                    case IF_TYPE_PROP_VIRTUAL:
                        newInterface.fType = Interface::Type::eDeviceVirtualInternalNetwork;
                        break;
                    case IF_TYPE_IEEE80211:
                        newInterface.fType = Interface::Type::eWIFI;
                        break;
                    case IF_TYPE_ETHERNET_CSMACD:
                        if (newInterface.fDescription->Contains ("VirtualBox Host-Only Ethernet Adapter"sv) or
                            newInterface.fDescription->Contains ("Hyper-V Virtual Ethernet Adapter"sv)) {
                            // a fairly good guess - not sure how to tell for sure
                            newInterface.fType = Interface::Type::eDeviceVirtualInternalNetwork;
                        }
                        else {
                            newInterface.fType = Interface::Type::eWiredEthernet;
                        }
                        break;
                    default:
                        DbgTrace ("Treating unknown currAddresses->IfType = {} type as eOther"_f, currAddresses->IfType);
                        newInterface.fType = Interface::Type::eOther;
                        break;
                }
                if (currAddresses->TunnelType != TUNNEL_TYPE_NONE) {
                    if (newInterface.fType) {
                        DbgTrace ("overwriting type {} with tunneltype"_f, newInterface.fType);
                    }
                    newInterface.fType = Interface::Type::eTunnel;
                }
                switch (currAddresses->OperStatus) {
                    case IfOperStatusUp:
                        newInterface.fStatus = Memory::NullCoalesce (newInterface.fStatus) +
                                               Set<Interface::Status> ({Interface::Status::eConnected, Interface::Status::eRunning});
                        break;
                    case IfOperStatusDown:
                        newInterface.fStatus = Memory::NullCoalesce (newInterface.fStatus); // keep any existing status values, but don't leave unknown
                        break;
                    case IfOperStatusDormant:
                    case IfOperStatusLowerLayerDown:
                        // Not sure about either of these - based on docs in https://msdn.microsoft.com/en-us/library/windows/hardware/ff553790(v=vs.85).aspx - not super clear
                        newInterface.fStatus = Memory::NullCoalesce (newInterface.fStatus) + Set<Interface::Status> ({Interface::Status::eConnected});
                        break;
                    default:
                        // Don't know how to interpret the other status states
                        DbgTrace ("ignoring unrecognized status: {}"_f, static_cast<int> (currAddresses->OperStatus));
                        break;
                }
                for (PIP_ADAPTER_UNICAST_ADDRESS pu = currAddresses->FirstUnicastAddress; pu != nullptr; pu = pu->Next) {
                    SocketAddress sa{pu->Address};
                    if (sa.IsInternetAddress ()) {
                        newInterface.fBindings.fAddressRanges.Add (
                            pu->OnLinkPrefixLength == 255 ? sa.GetInternetAddress () : CIDR{sa.GetInternetAddress (), pu->OnLinkPrefixLength});
                        newInterface.fBindings.fAddresses.Add (sa.GetInternetAddress ());
                    }
                }
                for (PIP_ADAPTER_ANYCAST_ADDRESS pa = currAddresses->FirstAnycastAddress; pa != nullptr; pa = pa->Next) {
                    SocketAddress sa{pa->Address};
                    if (sa.IsInternetAddress ()) {
                        newInterface.fBindings.fAddressRanges.Add (sa.GetInternetAddress ());
                        newInterface.fBindings.fAddresses.Add (sa.GetInternetAddress ());
                    }
                }
                // not FirstMulticastAddress: those are the groups joined (by any process), not this interface's addresses
                for (PIP_ADAPTER_GATEWAY_ADDRESS_LH pa = currAddresses->FirstGatewayAddress; pa != nullptr; pa = pa->Next) {
                    SocketAddress sa{pa->Address};
                    if (sa.IsInternetAddress ()) {
                        auto gws = newInterface.fGateways.value_or (Containers::Sequence<InternetAddress>{});
                        if (not gws.Contains (sa.GetInternetAddress ())) {
                            gws += sa.GetInternetAddress ();
                            newInterface.fGateways = gws;
                        }
                    }
                }
                for (PIP_ADAPTER_DNS_SERVER_ADDRESS_XP pa = currAddresses->FirstDnsServerAddress; pa != nullptr; pa = pa->Next) {
                    SocketAddress sa{pa->Address};
                    if (sa.IsInternetAddress ()) {
                        auto ds = newInterface.fDNSServers.value_or (Containers::Sequence<InternetAddress>{});
                        if (not ds.Contains (sa.GetInternetAddress ())) {
                            ds += sa.GetInternetAddress ();
                            newInterface.fDNSServers = ds;
                        }
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
                        newInterface.fTransmitSpeedBaud = Memory::NullCoalesce (newInterface.fTransmitSpeedBaud, owinfo->fTransmitSpeedBaud);
                        newInterface.fReceiveLinkSpeedBaud = Memory::NullCoalesce (newInterface.fReceiveLinkSpeedBaud, owinfo->fReceiveLinkSpeedBaud);
                    }
                    else {
                        // This happens for down/wifi-direct interfaces
                        // no biggie.
                        // DbgTrace ("Oops - didn't find wireless interface we should have: {}, avail-keys={}"_f, newInterface.fInternalInterfaceID, wirelessInfo2Merge.Keys ());
                    }
                }
                else {
                    WeakAssert (not wirelessInfo2Merge.ContainsKey (newInterface.fInternalInterfaceID));
                }
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                DbgTrace (L"newInterface={}"_f, newInterface);
#endif
                results.Add (newInterface);
            }
        }
        else if (dwRetVal == ERROR_BUFFER_OVERFLOW) {
            buf.GrowToSize_uninitialized (ulOutBufLen);
            goto Again;
        }
        else if (dwRetVal == ERROR_NO_DATA) {
            DbgTrace ("There are no network adapters enabled on the local system"_f);
        }
        else {
            Execution::ThrowSystemErrNo (dwRetVal);
        }

#if USE_NOISY_TRACE_IN_THIS_MODULE_
        DbgTrace ("returning {}"_f, results.MappedValues ());
#endif
        return results;
    }
}
#endif

/*
 ********************************************************************************
 ************************* Network::SystemInterfacesMgr *************************
 ********************************************************************************
 */
Traversal::Iterable<Interface> SystemInterfacesMgr::GetAll ()
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper ctx{"SystemInterfacesMgr::GetAll"};
#endif
#if qStroika_Platform_POSIX
    Traversal::Iterable<Interface> results = GetInterfaces_POSIX_ ();
#elif qStroika_Platform_Windows
    Traversal::Iterable<Interface> results = GetInterfaces_Windows_ ();
#else
    AssertNotImplemented ();
#endif
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    DbgTrace ("returning {}"_f, results);
#endif
    return results;
}

optional<Interface> SystemInterfacesMgr::GetById (const Interface::SystemIDType& internalInterfaceID)
{
    // Made some progress but must refactor the above a little more to be able avoid iterating and just fetch the desired interface (esp on macos).
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper ctx{"Network::GetById"};
#endif
    // @todo - a much more efficent implementation - maybe good enuf to use caller staleness cache with a few seconds staleness
    for (const Interface& i : GetAll ()) {
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

optional<Interface> SystemInterfacesMgr::GetContainingAddress (const InternetAddress& ia)
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper ctx{"Network::GetContainingAddress"};
#endif
    // @todo - a much more efficent implementation - maybe good enuf to use caller staleness cache with a few seconds staleness
    for (const Interface& i : GetAll ()) {
        if (i.fBindings.fAddressRanges.Any ([&ia] (CIDR c) { return c.GetRange ().Contains (ia); })) {
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
