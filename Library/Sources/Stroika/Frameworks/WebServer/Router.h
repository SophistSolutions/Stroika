/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Framework_WebServer_Router_h_
#define _Stroika_Framework_WebServer_Router_h_ 1

#include "../StroikaPreComp.h"

#include "../../Foundation/Characters/RegularExpression.h"
#include "../../Foundation/Characters/String.h"
#include "../../Foundation/Containers/Sequence.h"
#include "../../Foundation/Execution/Synchronized.h"

#include "CORS.h"
#include "Interceptor.h"
#include "Request.h"
#include "RequestHandler.h"

/*
 *
 *  \version    <a href="Code-Status.md#Alpha-Early">Alpha-Early</a>
 *
 *  \note   Inspired by, but fairly different from
 *          @see http://guides.rubyonrails.org/routing.html
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

namespace Stroika::Frameworks::WebServer {

    using namespace Stroika::Foundation;

    using Characters::RegularExpression;
    using Characters::String;
    using Containers::Sequence;

    class Router;

    /**
     * 
     * &&& HEAD and OPTIONS can have routes to be overridden. But by default they are handled automatically by the router.
     * 
     *      @todo - we probably want to add ability to generically parse out arguments from url, and include them to handler (as rails does - handy for ID in REST)
     *
     *      @todo need more generic matchign to fit in (maybe optional matcher that takes URL?, or even full Request).
     *
     *        NOTE - may verb match and path match each OPTIONALS in class and have maybe a LIST of THINGS we know how to match.
     *             VERB
     *             RELPATH
     *             HTTP HEADER (like SOAPACTION)
     *             RequestObject????
     *         then we use "Route" to make generic the mapping of a request to a Handler.
     *
     *       @todo NEED to support NESTED Routes (or aggregated).
     *               Key is need stuff like 'default error handling' - and just to somehow inherit/copy that.
     *
     * 
     * 
     *  &&& TODO DESCRIBE THREADSAFTY BETTER BUT I THINK THIS OBJECT IS IMMUTABLE (EXCEPT FOR CASE OF COPY/OVERWRITE)
     * 
     * \note - this must be EXTERNALLY synchronized - except that all read only methods are safe from any thread,
     *         because these are usually stored in a strucutre where they wont be updated.
     *         Just be sure the HANLDER argument is safe when called from multiple threads at the same time!
     */
    class Route {
    public:
        /**
         *  Any route to apply the handler, must match ALL argument constraints.
         *  If verbMatch is omitted, it it assumed to be 
         */
        Route (const RegularExpression& verbMatch, const RegularExpression& pathMatch, const RequestHandler& handler);
        Route (const RegularExpression& pathMatch, const RequestHandler& handler);
        Route (const function<bool (const String& method, const String& hostRelPath, const Request& request)>& requestMatcher, const RequestHandler& handler);

    public:
        /**
         * Check if the given request matches this Route.
         * Overload taking method/hostRelPath can be derived from request, but can be substituted with different values.
         * Overload with optional matcehs returns variable matches in the regexp for the path
         * 
         * We interpret routes as matching against a relative path from the root
         */
        nonvirtual bool Matches (const Request& request, Sequence<String>* pathRegExpMatches = nullptr) const;
        nonvirtual bool Matches (const String& method, const String& hostRelPath, const Request& request, Sequence<String>* pathRegExpMatches = nullptr) const;

    private:
        optional<pair<RegularExpression, RegularExpression>>                                               fVerbAndPathMatch_;
        optional<function<bool (const String& method, const String& hostRelPath, const Request& request)>> fRequestMatch_;
        RequestHandler                                                                                     fHandler_;

    private:
        friend class Router;
    };

    /**
    * &&& @TODO REVISIT - I THINK STANDARDC++ THREAD SUPP BUT IMMUTABLE (except for assign/copy)
     *  THREAD: must be externally synchronized, but all const methods are safe from any thread.
     * 
     *  If there is not an EXPLICIT route matched for HEAD, or OPTIONS, the those methods will be implemented
     *  automatically by the Router.
     */
    class Router : public Interceptor {
    private:
        using inherited = Interceptor;

    public:
        /**
         */
        Router ()              = delete;
        Router (const Router&) = default;
        Router (Router&&)      = default;
        Router (const Sequence<Route>& routes, const CORSOptions& corsOptions);

    public:
        nonvirtual Router& operator=(Router&) = default;

    public:
        /**
         * typically just examine host-relative part of URL, routes CAN examine any part of the request
         */
        nonvirtual optional<RequestHandler> Lookup (const Request& request) const;

    private:
        struct Rep_;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Router.inl"

#endif /*_Stroika_Framework_WebServer_Router_h_*/
