/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#include    "../../StroikaPreComp.h"

#include     "../../Execution/ErrNoException.h"

#include    "InternetAddress.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Memory;
using   namespace   Stroika::Foundation::IO;
using   namespace   Stroika::Foundation::IO::Network;



#define     qSupportPTONAndPTON_ (qPlatform_POSIX || (qPlatformWindows && (NTDDI_VERSION >= NTDDI_VISTA)))


namespace {
    constexpr   in_addr     kV4AddrAny_ =   { 0 };
    constexpr   in6_addr    kV6AddrAny_ =   { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 };
}


const   InternetAddress V4::kAddrAny    =   InternetAddress (kV4AddrAny_);
const   InternetAddress V6::kAddrAny    =   InternetAddress (kV6AddrAny_);



namespace {
    inline  const   in_addr kV4Localhost_ ()
    {
        // @todo - check if this localhost is right? May have byte order backwards - net or host byteorder???
        in_addr p;
        p.s_addr = INADDR_LOOPBACK;
        return p;
    }
    constexpr   in6_addr    kV6Localhost_   =   { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 };
}
const   InternetAddress V4::kLocalhost  =   InternetAddress (kV4Localhost_ ());
const   InternetAddress V6::kLocalhost  =   InternetAddress (kV6Localhost_);





/*
 ********************************************************************************
 ********************* IO::Network::InternetAddress *****************************
 ********************************************************************************
 */
InternetAddress::InternetAddress (const String& s, AddressFamily af)
    : fAddressFamily_ (AddressFamily::UNKNOWN)
{
    if (not s.empty ()) {
        if (af == AddressFamily::UNKNOWN) {
            // guess format - based on '.' versus ':' in name
            if (s.Contains ('.')) {
                af = AddressFamily::V4;
            }
            else if (s.Contains (':')) {
                af = AddressFamily::V6;
            }
        }
        switch (af) {
            case AddressFamily::V4: {
#if     qSupportPTONAndPTON_
                    Execution::ThrowErrNoIfNegative (inet_pton (AF_INET, s.AsUTF8 ().c_str (), &fV4_));
#elif   qPlatform_Windows
                    fV4_.s_addr = ::inet_addr (s.AsUTF8 ().c_str ());
#else
                    AssertNotImplemented ();
#endif
                    fAddressFamily_ = af;
                }
                break;
            case AddressFamily::V6: {
#if     qSupportPTONAndPTON_
                    Execution::ThrowErrNoIfNegative (inet_pton (AF_INET6, s.AsUTF8 ().c_str (), &fV6_));
#else
                    AssertNotImplemented ();
#endif
                    fAddressFamily_ = af;
                }
                break;
        }
    }
}

namespace   Stroika {
    namespace   Foundation {
        namespace   IO {
            namespace   Network {
                template    <>
                String  InternetAddress::As<String> () const
                {
                    switch (fAddressFamily_) {
                        case AddressFamily::UNKNOWN: {
                                return String ();
                            }
                            break;
                        case AddressFamily::V4: {
#if  qSupportPTONAndPTON_
                                char    buf[INET_ADDRSTRLEN + 1];
                                const char*   result  =   ::inet_ntop (AF_INET, &fV4_, buf, sizeof (buf));
                                return result == nullptr ? String () : String::FromUTF8 (result);
#else
                                AssertNotImplemented ();
                                return String ();
#endif
                            }
                            break;
                        case AddressFamily::V6: {
#if  qSupportPTONAndPTON_
                                char    buf[INET6_ADDRSTRLEN + 1];
                                const char*   result  =   ::inet_ntop (AF_INET6, &fV6_, buf, sizeof (buf));
                                return result == nullptr ? String () : String::FromUTF8 (result);
#else
                                AssertNotImplemented ();
                                return String ();
#endif
                            }
                            break;
                        default: {
                                RequireNotReached ();
                                return String ();
                            }
                            break;
                    }
                }
            }
        }
    }
}


bool    InternetAddress::IsLocalhostAddress () const
{
    Require (not empty ());
    switch (fAddressFamily_) {
        case AddressFamily::V4: {
                // 127.0.0.x
                return (fV4_.s_addr & 0xff000000) == 0x7f000000;
            }
            break;
        case AddressFamily::V6: {
                return
                    fV6_.s6_addr[0] == 0 and
                    fV6_.s6_addr[1] == 0 and
                    fV6_.s6_addr[2] == 0 and
                    fV6_.s6_addr[3] == 0 and
                    fV6_.s6_addr[4] == 0 and
                    fV6_.s6_addr[5] == 0 and
                    fV6_.s6_addr[6] == 0 and
                    fV6_.s6_addr[7] == 0 and
                    fV6_.s6_addr[8] == 0 and
                    fV6_.s6_addr[9] == 0 and
                    fV6_.s6_addr[10] == 0 and
                    fV6_.s6_addr[11] == 0 and
                    fV6_.s6_addr[12] == 0 and
                    fV6_.s6_addr[13] == 0 and
                    fV6_.s6_addr[14] == 1 and
                    fV6_.s6_addr[15] == 0
                    ;
            }
            break;
    }
    AssertNotReached ();
    return false;
}

bool    InternetAddress::IsPrivateAddress () const
{
    AssertNotImplemented ();
    return false;
}

bool    InternetAddress::IsMulticastAddress () const
{
    Require (not empty ());
    switch (fAddressFamily_) {
        case AddressFamily::V4: {
                // Not sure - might have byte order backwards??? or totally wrong - a bit of a guess?
                return (fV4_.s_addr & 0xf0000000) == 0xE0000000;
            }
            break;
        case AddressFamily::V6: {
                return (fV6_.s6_addr[0] == 0xff);
            }
            break;
    }
    AssertNotReached ();
    return false;
}
