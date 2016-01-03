/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_HTTP_Status_h_
#define _Stroika_Foundation_IO_Network_HTTP_Status_h_   1

#include    "../../../StroikaPreComp.h"

#include    <map>
#include    <string>

#include    "../../../Configuration/Common.h"



/**
 * TODO:
 *      (o)
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   IO {
            namespace   Network {
                namespace   HTTP {


                    using       Status  =   unsigned int;


                    /*
                     * See http://www.w3.org/Protocols/rfc2616/rfc2616-sec10.html for details on these status codes.
                     */
                    namespace   StatusCodes {
                        constexpr   Status  kOK                 =   200;
                        constexpr   Status  kMovedPermanently   =   301;
                        constexpr   Status  kNotModified        =   304;
                        /*
                         * Bad CLIENT request.
                         * The request could not be understood by the server due to malformed syntax.
                         * The client SHOULD NOT repeat the request without modifications.
                         */
                        constexpr   Status  kBadRequest         =   400;
                        constexpr   Status  kUnauthorized       =   401;
                        constexpr   Status  kNotFound           =   404;
                        /*
                         * This is principally for internal server exceptions.
                         */
                        constexpr   Status  kInternalError      =   500;
                        /*
                         * Tndicates that the server is temporarily unavailable, usually due to high load or maintenance.
                         */
                        constexpr   Status  kServiceUnavailable =   503;
                    }


                }
            }
        }
    }
}



#endif  /*_Stroika_Foundation_IO_Network_HTTP_Status_h_*/
