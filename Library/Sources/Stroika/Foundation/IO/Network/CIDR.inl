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
    }
    inline InternetAddress CIDR::GetBaseInternetAddress () const
    {
        return fBaseAddress_;
    }
    inline unsigned int CIDR::GetNumberOfSignificantBits () const
    {
        return fSignificantBits_;
    }
    inline strong_ordering CIDR::operator<=> (const CIDR& rhs) const
    {
        strong_ordering r = fSignificantBits_ <=> rhs.fSignificantBits_;
        if (r == 0) {
            return fBaseAddress_.KeepSignifcantBits (fSignificantBits_) <=> rhs.fBaseAddress_.KeepSignifcantBits (fSignificantBits_);
        }
        else {
            return r;
        }
    }
    inline bool CIDR::operator== (const CIDR& rhs) const
    {
        return fSignificantBits_ == rhs.fSignificantBits_ and fBaseAddress_.KeepSignifcantBits (fSignificantBits_) == rhs.fBaseAddress_.KeepSignifcantBits (fSignificantBits_);
    }

}

#endif /*_Stroika_Foundation_IO_Network_CIDR_inl_*/
