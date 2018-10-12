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
#include "../../../Time/DateTime.h"

/*
 * TODO:
 *      (o)
 */

namespace Stroika::Foundation::IO::Network::HTTP {

    using Characters::String;
    using Common::KeyValuePair;
    using Containers::Mapping;

    /**
     *  \brief object representing an HTTP cookie - from https://tools.ietf.org/html/rfc6265
     */
    struct Cookie : KeyValuePair<String, String> {
        /*
         * https://tools.ietf.org/html/rfc6265#section-4.1.2.1
         */
        optional<Time::DateTime> fExpires;

        /*
         * https://tools.ietf.org/html/rfc6265#section-4.1.2.2
         */
        optional<int> fMaxAge;

        /*
         * https://tools.ietf.org/html/rfc6265#section-4.1.2.3
         */
        optional<String> fDomain;

        /*
         * https://tools.ietf.org/html/rfc6265#section-4.1.2.4
         */
        optional<String> fPath;

        /*
         * https://tools.ietf.org/html/rfc6265#section-4.1.2.5
         */
        optional<bool> fSecure;

        /*
         * https://tools.ietf.org/html/rfc6265#section-4.1.2.6
         */
        optional<bool> fHttpOnly;

        /*
         * https://tools.ietf.org/html/rfc6265#section-4.1.1
         */
        optional<Mapping<String, String>> fOtherAttributes;

        /**
         *  \brief render as a string suitable for a cookie header
         *      @see https://tools.ietf.org/html/rfc6265#section-4.2.1
         */
        nonvirtual String Encode () const;

        /**
         *  Decode an http cookie into an object.
         *      @see https://tools.ietf.org/html/rfc6265#section-4.2.1
         */
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
