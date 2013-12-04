/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_NetworkInterfaces_h_
#define _Stroika_Foundation_IO_Network_NetworkInterfaces_h_    1

#include    "../../StroikaPreComp.h"

#if     qPlatform_POSIX
#include    <arpa/inet.h>
#elif   qPlatform_Windows
#include    <WinSock2.h>
#include    <inaddr.h>
#include    <in6addr.h>
#endif

#include    "../../Characters/String.h"
#include    "../../Configuration/Common.h"

#include    "InternetAddress.h"


/**
 *  \file
 *
 *  \version    <a href="code_status.html#Alpha">Alpha</a>
 *
 * TODO:
 *
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   IO {
            namespace   Network {


                using   Characters::String;


                //// @todo Add somthing to return
                /// struct INTERFCE { name; set<IPADDR> ()}
                //

                // This isn't always well defined, but is typically. This is the primary ip address used to address this machine.
                // This CAN return an empty address if none available (like not connected to a network).
                //. (if no net do we return localhost?))
                InternetAddress GetPrimaryInternetAddress ();


                //// ADD API for listint all interfaces (as above)

                //// add options for filtering out disabled ones (not onlnie)
                /// add optikon for fitlering out localhost


            }
        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "NetworkInterfaces.inl"

#endif  /*_Stroika_Foundation_IO_Network_NetworkInterfaces_h_*/
