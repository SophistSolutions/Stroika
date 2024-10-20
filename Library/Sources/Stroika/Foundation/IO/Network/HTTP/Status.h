/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_HTTP_Status_h_
#define _Stroika_Foundation_IO_Network_HTTP_Status_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

/**
 */

namespace Stroika::Foundation::IO::Network::HTTP {

    using Status = unsigned int;

    /*
     * See https://www.rfc-editor.org/rfc/rfc9110.html#name-status-codes for details on these status codes.
     */
    namespace StatusCodes {
        constexpr Status kOK        = 200;
        constexpr Status kCreated   = 201;
        constexpr Status kNoContent = 204;

        constexpr Status kMovedPermanently = 301;
        constexpr Status kNotModified      = 304;

        /*
         * Bad CLIENT request.
         * The request could not be understood by the server due to malformed syntax.
         * The client SHOULD NOT repeat the request without modifications.
         */
        constexpr Status kBadRequest       = 400;
        constexpr Status kUnauthorized     = 401;
        constexpr Status kNotFound         = 404;
        constexpr Status kMethodNotAllowed = 405;
        /**
         *  The server did not receive a complete request message within the time that it was prepared to wait.
         */
        constexpr Status kRequestTimeout = 408;
        constexpr Status kConflict       = 408;

        /**
         *  \brief The 429 status code indicates that the user has sent too many requests in a given amount of time ("rate limiting").
         * 
         *  https://www.rfc-editor.org/rfc/rfc6585.html#section-4
         */
        constexpr Status kTooManyRequests = 429;

        /*
         * This is principally for internal server exceptions.
         */
        constexpr Status kInternalError = 500;
        /*
         * Tndicates that the server is temporarily unavailable, usually due to high load or maintenance.
         */
        constexpr Status kServiceUnavailable = 503;
        /*
         *  The server, while acting as a gateway or proxy, did not receive a timely response from the upstream server specified by the URI (e.g. HTTP, FTP, LDAP)
         *  or some other auxiliary server (e.g. DNS) it needed to access in attempting to complete the request.
         */
        constexpr Status kGatewayTimeout = 504;
    }

}

#endif /*_Stroika_Foundation_IO_Network_HTTP_Status_h_*/
