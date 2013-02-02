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



#define     qSupportPTONAndPTON (qPlatform_POSIX || (qPlatformWindows && (NTDDI_VERSION >= NTDDI_VISTA)))





/*
 ********************************************************************************
 ********************* IO::Network::InternetAddress *****************************
 ********************************************************************************
 */
InternetAddress::InternetAddress (const String& s, AddressFamily af)
    : fAddressFamily_ (AddressFamily::UNKNOWN)
    , fV4_ ()
    , fV6_ ()
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
#if     qSupportPTONAndPTON
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
#if     qSupportPTONAndPTON
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
#if  qSupportPTONAndPTON
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
#if  qSupportPTONAndPTON
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