/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#ifndef _Stroika_Frameworks_NetworkMontior_Ping_h_
#define _Stroika_Frameworks_NetworkMontior_Ping_h_ 1

#include "../StroikaPreComp.h"

#include "../../Foundation/Characters/String.h"
#include "../../Foundation/IO/Network/InternetAddress.h"
#include "../../Foundation/Memory/Optional.h"
#include "../../Foundation/Time/Duration.h"

/**
 *
 *  \version    <a href="code_status.html#Alpha-Early">Alpha-Early</a>
 *
 * TODO:
 *
 *      @todo
 */

namespace Stroika {
    namespace Frameworks {
        namespace NetworkMontior {

            using namespace Stroika::Foundation;

            using IO::Network::InternetAddress;
            using Memory::Optional;
            using Time::Duration;
            using Time::DurationSecondsType;

            struct PingOptions {
                Optional<unsigned int>  fMaxHops;
                static constexpr size_t kMinPacketSize     = 12;
                static constexpr size_t kDefaultPacketSize = 32;
                Optional<size_t>        fPacketSize;

                /**
                 *  @see Characters::ToString ();
                 */
                nonvirtual Characters::String ToString () const;
            };

            /**
             *  @todo - handle options 
             *  @todo - document/define exceptions
             */
            Duration Ping (const InternetAddress& addr, const PingOptions& options = {});
        }
    }
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Ping.inl"

#endif /*_Stroika_Frameworks_NetworkMontior_Ping_h_*/
