/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Framework_WebServer_RequestHandler_h_
#define _Stroika_Framework_WebServer_RequestHandler_h_  1

#include    "../StroikaPreComp.h"

#include    <string>
#include    <vector>

#include    "../../Foundation/Characters/String.h"
#include    "../../Foundation/Configuration/Common.h"
#include    "../../Foundation/DataExchange/InternetMediaType.h"

#include    "Connection.h"


/*
 * Design Choice Notes:
 *
 *      o   RequestHandler::CanHandleRequest() -> bool method, or association list of PATTERN->RequestHandler?
 *
 *          We COULD either have a special RequestHandler maintaining a list of owned RequestHandler, with KEYs
 *          of patterns like with Django:
 *
 *          extra_patterns = patterns('',
 *              url(r'^reports/(?P<id>\d+)/$', 'credit.views.report', name='credit-reports'),
 *              url(r'^charge/$', 'credit.views.charge', name='credit-charge'),
 *          )
 *
 *          However, in terms of performance and flexability its not clear the regular expression matching would be any more efficent or easier to code than
 *          just having a C++ method on RequestHandler().
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
