/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#ifndef _Stroika_Framework_WebServer_CORS_h_
#define _Stroika_Framework_WebServer_CORS_h_ 1

#include "Stroika/Frameworks/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Containers/Set.h"

/*
 *  \note Code-Status:  <a href="Code-Status.md#Alpha">Alpha</a>
 */

namespace Stroika::Frameworks::WebServer {

    using namespace Stroika::Foundation;
    using Characters::String;
    using Containers::Set;

    /**
     *  Options for how the HTTP Server handles CORS (mostly HTTP OPTIONS requests)
     */
    struct CORSOptions {
        /**
         */
        static constexpr bool kAllowCredentials_Default{true};

        /**
         */
        static constexpr unsigned int kAccessControlMaxAge_Default{24 * 60 * 60};

        /**
         *  \brief any origin, or any header (in fAllowedOrigins, fAllowedHeaders)
         */
        static constexpr string_view kAccessControlWildcard = "*"sv;

        /**
         *   true or false if credentials allowed on CORS request
         *      \see https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers/Access-Control-Allow-Credentials
         *
         *  \note Allowed only for an origin fAllowedOrigins names - not with it * (any origin), beside which a browser refuses
         *        credentials (https://fetch.spec.whatwg.org/#cors-protocol-and-credentials).
         */
        bool fAllowCredentials{kAllowCredentials_Default};

        /**
         *  `Access-Control-Max-Age` header. How long the `Access-Control-Allow-Methods` and `Access-Control-Allow-Headers` headers can be cached.
         *  (rarely set - default good)
         */
        unsigned int fAccessControlMaxAge{kAccessControlMaxAge_Default};

        /**
         *  This can be {"*"} meaning any origin (default).
         *  Or it can be a list of values present in HTTP Origin Headers (typically just hostname but can be Host:port)
         */
        Set<String> fAllowedOrigins{kAccessControlWildcard};

        /**
         *  This is the set of headers which will be allowed by Access-Control-Request-Headers OPTIONS requests.
         *  The default - * - kAccessControlWildcard - any - is typically just fine.
         */
        Set<String> fAllowedHeaders{kAccessControlWildcard};

        /**
         *  @see Characters::ToString ();
         */
        Characters::String ToString () const;
    };

    /**
     *  \brief CORSOptions{} - each field its default
     */
    inline const CORSOptions kDefault_CORSOptions{};

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "CORS.inl"

#endif /*_Stroika_Framework_WebServer_CORS_h_*/
