/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_HTTP_Headers_h_
#define _Stroika_Foundation_IO_Network_HTTP_Headers_h_ 1

#include "../../../StroikaPreComp.h"

#include <map>
#include <string>

#include "../../../Configuration/Common.h"

/*
 * TODO:
 *      @todo       When we have a good C++ 'static string' class - maybe use that here.
 *                  Maybe ONLY can do once we have compiler constexpr support?
 */

namespace Stroika::Foundation::IO::Network::HTTP {

    // standard HTTP headers one might want to access/retrieve
    namespace HeaderName {

        constexpr wchar_t kCacheControl[]                  = L"Cache-Control";
        constexpr wchar_t kContentType[]                   = L"Content-Type";
        constexpr wchar_t kContentLength[]                 = L"Content-Length";
        constexpr wchar_t kConnection[]                    = L"Connection";
        constexpr wchar_t kServer[]                        = L"Server";
        constexpr wchar_t kDate[]                          = L"Date";
        constexpr wchar_t kETag[]                          = L"ETag";
        constexpr wchar_t kLastModified[]                  = L"Last-Modified";
        constexpr wchar_t kUserAgent[]                     = L"User-Agent";
        constexpr wchar_t kSOAPAction[]                    = L"SOAPAction";
        constexpr wchar_t kAcceptEncoding[]                = L"Accept-Encoding";
        constexpr wchar_t kExpect[]                        = L"Expect";
        constexpr wchar_t kExpires[]                       = L"Expires";
        constexpr wchar_t kTransferEncoding[]              = L"Transfer-Encoding";
        constexpr wchar_t kAccessControlAllowCredentials[] = L"Access-Control-Allow-Credentials";
        constexpr wchar_t kAccessControlAllowOrigin[]      = L"Access-Control-Allow-Origin";
        constexpr wchar_t kAccessControlAllowHeaders[]     = L"Access-Control-Allow-Headers";
        constexpr wchar_t kAccessControlAllowMethods[]     = L"Access-Control-Allow-Methods";
        constexpr wchar_t kAccessControlMaxAge[]           = L"Access-Control-Max-Age";
        constexpr wchar_t kAuthorization[]                 = L"Authorization";
        constexpr wchar_t kAllow[]                         = L"Allow";
        constexpr wchar_t kKeepAlive[]                     = L"Keep-Alive";
        constexpr wchar_t kReferrer[]                      = L"Referer"; // intentionally spelled this way - misspelled in the HTTP RFC
    }

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Headers.inl"

#endif /*_Stroika_Foundation_IO_Network_HTTP_Headers_h_*/
