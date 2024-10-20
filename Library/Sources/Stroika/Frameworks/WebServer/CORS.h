/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
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
         *   true or false if credentials allowed on CORS request
         *      \see https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers/Access-Control-Allow-Credentials
         */
        optional<bool> fAllowCredentials;

        /**
         */
        static constexpr bool kAllowCredentials_Default{true};

        /**
         *  `Access-Control-Max-Age` header. How long the `Access-Control-Allow-Methods` and `Access-Control-Allow-Headers` headers can be cached.
         *  (rarely set - default good)
         */
        optional<unsigned int> fAccessControlMaxAge;

        static constexpr unsigned int kAccessControlMaxAge_Default{24 * 60 * 60};

        /**
         *  This can be {"*"} meaning any origin (default). 
         *  Or it can be a list of values present in HTTP Origin Headers (typically just hostname but can be Host:port)
         */
        optional<Set<String>> fAllowedOrigins;

        /**
         */
        static constexpr string_view kAccessControlWildcard = "*"sv;

        /**
         *  This is the set of headers which will be allowed by Access-Control-Request-Headers OPTIONS requests.
         *  The default - * - kAccessControlWildcard - any - is typically just fine.
         */
        optional<Set<String>> fAllowedHeaders;

        /**
         *  @see Characters::ToString ();
         */
        Characters::String ToString () const;
    };
    inline const CORSOptions kDefault_CORSOptions{CORSOptions::kAllowCredentials_Default, CORSOptions::kAccessControlMaxAge_Default,
                                                  Set<String>{CORSOptions::kAccessControlWildcard}, Set<String>{CORSOptions::kAccessControlWildcard}};

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "CORS.inl"

#endif /*_Stroika_Framework_WebServer_CORS_h_*/
