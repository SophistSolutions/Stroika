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
#include "Stroika/Foundation/IO/Network/Interface.h"
#include "Stroika/Foundation/IO/Network/SocketAddress.h"
#include "Stroika/Foundation/IO/Network/URI.h"
#include "Stroika/Foundation/Memory/BLOB.h"
#include "Stroika/Foundation/Time/Duration.h"

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
        optional<Foundation::Time::Duration> fMaxAge; // CACHE-CONTROL max-age: how long the advertisement is good for (Serialize writes kDefaultMaxAge if missing)

        /**
         *  The network it arrived on - its interface's Interface::fInterfaceID - as a Client::Listener or Client::Search heard it
         *  (nullopt: not known). Not part of the message: Serialize ignores it, and DeSerialize leaves it nullopt.
         */
        optional<Foundation::IO::Network::Interface::SystemIDType> fReceivedOn;

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
     *  The CACHE-CONTROL max-age Serialize writes when an Advertisement has no fMaxAge. The UPnP Device Architecture says it
     *  SHOULD be at least 1800 seconds.
     */
    inline constexpr Foundation::Time::DurationSeconds kDefaultMaxAge{1800.0};

    /**
     */
    enum class SearchOrNotify {
        SearchResponse,
        Notify
    };

    /**
     *  \brief The SSDP packet saying ad: a NOTIFY (ssdp:alive - or ssdp:byebye, given fAlive false), or an answer to a search
     *         (headLine "HTTP/1.1 200 OK")
     *
     *  Each with the headers the UPnP Device Architecture gives it: a NOTIFY's HOST is notifyGroup - the multicast group it goes
     *  to, V4::kSocketAddress or V6::kSocketAddress (IPv4's if not given); an ssdp:byebye has no CACHE-CONTROL, LOCATION or
     *  SERVER; a search answer has EXT and DATE (now, as an HTTP date), and no HOST or NTS.
     */
    BLOB Serialize (const String& headLine, SearchOrNotify searchOrNotify, const Advertisement& ad,
                    const optional<Foundation::IO::Network::SocketAddress>& notifyGroup = nullopt);

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
