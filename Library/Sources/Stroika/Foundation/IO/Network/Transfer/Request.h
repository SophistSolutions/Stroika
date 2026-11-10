/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_Transfer_Request_h_
#define _Stroika_Foundation_IO_Network_Transfer_Request_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Common/Common.h"
#include "Stroika/Foundation/Containers/Mapping.h"
#include "Stroika/Foundation/DataExchange/InternetMediaType.h"
#include "Stroika/Foundation/DataExchange/TypedBLOB.h"
#include "Stroika/Foundation/IO/Network/HTTP/Headers.h"
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
    using DataExchange::TypedBLOB;
    using Memory::BLOB;

    /**
     *  \note  DESIGN-NOTE:
     *      Chose not move request/response stuff to HTTP module- so re-usable in framework
     *      code which does webserver, because though the abstract API is identical, we
     *      have very different needs about to what to specify/retrieve in one case or the other.
     *
     *      @todo maybe reconsider?
     */
    struct Request {
        String fMethod;

        /*
         * an HTTP request contains an authority-relative URL (basically path + query - maybe also fragment but that will be ignored)
         */
        URI fAuthorityRelativeURL;

        /**
         *  The request's headers, beyond those the connection writes itself. An HTTP::Headers, so names compare ignoring case,
         *  as HTTP's do (RFC 9110, 5.1): adding "content-type" replaces a "Content-Type".
         */
        HTTP::Headers fOverrideHeaders;

        /*
         *  usually empty, but provided for some methods like POST
         */
        BLOB fData;

        /**
         *  fOverrideHeaders' Content-Type
         */
        nonvirtual optional<InternetMediaType> GetContentType () const;

        /**
         *  Sets (or, given nullopt, removes) fOverrideHeaders' Content-Type
         */
        nonvirtual void SetContentType (const optional<InternetMediaType>& ct);

        /**
         */
        nonvirtual TypedBLOB GetTypedBLOB () const;

        /**
         */
        nonvirtual void SetTypedBLOB (const TypedBLOB& tb);

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
