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
         *  This can be {L"*"} meaning any origin (default). 
         *  Or it can be a list of values present in HTTP Origin Headers (typically just hostname but can be Host:port)
         */
        optional<Set<String>> fAllowedOrigins;

        /**
         */
        static constexpr wstring_view kAccessControlWildcard = L"*"sv;

        /**
         *  This is the set of headers which will be allowed by Access-Control-Request-Headers OPTIONS requests.
         *  The default - * - kAccessControlWildcard - any - is typically just fine.
         */
        optional<Set<String>> fAllowedHeaders;
    };
#if qCompiler_cpp17ExplicitInlineStaticMemberOfTemplate_Buggy
    //HACK - STORED in ConnectionManager.cpp
    extern const CORSOptions kDefault_CORSOptions;
#else
    inline const CORSOptions kDefault_CORSOptions{CORSOptions::kAllowCredentials_Default, CORSOptions::kAccessControlMaxAge_Default, Set<String>{CORSOptions::kAccessControlWildcard}, Set<String>{CORSOptions::kAccessControlWildcard}};
#endif

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "CORS.inl"

#endif /*_Stroika_Framework_WebServer_CORS_h_*/
