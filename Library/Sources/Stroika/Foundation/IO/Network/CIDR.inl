/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
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
        : CIDR (internetAddress, significantBits.value_or (static_cast<unsigned int> (*internetAddress.GetAddressSize () * 8)))
    {
    }
    inline InternetAddress CIDR::GetInternetAddress () const
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
        strong_ordering r = fSignificantBits_ <=> rhs.fSignificantBits_;
        if (r == 0) {
            // @todo - FIX to ONLY examine significant bits of the address!!!
            return fBaseAddress_ <=> rhs.fBaseAddress_;
        }
        else {
            return r;
        }
    }
#endif
    inline bool CIDR::operator== (const CIDR& rhs) const
    {
        // @todo - FIX to ONLY examine significant bits of the address!!!
        return fBaseAddress_ == rhs.fBaseAddress_ and fSignificantBits_ == rhs.fSignificantBits_;
    }

}

#endif /*_Stroika_Foundation_IO_Network_CIDR_inl_*/
