/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_HTTP_Cookie_h_
#define _Stroika_Foundation_IO_Network_HTTP_Cookie_h_ 1

#include "../../../StroikaPreComp.h"

#include "../../../Characters/String.h"
#include "../../../Characters/ToString.h"
#include "../../../Common/KeyValuePair.h"
#include "../../../Common/Property.h"
#include "../../../Containers/Mapping.h"
#include "../../../Streams/InputStream.h"
#include "../../../Time/DateTime.h"

/**
 */

namespace Stroika::Foundation::IO::Network::HTTP {

    using Characters::Character;
    using Characters::String;
    using Common::KeyValuePair;
    using Containers::Collection;
    using Containers::Mapping;

    /**
     *  \brief object representing an HTTP cookie - from https://tools.ietf.org/html/rfc6265
     */
    struct Cookie : KeyValuePair<String, String> {
        /**
         */
        Cookie () = default;
        Cookie (const String& name, const String& value);
        Cookie (const String& name, const String& value, const Mapping<String, String>& attributes);

        /*
         * https://tools.ietf.org/html/rfc6265#section-4.1.2.1
         */
        static constexpr wstring_view kExpiresAttributeLabel = L"Expires"sv;
        optional<Time::DateTime>      fExpires;

        /*
         * https://tools.ietf.org/html/rfc6265#section-4.1.2.2
         */
        static constexpr wstring_view kMaxAgeAttributeLabel = L"Max-Age"sv;
        optional<int>                 fMaxAge;

        /*
         * https://tools.ietf.org/html/rfc6265#section-4.1.2.3
         */
        static constexpr wstring_view kDomainAttributeLabel = L"Domain"sv;
        optional<String>              fDomain;

        /*
         * https://tools.ietf.org/html/rfc6265#section-4.1.2.4
         */
        static constexpr wstring_view kPathAttributeLabel = L"Path"sv;
        optional<String>              fPath;

        /*
         * https://tools.ietf.org/html/rfc6265#section-4.1.2.5
         *
         *  valueless attribute- just presence/absense of this attribute counts
         */
        static constexpr wstring_view kSecureAttributeLabel = L"Secure"sv;
        bool                          fSecure{false};

        /*
         * https://tools.ietf.org/html/rfc6265#section-4.1.2.6
         *
         *  valueless attribute- just presence/absense of this attribute counts
         */
        static constexpr wstring_view kHttpOnlyAttributeLabel = L"HttpOnly"sv;
        bool                          fHttpOnly{false};

        /*
         * https://tools.ietf.org/html/rfc6265#section-4.1.1
         */
        optional<Mapping<String, String>> fOtherAttributes;

        /**
         *  Return a combined mapping of the other attributes with the expicit (known name) attributes)
         */
        nonvirtual Mapping<String, String> GetAttributes () const;

        /**
         */
        nonvirtual void AddAttribute (const String& aEqualsBAttributePair);
        nonvirtual void AddAttribute (const String& key, const String& value);

        /**
         *  \brief render as a string suitable for a cookie header
         *      @see https://tools.ietf.org/html/rfc6265#section-4.2.1
         */
        template <typename T = String>
        nonvirtual String As () const;

        /**
         *  Parse (decode) an http cookie into an object.
         *      @see https://tools.ietf.org/html/rfc6265#section-4.2.1
         *
         *  \req src.IsSeekable () for InputStream overload
         */
        static Cookie Parse (Streams::InputStream<Character>::Ptr src);
        static Cookie Parse (const String& src);

        /**
         *  @see Characters::ToString ();
         */
        nonvirtual Characters::String ToString () const;

#if __cpp_impl_three_way_comparison >= 201907
    public:
        /**
         */
        nonvirtual bool operator== (const Cookie& rhs) const = default;
#endif
    };
    template <>
    String Cookie::As<String> () const;

#if __cpp_impl_three_way_comparison < 201907
    bool operator== (const Cookie& lhs, const Cookie& rhs);
    bool operator!= (const Cookie& lhs, const Cookie& rhs);
#endif

    /**
     *  This corresponds to the value of the Cookie, or Set-Cookie in an HTTP Request header
     *  @see https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers/Cookie
     *  @see https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers/Set-Cookie
     */
    class CookieList {
    public:
        /**
         */
        CookieList ();
        CookieList (const CookieList& src);
        CookieList (CookieList&& src);
        CookieList (const Mapping<String, String>& basicCookies);
        CookieList (const Collection<Cookie>& cookieDetails);

    public:
        nonvirtual CookieList& operator= (const CookieList& rhs);

    public:
        /**
         *  This representation omits the cookie attributes.
         */
        Common::Property<Mapping<String, String>> cookies; // key-value-pair, as would appear in HTTP Cookie: header

    public:
        /**
         *  This representation includes any cookie attributes.
         */
        Common::Property<Collection<Cookie>> cookieDetails; // key-value-pair, as would appear in HTTP Cookie: header

    public:
        /**
         *  \brief render as a string suitable for a cookie header
         *      @see https://tools.ietf.org/html/rfc6265#section-4.2.1
         */
        nonvirtual String EncodeForCookieHeader () const;

    public:
        /**
         *  Parse (decode) the string as a CookieList. The input format can be the value of a Cookie: HTTP header (which can produce multiple cooklies)
         *  or a Set-Cookie: HTTP header (in which case it produces a single entry cookie list, possibly containing attributes)
         */
        static CookieList Parse (const String& cookieValueArg);

    public:
        /**
         *  @see Characters::ToString ();
         */
        nonvirtual Characters::String ToString () const;

#if __cpp_impl_three_way_comparison >= 201907
    public:
        /**
         */
        nonvirtual bool operator== (const CookieList& rhs) const;
#endif

#if __cpp_impl_three_way_comparison < 201907
    private:
        friend bool operator== (const CookieList& lhs, const CookieList& rhs);
        friend bool operator!= (const CookieList& lhs, const CookieList& rhs);
#endif
    private:
        Collection<Cookie> fCookieDetails_; // redundant representation
    };

#if __cpp_impl_three_way_comparison < 201907
    bool operator== (const CookieList& lhs, const CookieList& rhs);
    bool operator!= (const CookieList& lhs, const CookieList& rhs);
#endif

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Cookie.inl"

#endif /*_Stroika_Foundation_IO_Network_HTTP_Cookie_h_*/
