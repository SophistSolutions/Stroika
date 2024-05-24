/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include <cstdio>
#include <filesystem>
#include <random>

#if qPlatform_POSIX
#include <arpa/inet.h>
#include <net/if_arp.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>
#if qPlatform_Linux
#include <linux/ethtool.h>
#include <linux/sockios.h>
#include <linux/wireless.h>
#elif qPlatform_MacOS
#include <net/if.h>
#endif
#elif qPlatform_Windows
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
#if qPlatform_Windows
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
//#define USE_NOISY_TRACE_IN_THIS_MODULE_ 1

#if defined(_MSC_VER)
// support use of Iphlpapi - but better to reference here than in lib entry of project file cuz
// easiser to see/modularize (and only pulled in if this module is referenced)
#pragma comment(lib, "Iphlpapi.lib")
#endif

namespace {
    // Windows uses '-' as separator, and linux ':'. Pick arbitrarily (more linux machines
    // than windows, or soon will be)
    auto PrintMacAddr_ (const uint8_t* macaddrBytes, [[maybe_unused]] const uint8_t* macaddrBytesEnd) -> String
    {
        Require (macaddrBytesEnd - macaddrBytes == 6);
        char buf[100]{};
        (void)snprintf (buf, sizeof (buf), "%02x:%02x:%02x:%02x:%02x:%02x", macaddrBytes[0], macaddrBytes[1], macaddrBytes[2],
                        macaddrBytes[3], macaddrBytes[4], macaddrBytes[5]);
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
    Characters::StringBuilder sb;
    sb << "{"sv;
    sb << "SSID: "sv << fSSID << ", "sv;
    sb << "State: "sv << fState << ", "sv;
    sb << "ConnectionMode: "sv << fConnectionMode << ", "sv;
    sb << "ProfileName: "sv << fProfileName << ", "sv;
    sb << "BSSType: "sv << fBSSType << ", "sv;
    sb << "MACAddress: "sv << fMACAddress << ", "sv;
    sb << "PhysicalConnectionType: "sv << fPhysicalConnectionType << ", "sv;
    sb << "SignalQuality: "sv << fSignalQuality << ", "sv;
    sb << "SecurityEnabled: "sv << fSecurityEnabled << ", "sv;
    sb << "8021XEnabled: "sv << f8021XEnabled << ", "sv;
    sb << "AuthAlgorithm: "sv << fAuthAlgorithm << ", "sv;
    sb << "Cipher: "sv << fCipher;
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
    Characters::StringBuilder sb;
    sb << "{"sv;
    sb << "BoundAddressRanges: "sv << fAddressRanges << ", "sv;
    sb << "BoundAddresses: "sv << fAddresses;
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
    Characters::StringBuilder sb;
    sb << "{"sv;
    sb << "Internal-Interface-ID: "sv << fInternalInterfaceID << ", "sv;
#if qPlatform_POSIX
    sb << "InterfaceName: "sv << GetInterfaceName () << ", "sv;
#endif
    sb << "Friendly-Name: "sv << fFriendlyName << ", "sv;
    if (fDescription) {
        sb << "Description: "sv << *fDescription << ", "sv;
    }
    if (fNetworkGUID) {
        sb << "Network-GUID: "sv << *fNetworkGUID << ", "sv;
    }
    if (fType) {
        sb << "Type: "sv << *fType << ", "sv;
    }
    if (fHardwareAddress) {
        sb << "Hardware-Address: "sv << *fHardwareAddress << ", "sv;
    }
    if (fTransmitSpeedBaud) {
        sb << "Transmit-Speed-Baud: "sv << *fTransmitSpeedBaud << ", "sv;
    }
    if (fReceiveLinkSpeedBaud) {
        sb << "Receive-Link-Speed-Baud: "sv << *fReceiveLinkSpeedBaud << ", "sv;
    }
    if (fWirelessInfo) {
        sb << "Wireless-Info: "sv << fWirelessInfo << ", "sv;
    }
    sb << "Bindings: "sv << fBindings << ", "sv;

    sb << "Gateways: "sv << fGateways << ", "sv;
    sb << "DNS-Servers: "sv << fDNSServers << ", "sv;
    if (fStatus) {
        sb << "Status: "sv << *fStatus;
    }
    sb << "}"sv;
    return sb;
}

/*
 ********************************************************************************
 ************************** Network::GetInterfaces ******************************
 ********************************************************************************
 */
#if qPlatform_POSIX
namespace {

    // NB: On macos, we get:
    //   Interface.cpp:210:71: runtime error: member access within misaligned address 0x70000a774c74 for type 'const ifreq', which requires 8 byte alignment
    //        0x70000a774c74: note: pointer points here
#if qMacUBSanitizerifreqAlignmentIssue_Buggy
    Stroika_Foundation_Debug_ATTRIBUTE_NO_SANITIZE_UNDEFINED
#endif
        Interface
        GetInterfaces_POSIX_mkInterface_ (int sd, const ifreq* i, optional<Interface> prevInterfaceObject2Update)
    {
        Interface newInterface            = prevInterfaceObject2Update.value_or (Interface{});
        newInterface.fInternalInterfaceID = String::FromSDKString (i->ifr_name);
        newInterface.fFriendlyName = newInterface.fInternalInterfaceID; // not great - maybe find better name - but this will do for now...
        auto getFlags              = [] (int sd, const char* name) {
            ifreq ifreq{};
            Characters::CString::Copy (ifreq.ifr_name, NEltsOf (ifreq.ifr_name), name);
            int r = ::ioctl (sd, SIOCGIFFLAGS, (char*)&ifreq);
            Assert (r == 0 or errno == ENXIO); // ENXIO happens on MacOS sometimes, but never seen on linux
            return r == 0 ? ifreq.ifr_flags : 0;
        };
        int flags = getFlags (sd, i->ifr_name);
#if qPlatform_Linux
        auto getWirelessFlag = [] (int sd, const char* name) -> bool {
#if defined(SIOCGIWNAME)
            iwreq pwrq{};
            Characters::CString::Copy (pwrq.ifr_name, NEltsOf (pwrq.ifr_name), name);
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
                newInterface.fHardwareAddress = PrintMacAddr_ (reinterpret_cast<const uint8_t*> (tmp.ifr_hwaddr.sa_data),
                                                               reinterpret_cast<const uint8_t*> (tmp.ifr_hwaddr.sa_data) + 6);
            }
        }
#endif

#if qPlatform_Linux || qPlatform_MacOS
        auto getNetMaskAsPrefix = [] (int sd, const char* name) -> optional<unsigned int> {
            ifreq ifreq{};
            Characters::CString::Copy (ifreq.ifr_name, NEltsOf (ifreq.ifr_name), name);
            int r = ::ioctl (sd, SIOCGIFNETMASK, (char*)&ifreq);
            // On MacOS this often fails, but I've never seen it fail on Linux
            if (r == 0) {
#if qPlatform_Linux
                SocketAddress sa{ifreq.ifr_netmask};
#elif qPlatform_MacOS
                SocketAddress sa{ifreq.ifr_addr};
#endif
                if (sa.IsInternetAddress ()) {
                    InternetAddress ia = sa.GetInternetAddress ();
                    size_t          prefixLen{};
                    for (bool b : ia.As<vector<bool>> ()) {
                        if (b) {
                            prefixLen++;
                        }
                        else {
                            break;
                        }
                    }
                    return prefixLen;
                }
            }
            return nullopt;
        };
#endif

#if qPlatform_Linux || qPlatform_MacOS
        auto getDefaultGateway = [] (const char* name) -> optional<InternetAddress> {
            try {
#if qPlatform_Linux
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
                        swscanf (line[2].c_str (), L"%02x%02x%02x%02x", &tmp[3], &tmp[2], &tmp[1], &tmp[0]);
                        return InternetAddress{static_cast<byte> (tmp[0]), static_cast<byte> (tmp[1]), static_cast<byte> (tmp[2]),
                                               static_cast<byte> (tmp[3])};
                    }
                }
#elif qPlatform_MacOS
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
                pr.SetStdOut (useStdOut);
                pr.Run ();
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
        if (auto gw = getDefaultGateway (i->ifr_name)) {
            auto gws = newInterface.fGateways.value_or (Containers::Sequence<InternetAddress>{});
            if (not gws.Contains (*gw)) {
                gws += *gw;
                newInterface.fGateways = gws;
            }
        }
#endif

#if qPlatform_Linux
        auto getSpeed = [] (int sd, const char* name) -> optional<uint64_t> {
            ifreq ifreq{};
            Characters::CString::Copy (ifreq.ifr_name, NEltsOf (ifreq.ifr_name), name);
            ethtool_cmd edata{};
            ifreq.ifr_data = reinterpret_cast<caddr_t> (&edata);
            edata.cmd      = ETHTOOL_GSET;
            int r          = ioctl (sd, SIOCETHTOOL, &ifreq);
            if (r != 0) {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                DbgTrace ("No speed for interface %s, errno=%d", name, errno); // typicall errno=22{EINVAL} on linux
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
        newInterface.fTransmitSpeedBaud    = getSpeed (sd, i->ifr_name);
        newInterface.fReceiveLinkSpeedBaud = newInterface.fTransmitSpeedBaud;
#endif

        {
            Containers::Set<Interface::Status> status = Memory::NullCoalesce (newInterface.fStatus);
            if (flags & IFF_RUNNING) {
                // see https://stackoverflow.com/questions/11679514/what-is-the-difference-between-iff-up-and-iff-running for difference between IFF_UP and IFF_RUNNING
                status.Add (Interface::Status::eConnected);
                status.Add (Interface::Status::eRunning);
            }
            else {
                // see if we can check if physical cable plugged in - https://stackoverflow.com/questions/808560/how-to-detect-the-physical-connected-state-of-a-network-cable-connector
#if qPlatform_Linux
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
                if (checkCarrierKnownSet (i->ifr_name)) {
                    status.Add (Interface::Status::eConnected);
                }
#endif
            }
            newInterface.fStatus = status;
        }
        {
            SocketAddress sa{i->ifr_addr};
            if (sa.IsInternetAddress ()) {
#if qPlatform_Linux || qPlatform_MacOS
                newInterface.fBindings.fAddressRanges.Add (CIDR{sa.GetInternetAddress (), getNetMaskAsPrefix (sd, i->ifr_name)});
#else
                newInterface.fBindings.fAddressRanges.Add (sa.GetInternetAddress ());
#endif
                newInterface.fBindings.fAddresses.Add (sa.GetInternetAddress ());
            }
        }
#if USE_NOISY_TRACE_IN_THIS_MODULE_
        DbgTrace ("GetInterfaces_POSIX_mkInterface_ returns {}"_f, newInterface);
#endif
        return newInterface;
    }
#if qMacUBSanitizerifreqAlignmentIssue_Buggy
    Stroika_Foundation_Debug_ATTRIBUTE_NO_SANITIZE_UNDEFINED
#endif
        Traversal::Iterable<Interface>
        GetInterfaces_POSIX_ ()
    {
        KeyedCollection<Interface, String> results{[] (const Interface& i) { return i.fInternalInterfaceID; }};

        ifreq  ifreqs[128]{};
        ifconf ifconf{sizeof (ifreqs), {reinterpret_cast<char*> (ifreqs)}};

        int sd = ::socket (PF_INET, SOCK_STREAM, 0);
        Assert (sd >= 0);
        [[maybe_unused]] auto&& cleanup = Execution::Finally ([sd] () noexcept { ::close (sd); });

        [[maybe_unused]] int r = ::ioctl (sd, SIOCGIFCONF, (char*)&ifconf);
        Assert (r == 0);

        for (const ifreq* i = std::begin (ifreqs);
             reinterpret_cast<const char*> (i) - reinterpret_cast<const char*> (std::begin (ifreqs)) < ifconf.ifc_len;) {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
            DbgTrace ("interface: ifr_name=%s; ifr_addr.sa_family = %d", i->ifr_name, i->ifr_addr.sa_family);
#endif
            String interfaceName{String::FromSDKString (i->ifr_name)};

            // @todo - On MacOS - we get multiple copies of the same interface (one for each address family on that interface). Redo this code
            // to be smarter about merging these
            Interface newInterface = GetInterfaces_POSIX_mkInterface_ (sd, i, results.Lookup (interfaceName));

            results.Add (newInterface);

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
        return move (results);
    }
}
#endif

#if qPlatform_Windows
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
                (DWORD (WINAPI*) (_In_ HANDLE hClientHandle, _In_ CONST GUID * pInterfaceGuid, _In_ WLAN_INTF_OPCODE OpCode,
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
        (_In_ HANDLE hClientHandle, _In_ CONST GUID* pInterfaceGuid, _In_ WLAN_INTF_OPCODE OpCode, _Reserved_ PVOID pReserved, _Out_ PDWORD pdwDataSize,
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
                                return Characters::Format ("{}"_f, static_cast<int> (s));
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
            if (e.code () == error_code{ERROR_SERVICE_NOT_ACTIVE, system_category ()}) {
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

                static constexpr GUID kZeroGUID_{};
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
                for (PIP_ADAPTER_MULTICAST_ADDRESS pm = currAddresses->FirstMulticastAddress; pm != nullptr; pm = pm->Next) {
                    SocketAddress sa{pm->Address};
                    if (sa.IsInternetAddress ()) {
                        newInterface.fBindings.fAddressRanges.Add (sa.GetInternetAddress ());
                        newInterface.fBindings.fAddresses.Add (sa.GetInternetAddress ());
                    }
                }
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
#if qPlatform_POSIX
    Traversal::Iterable<Interface> results = GetInterfaces_POSIX_ ();
#elif qPlatform_Windows
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
