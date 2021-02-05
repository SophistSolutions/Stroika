/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Framework_WebServer_CORS_h_
#define _Stroika_Framework_WebServer_CORS_h_ 1

#include "../StroikaPreComp.h"

#include "../../Foundation/Characters/String.h"
#include "../../Foundation/Containers/Set.h"

/*
 */

/*
 *  \version    <a href="Code-Status.md#Alpha-Early">Alpha-Early</a>
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
        optional<bool> fAllowCredentials;

        static constexpr bool kAllowCredentials_Default{true};

        /**
         *  `Access-Control-Max-Age` header. How long the `Access-Control-Allow-Methods` and `Access-Control-Allow-Headers` headers can be cached.
         *  (rarely set - default good)
         */
        optional<unsigned int> fAccessControlMaxAge;

        static constexpr unsigned int kAccessControlMaxAge_Default{24*60*60};
        static constexpr optional<unsigned int> kAccessControlMaxAge_ProtocolDefault{5};    // if header omitted, thats what recipients will assume

        /**
         *  This can be {L"*"} meaning any origin (default). 
         *  Or it can be a list of values present in HTTP Origin Headers (typically just hostname but can be Host:port)
         */
        optional<Set<String>> fAllowedOrigins;

        /**
         */
        static constexpr wstring_view kAccessControlAllowOriginWildcard = L"*";

        /**
         *  The webserver automatically adds appropirate HTTP headers for standard HTTP communicaiton, but use this to
         *  add in extra, perhaps custom HTTP Headers to be allowed via CORS.
         */
        optional<Set<String>> fAllowedExtraHTTPHeaders;
    };
    inline const CORSOptions kDefault_CORSOptions{CORSOptions::kAllowCredentials_Default, CORSOptions::kAccessControlMaxAge_Default, Set<String>{CORSOptions::kAccessControlAllowOriginWildcard}, Set<String>{}};

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "CORS.inl"

#endif /*_Stroika_Framework_WebServer_CORS_h_*/
