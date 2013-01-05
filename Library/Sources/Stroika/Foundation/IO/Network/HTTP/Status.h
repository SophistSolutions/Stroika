/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_HTTP_Status_h_
#define _Stroika_Foundation_IO_Network_HTTP_Status_h_   1

#include    "../../../StroikaPreComp.h"

#include    <map>
#include    <string>

#include    "../../../Configuration/Common.h"


/*
 * TODO:
 *      (o)
 */


namespace   Stroika {
    namespace   Foundation {
        namespace   IO {
            namespace   Network {
                namespace   HTTP {



                    typedef unsigned int    Status;
                    /*
                     * See http://www.w3.org/Protocols/rfc2616/rfc2616-sec10.html for details on these status codes.
                     */
                    namespace   StatusCodes {
                        const   Status  kOK                 =   200;
                        const   Status  kMovedPermanently   =   301;
                        const   Status  kNotModified        =   304;
                        /*
                         * Bad CLIENT request.
                         * The request could not be understood by the server due to malformed syntax.
                         * The client SHOULD NOT repeat the request without modifications.
                         */
                        const   Status  kBadRequest         =   400;
                        const   Status  kUnauthorized       =   401;
                        const   Status  kNotFound           =   404;
                        /*
                         * This is principally for internal server exceptions.
                         */
                        const   Status  kInternalError      =   500;
                        /*
                         * Tndicates that the server is temporarily unavailable, usually due to high load or maintenance.
                         */
                        const   Status  kServiceUnavailable =   503;
                    }



                }
            }
        }
    }
}
#endif  /*_Stroika_Foundation_IO_Network_HTTP_Status_h_*/
