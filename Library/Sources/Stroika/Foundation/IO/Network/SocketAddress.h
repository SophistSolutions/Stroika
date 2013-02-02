/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_SocketAddress_h_
#define _Stroika_Foundation_IO_Network_SocketAddress_h_    1

#include    "../../StroikaPreComp.h"

#if     qPlatform_POSIX
//#include    <unistd.h>
#include    <sys/socket.h>
#elif   qPlatform_Windows
#include    <WinSock2.h>
#include    <ws2tcpip.h>
#endif

#include    "InternetAddress.h"


/**
 * TODO:
 *
 *
 */


namespace   Stroika {
    namespace   Foundation {
        namespace   IO {
            namespace   Network {

                /**
                 */
                class SocketAddress {
                public:
                    typedef u_short FamilyType;

                public:
                    SocketAddress ();
                    SocketAddress (const sockaddr& iaddr);
                    SocketAddress (const sockaddr_in& iaddr);
                    SocketAddress (const InternetAddress& iaddr, uint16_t portNumber);

                public:
                    /**
                     *  Check if unspecified.
                     */
                    nonvirtual  bool    empty () const;

                public:
                    /**
                     *  Make it empty().
                     */
                    nonvirtual  void    clear ();

                public:
                    /**
                     *  Returns the address family (POSIX defined - but without any defined type????)...
                     *  af_family field of sockaddr.
                     */
                    nonvirtual  FamilyType    GetAddressFamily () const;

                public:
                    /**
                     *  SocketAddresses can refer to InternetAddresses, but they can also refer to local structures like
                     *  named pipes, special files, etc. This is true if IPv4 or IPv6.
                     */
                    nonvirtual  bool    IsInternetAddress () const;

                public:
                    /**
                     *  \req IsInternetAddress()
                     */
                    nonvirtual  InternetAddress    GetInternetAddress () const;

                public:
                    /**
                     *  \req IsInternetAddress()
                     */
                    nonvirtual  uint16_t    GetPort () const;

                public:
                    /**
                     *  As<T> supported variants include:
                     *      As<sockaddr> ();
                     *      As<sockaddr_in> ();        // requires GetAddressFamily() == AF_INET
                     *      As<sockaddr_in6> ();       // requires GetAddressFamily() == AF_INET6
                     */
                    template    <typename T>
                    nonvirtual  T   As () const;

                private:
                    sockaddr    fSocketAddress_;
                };

                // Supported specializations
                template    <>
                sockaddr        SocketAddress::As<sockaddr> () const;
                template    <>
                sockaddr_in     SocketAddress::As<sockaddr_in> () const;
                template    <>
                sockaddr_in6    SocketAddress::As<sockaddr_in6> () const;


            }
        }
    }
}
#endif  /*_Stroika_Foundation_IO_Network_SocketAddress_h_*/




/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "SocketAddress.inl"
