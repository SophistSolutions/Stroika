/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_Transfer_Request_h_
#define _Stroika_Foundation_IO_Network_Transfer_Request_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Configuration/Common.h"
#include "Stroika/Foundation/Containers/Mapping.h"
#include "Stroika/Foundation/DataExchange/InternetMediaType.h"
#include "Stroika/Foundation/IO/Network/URI.h"
#include "Stroika/Foundation/Memory/BLOB.h"
#include "Stroika/Foundation/Time/Realtime.h"

/**
 *
 *  \note Code-Status:  <a href="Code-Status.md#Beta">Beta</a>
 *
 */

namespace Stroika::Foundation::IO::Network::Transfer {

    using Characters::String;
    using Containers::Mapping;
    using DataExchange::InternetMediaType;
    using Memory::BLOB;

    /**
     *  \note  DESIGN-NOTE:
     *      Chose not move request/response stuff to HTTP module- so re-usable in framework
     *      code which does webserver, because though the abstract API is identical, we
     *      have very different needs about to what to specify/retrieve in one case or the other.
     *
     *      @todo maybe reconsider?
     * 
     *  @todo   http://stroika-bugs.sophists.com/browse/STK-724 - IO::Network::Transfer::Request/Response should use IO::Network::HTTP::Headers (not mapping)
     */
    struct Request {
        String fMethod;

        /*
         * an HTTP request contains an authority-relative URL (basically path + query - maybe also fragment but that will be ignored)
         */
        URI                     fAuthorityRelativeURL;
        Mapping<String, String> fOverrideHeaders;

        /*
         *  usually empty, but provided for some methods like POST
         */
        BLOB fData;

        /**
         *  Scans fOverrideHeaders
         */
        nonvirtual InternetMediaType GetContentType () const;

        /**
         * updates fOverrideHeaders
         */
        nonvirtual void SetContentType (const InternetMediaType& ct);

        /**
         *  @see Characters::ToString ();
         */
        nonvirtual String ToString () const;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Request.inl"

#endif /*_Stroika_Foundation_IO_Network_Transfer_Request_h_*/
