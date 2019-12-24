/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Frameworks_UPnP_SSDP_Advertisement_h_
#define _Stroika_Frameworks_UPnP_SSDP_Advertisement_h_ 1

#include "../../StroikaPreComp.h"

#if defined(__cpp_impl_three_way_comparison)
#include <compare>
#endif

#include <optional>

#include "../../../Foundation/Characters/String.h"
#include "../../../Foundation/Characters/String_Constant.h"
#include "../../../Foundation/Configuration/Common.h"
#include "../../../Foundation/Containers/Mapping.h"
#include "../../../Foundation/IO/Network/SocketAddress.h"
#include "../../../Foundation/IO/Network/URI.h"
#include "../../../Foundation/Memory/BLOB.h"

/**
 *  \file
 *
 *  See http://quimby.gnus.org/internet-drafts/draft-cai-ssdp-v1-03.txt
 *  for details on the SSDP specification.
 *
 *  And http://www.upnp-hacks.org/upnp.html for more hints.
 */

namespace Stroika::Frameworks::UPnP::SSDP {

    using Foundation::Characters::String;
    using Foundation::Characters::String_Constant;
    using Foundation::Containers::Mapping;
    using Foundation::IO::Network::URI;
    using Foundation::Memory::BLOB;

    /**
     */
    struct Advertisement {
        optional<bool>          fAlive; // else Bye notification, or empty if neither
        String                  fUSN;
        URI                     fLocation;
        String                  fServer;
        String                  fTarget; // usually ST header (or NT for notify)
        Mapping<String, String> fRawHeaders;

#if __cpp_impl_three_way_comparison < 201711
        bool operator== (const Advertisement& rhs) const
        {
            if (fAlive != rhs.fAlive) {
                return false;
            }
            if (fUSNs != rhs.fUSNs) {
                return false;
            }
            if (fLocation != rhs.fLocation) {
                return false;
            }
            if (fServer != rhs.fServer) {
                return false;
            }
            if (fTarget != rhs.fTarget) {
                return false;
            }
            if (fRawHeaders != rhs.fRawHeaders) {
                return false;
            }
            return true;
        }
#else
        auto operator<=> (const Advertisement&) const = default;
#endif

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

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Advertisement.inl"

#endif /*_Stroika_Frameworks_UPnP_SSDP_Advertisement_h_*/
