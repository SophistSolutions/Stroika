/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#ifndef _Stroika_Framework_WebServer_RequestHandler_h_
#define _Stroika_Framework_WebServer_RequestHandler_h_  1

#include    "../StroikaPreComp.h"

#include    "../../Foundation/Characters/String.h"
#include    "../../Foundation/Configuration/Common.h"
#include    "../../Foundation/DataExchange/InternetMediaType.h"

#include    "Request.h"
#include    "Response.h"


/*
 */

/*
 * TODO:
 *      (o)
 */

namespace   Stroika {
    namespace   Frameworks {
        namespace   WebServer {

            using   namespace   Stroika::Foundation;
            using   Characters::String;
            using   DataExchange::InternetMediaType;


            using   RequestHandler = function<void(Request* request, Response* response)>;
#if 0

            /*
             * A request handler should be understood to be stateless - as far as the connection is concerned.
             * ??? Maybe - or maybe have add/remove or notication so assocaited?? For now - assume stateless - and just called
             * with HandleRequest ...
             *
             * Also - a RequestHandler should be careful about threads, as it could be called first on one thread, and
             * then - possibly at the same time - on another thread. The same handler can be used multiple times (multiple sessions).
             */
            struct  RequestHandler {
            protected:
                RequestHandler ();
            public:
                virtual ~RequestHandler ();

            public:
                // CanHandleRequest() can peek at data in the connection - but cannot read, or directly or indirecly modify anything
                // NB: Its called after the HTTP Header has been read (thats already embodied in teh connection Request object)
                virtual bool    CanHandleRequest (const Connection& connection) const = 0;

                // This will write an answer to the connection Response object
                virtual void    HandleRequest (Connection& connection) = 0;
            };
#endif

        }
    }
}




/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "Request.inl"

#endif  /*_Stroika_Framework_WebServer_RequestHandler_h_*/
