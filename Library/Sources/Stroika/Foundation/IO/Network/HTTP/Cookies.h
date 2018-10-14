/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_HTTP_Cookies_h_
#define _Stroika_Foundation_IO_Network_HTTP_Cookies_h_ 1

#include "../../../StroikaPreComp.h"

#include "../../../Characters/String.h"
#include "../../../Characters/ToString.h"
#include "../../../Common/KeyValuePair.h"
#include "../../../Containers/Mapping.h"
#include "../../../Streams/InputStream.h"
#include "../../../Time/DateTime.h"

/*
 * TODO:
 *      (o)
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
        static constexpr wchar_t kExpiresAttributeLabel[] = L"Expires";
        optional<Time::DateTime> fExpires;

        /*
         * https://tools.ietf.org/html/rfc6265#section-4.1.2.2
         */
        static constexpr wchar_t kMaxAgeAttributeLabel[] = L"Max-Age";
        optional<int>            fMaxAge;

        /*
         * https://tools.ietf.org/html/rfc6265#section-4.1.2.3
         */
        static constexpr wchar_t kDomainAttributeLabel[] = L"Domain";
        optional<String>         fDomain;

        /*
         * https://tools.ietf.org/html/rfc6265#section-4.1.2.4
         */
        static constexpr wchar_t kPathAttributeLabel[] = L"Path";
        optional<String>         fPath;

        /*
         * https://tools.ietf.org/html/rfc6265#section-4.1.2.5
         *
         *  valueless attribute- just presence/absense of this attribute counts
         */
        static constexpr wchar_t kSecureAttributeLabel[] = L"Secure";
        bool                     fSecure{false};

        /*
         * https://tools.ietf.org/html/rfc6265#section-4.1.2.6
         *
         *  valueless attribute- just presence/absense of this attribute counts
         */
        static constexpr wchar_t kHttpOnlyAttributeLabel[] = L"HttpOnly";
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
#include "Cookies.inl"

#endif /*_Stroika_Foundation_IO_Network_HTTP_Cookies_h_*/
