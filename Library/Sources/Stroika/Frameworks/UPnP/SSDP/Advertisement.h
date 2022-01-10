/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Frameworks_UPnP_SSDP_Advertisement_h_
#define _Stroika_Frameworks_UPnP_SSDP_Advertisement_h_ 1

#include "../../StroikaPreComp.h"

#include <compare>
#include <optional>

#include "../../../Foundation/Characters/String.h"
#include "../../../Foundation/Common/Property.h"
#include "../../../Foundation/Configuration/Common.h"
#include "../../../Foundation/Containers/Mapping.h"
#include "../../../Foundation/DataExchange/ObjectVariantMapper.h"
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
         *  Mapper to facilitiate serialization
         */
        static const Foundation::Common::ConstantProperty<Foundation::DataExchange::ObjectVariantMapper> kMapper;

        /**
         *  @see Characters::ToString ();
         */
        nonvirtual String ToString () const;

    private:
        static Foundation::DataExchange::ObjectVariantMapper kMapperGetter_ ();
    };
    inline const Foundation::Common::ConstantProperty<Foundation::DataExchange::ObjectVariantMapper> Advertisement::kMapper{Advertisement::kMapperGetter_};

    /**
     */
    static const String kTarget_UPNPRootDevice{L"upnp:rootdevice"sv};

    /**
     */
    static const String kTarget_SSDPAll{L"ssdp:all"sv};

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
