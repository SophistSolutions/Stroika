/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_HTTP_Cookie_h_
#define _Stroika_Foundation_IO_Network_HTTP_Cookie_h_ 1

#include "../../../StroikaPreComp.h"

#include "../../../Characters/String.h"
#include "../../../Characters/ToString.h"
#include "../../../Common/KeyValuePair.h"
#include "../../../Containers/Mapping.h"
#include "../../../Streams/InputStream.h"
#include "../../../Time/DateTime.h"

/**
 */

namespace Stroika::Foundation::IO::Network::HTTP {

    using Characters::Character;
    using Characters::String;
    using Common::KeyValuePair;
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
        optional<Time::DateTime> fExpires;

        /*
         * https://tools.ietf.org/html/rfc6265#section-4.1.2.2
         */
        static constexpr wstring_view kMaxAgeAttributeLabel = L"Max-Age"sv;
        optional<int>            fMaxAge;

        /*
         * https://tools.ietf.org/html/rfc6265#section-4.1.2.3
         */
        static constexpr wstring_view kDomainAttributeLabel = L"Domain"sv;
        optional<String>         fDomain;

        /*
         * https://tools.ietf.org/html/rfc6265#section-4.1.2.4
         */
        static constexpr wstring_view kPathAttributeLabel = L"Path"sv;
        optional<String>         fPath;

        /*
         * https://tools.ietf.org/html/rfc6265#section-4.1.2.5
         *
         *  valueless attribute- just presence/absense of this attribute counts
         */
        static constexpr wstring_view kSecureAttributeLabel = L"Secure"sv;
        bool                     fSecure{false};

        /*
         * https://tools.ietf.org/html/rfc6265#section-4.1.2.6
         *
         *  valueless attribute- just presence/absense of this attribute counts
         */
        static constexpr wstring_view kHttpOnlyAttributeLabel = L"HttpOnly"sv;
        bool                     fHttpOnly{false};

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
        nonvirtual String Encode () const;

        /**
         *  Decode an http cookie into an object.
         *      @see https://tools.ietf.org/html/rfc6265#section-4.2.1
         *
         *  \req src.IsSeekable () for InputStream overload
         */
        static Cookie Decode (Streams::InputStream<Character>::Ptr src);
        static Cookie Decode (const String& src);

        /**
         *  @see Characters::ToString ();
         */
        nonvirtual Characters::String ToString () const;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Cookie.inl"

#endif /*_Stroika_Foundation_IO_Network_HTTP_Cookie_h_*/
