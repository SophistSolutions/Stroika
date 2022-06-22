/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_CIDR_h_
#define _Stroika_Foundation_IO_Network_CIDR_h_ 1

#include "../../StroikaPreComp.h"

#if defined(__cpp_impl_three_way_comparison)
#include <compare>
#endif

#include "../../Characters/String.h"
#include "../../Traversal/DiscreteRange.h"

#include "InternetAddress.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 *
 * TODO:
 */

namespace Stroika::Foundation::IO::Network {

    using Characters::String;

    /**
     *  \note <a href="Design Overview.md#Comparisons">Comparisons</a>:
     *      o   Standard Stroika Comparison support (operator<=>,operator==, etc);
     *
     *      Compare the significant bits of the CIDR.
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
         *      And the internet-address part will be parsed as by InternetAddress{String} CTOR, and will also throw
         *      on bad format (basically must be valid numeric internet address format).
         * 
         *      This (string) API should always work with the output of CIDR::As<String> () - but will NOT in general work
         *      with the output of CIDR::ToString () (since the later is for display purposes).
         */
        CIDR ()                = delete;
        CIDR (const CIDR& src) = default;
        CIDR (CIDR&& src)      = default;
        CIDR (const InternetAddress& internetAddress, optional<unsigned int> significantBits = nullopt);
        CIDR (const InternetAddress& internetAddress, unsigned int significantBits);
        CIDR (const String& cidrNotation, InternetAddress::AddressFamily addressFamily = InternetAddress::AddressFamily::UNKNOWN);

    public:
        /**
         */
        nonvirtual CIDR& operator= (CIDR&& rhs) = default;
        nonvirtual CIDR& operator= (const CIDR& rhs) = default;

    public:
        /**
         */
        nonvirtual InternetAddress GetBaseInternetAddress () const;

    public:
        /**
         *  Note - for IPv4 and class C, =24
         */
        nonvirtual unsigned int GetNumberOfSignificantBits () const;

    public:
        /** 
         *  A CIDR designates a range of IP addresses, and GetRange returns that range.
         *
         *  \par Example Usage
         *      \code
         *          CIDR cidr { L"192.168.243.0/24" };
         *          InternetAddress lb = cidr.GetRange ().GetLowerBound ();
         *          InternetAddress ub = cidr.GetRange ().GetUpperBound ();
         *          unsigned int    nAddresses = cird.GetRange ().GetNumberOfContainedPoints ();
         *          for (InternetAddress ia : cidr.GetRange ()) {
         *              print (ia);
         *          }
         *      \endcode
         */
        nonvirtual Traversal::DiscreteRange<InternetAddress> GetRange () const;

    public:
        /**
         *  Only specifically specialized variants are supported. As<T> supported variants include:
         *      As<String> ();
         *
         *  \note   As<String> () will always produce a numerical representation, whereas ToString () - will sometimes produce
         *          a textual shortcut, like "INADDR_ANY".
         */
        template <typename T>
        nonvirtual T As () const;

#if __cpp_impl_three_way_comparison >= 201907
    public:
        /**
         */
        nonvirtual strong_ordering operator<=> (const CIDR& rhs) const;
#endif

    public:
        /**
         */
        nonvirtual bool operator== (const CIDR& rhs) const;

    public:
        /**
         *  @see Characters::ToString ()
         */
        nonvirtual String ToString () const;

    private:
        InternetAddress fBaseAddress_;
        unsigned int    fSignificantBits_{}; // for IPv4 and class C, =24
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "CIDR.inl"

#endif /*_Stroika_Foundation_IO_Network_CIDR_h_*/
