/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Frameworks_UPnP_SSDP_Common_h_
#define _Stroika_Frameworks_UPnP_SSDP_Common_h_ 1

#include    "../../StroikaPreComp.h"

#include    "../../../Foundation/Characters/String.h"
#include    "../../../Foundation/Configuration/Common.h"
#include    "../../../Foundation/IO/Network/SocketAddress.h"



namespace   Stroika {
    namespace   Frameworks {
        namespace   UPnP {
            namespace   SSDP {


                namespace V4 {
                    extern  const       Foundation::IO::Network::SocketAddress  kSocketAddress;
                }
                namespace V6 {
                    // NYI
                }


            }
        }
    }
}
#endif  /*_Stroika_Frameworks_UPnP_SSDP_Common_h_*/




/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "Common.inl"
