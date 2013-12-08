/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Frameworks_UPnP_SSDP_Advertisement_h_
#define _Stroika_Frameworks_UPnP_SSDP_Advertisement_h_ 1

#include    "../../StroikaPreComp.h"

#include    "../../../Foundation/Characters/String.h"
#include    "../../../Foundation/Containers/Mapping.h"
#include    "../../../Foundation/Memory/Optional.h"
#include    "../../../Foundation/Configuration/Common.h"
#include    "../../../Foundation/IO/Network/SocketAddress.h"


/**
 *  \file
 *
 *  See http://quimby.gnus.org/internet-drafts/draft-cai-ssdp-v1-03.txt
 *  for details on the SSDP specification.
 *
 *  And http://www.upnp-hacks.org/upnp.html for more hints.
 */


namespace   Stroika {
    namespace   Frameworks {
        namespace   UPnP {
            namespace   SSDP {


                using   Foundation::Memory::Optional;
                using   Foundation::Characters::String;
                using   Foundation::Containers::Mapping;


                /**
                 */
                struct  Advertisement {
                    Optional<bool>      fAlive; // else Bye notification, or empty if neither
                    String                      fUSN;
                    String                      fLocation;
                    String                      fServer;
                    String                      fST;         // usually ST header (or NT for notify)
                    Mapping<String, String>     fRawHeaders;
                };


            }
        }
    }
}




/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "Advertisement.inl"

#endif  /*_Stroika_Frameworks_UPnP_SSDP_Advertisement_h_*/
