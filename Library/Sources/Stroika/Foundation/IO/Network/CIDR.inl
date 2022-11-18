/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_CIDR_inl_
#define _Stroika_Foundation_IO_Network_CIDR_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

namespace Stroika::Foundation::IO::Network {

    /*
     ********************************************************************************
     ******************************** Netowrk::CIDR *********************************
     ********************************************************************************
     */
    inline CIDR::CIDR (const InternetAddress& internetAddress, optional<unsigned int> significantBits)
        : CIDR{internetAddress, significantBits.value_or (static_cast<unsigned int> (*internetAddress.GetAddressSize () * 8))}
    {
        Require (internetAddress.GetAddressFamily () == InternetAddress::AddressFamily::V4 or internetAddress.GetAddressFamily () == InternetAddress::AddressFamily::V6);
    }
    inline InternetAddress CIDR::GetBaseInternetAddress () const
    {
        return fBaseAddress_;
    }
    inline unsigned int CIDR::GetNumberOfSignificantBits () const
    {
        return fSignificantBits_;
    }
#if __cpp_impl_three_way_comparison >= 201907
    inline strong_ordering CIDR::operator<=> (const CIDR& rhs) const
    {
        /*
         *  NOTE, while it may not make alot of sense for fSignificantBits_ to be the primary part of the default sort key
         *  the reason for this choice is that the part where we compare the base address doesn't make much sense
         *  except when the significant bits on both sides agree.
         * 
         *  Well, suppose not true. COULD just use 'less' or 'greater' of the two sigbits, and compare the other first. Would only
         *  end up with equal after comparing both, so maybe that would still make sense.
         */
        strong_ordering r = fSignificantBits_ <=> rhs.fSignificantBits_;
        if (r == 0) {
            return fBaseAddress_.KeepSignifcantBits (fSignificantBits_) <=> rhs.fBaseAddress_.KeepSignifcantBits (fSignificantBits_);
        }
        else {
            return r;
        }
    }
#else
    inline bool CIDR::operator<(const CIDR& rhs) const
    {
        using Common::strong_ordering;
        using Common::ThreeWayCompare;
        strong_ordering r = ThreeWayCompare (fSignificantBits_, rhs.fSignificantBits_);
        if (r == Common::kEqual) {
            return ThreeWayCompare (fBaseAddress_.KeepSignifcantBits (fSignificantBits_), rhs.fBaseAddress_.KeepSignifcantBits (fSignificantBits_)) == Common::kLess;
        }
        else {
            return r == Common::kLess;
        }
    }
#endif
    inline bool CIDR::operator== (const CIDR& rhs) const
    {
        return fSignificantBits_ == rhs.fSignificantBits_ and fBaseAddress_.KeepSignifcantBits (fSignificantBits_) == rhs.fBaseAddress_.KeepSignifcantBits (fSignificantBits_);
    }

}

#endif /*_Stroika_Foundation_IO_Network_CIDR_inl_*/
