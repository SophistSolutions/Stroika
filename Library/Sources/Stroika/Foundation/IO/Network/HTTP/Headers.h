/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_HTTP_Headers_h_
#define _Stroika_Foundation_IO_Network_HTTP_Headers_h_ 1

#include "../../../StroikaPreComp.h"

#include "../../../Characters/String.h"
#include "../../../Common/Property.h"
#include "../../../Configuration/Common.h"
#include "../../../Containers/Collection.h"
#include "../../../Containers/Mapping.h"
#include "../../../Containers/Set.h"
#include "../../../DataExchange/InternetMediaType.h"
#include "../../../Debug/AssertExternallySynchronizedLock.h"

#include "../URI.h"

#include "CacheControl.h"
#include "Cookie.h"
#include "ETag.h"
#include "IfNoneMatch.h"
#include "KeepAlive.h"

/**
 */
namespace Stroika::Foundation::IO::Network::HTTP {

    using Characters::String;
    using Common::KeyValuePair;
    using Containers::Collection;
    using Containers::Mapping;
    using Containers::Set;
    using DataExchange::InternetMediaType;
    using Traversal::Iterable;

    /** 
     * standard HTTP headers one might want to access/retrieve
     */
    namespace HeaderName {

        constexpr wstring_view kAcceptEncoding                = L"Accept-Encoding"sv;
        constexpr wstring_view kAllow                         = L"Allow"sv;
        constexpr wstring_view kAccessControlAllowCredentials = L"Access-Control-Allow-Credentials"sv;
        constexpr wstring_view kAccessControlAllowOrigin      = L"Access-Control-Allow-Origin"sv;
        constexpr wstring_view kAccessControlAllowHeaders     = L"Access-Control-Allow-Headers"sv;
        constexpr wstring_view kAccessControlAllowMethods     = L"Access-Control-Allow-Methods"sv;
        constexpr wstring_view kAccessControlRequestHeaders   = L"Access-Control-Request-Headers"sv;
        constexpr wstring_view kAccessControlMaxAge           = L"Access-Control-Max-Age"sv;
        constexpr wstring_view kAuthorization                 = L"Authorization"sv;
        constexpr wstring_view kCacheControl                  = L"Cache-Control"sv;
        constexpr wstring_view kContentLength                 = L"Content-Length"sv;
        constexpr wstring_view kContentType                   = L"Content-Type"sv;
        constexpr wstring_view kConnection                    = L"Connection"sv;
        constexpr wstring_view kCookie                        = L"Cookie"sv;
        constexpr wstring_view kDate                          = L"Date"sv;
        constexpr wstring_view kETag                          = L"ETag"sv;
        constexpr wstring_view kExpect                        = L"Expect"sv;
        constexpr wstring_view kExpires                       = L"Expires"sv;
        constexpr wstring_view kHost                          = L"Host"sv;
        constexpr wstring_view kIfNoneMatch                   = L"If-None-Match"sv;
        constexpr wstring_view kIfModifiedSince               = L"If-Modified-Since"sv;
        constexpr wstring_view kKeepAlive                     = L"Keep-Alive"sv;
        constexpr wstring_view kLastModified                  = L"Last-Modified"sv;
        constexpr wstring_view kLocation                      = L"Location"sv;
        constexpr wstring_view kOrigin                        = L"Origin"sv;
        constexpr wstring_view kReferrer                      = L"Referer"sv; // intentionally spelled this way - misspelled in the HTTP RFC
        constexpr wstring_view kServer                        = L"Server"sv;
        constexpr wstring_view kSetCookie                     = L"Set-Cookie"sv;
        constexpr wstring_view kSOAPAction                    = L"SOAPAction"sv;
        constexpr wstring_view kTransferEncoding              = L"Transfer-Encoding"sv;
        constexpr wstring_view kUserAgent                     = L"User-Agent"sv;
        constexpr wstring_view kVary                          = L"Vary"sv;

    }

    /**
     */
    constexpr auto kHeaderNameEqualsComparer = String::EqualsComparer{Characters::CompareOptions::eCaseInsensitive};

    /**
     * SB roughly equiv to Association<String,String> but thats not supported in Stroika yet.
     * But for now mainly looking like Mapping<String,String> - since works in HF, and ..
     */
    class Headers : private Debug::AssertExternallySynchronizedLock {
    public:
        /**
         */
        Headers ();
        Headers (const Headers& src);
        Headers (Headers&& src);
        explicit Headers (const Iterable<KeyValuePair<String, String>>& src);

    public:
        nonvirtual Headers& operator= (const Headers& rhs) = default;
        nonvirtual Headers& operator= (Headers&& rhs);

#if qDebug
    public:
        /**
         *  Allow users of the Headers object to have it share a 'assure externally synchronized' context.
         */
        using Debug::AssertExternallySynchronizedLock::SetAssertExternallySynchronizedLockContext;
#endif

    public:
        /**
         *  For now - this returns 0 or one value.
         *  @todo - this should return ALL matching if there are multiple.
         */
        nonvirtual optional<String> LookupOne (const String& name) const;

    public:
        /**
         * Some HTTP headers can appear multiple times (such as Set-Cookie). Return all with the given name.
         */
        nonvirtual Collection<String> LookupAll (const String& name) const;

    public:
        /**
         *  Add (or sometimes replace) the given header name/value pair. Whether this replaces
         *  or adds depends on the particular headerName. Some which are recognized to appear just once
         *  are replaced (such as Cookie); Some are recognied as appearing multiple times (such as Set-Cookie)
         *  and these are appended. Use Set/Remove to avoid ambiguity.
         */
        nonvirtual void Add (const String& headerName, const String& value);

    public:
        /**
         *  Remove ALL occurrances of the given header name.
         *  For the two-arg overload, remove the given header with teh given value.
         * 
         *  Returns the number of items removed.
         */
        nonvirtual size_t Remove (const String& headerName);
        nonvirtual size_t Remove (const String& headerName, const String& value);

    public:
        /**
         *  Set to any string value, or to nullopt to clear the option.
         */
        nonvirtual void Set (const String& headerName, const optional<String>& value);
        
    public:
        /**
         *  Property with the optional<String> value of the Access-Control-Allow-Origin header.
         *  \see https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers/Access-Control-Allow-Origin
         *
         *  This can be nullopt (meaning not present), "*", or a hostname[:port] 
         */
        Common::Property<optional<String>> accessControlAllowOrigin;

    public:
        /**
         *  Property with the optional<CacheControl> value of the Allow header.
         *  \see https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers/Allow
         *
         *  This property automatically parses the header into an set of strings (because order here doesn't matter) 
         */
        Common::Property<optional<Containers::Set<String>>> allow;

    public:
        /**
         *  Property with the optional<CacheControl> value of the Cache-Control header.
         */
        Common::Property<optional<CacheControl>> cacheControl;

    public:
        /**
         *  Value of the HTTP 1.1 and earlier Connection header (connection property).
         */
        enum ConnectionValue {
            eKeepAlive,
            eClose,
        };

    public:
        /**
         *  Property with the optional<ConnectionValue> value of the Connection header.
         *
         *  Header mostly just used for HTTP 1.1 and earlier.
         */
        Common::Property<optional<ConnectionValue>> connection;

    public:
        /**
         *  Property with the optional<uint64_t> value of the Content-Length header.
         *  \note this refers to the (possibly encoded) size in bytes of the HTTP payload body (so if zipped its the size of the zipfile not the size after unzip).
         *
         *  \par Example Usage
         *      \code
         *          optional<uint64_t> contentLenghth = fHeaders_.contentLength;
         *      \endcode         
         */
        Common::Property<optional<uint64_t>> contentLength;

    public:
        /**
         *  Property with the optional<InternetMediaType> value of the Content-Type header.
         *
         *  \par Example Usage
         *      \code
         *          optional<InternetMediaType> contentType = fHeaders_.contentType;
         *          fHeaders_.contentType = nullopt; // remove the content-type header
         *      \endcode         
         */
        Common::Property<optional<InternetMediaType>> contentType;

    public:
        /**
         *  @see https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers/Cookie
         * 
         *  if the list is empty, this corresponds to no header presnt
         */
        Common::Property<CookieList> cookie;

    public:
        /**
         *  Property with the optional<ETag> value of the ETag header.
         */
        Common::Property<optional<HTTP::ETag>> ETag;

    public:
        /**
         *  Property with the optional<String> value of the Host header.
         *  @see https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers/Host
         *  This is a request-only Header.
         */
        Common::Property<optional<String>> host;

    public:
        /**
         *  Property with the optional<IfNoneMatch> value of the IF-None-Match header.
         */
        Common::Property<optional<IfNoneMatch>> ifNoneMatch;

    public:
        /**
         *  https://tools.ietf.org/html/rfc2068#section-19.7.1.1
         *  https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers/Keep-Alive
         */
        Common::Property<optional<HTTP::KeepAlive>> keepAlive;

    public:
        /**
         *  Property with the optional<URI> value of the Location header.
         *  https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers/Location
         */
        Common::Property<optional<URI>> location;

    public:
        /**
         *  Property with the optional<URI> value of the Origin header.
         *  @see https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers/Origin
         *  This is a request-only Header sent with CORS requests, as well as with POST requests
         */
        Common::Property<optional<URI>> origin;

    public:
        /**
         *  Property with the optional<String> value of the Server header.
         *  https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers/Server
         */
        Common::Property<optional<String>> server;

    public:
        /**
         * Encoded in HTTP as a SEQUENCE of separate HTTP Headers;
         * empty list amounts to no headers present
         *  @see https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers/Set-Cookie
         */
        Common::Property<CookieList> setCookie;

    public:
        /**
         *  @see https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers/Vary
         *  This is a response-only Header.
         */
        Common::Property<optional<Containers::Set<String>>> vary;

    public:
        /**
         *  Returns the combined set of headers (list Key:Value pairs). Note this may not be returned in
         *  the same order and exactly losslessly identically to what was passed in.
         * 
         *  Supported T types:
         *      o   Mapping<String,String>
         *      o   Collection<KeyValuePair<String,String>>
         *      o   Iterable<KeyValuePair<String,String>>
         */
        template <typename T = Iterable<KeyValuePair<String, String>>>
        nonvirtual T As () const;

    public:
        /**
         *  @see Characters::ToString ();
         */
        nonvirtual String ToString () const;

#if __cpp_impl_three_way_comparison >= 201907
    public:
        /**
         */
        nonvirtual strong_ordering operator<=> (const Headers& rhs) const;
        nonvirtual bool            operator== (const Headers& rhs) const;
#endif

    private:
        enum class AddOrSet { eAdd, eSet, eRemove };
        // UpdateBuiltin_ returns true iff headerName was a parsed/builtin type, and false for 'extra' headers: to find out # elts changed, use nChanges optional parameter
        nonvirtual bool UpdateBuiltin_ (AddOrSet flag, const String& headerName, const optional<String>& value, size_t* nRemoveals = nullptr);
        nonvirtual void SetExtras_ (const String& headerName, const optional<String>& value);

    private:
        // Could have properties lookup once when loading and store here. Or could have
        // them dynamically lookup in fExtraHeaders_. Just put the ones here in special variables
        // that are very commonly checked for, so their check/update will be a bit quicker.
        Collection<KeyValuePair<String, String>> fExtraHeaders_;
        optional<CacheControl>                   fCacheControl_;
        optional<uint64_t>                       fContentLength_;
        optional<InternetMediaType>              fContentType_;
        optional<CookieList>                     fCookieList_;      // store optional cuz often missing, and faster init
        optional<HTTP::ETag>                     fETag_;
        optional<String>                         fHost_;
        optional<IfNoneMatch>                    fIfNoneMatch_;
        optional<CookieList>                     fSetCookieList_;   // store optional cuz often missing, and faster init
        optional<Containers::Set<String>>        fVary_;

#if __cpp_impl_three_way_comparison < 201907
    private:
        friend bool operator== (const Headers& lhs, const Headers& rhs);
        friend bool operator!= (const Headers& lhs, const Headers& rhs);
#endif
    };
    template <>
    Mapping<String, String> Headers::As () const;
    template <>
    Collection<KeyValuePair<String, String>> Headers::As () const;

#if __cpp_impl_three_way_comparison < 201907
    bool operator== (const Headers& lhs, const Headers& rhs);
    bool operator!= (const Headers& lhs, const Headers& rhs);
#endif

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Headers.inl"

#endif /*_Stroika_Foundation_IO_Network_HTTP_Headers_h_*/
