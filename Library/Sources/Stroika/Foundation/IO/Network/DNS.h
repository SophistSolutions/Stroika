/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_DNS_h_
#define _Stroika_Foundation_IO_Network_DNS_h_ 1

#include "../../StroikaPreComp.h"

#include <optional>

#include "../../Characters/String.h"
#include "../../Containers/Sequence.h"

#include "InternetAddress.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Late</a>
 *
 * TODO:
 *      @todo   add CTOR args for DNS resolve instances with params like timeouts.
 *
 *      @todo   add ctor params for prefer IPV4, IPV6 or both
 *
 *      @todo   Possibly support async lookup - getaddrinfo_a() or Windows variation doing async lookup
 */

namespace Stroika::Foundation::IO::Network {

    using Characters::String;
    using Containers::Sequence;

    /**
     *  DNS (Domain Name Service) Resolver.
     *
     *  \note This code used to return Collection<String>/Collection<InternetAddress> but I noticed that
     *        http://man7.org/linux/man-pages/man3/getaddrinfo.3.html says "the application should try using
     *        the addresses in the order in which they are returned" so i switched to Sequence<> to preserve
     *        the order.
     */
    class DNS {
    public:
        /**
         *  Returns the default dns resolver.
         */
        static DNS Default ();

    public:
        DNS ();

    public:
        /**
         */
        struct HostEntry {
            Sequence<InternetAddress> fAddressList;
            Sequence<String>          fAliases;
            String                    fCanonicalName; // aka hostname?
        };

    public:
        /**
         */
        nonvirtual HostEntry GetHostEntry (const String& hostNameOrAddress) const;

    public:
        /**
         *  Lookup the dns name associated with the given ip address. This uses internet
         *  DNS PTR records.
         */
        nonvirtual optional<String> ReverseLookup (const InternetAddress& address) const;

    public:
        /**
         *  Lookup the dns name associated with the given ip address. This uses internet
         *  DNS PTR records, but don't through except in extreme (low memory) circumstances. Just return missing.
         */
        nonvirtual optional<String> QuietReverseLookup (const InternetAddress& address) const;

    public:
        /**
         */
        nonvirtual Sequence<InternetAddress> GetHostAddresses (const String& hostNameOrAddress) const;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "DNS.inl"

#endif /*_Stroika_Foundation_IO_Network_DNS_h_*/
