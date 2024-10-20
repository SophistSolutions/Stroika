/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Framework_WebServer_Router_h_
#define _Stroika_Framework_WebServer_Router_h_ 1

#include "Stroika/Frameworks/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/RegularExpression.h"
#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Containers/Sequence.h"

#include "Stroika/Frameworks/WebServer/CORS.h"
#include "Stroika/Frameworks/WebServer/Interceptor.h"
#include "Stroika/Frameworks/WebServer/Request.h"
#include "Stroika/Frameworks/WebServer/RequestHandler.h"

/*
 *  \note Code-Status:  <a href="Code-Status.md#Alpha">Alpha</a>
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
 *          However, in terms of performance and flexibility its not clear the regular expression matching would be any more efficient or easier to code than
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
     *      @todo need more generic matching to fit in (maybe optional matcher that takes URL?, or even full Request).
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
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety-For-Envelope-Letter-Internally-Synchronized">C++-Standard-Thread-Safety-For-Envelope-Letter-Internally-Synchronized</a>
     *              But note that Matches() is a const method, so it can safely be called from any number of threads
     *              simultaneously.
     * 
     *  \req it is expected aggregated handlers provided MUST be <a href="Thread-Safety.md#C++-Standard-Thread-Safety-For-Envelope-Letter-Internally-Synchronized">C++-Standard-Thread-Safety-For-Envelope-Letter-Internally-Synchronized</a> as well.
     */
    class Route {
    public:
        /**
         *  Any route to apply the handler, must match ALL argument constraints.
         *  If verbMatch is omitted, it it assumed to be IO::Network::HTTP::MethodsRegEx::kGet (NOT RegularExpression::kAny)
         * 
         *  \note that the request handler is called with any String arguments based on the pathMatch regular expression.
         * 
         *  \note that Routes that match on a hostRelativeURI, the hostRelativeURI is first normalized (funny characters translated to unicode, sequences of // removed etc)
         *        before matching against the regular expression.
         * 
         *  \par Example Usage (GET with explicit method regexp)
         *      \code
         *          Route{HTTP::MethodsRegEx::kGet, "session(/?)"_RegEx, [this] (Message* m) {
         *              WriteResponse (&m->rwResponse (), kSession_, kMapper.FromObject (fWSImpl_->Session_GET ()));
         *          }},
         *      \endcode
         *
         *  \par Example Usage (GET with defaulted GET method spec, and no arguments)
         *      \code
         *          Route{"session(/?)"_RegEx, [this] (Message* m) {
         *              WriteResponse (&m->rwResponse (), kSession_, kMapper.FromObject (fWSImpl_->Session_GET ()));
         *          }},
         *      \endcode
         * 
         *  \par Example Usage (GET with arg parsed from URL path)
         *      \code
         *          Route{"resource/(.+)"_RegEx, [this] (Message* m, const String& resID) {
         *              auto r = fWSImpl_->resource_GET (resID);
         *              m->rwResponse().contentType  = get<InternetMediaType> (r);
         *              m->rwResponse().write (get<BLOB> (r));
         *          }},
         *      \endcode
         * 
         *  \par Example Usage (POST, and grab params from Body)
         *      \code
         *          Route{HTTP::MethodsRegEx::kPost, "HR(/?)"_RegEx, [this] (Message* m) {
         *              if (optional<InternetMediaType> ct = m->request ().contentType()) {
         *                  WriteResponse (&m->rwResponse (), kHR_, kMapper.FromObject (fWSImpl_->HR_POST (*ct, m->rwRequest().GetBody ())));
         *              }
         *              else {
         *                  Execution::Throw (Execution::RuntimeErrorException{"expected Content-Type HTTP Request header"});
         *              }
         *          }},
         *      \endcode
         * 
         */
        Route (const RegularExpression& verbMatch, const RegularExpression& pathMatch, const RequestHandler& handler);
        Route (const RegularExpression& pathMatch, const RequestHandler& handler);
        Route (const function<bool (const String& method, const String& hostRelPath, const Request& request)>& requestMatcher,
               const RequestHandler&                                                                           handler);

    public:
        /**
         * Check if the given request matches this Route.
         * Overload taking method/hostRelPath can be derived from request, but can be substituted with different values.
         * Overload with optional matches returns variable matches in the regexp for the path
         * 
         * We interpret routes as matching against a relative path from the root
         */
        nonvirtual bool Matches (const Request& request, Sequence<String>* pathRegExpMatches = nullptr) const;
        nonvirtual bool Matches (const String& method, const String& hostRelPath, const Request& request,
                                 Sequence<String>* pathRegExpMatches = nullptr) const;

    private:
        // @todo instead of two optionals - this should be 'variant'
        optional<pair<RegularExpression, RegularExpression>>                                               fVerbAndPathMatch_;
        optional<function<bool (const String& method, const String& hostRelPath, const Request& request)>> fRequestMatch_;
        RequestHandler                                                                                     fHandler_;

    private:
        friend class Router;
    };

    /**
     *  If there is not an EXPLICIT route matched for HEAD, or OPTIONS, the those methods will be implemented
     *  automatically by the Router.
     * 
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety-For-Envelope-Letter-Internally-Synchronized">C++-Standard-Thread-Safety-For-Envelope-Letter-Internally-Synchronized</a>
     *              But note that HandleMessage() is a const method, so it can safely be called from any number of threads
     *              simultaneously.
     */
    class Router : public Interceptor {
    private:
        using inherited = Interceptor;

    public:
        /**
         */
        Router ()                       = delete;
        Router (Router&&) noexcept      = default;
        Router (const Router&) noexcept = default;
        Router (const Sequence<Route>& routes, const CORSOptions& corsOptions);

    public:
        nonvirtual Router& operator= (Router&&) noexcept = default;
        nonvirtual Router& operator= (const Router&)     = default;

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
