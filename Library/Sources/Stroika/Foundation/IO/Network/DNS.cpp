/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#include    "../../StroikaPreComp.h"

#include    <cstdio>

#if     qPlatform_POSIX
#include    <unistd.h>
#include    <sys/types.h>
#include    <sys/socket.h>
#include    <netdb.h>
#elif   qPlatform_Windows
#include    <WinSock2.h>
#include    <WS2tcpip.h>
#endif

#include    "../../Characters/Format.h"
#include    "../../Containers/Collection.h"
#include    "../../Execution/ErrNoException.h"
#include    "../../Execution/Finally.h"
#if     qPlatform_Windows
#include    "../../../Foundation/Execution/Platform/Windows/Exception.h"
#endif
#include    "../../Execution/StringException.h"
#include    "SocketAddress.h"

#include    "DNS.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Characters;
using   namespace   Stroika::Foundation::Containers;
using   namespace   Stroika::Foundation::Execution;
using   namespace   Stroika::Foundation::Memory;
using   namespace   Stroika::Foundation::IO;
using   namespace   Stroika::Foundation::IO::Network;



// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1



/*
 ********************************************************************************
 ************************** Network::GetInterfaces ******************************
 ********************************************************************************
 */
DNS DNS::Default ()
{
    static  DNS sDefaultDNS_;
    return sDefaultDNS_;
}

DNS::HostEntry   DNS::GetHostEntry (const String& hostNameOrAddress) const
{
    HostEntry   result;

    addrinfo hints;
    addrinfo* res = nullptr;
    memset ((void*)&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_CANONNAME;
#if defined (AI_IDN)
    hints.ai_flags |= AI_IDN;
#endif
#if defined (AI_CANONIDN)
    hints.ai_flags |= AI_CANONIDN;
#endif
    string  tmp =   hostNameOrAddress.AsUTF8<string> (); // BAD - SB tstring - or??? not sure what...
    int errCode = ::getaddrinfo (tmp.c_str (), nullptr, &hints, &res);
    AssertNotNull (res);
    Execution::Finally cleanup ([res] {
        freeaddrinfo (res);
    });
    if (errCode != 0) {
        DoThrow (StringException (Format (L"DNS-Error: %s (%d)", gai_strerror (errCode), errCode)));
    }

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
            result.fAliases.Add (String::FromUTF8 (i->ai_canonname));
        }
        SocketAddress sa { *i->ai_addr };
        if (sa.IsInternetAddress ()) {
            result.fAddressList.Add (sa.GetInternetAddress ());
        }
    }

#if     USE_NOISY_TRACE_IN_THIS_MODULE_
    DbgTrace (L"Lookup(%s)", hostNameOrAddress.c_str ());
    DbgTrace (L"CANONNAME: %s", result.fCanonicalName.c_str ());
    for (String i : result.fAliases) {
        DbgTrace (L" ALIAS: %s", i.c_str ());
    }
    for (InternetAddress i : result.fAddressList) {
        DbgTrace (L" ADDR: %s", i.As<String> ().c_str ());
    }
#endif
    return result;
}

Optional<String>   DNS::ReverseLookup (const InternetAddress& address) const
{
    char hbuf[NI_MAXHOST];
    SocketAddress   sa { address, 0 };
    sockaddr_storage sadata = sa.As<sockaddr_storage> ();
    int flags = NI_NAMEREQD;
#if     defined (NI_IDN)
    flags |= NI_IDN;
#endif
    int errCode = getnameinfo (reinterpret_cast<const sockaddr*> (&sadata), sizeof (sadata), hbuf, sizeof(hbuf),  NULL, 0, flags);
    switch (errCode) {
        case 0:
            //@todo handle I18N more carefully
            //  NI_IDN -- If this flag is used, then the name found in the lookup process is converted from IDN format
            //  to the locale's encoding if necessary. ASCII-only names are not affected by the conversion, which makes
            //  this flag usable in existing programs and environments.
            return String::FromUTF8 (hbuf);
        case EAI_NONAME:
            return Optional<String> ();
        default:
            DoThrow (StringException (Format (L"DNS-Error: %s (%d)", gai_strerror (errCode), errCode)));
    }
}

Collection<InternetAddress> DNS::GetHostAddresses (const String& hostNameOrAddress) const
{
    return GetHostEntry (hostNameOrAddress).fAddressList;
}
