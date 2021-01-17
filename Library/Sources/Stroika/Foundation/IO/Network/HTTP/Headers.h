/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_HTTP_Headers_h_
#define _Stroika_Foundation_IO_Network_HTTP_Headers_h_ 1

#include "../../../StroikaPreComp.h"

#include <map>
#include <string>

#include "../../../Configuration/Common.h"

/**
 */

namespace Stroika::Foundation::IO::Network::HTTP {

    /** 
     * standard HTTP headers one might want to access/retrieve
     */
    namespace HeaderName {

        constexpr wstring_view kCacheControl                  = L"Cache-Control"sv;
        constexpr wstring_view kContentType                   = L"Content-Type"sv;
        constexpr wstring_view kContentLength                 = L"Content-Length"sv;
        constexpr wstring_view kConnection                    = L"Connection"sv;
        constexpr wstring_view kServer                        = L"Server"sv;
        constexpr wstring_view kDate                          = L"Date"sv;
        constexpr wstring_view kETag                          = L"ETag"sv;
        constexpr wstring_view kLastModified                  = L"Last-Modified"sv;
        constexpr wstring_view kUserAgent                     = L"User-Agent"sv;
        constexpr wstring_view kSOAPAction                    = L"SOAPAction"sv;
        constexpr wstring_view kAcceptEncoding                = L"Accept-Encoding"sv;
        constexpr wstring_view kExpect                        = L"Expect"sv;
        constexpr wstring_view kExpires                       = L"Expires"sv;
        constexpr wstring_view kTransferEncoding              = L"Transfer-Encoding"sv;
        constexpr wstring_view kAccessControlAllowCredentials = L"Access-Control-Allow-Credentials"sv;
        constexpr wstring_view kAccessControlAllowOrigin      = L"Access-Control-Allow-Origin"sv;
        constexpr wstring_view kAccessControlAllowHeaders     = L"Access-Control-Allow-Headers"sv;
        constexpr wstring_view kAccessControlAllowMethods     = L"Access-Control-Allow-Methods"sv;
        constexpr wstring_view kAccessControlMaxAge           = L"Access-Control-Max-Age"sv;
        constexpr wstring_view kAuthorization                 = L"Authorization"sv;
        constexpr wstring_view kAllow                         = L"Allow"sv;
        constexpr wstring_view kKeepAlive                     = L"Keep-Alive"sv;
        constexpr wstring_view kReferrer                      = L"Referer"sv; // intentionally spelled this way - misspelled in the HTTP RFC
        constexpr wstring_view kIfNoneMatch                   = L"If-None-Match"sv;
        constexpr wstring_view kIfModifiedSince               = L"If-Modified-Since"sv;

    }

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Headers.inl"

#endif /*_Stroika_Foundation_IO_Network_HTTP_Headers_h_*/
