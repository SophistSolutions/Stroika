/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_CIDR_h_
#define _Stroika_Foundation_IO_Network_CIDR_h_ 1

#include "../../StroikaPreComp.h"

#include "../../Characters/String.h"
#include "../../Memory/Optional.h"
#include "../../Traversal/DiscreteRange.h"

#include "InternetAddress.h"

/**
 *  \file
 *
 *  \version    <a href="code_status.html#Beta">Beta</a>
 *
 * TODO:
 */

namespace Stroika {
    namespace Foundation {
        namespace IO {
            namespace Network {

                using Characters::String;

                /**
                 *
                 */
                class CIDR {
                public:
                    /**
                     *  For CIDR (const InternetAddress& internetAddress) CTOR
                     *      \req internetAddress.GetAddressFamily () is V4 or V6 (not unknown).
                     *
                     *  For CIDR (const String& cidrNotation... CTOR)
                     *      cidrNotation ends with "/NNN" where NNN are digits of an interger where the numberOfBits. This will
                     *      throw an exception if the input is ill-formed (e.g. if number of bits too large for the address).
                     */
                    CIDR (const InternetAddress& internetAddress);
                    CIDR (const CIDR& src) = default;
                    CIDR (const InternetAddress& internetAddress, unsigned int significantBits);
                    CIDR (const String& cidrNotation, InternetAddress::AddressFamily addressFamily = InternetAddress::AddressFamily::UNKNOWN);

                public:
                    /**
                     */
                    nonvirtual CIDR& operator= (const CIDR& rhs) = default;

                public:
                    /**
                     */
                    nonvirtual InternetAddress GetInternetAddress () const;

                public:
                    /**
                     *  Note - for IPv4 and class C, =24
                     */
                    nonvirtual unsigned int GetNumberOfSignificantBits () const;

#if 0
                // @todo - but tricky cuz InternetAddress doesn't naturally fit with DiscreteRange
                public:
                    /**
                     */
                    nonvirtual Traversal::DiscreteRange<InternetAddress> GetRange () const;
#endif

                public:
                    nonvirtual bool operator== (const CIDR& rhs) const;

                public:
                    /**
                     *  @see Characters::ToString ()
                     *
                     */
                    nonvirtual String ToString () const;

                private:
                    InternetAddress fBaseAddress_;
                    unsigned int    fSignificantBits_{}; // for IPv4 and class C, =24
                };
            }
        }
    }
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "CIDR.inl"

#endif /*_Stroika_Foundation_IO_Network_CIDR_h_*/
