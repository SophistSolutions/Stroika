/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_InternetProtocol_ICMP_inl_
#define _Stroika_Foundation_IO_Network_InternetProtocol_ICMP_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

namespace Stroika::Foundation::IO::Network::InternetProtocol::ICMP {

    namespace V4 {

        /*
         ********************************************************************************
         **************************** TTLExpiredException *******************************
         ********************************************************************************
         */
        inline InternetAddress TTLExpiredException::GetReachedIP () const
        {
            return fReachedIP_;
        }

        /*
         ********************************************************************************
         ********************** DestinationUnreachableException *************************
         ********************************************************************************
         */
        inline unsigned int DestinationUnreachableException::GetCode () const
        {
            return fCode_;
        }
        inline InternetAddress DestinationUnreachableException::GetReachedIP () const
        {
            return fReachedIP_;
        }

        /*
         ********************************************************************************
         ***************************** UnknownICMPPacket ********************************
         ********************************************************************************
         */
        inline ICMP_PacketTypes UnknownICMPPacket::GetType () const
        {
            return fType_;
        }
    }

}
#endif /*_Stroika_Foundation_IO_Network_InternetProtocol_ICMP_inl_*/
