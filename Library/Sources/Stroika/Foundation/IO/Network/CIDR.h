/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_CIDR_h_
#define _Stroika_Foundation_IO_Network_CIDR_h_ 1

#include "../../StroikaPreComp.h"

#include "../../Characters/String.h"

#include "InternetAddressRange.h"

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

    public:
        /** 
         *  A CIDR designates a range of IP addresses, and GetRange returns that range.
         *
         *  \par Example Usage
         *      \code
         *      CIDR cidr { L"192.168.243.0/24" };
         *      InternetAddress lb = cidr.GetRange ().GetLowerBound ();
         *      InternetAddress ub = cidr.GetRange ().GetUpperBound ();
         *      unsigned int    nAddresses = cird.GetRange ().GetNumberOfContainedPoints ();
         *      for (InternetAddress ia : cidr.GetRange ()) {
         *          print (ia);
         *      }
         *      \endcode
         */
        nonvirtual InternetAddressRange GetRange () const;

    public:
        /*
            *  return true if the CIDRs are equal. This SHOULD only pay attention to the 'significant bits' of the base address, but currently doesn't (@todo fix/bug)
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
