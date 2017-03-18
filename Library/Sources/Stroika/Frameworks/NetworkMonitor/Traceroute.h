/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#ifndef _Stroika_Frameworks_NetworkMontior_Traceroute_h_
#define _Stroika_Frameworks_NetworkMontior_Traceroute_h_ 1

#include "../StroikaPreComp.h"

#include "../../Foundation/Containers/Sequence.h"
#include "../../Foundation/IO/Network/InternetAddress.h"
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

			using Containers::Sequence;
			using IO::Network::InternetAddress;
			using Time::Duration;
			using Time::DurationSecondsType;

			struct Hop {
				Duration fTime;
				InternetAddress fAddress;
			};

			/**
			 */
			Sequence<Hop> Traceroute (const InternetAddress& addr);


        }
    }
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Traceroute.inl"

#endif /*_Stroika_Frameworks_NetworkMontior_Traceroute_h_*/
