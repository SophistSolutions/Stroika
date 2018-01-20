/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Frameworks_UPnP_SSDP_Advertisement_h_
#define _Stroika_Frameworks_UPnP_SSDP_Advertisement_h_ 1

#include "../../StroikaPreComp.h"

#include "../../../Foundation/Characters/String.h"
#include "../../../Foundation/Characters/String_Constant.h"
#include "../../../Foundation/Configuration/Common.h"
#include "../../../Foundation/Containers/Mapping.h"
#include "../../../Foundation/IO/Network/SocketAddress.h"
#include "../../../Foundation/IO/Network/URL.h"
#include "../../../Foundation/Memory/BLOB.h"
#include "../../../Foundation/Memory/Optional.h"

/**
 *  \file
 *
 *  See http://quimby.gnus.org/internet-drafts/draft-cai-ssdp-v1-03.txt
 *  for details on the SSDP specification.
 *
 *  And http://www.upnp-hacks.org/upnp.html for more hints.
 */

namespace Stroika {
    namespace Frameworks {
        namespace UPnP {
            namespace SSDP {

                using Foundation::Characters::String;
                using Foundation::Characters::String_Constant;
                using Foundation::Containers::Mapping;
                using Foundation::IO::Network::URL;
                using Foundation::Memory::BLOB;
                using Foundation::Memory::Optional;

                /**
                 */
                struct Advertisement {
                    Optional<bool>          fAlive; // else Bye notification, or empty if neither
                    String                  fUSN;
                    URL                     fLocation;
                    String                  fServer;
                    String                  fTarget; // usually ST header (or NT for notify)
                    Mapping<String, String> fRawHeaders;

                    /**
                     *  @see Characters::ToString ();
                     */
                    nonvirtual String ToString () const;
                };

                /**
                 */
                static const String_Constant kTarget_UPNPRootDevice{L"upnp:rootdevice"};

                /**
                 */
                static const String_Constant kTarget_SSDPAll{L"ssdp:all"};

                /**
                 */
                enum class SearchOrNotify {
                    SearchResponse,
                    Notify
                };

                /**
                 */
                BLOB Serialize (const String& headLine, SearchOrNotify searchOrNotify, const Advertisement& ad);

                /**
                 */
                void DeSerialize (const BLOB& b, String* headLine, Advertisement* advertisement);
            }
        }
    }
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Advertisement.inl"

#endif /*_Stroika_Frameworks_UPnP_SSDP_Advertisement_h_*/
