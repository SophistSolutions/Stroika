/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include <cstdio>

#if qPlatform_POSIX
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>
#elif qPlatform_Windows
#include <WinSock2.h>

#include <WS2tcpip.h>
#endif

#include "../../Characters/Format.h"
#include "../../Common/TemplateUtilities.h"
#include "../../Containers/Collection.h"
#include "../../Execution/Exceptions.h"
#include "../../Execution/Finally.h"
#if qPlatform_Windows
#include "../../../Foundation/Execution/Platform/Windows/Exception.h"
#include "Platform/Windows/WinSock.h"
#endif
#include "../../Execution/Exceptions.h"
#include "SocketAddress.h"

#include "DNS.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::Execution;
using namespace Stroika::Foundation::Memory;
using namespace Stroika::Foundation::IO;
using namespace Stroika::Foundation::IO::Network;

#if qPlatform_Windows
// API should return char* but MSFT returns WIDECHARS sometimes - undo that
#undef gai_strerror
#define gai_strerror gai_strerrorA
#endif // qW

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1

namespace {
    // @todo - somewhat rough draft - not sure if we need better default_error_condition, or equivilant() overrides
    class getaddrinfo_error_category_ : public error_category { // categorize an error
    public:
        virtual const char* name () const noexcept override
        {
            return "DNS error"; // used to return return "getaddrinfo"; - but the name DNS is more widely recognized, and even though this could be from another source, this name is more clear
        }
        virtual error_condition default_error_condition (int ev) const noexcept override
        {
            switch (ev) {
#if EAI_ADDRFAMILY
                case EAI_ADDRFAMILY:
                    return std::error_condition{errc::address_family_not_supported}; // best approximartion I can find
#endif
#if EAI_NONAME
                case EAI_NONAME:
                    return error_condition{errc::no_such_device}; // best approximartion I can find
#endif
#if EAI_MEMORY
                case EAI_MEMORY:
                    return error_condition{errc::not_enough_memory};
#endif
            }
            return error_condition{errc::bad_message}; // no idea what to return here
        }
        virtual string message (int _Errval) const override
        {
            // On visual studio - vs2k17 - we get a spurrious space at the end of the message. This
            // isn't called for by the spec - http://pubs.opengroup.org/onlinepubs/007904875/functions/gai_strerror.html
            // but isn't prohibited either. Strip it.
            const char* result = ::gai_strerror (_Errval);
            while (isspace (*result)) {
                ++result;
            }
            const char* e = result + ::strlen (result);
            while (result < e and isspace (*(e - 1))) {
                e--;
            }
            return string{result, e};
        }
    };
    const error_category& DNS_error_category () noexcept { return Common::Immortalize<getaddrinfo_error_category_> (); }
}

/*
 ********************************************************************************
 ************************** Network::GetInterfaces ******************************
 ********************************************************************************
 */
DNS DNS::Default ()
{
    static const DNS kDefaultDNS_;
    return kDefaultDNS_;
}

DNS::DNS ()
{
#if qPlatform_Windows
    IO::Network::Platform::Windows::WinSock::AssureStarted ();
#endif
}

DNS::HostEntry DNS::GetHostEntry (const String& hostNameOrAddress) const
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper ctx{Stroika_Foundation_Debug_OptionalizeTraceArgs (L"DNS::HostEntry DNS::GetHostEntry", L"hostNameOrAddress=%s",
                                                                                 Characters::ToString (hostNameOrAddress).c_str ())};
#endif
    HostEntry result;

    addrinfo hints{};
    hints.ai_family   = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags    = AI_CANONNAME;
#if defined(AI_IDN)
    hints.ai_flags |= AI_IDN;
#endif
#if defined(AI_CANONIDN)
    hints.ai_flags |= AI_CANONIDN;
#endif
    string tmp = hostNameOrAddress.AsUTF8<string> (); // BAD - SB tstring - or??? not sure what... - I think need to map to Punycode
    if (not tmp.empty () and tmp[0] == '[' and tmp[tmp.size () - 1] == ']' and isdigit (tmp[1])) {
        // only allowed [] around numeric ip addresses
        tmp = tmp.substr (1, tmp.size () - 2);
    }
    addrinfo*               res     = nullptr;
    int                     errCode = ::getaddrinfo (tmp.c_str (), nullptr, &hints, &res);
    [[maybe_unused]] auto&& cleanup = Execution::Finally ([res] () noexcept { ::freeaddrinfo (res); });
    if (errCode != 0) {
        // @todo - I think we need to capture erron as well if errCode == EAI_SYSTEM (see http://man7.org/linux/man-pages/man3/getaddrinfo.3.html)
        Throw (SystemErrorException (errCode, DNS_error_category ()));
    }
    AssertNotNull (res); // else would have thrown

    // @todo proplerly support http://www.ietf.org/rfc/rfc3987.txt and UTF8 etc.
    // See http://linux.die.net/man/3/getaddrinfo for info on glibc support for AI_IDN etc..
    // and how todo on windows (or do myself portably?)
    // MAYBER done OK?
    //
    //  NI_IDN -- If this flag is used, then the name found in the lookup process is converted from IDN format
    //  to the locale's encoding if necessary. ASCII-only names are not affected by the conversion, which makes
    //  this flag usable in existing programs and environments.
    //

    if (res->ai_canonname != nullptr) {
        // utf8 part a WAG
        result.fCanonicalName = String::FromUTF8 (res->ai_canonname);
    }

    for (addrinfo* i = res; i != nullptr; i = i->ai_next) {
        if (i != res and i->ai_canonname != nullptr and i->ai_canonname[0] != '\0') {
            result.fAliases += String::FromUTF8 (i->ai_canonname);
        }
        SocketAddress sa{*i->ai_addr};
        if (sa.IsInternetAddress ()) {
            result.fAddressList += sa.GetInternetAddress ();
        }
    }

#if USE_NOISY_TRACE_IN_THIS_MODULE_
    DbgTrace (L"Lookup(%s)", hostNameOrAddress.c_str ());
    DbgTrace (L"CANONNAME: %s", result.fCanonicalName.c_str ());
    for (const String& i : result.fAliases) {
        DbgTrace (L" ALIAS: %s", i.c_str ());
    }
    for (const InternetAddress& i : result.fAddressList) {
        DbgTrace (L" ADDR: %s", i.As<String> ().c_str ());
    }
#endif
    return result;
}

optional<String> DNS::ReverseLookup (const InternetAddress& address) const
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper ctx{Stroika_Foundation_Debug_OptionalizeTraceArgs (L"DNS::HostEntry DNS::ReverseLookup", L"address=%s",
                                                                                 Characters::ToString (address).c_str ())};
#endif
    char             hbuf[NI_MAXHOST];
    SocketAddress    sa{address, 0};
    sockaddr_storage sadata = sa.As<sockaddr_storage> ();
    int              flags  = NI_NAMEREQD;
#if defined(NI_IDN)
    flags |= NI_IDN;
#endif
    int errCode = ::getnameinfo (reinterpret_cast<const sockaddr*> (&sadata), static_cast<socklen_t> (sa.GetRequiredSize ()), hbuf,
                                 sizeof (hbuf), NULL, 0, flags);
    switch (errCode) {
        case 0:
            //@todo handle I18N more carefully
            //  NI_IDN -- If this flag is used, then the name found in the lookup process is converted from IDN format
            //  to the locale's encoding if necessary. ASCII-only names are not affected by the conversion, which makes
            //  this flag usable in existing programs and environments.
            return String::FromUTF8 (hbuf);
        case EAI_NONAME:
            return {};
        default:
            Throw (SystemErrorException{errCode, DNS_error_category ()});
    }
}

optional<String> DNS::QuietReverseLookup (const InternetAddress& address) const
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper ctx{Stroika_Foundation_Debug_OptionalizeTraceArgs (L"DNS::HostEntry DNS::ReverseLookup", L"address=%s",
                                                                                 Characters::ToString (address).c_str ())};
#endif
    char             hbuf[NI_MAXHOST];
    SocketAddress    sa{address, 0};
    sockaddr_storage sadata = sa.As<sockaddr_storage> ();
    int              flags  = NI_NAMEREQD;
#if defined(NI_IDN)
    flags |= NI_IDN;
#endif
    int errCode = ::getnameinfo (reinterpret_cast<const sockaddr*> (&sadata), static_cast<socklen_t> (sa.GetRequiredSize ()), hbuf,
                                 sizeof (hbuf), NULL, 0, flags);
    switch (errCode) {
        case 0:
            //@todo handle I18N more carefully
            //  NI_IDN -- If this flag is used, then the name found in the lookup process is converted from IDN format
            //  to the locale's encoding if necessary. ASCII-only names are not affected by the conversion, which makes
            //  this flag usable in existing programs and environments.
            return String::FromUTF8 (hbuf);
        default:
            return {};
    }
}

Sequence<InternetAddress> DNS::GetHostAddresses (const String& hostNameOrAddress) const
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper ctx{Stroika_Foundation_Debug_OptionalizeTraceArgs (L"DNS::HostEntry DNS::GetHostAddresses", L"address=%s",
                                                                                 Characters::ToString (address).c_str ())};
#endif
    return GetHostEntry (hostNameOrAddress).fAddressList;
}

Sequence<InternetAddress> DNS::GetHostAddresses (const String& hostNameOrAddress, InternetAddress::AddressFamily family) const
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper ctx{Stroika_Foundation_Debug_OptionalizeTraceArgs (L"DNS::HostEntry DNS::GetHostAddresses", L"address=%s, family=%s",
                                                                                 Characters::ToString (address).c_str (),
                                                                                 Characters::ToString (family).c_str ())};
#endif
    auto h = GetHostEntry (hostNameOrAddress).fAddressList;
    for (auto i = h.begin (); i != h.end (); ++i) {
        if (i->GetAddressFamily () != family) {
            h.Remove (i);
        }
    }
    return h;
}

InternetAddress DNS::GetHostAddress (const String& hostNameOrAddress) const
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper ctx{Stroika_Foundation_Debug_OptionalizeTraceArgs (L"DNS::HostEntry DNS::GetHostAddresses", L"address=%s",
                                                                                 Characters::ToString (address).c_str ())};
#endif
    auto h = GetHostEntry (hostNameOrAddress).fAddressList;
    if (h.empty ()) {
        Execution::Throw (RuntimeErrorException{"No associated addresses"sv});
    }
    return h[0];
}

InternetAddress DNS::GetHostAddress (const String& hostNameOrAddress, InternetAddress::AddressFamily family) const
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper ctx{Stroika_Foundation_Debug_OptionalizeTraceArgs (L"DNS::HostEntry DNS::GetHostAddresses", L"address=%s, family=%s",
                                                                                 Characters::ToString (address).c_str (),
                                                                                 Characters::ToString (family).c_str ())};
#endif
    auto h = GetHostEntry (hostNameOrAddress).fAddressList;
    for (auto i = h.begin (); i != h.end (); ++i) {
        if (i->GetAddressFamily () != family) {
            h.Remove (i);
        }
    }
    if (h.empty ()) {
        Execution::Throw (RuntimeErrorException{"No associated addresses"sv});
    }
    return h[0];
}
