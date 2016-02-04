/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#ifndef _Stroika_Framework_WebServer_Router_h_
#define _Stroika_Framework_WebServer_Router_h_ 1

#include    "../StroikaPreComp.h"

#include    "../../Foundation/Characters/String.h"
#include    "../../Foundation/Characters/RegularExpression.h"
#include    "../../Foundation/Containers/Sequence.h"
#include    "../../Foundation/Execution/Synchronized.h"
#include    "../../Foundation/IO/Network/URL.h"

#include    "Request.h"
#include    "RequestHandler.h"


/*
 *  SUPER ROUGH DRAFT - inspired (soemwaht) by rails router
 * TODO:
 *
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

namespace   Stroika {
    namespace   Frameworks {
        namespace   WebServer {

            using   namespace   Stroika::Foundation;

            using   Characters::String;
            using   Characters::RegularExpression;
            using   Containers::Sequence;
            using   IO::Network::URL;
            using   Memory::Optional;



            class   Router;

            /**
             *      @todo - we probably want to add ability to generically parse out arguments from url, and include them to handler (as rails does - handy for ID in REST)
             *
             *  @todo need more generic matchign to fit in (maybe optional matcher that takes URL?, or even full Request).
             *
              * \note - this is internally synchonized (THREADSAFE)

              **        NOTE - may verb match and path match each OPTIONALS in class and have maybe a LIST of THINGS we know how to match.
              *             VERB
              *             RELPATH
              *             HTTP HEADER (like SOAPACTION)
              *             RequestObject????
              *         then we use "Route" to make generic the mapping of a request to a Handler.
              (
            *       @todo NEED to support NESTED Routes (or aggregated).
            *               Key is need stuff like 'default error handling' - and just to somehow inherit/copy that.
            */
            struct  Route {
            private:
                RegularExpression   fVerbMatch;
                RegularExpression   fPathMatch;
                RequestHandler      fHandler;
            public:

                Route (const RegularExpression& verbMatch, const RegularExpression& pathMatch, const RequestHandler& handler)
                    : fVerbMatch (verbMatch)
                    , fPathMatch (pathMatch)
                    , fHandler (handler)
                {
                }
                Route (const RegularExpression& pathMatch, const RequestHandler& handler)
                    : fVerbMatch (RegularExpression (L".*", RegularExpression::SyntaxType::eECMAScript))
                    , fPathMatch (pathMatch)
                    , fHandler (handler)
                {
                }
            private:
                friend class Router;
            };


            /**
            *       UNCLEAR where to put synconized - insizde Router or outside?
            */
            class   Router {
            public:
                Router (const Sequence<Route>& routes);

            public:
                // typically just examine host-relative part of URL
                nonvirtual  Optional<RequestHandler>   Lookup (const Request& request) const;

                //public:
                // typically just examine host-relative part of URL
                //    nonvirtual  Optional<Handler>   Lookup (const String& method, const URL& url) const;

            private:
                Execution::Synchronized<Sequence<Route>>  fRoutes_;
            };


        }
    }
}




/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "Router.inl"

#endif  /*_Stroika_Framework_WebServer_Router_h_*/
