/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#ifndef _Stroika_Framework_WebServer_Router_h_
#define _Stroika_Framework_WebServer_Router_h_ 1

#include    "../StroikaPreComp.h"

#include    "../../Foundation/Characters/String.h"
#include    "../../Foundation/Characters/RegularExpression.h"
#include    "../../Foundation/Containers/Collection.h"
#include    "../../Foundation/Execution/Synchronized.h"
#include    "../../Foundation/IO/Network/URL.h"

#include    "Request.h"
#include    "Response.h"


/*
 *  SUPER ROUGH DRAFT - inspired (soemwaht) by rails router
 * TODO:
 *
 */

namespace   Stroika {
    namespace   Frameworks {
        namespace   WebServer {

            using   namespace   Stroika::Foundation;

            using   Characters::String;
            using   Characters::RegularExpression;
            using   Containers::Collection;
            using   IO::Network::URL;
            using   Memory::Optional;

            using   Handler = function<void(Request* request, Request* response)>;


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
            */
            struct  Route {
            private:
                RegularExpression   fVerbMatch;
                RegularExpression   fPathMatch;
                Handler             fHandler;
            public:

                Route (const RegularExpression& verbMatch, const RegularExpression& pathMatch, const Handler& handler)
                    : fVerbMatch (verbMatch)
                    , fPathMatch (pathMatch)
                    , fHandler (handler)
                {
                }
                Route (const RegularExpression& pathMatch, const Handler& handler)
                    : fVerbMatch (RegularExpression (L".*", RegularExpression::SyntaxType::eECMAScript))
                    , fPathMatch (pathMatch)
                    , fHandler (handler)
                {
                }
            private:
                friend class Router;
            };


            /**
            */
            class   Router {
            public:
                Router (const Collection<Route>& routes);

            public:
                // typically just examine host-relative part of URL
                nonvirtual  Optional<Handler>   Lookup (const Request& request) const;

                //public:
                // typically just examine host-relative part of URL
                //    nonvirtual  Optional<Handler>   Lookup (const String& method, const URL& url) const;

            private:
                Execution::Synchronized<Collection<Route>>  fRoutes_;
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
