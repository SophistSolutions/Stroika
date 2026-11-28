/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */

namespace Stroika::Foundation::IO::Network::InternetProtocol::ICMP {

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

    namespace V4 {

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

    //////////////////////////// DEPRECATED BELOW.../////////////////////////////

    namespace V4 {
        using DestinationUnreachableException
            [[deprecated ("Since Stroika v3.0d26, use ICMP::DestinationUnreachableException - for IPv4 and IPv6 alike")]] =
                ICMP::DestinationUnreachableException;
        using TTLExpiredException [[deprecated ("Since Stroika v3.0d26, use ICMP::TTLExpiredException - for IPv4 and IPv6 alike")]] =
            ICMP::TTLExpiredException;
    }

}
