/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_SocketAddress_h_
#define _Stroika_Foundation_IO_Network_SocketAddress_h_ 1

#include "../../StroikaPreComp.h"

#if qPlatform_POSIX
#include <sys/socket.h>
#elif qPlatform_Windows
#include <WinSock2.h>

#include <WS2tcpip.h>
#endif

#include "InternetAddress.h"

/**
 * TODO:
 *  @todo   Optimize storage for case when sockaddr_storage is too large.
 */

namespace Stroika::Foundation::IO::Network {

    /**
     *  Freindly C++ wrapper on berkley socket sockaddr structure (so sockaddr, sockaddr_in, sockaddr_storage, etc).
     *  A SocketAddress is the combination of an @see InternetAddress with a port#.
     */
    class SocketAddress {
    public:
        /**
        */
        enum class FamilyType : u_short {
            UNSPEC = AF_UNSPEC,
            UNIX   = AF_UNIX,
            INET   = AF_INET,
            INET6  = AF_INET6,
        };
        static constexpr FamilyType INET  = FamilyType::INET;
        static constexpr FamilyType INET6 = FamilyType::INET6;

    public:
        /**
         *  any raw sock_addr, sockaddr_in, or sockaddr_storage arguments must already be in network order.
         *
         *  The overload with InternetAddress and portNumber, take care of this automatically, with the portNumber
         *  in host-byte-order.
         */
        SocketAddress ();
        SocketAddress (const sockaddr& iaddr);
        SocketAddress (const sockaddr_in& iaddr);
        SocketAddress (const sockaddr_in6& iaddr);
        SocketAddress (const sockaddr_storage& iaddr);
#if qPlatform_Windows
        SocketAddress (const SOCKET_ADDRESS& sockaddr);
#endif
        SocketAddress (const InternetAddress& iaddr, uint16_t portNumber);

    public:
        /**
         *  Check if unspecified.
         */
        nonvirtual bool empty () const;

    public:
        /**
         *  Make it empty().
         */
        nonvirtual void clear ();

    public:
        /**
         *  Returns the address family (POSIX defined - but without any defined type): af_family field of sockaddr.
         */
        nonvirtual FamilyType GetAddressFamily () const;

    public:
        /**
         *  SocketAddresses can refer to InternetAddresses, but they can also refer to local structures like
         *  named pipes, special files, etc. This is true if IPv4 or IPv6.
         */
        nonvirtual bool IsInternetAddress () const;

    public:
        /**
         *  \req IsInternetAddress()
         */
        nonvirtual InternetAddress GetInternetAddress () const;

    public:
        /**
         *  \req IsInternetAddress()
         */
        nonvirtual uint16_t GetPort () const;

    public:
        /**
         *  Only specifically specialized variants are supported. As<T> supported variants include:
         *      As<sockaddr> ();
         *      As<sockaddr_storage> ()
         *      As<sockaddr_in> ();        // requires GetAddressFamily() == AF_INET
         *      As<sockaddr_in6> ();       // requires GetAddressFamily() == AF_INET6
         *
         *  \note As<sockaddr_storage> () is nearly always best!
         */
        template <typename T>
        nonvirtual T As () const;

    public:
        /**
         *  @see Characters::ToString ()
         */
        nonvirtual String ToString () const;

    private:
        sockaddr_storage fSocketAddress_;
    };

    // Supported specializations
    template <>
    sockaddr SocketAddress::As<sockaddr> () const;
    template <>
    sockaddr_storage SocketAddress::As<sockaddr_storage> () const;
    template <>
    sockaddr_in SocketAddress::As<sockaddr_in> () const;
    template <>
    sockaddr_in6 SocketAddress::As<sockaddr_in6> () const;

    /**
     *  Take the argument list of internet addresses and convert them to an equivilent list of SocketAddresses - with the given port.
     *
     *  \par Example Usage
     *      \code
     *          Listener l{SocketAddresses (InternetAddresses_Any (), usingPortNumber), options.fBindFlags.Value (), ....
     *      \endcode
     */
    Traversal::Iterable<SocketAddress> SocketAddresses (const Traversal::Iterable<InternetAddress>& internetAddresses, uint16_t portNumber);

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "SocketAddress.inl"

#endif /*_Stroika_Foundation_IO_Network_SocketAddress_h_*/
