/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_InternetAddress_h_
#define _Stroika_Foundation_IO_Network_InternetAddress_h_    1

#include    "../../StroikaPreComp.h"

#if     qPlatform_POSIX
#include    <arpa/inet.h>
#elif   qPlatform_Windows
#include    <WinSock2.h>
#include    <inaddr.h>
#include    <in6addr.h>
#endif

#include    "../../Characters/String.h"
#include    "../../Configuration/Common.h"



/**
 * TODO:
 *
 *      @todo   add operator== method, int compare(), and operator< methods
 *
 *      @todo   IPV6 code not fully implemented on windows (pre-windows-vista)
 *
 *      @todo   Future versions may support converting from IPV4 address to IPV6 by assigning an
 *              IPV4 and saying As<in6_addr> ()? Or maybe have ToIPV6() method?
 */


namespace   Stroika {
    namespace   Foundation {
        namespace   IO {
            namespace   Network {

                using   Characters::String;


                /**
                 *  InternetAddress repesents an internet IP address. Its easily convertable to/from
                 *  the variety of POSIX/Berkley socket formats.
                 *
                 *  InternetAddress supports either IPv4 or IPv6 format addresses.
                 */
                class InternetAddress {
                public:
                    /**
                     *  This can be V4, V6, or UNKNOWN. The value of this flag is the internet af_family type (e.g. AF_INET).
                     */
                    enum class  AddressFamily {
                        UNKNOWN = AF_UNSPEC,
                        V4 = AF_INET,
                        V6 = AF_INET6,
                    };

                public:
                    /**
                     *  Construct an InternetAddress object from a string (with optionally specified address family).
                     *  If the address is unparsable according to the rules specified, an exception will be thrown.
                     */
                    InternetAddress (const String& s, AddressFamily af = AddressFamily::UNKNOWN);
#if     qPlatform_POSIX
                    /**
                     *  Construct an InternetAddress from in_addr_t (v4 ip addr as a long).
                     */
                    InternetAddress (const in_addr_t& i);
#endif
                    /**
                     *  Construct an InternetAddress from in_addr - V4 address.
                     */
                    InternetAddress (const in_addr& i);
                    /**
                     *  Construct an InternetAddress from in6_addr - V6 address.
                     */
                    InternetAddress (const in6_addr& i);

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
                     *  This can be V4, V6, or UNKNOWN, and iff UNKNOWN, then empty () will return true.
                     */
                    nonvirtual  AddressFamily   GetAddressFamily () const;

                public:
                    /**
                     *  As<T> supported variants include:
                     *      As<String> ();
                     *      As<in_addr_t> ();       // qPlatform_POSIX ONLY
                     *      As<in_addr> ();         // GetAddressFamily () == V4 only
                     *      As<in6_addr> ();        // GetAddressFamily () == V6 only
                     */
                    template    <typename T>
                    nonvirtual  T   As () const;

                private:
                    AddressFamily   fAddressFamily_;
                    in_addr         fV4_;
                    in6_addr        fV6_;
                };


            }
        }
    }
}
#endif  /*_Stroika_Foundation_IO_Network_InternetAddress_h_*/





/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "InternetAddress.inl"
