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
#include "../../../Execution/Property.h"

#include "CacheControl.h"
#include "ETag.h"
#include "IfNoneMatch.h"

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
        Headers (const Headers& src);
        Headers (Headers&& src);
        explicit Headers (const Iterable<KeyValuePair<String, String>>& src);

    public:
        nonvirtual Headers& operator= (const Headers& rhs) = default;
        nonvirtual Headers& operator= (Headers&& rhs);

    public:
        /**
         *  For now - this returns 0 or one value.
         *  @todo - this should return ALL matching if there are multiple.
         */
        nonvirtual optional<String> LookupOne (const String& name) const;

    public:
        /**
         *  Set to any string value, or to nullopt to clear the option.
         */
        nonvirtual void SetHeader (const String& name, const optional<String>& value);

    public:
        /**
         *  Property with the optional<CacheControl> value of the Cache-Control header.
         */
        Execution::Property<optional<CacheControl>> pCacheControl{
            [=] () {
                lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
                return fCacheControl_;
            },
            [=] (const auto& cacheControl) {
                lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
                fCacheControl_ = cacheControl;
            }};

    public:
        /**
         *  Property with the optional<uint64_t> value of the Content-Length header.
         */
        Execution::Property<optional<uint64_t>> pContentLength{
            [=] () {
                lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
                return fContentLength_;
            },
            [=] (auto contentLength) {
                lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
                fContentLength_ = contentLength;
            }};

    public:
        /**
         *  Property with the optional<InternetMediaType> value of the Content-Type header.
         */
        Execution::Property<optional<InternetMediaType>> pContentType{
            [=] () {
                lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
                return fContentType_;
            },
            [=] (const auto& contentType) {
                lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
                fContentType_ = contentType;
            }};

    public:
        /**
         *  Property with the optional<ETag> value of the ETag header.
         */
        Execution::Property<optional<ETag>> pETag{
            [=] () {
                lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
                return fETag_;
            },
            [=] (const auto& etag) {
                lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
                fETag_ = etag;
            }};

    public:
        /**
         *  Property with the optional<IfNoneMatch> value of the IF-None-Match header.
         */
        Execution::Property<optional<IfNoneMatch>> pIfNoneMatch{
            [=] () {
                lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
                return fIfNoneMatch_;
            },
            [=] (const auto& ifNoneMatch) {
                lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
                fIfNoneMatch_ = ifNoneMatch;
            }};

    public:
        enum ConnectionValue {
            eKeepAlive,
            eClose,
        };
        Execution::Property<optional<ConnectionValue>> pConnection{
            [=] () -> optional<ConnectionValue> {
                return GetHeader_Connection_ ();
            },
            [=] (const auto& connectionValue) {
                SetHeader_Connection_ (connectionValue);
            }};

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
        nonvirtual optional<ConnectionValue> GetHeader_Connection_ () const;
        nonvirtual void                      SetHeader_Connection_ (const optional<ConnectionValue>& connectionValue);

    private:
        // Could have properties lookup once when loading and store here. Or could have
        // them dynamically lookup in fExtraHeaders_. Just put the ones here in special variables
        // that are very commonly checked for, so their check/update will be a bit quicker.
        Collection<KeyValuePair<String, String>> fExtraHeaders_;
        optional<CacheControl>                   fCacheControl_;
        optional<uint64_t>                       fContentLength_;
        optional<InternetMediaType>              fContentType_;
        optional<ETag>                           fETag_;
        optional<IfNoneMatch>                    fIfNoneMatch_;

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
