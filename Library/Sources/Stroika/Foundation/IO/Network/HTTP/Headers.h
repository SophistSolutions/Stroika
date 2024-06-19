/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_HTTP_Headers_h_
#define _Stroika_Foundation_IO_Network_HTTP_Headers_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Common/Property.h"
#include "Stroika/Foundation/Configuration/Common.h"
#include "Stroika/Foundation/Containers/Association.h"
#include "Stroika/Foundation/Containers/Collection.h"
#include "Stroika/Foundation/Containers/Mapping.h"
#include "Stroika/Foundation/Containers/Set.h"
#include "Stroika/Foundation/DataExchange/InternetMediaType.h"
#include "Stroika/Foundation/Debug/AssertExternallySynchronizedMutex.h"
#include "Stroika/Foundation/IO/Network/HTTP/CacheControl.h"
#include "Stroika/Foundation/IO/Network/HTTP/ContentCoding.h"
#include "Stroika/Foundation/IO/Network/HTTP/Cookie.h"
#include "Stroika/Foundation/IO/Network/HTTP/ETag.h"
#include "Stroika/Foundation/IO/Network/HTTP/IfNoneMatch.h"
#include "Stroika/Foundation/IO/Network/HTTP/KeepAlive.h"
#include "Stroika/Foundation/IO/Network/HTTP/TransferEncoding.h"
#include "Stroika/Foundation/IO/Network/URI.h"

/**
 */
namespace Stroika::Foundation::IO::Network::HTTP {

    using Characters::String;
    using Common::KeyValuePair;
    using Containers::Association;
    using Containers::Collection;
    using Containers::Mapping;
    using Containers::Set;
    using DataExchange::InternetMediaType;
    using Traversal::Iterable;

    /** 
     * standard HTTP headers one might want to access/retrieve
     */
    namespace HeaderName {

        constexpr string_view kAcceptEncoding                = "Accept-Encoding"sv;
        constexpr string_view kAllow                         = "Allow"sv;
        constexpr string_view kAccessControlAllowCredentials = "Access-Control-Allow-Credentials"sv;
        constexpr string_view kAccessControlAllowOrigin      = "Access-Control-Allow-Origin"sv;
        constexpr string_view kAccessControlAllowHeaders     = "Access-Control-Allow-Headers"sv;
        constexpr string_view kAccessControlAllowMethods     = "Access-Control-Allow-Methods"sv;
        constexpr string_view kAccessControlRequestHeaders   = "Access-Control-Request-Headers"sv;
        constexpr string_view kAccessControlMaxAge           = "Access-Control-Max-Age"sv;
        constexpr string_view kAuthorization                 = "Authorization"sv;
        constexpr string_view kCacheControl                  = "Cache-Control"sv;
        constexpr string_view kContentLength                 = "Content-Length"sv;
        constexpr string_view kContentType                   = "Content-Type"sv;
        constexpr string_view kConnection                    = "Connection"sv;
        constexpr string_view kCookie                        = "Cookie"sv;
        constexpr string_view kDate                          = "Date"sv;
        constexpr string_view kETag                          = "ETag"sv;
        constexpr string_view kExpect                        = "Expect"sv;
        constexpr string_view kExpires                       = "Expires"sv;
        constexpr string_view kHost                          = "Host"sv;
        constexpr string_view kIfNoneMatch                   = "If-None-Match"sv;
        constexpr string_view kIfModifiedSince               = "If-Modified-Since"sv;
        constexpr string_view kKeepAlive                     = "Keep-Alive"sv;
        constexpr string_view kLastModified                  = "Last-Modified"sv;
        constexpr string_view kLocation                      = "Location"sv;
        constexpr string_view kOrigin                        = "Origin"sv;
        constexpr string_view kReferrer                      = "Referer"sv; // intentionally spelled this way - misspelled in the HTTP RFC
        constexpr string_view kServer                        = "Server"sv;
        constexpr string_view kSetCookie                     = "Set-Cookie"sv;
        constexpr string_view kSOAPAction                    = "SOAPAction"sv;
        constexpr string_view kTransferEncoding              = "Transfer-Encoding"sv;
        constexpr string_view kUserAgent                     = "User-Agent"sv;
        constexpr string_view kVary                          = "Vary"sv;

    }

    /**
     *  \note from https://www.ietf.org/rfc/rfc2616.txt
     *      The field-names given are not limited to the set of standard
     *      request-header fields defined by this specification. Field names are
     *      case-insensitive.
     */
    constexpr auto kHeaderNameEqualsComparer  = String::EqualsComparer{Characters::eCaseInsensitive};
    constexpr auto kHeaderNameInOrderComparer = String::LessComparer{Characters::eCaseInsensitive};

    /**
     * \brief roughly equivalent to Association<String,String>, except that the class is smart about certain
     *        keys and will automatically fold them together.
     * 
     *  \note From https://www.rfc-editor.org/rfc/rfc7230#section-3.2.2
     * 
     *      The order in which header fields with differing field names are
     *      received is not significant.  However, it is good practice to send
     *      header fields that contain control data first, such as Host on
     *      requests and Date on responses, so that implementations can decide
     *      when not to handle a message as early as possible.  A server MUST NOT
     *      apply a request to the target resource until the entire request
     *      header section is received, since later header fields might include
     *      conditionals, authentication credentials, or deliberately misleading
     *      duplicate header fields that would impact request processing.
     *
     *      A sender MUST NOT generate multiple header fields with the same field
     *      name in a message unless either the entire field value for that
     *      header field is defined as a comma-separated list [i.e., #(values)]
     *      or the header field is a well-known exception (as noted below).
     *
     *      A recipient MAY combine multiple header fields with the same field
     *      name into one "field-name: field-value" pair, without changing the
     *      semantics of the message, by appending each subsequent field value to
     *      the combined field value in order, separated by a comma.  The order
     *      in which header fields with the same field name are received is
     *      therefore significant to the interpretation of the combined field
     *      value; a proxy MUST NOT change the order of these field values when
     *      forwarding a message.
     *
     *          Note: In practice, the "Set-Cookie" header field ([RFC6265]) often
     *          appears multiple times in a response message and does not use the
     *          list syntax, violating the above requirements on multiple header
     *          fields with the same name.  Since it cannot be combined into a
     *          single field-value, recipients ought to handle "Set-Cookie" as a
     *          special case while processing header fields.  (See Appendix A.2.3
     *          of [Kri2001] for details.)
     */
    class Headers {
    public:
        /**
         *  ExtendableProperty allow the value of some headers to be overridden by some containing object.
         *  When copying a Header, we don't reference that external object any longer. So copying must
         *  by value, and copy that current value.
         */
        Headers ();
        Headers (Headers&& src);
        Headers (const Headers& src);
        explicit Headers (const Iterable<pair<String, String>>& src);
        explicit Headers (const Iterable<KeyValuePair<String, String>>& src);

    public:
        nonvirtual Headers& operator= (Headers&& rhs) noexcept;
        nonvirtual Headers& operator= (const Headers& rhs);

#if qStroika_Foundation_Debug_AssertExternallySynchronizedMutex_Enabled
    public:
        /**
         *  Allow users of the Headers object to have it share a 'assure externally synchronized' context.
         */
        void SetAssertExternallySynchronizedMutexContext (const shared_ptr<Debug::AssertExternallySynchronizedMutex::SharedContext>& sharedContext);
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
         *  are replaced (such as Cookie); Some are recognized as appearing multiple times (such as Set-Cookie)
         *  and these are appended. Use Set/Remove to avoid ambiguity.
         */
        nonvirtual void Add (const String& headerName, const String& value);
        nonvirtual void Add (const KeyValuePair<String, String>& hrdAndValue);
        nonvirtual void Add (const pair<String, String>& hrdAndValue);

    public:
        /**
         *  \brief not the same as assignment - only for headers set in argument, replace those in this header object.
         */
        nonvirtual void AddAll (const Headers& headers);

    public:
        /**
         */
        nonvirtual void operator+= (const pair<String, String>& hrdAndValue);
        nonvirtual void operator+= (const KeyValuePair<String, String>& hrdAndValue);
        nonvirtual void operator+= (const Headers& headers);

    public:
        /**
         *  Remove ALL occurrences of the given header name.
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
         *  \brief HTTP Response header controlling how clients will cache this response.
         * 
         *  Property with the optional<CacheControl> value of the Cache-Control header.
         * 
         *   From https://tools.ietf.org/html/rfc7234#page-5
         * 
         *      Although caching is an entirely OPTIONAL feature
         *      of HTTP, it can be assumed that reusing a cached response is
         *      desirable and that such reuse is the default behavior when no
         *      requirement or local configuration prevents it.  Therefore, HTTP
         *      cache requirements are focused on preventing a cache from either
         *      storing a non-reusable response or reusing a stored response
         *      inappropriately, rather than mandating that caches always store and
         *      reuse particular responses.
         * 
         *  https://tools.ietf.org/html/rfc2616#section-13.4
         * 
         *      Unless specifically constrained by a cache-control (section 14.9)
         *      directive, a caching system MAY always store a successful response
         *      (see section 13.8) as a cache entry, MAY return it without validation
         *      if it is fresh, and MAY return it after successful validation. If
         *      there is neither a cache validator nor an explicit expiration time
         *      associated with a response, we do not expect it to be cached, but
         *      certain caches MAY violate this expectation (for example, when little
         *      or no network connectivity is available).
         * 
         * But then contradicting the above
         * https://tools.ietf.org/html/rfc2616#section-9.5
         *      (about POST)
         *      Responses to this method are not cacheable, unless the response
         *      includes appropriate Cache-Control or Expires header fields
         * 
         *  similarly for PUT/POST/DELETE (from same document) - those appear to not be cacheable.
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
         *          optional<uint64_t> contentLength = fHeaders_.contentLength;
         *      \endcode         
         */
        Common::ExtendableProperty<optional<uint64_t>> contentLength;

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
         *  Property with the optional<DateTime> value of the Date header.
         *
         *  \par Example Usage
         *      \code
         *      \endcode         
         */
        Common::Property<optional<Time::DateTime>> date;

    public:
        /**
         *  @see https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers/Cookie
         * 
         *  if the list is empty, this corresponds to no header present
         */
        Common::Property<CookieList> cookie;

    public:
        /**
         *  Property with the optional<ETag> value of the ETag header.
         */
        Common::ExtendableProperty<optional<HTTP::ETag>> ETag;

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
         *  Property with the optional<Set<TransferEncoding>> value of the Transfer-Encoding header.
         *
         *  This is very typically OMITTED, and when present in a Response Header, its typically value will just be TransferEncoding::eChunked
         *  because the default - TransferEncoding::eIdentity is understood when the header is missing.
         * 
         *  \note - this is generally NOT to be used for compression (though it can be).
         * 
         *  \note because a TransferEncodings object with the single entry eIdentity is the default, that state is treated as equivalent to missing (so assign of such an array to this property results in it being missing)
         */
        Common::ExtendableProperty<optional<TransferEncodings>> transferEncoding;

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
         *  \note - if Mapping<String,String> is the target type, and if setCookie headers are present, some maybe omitted in the
         *          resulting Mapping<>
         * 
         *  Supported T types:
         *      o   Association<String,String>
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

    public:
        /**
         */
        nonvirtual bool operator== (const Headers& rhs) const;

    private:
        enum class AddOrSet {
            eAdd,
            eSet,
            eRemove
        };
        // UpdateBuiltin_ returns true iff headerName was a parsed/builtin type, and false for 'extra' headers: to find out # elts changed, use nChanges optional parameter
        nonvirtual bool UpdateBuiltin_ (AddOrSet flag, const String& headerName, const optional<String>& value, size_t* nRemoveals = nullptr);
        nonvirtual void SetExtras_ (const String& headerName, const optional<String>& value);

    private:
        // Could have properties lookup once when loading and store here. Or could have
        // them dynamically lookup in fExtraHeaders_. Just put the ones here in special variables
        // that are very commonly checked for, so their check/update will be a bit quicker.
        Collection<KeyValuePair<String, String>> fExtraHeaders_;
        optional<CacheControl>                   fCacheControl_;
        optional<uint64_t> fContentLength_; // must access through property to access extended property handlers (except root getter/setter)
        optional<InternetMediaType> fContentType_;
        optional<CookieList>        fCookieList_; // store optional cuz often missing, and faster init
        optional<Time::DateTime>    fDate_;
        optional<HTTP::ETag>        fETag_; // must access through property to access extended property handlers (except root getter/setter)
        optional<String>            fHost_;
        optional<IfNoneMatch>       fIfNoneMatch_;
        optional<CookieList>        fSetCookieList_; // store optional cuz often missing, and faster init
        optional<TransferEncodings> fTransferEncoding_; // must access through property to access extended property handlers (except root getter/setter)
        optional<Containers::Set<String>>                              fVary_;
        [[no_unique_address]] Debug::AssertExternallySynchronizedMutex fThisAssertExternallySynchronized_;
    };
    template <>
    Association<String, String> Headers::As () const;
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
