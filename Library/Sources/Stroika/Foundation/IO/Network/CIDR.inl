/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_CIDR_inl_
#define _Stroika_Foundation_IO_Network_CIDR_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

namespace Stroika {
    namespace Foundation {
        namespace IO {
            namespace Network {

                /*
                 ********************************************************************************
                 ******************************** Netowrk::CIDR *********************************
                 ********************************************************************************
                 */
                inline CIDR::CIDR (const InternetAddress& internetAddress, unsigned int significantBits)
                    : fBaseAddress_ (internetAddress)
                    , fSignificantBits_ (significantBits)
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
                inline bool CIDR::operator== (const CIDR& rhs) const
                {
                    // @todo - FIX to ONLY examine significant bits of the address!!!
                    return fBaseAddress_ == rhs.fBaseAddress_ and fSignificantBits_ == rhs.fSignificantBits_;
                }
            }
        }
    }
}
#endif /*_Stroika_Foundation_IO_Network_CIDR_inl_*/
