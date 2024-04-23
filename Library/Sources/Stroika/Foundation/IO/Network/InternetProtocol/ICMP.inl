/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */

namespace Stroika::Foundation::IO::Network::InternetProtocol::ICMP {

    namespace V4 {

        /*
         ********************************************************************************
         **************************** TTLExpiredException *******************************
         ********************************************************************************
         */
        inline InternetAddress TTLExpiredException::GetUnreachedIP () const
        {
            return fUnreachedIP_;
        }

        /*
         ********************************************************************************
         ************************ DestinationUnreachableException ***********************
         ********************************************************************************
         */
        inline uint8_t DestinationUnreachableException::GetCode () const
        {
            return fCode_;
        }
        inline InternetAddress DestinationUnreachableException::GetUnreachedIP () const
        {
            return fUnreachedIP_;
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
