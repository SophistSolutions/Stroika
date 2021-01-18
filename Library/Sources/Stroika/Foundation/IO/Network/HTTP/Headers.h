/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_HTTP_Headers_h_
#define _Stroika_Foundation_IO_Network_HTTP_Headers_h_ 1

#include "../../../StroikaPreComp.h"

#include "../../../Characters/String.h"
#include "../../../Configuration/Common.h"
#include "../../../Containers/Collection.h"
#include "../../../Containers/Mapping.h"
#include "../../../DataExchange/InternetMediaType.h"
#include "../../../Debug/AssertExternallySynchronizedLock.h"

#include "CacheControl.h"
#include "ETag.h"

/**
 */
namespace Stroika::Foundation::IO::Network::HTTP {

    using Characters::String;
    using Common::KeyValuePair;
    using Containers::Collection;
    using Containers::Mapping;
    using DataExchange::InternetMediaType;
    using Traversal::Iterable;

    /** 
     * standard HTTP headers one might want to access/retrieve
     */
    namespace HeaderName {

        constexpr wstring_view kCacheControl                  = L"Cache-Control"sv;
        constexpr wstring_view kContentLength                 = L"Content-Length"sv;
        constexpr wstring_view kContentType                   = L"Content-Type"sv;
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

    /**
     * SB roughly equiv to Association<String,String> but thats not supported in Stroika yet.
     * But for now mainly looking like Mapping<String,String> - since works in HF, and ..
     */
    class Headers : private Debug::AssertExternallySynchronizedLock {
    public:
        /**
         */
        Headers () = default;
        Headers (const Iterable<KeyValuePair<String, String>>& src);

    public:
        /**
         *  Set to any string value, or to nullopt to clear the option.
         */
        nonvirtual void SetHeader (const String& name, const optional<String>& value);

    public:
        /**
         *  Return the unsigned integer value of the Content-Length header.
         */
        nonvirtual optional<CacheControl> GetCacheControl () const;

    public:
        /**
         *  @see GetContentLength
         */
        nonvirtual void SetCacheControl (const optional<CacheControl>& cacheControl);

    public:
        /**
         *  Return the unsigned integer value of the Content-Length header.
         */
        nonvirtual optional<uint64_t> GetContentLength () const;

    public:
        /**
         *  @see GetContentLength
         */
        nonvirtual void SetContentLength (const optional<uint64_t>& contentLength);

    public:
        /**
         *  Return the HTTP message body Content-Type, if any given
         */
        nonvirtual optional<InternetMediaType> GetContentType () const;

    public:
        /**
         *  @see GetContentType
         */
        nonvirtual void SetContentType (const optional<InternetMediaType>& contentType);

    public:
        /**
         *  Return the HTTP header ETag, if any given
         */
        nonvirtual optional<ETag> GetETag () const;

    public:
        /**
         *  @see GetETag
         */
        nonvirtual void SetETag (const optional<ETag>& etag);

    public:
        /**
         *  Returns the combined set of headers (list Key:Value pairs). Note this may not be returned in
         *  the same order and exactly losslessly identically to what was passed in.
         * 
         *  Supported T types:
         *      o   Mapping<String,String>
         *      o   Collection<KeyValuePair<String,String>>
         */
        template <typename T>
        nonvirtual T As () const;

    private:
        Collection<KeyValuePair<String, String>> fExtraHeaders_;
        optional<CacheControl>                   fCacheControl_;
        optional<uint64_t>                       fContentLength_;
        optional<InternetMediaType>              fContentType_;
        optional<ETag>                           fETag_;
    };
    template <>
    Mapping<String, String> Headers::As () const;
    template <>
    Collection<KeyValuePair<String, String>> Headers::As () const;

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Headers.inl"

#endif /*_Stroika_Foundation_IO_Network_HTTP_Headers_h_*/
