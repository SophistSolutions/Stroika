/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#ifndef _Stroika_Frameworks_UPnP_SSDP_Advertisement_h_
#define _Stroika_Frameworks_UPnP_SSDP_Advertisement_h_ 1

#include "Stroika/Frameworks/StroikaPreComp.h"

#include <compare>
#include <optional>

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Common/Common.h"
#include "Stroika/Foundation/Containers/Mapping.h"
#include "Stroika/Foundation/DataExchange/ObjectVariantMapper.h"
#include "Stroika/Foundation/Execution/LazyInitialized.h"
#include "Stroika/Foundation/IO/Network/SocketAddress.h"
#include "Stroika/Foundation/IO/Network/URI.h"
#include "Stroika/Foundation/Memory/BLOB.h"

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

        bool operator== (const Advertisement&) const = default;

        /**
         *  Mapper to facilitate serialization
         */
        static const Foundation::Execution::LazyInitialized<Foundation::DataExchange::ObjectVariantMapper> kMapper;

        /**
         *  @see Characters::ToString ();
         */
        nonvirtual String ToString () const;

    private:
        static Foundation::DataExchange::ObjectVariantMapper kMapperGetter_ ();
    };
    inline const Foundation::Execution::LazyInitialized<Foundation::DataExchange::ObjectVariantMapper> Advertisement::kMapper{Advertisement::kMapperGetter_};

    /**
     */
    static const String kTarget_UPNPRootDevice{"upnp:rootdevice"sv};

    /**
     */
    static const String kTarget_SSDPAll{"ssdp:all"sv};

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
     *  \brief Parse any SSDP packet - a NOTIFY, an M-SEARCH, or a search response ("HTTP/1.1 200 OK")
     *
     *  *headLine is the packet's first line - which says which of those it is; the caller checks it. fTarget is the NT header
     *  (a NOTIFY) or the ST header (a search or search response), fAlive the NTS header, and every header lands in fRawHeaders
     *  too. A Location that does not parse as a URI is treated as missing (traced, not thrown).
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
