/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_Transfer_Request_h_
#define _Stroika_Foundation_IO_Network_Transfer_Request_h_ 1

#include "../../../StroikaPreComp.h"

#include "../../../Characters/String.h"
#include "../../../Configuration/Common.h"
#include "../../../Containers/Mapping.h"
#include "../../../DataExchange/InternetMediaType.h"
#include "../../../Memory/BLOB.h"
#include "../../../Time/Realtime.h"

#include "../URL.h"

/**
 *
 *  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Late</a>
 *
 *
 * TODO:
 *
 *
 */

namespace Stroika::Foundation::IO::Network::Transfer {

    using Characters::String;
    using Containers::Mapping;
    using DataExchange::InternetMediaType;
    using Memory::BLOB;

    /**
     *  \note  DESIGN-NOTE:
     *      Chose not move request/repsonse stuff to HTTP module- so re-usable in framework
     *      code which does webserver, because though the abstract API is identical, we
     *      have very differnt needs about to what to specify/retrieve in one case or the other.
     *
     *      @todo maybe reconsider?
     */
    struct Request {
        String                  fMethod;
        Mapping<String, String> fOverrideHeaders;
        BLOB                    fData; // usually empty, but provided for some methods like POST

        /**
         *  Scans fOverrideHeaders
         */
        nonvirtual InternetMediaType GetContentType () const;

        /**
         * updates fOverrideHeaders
         */
        nonvirtual void SetContentType (const InternetMediaType& ct);
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Request.inl"

#endif /*_Stroika_Foundation_IO_Network_Transfer_Request_h_*/
