/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_Charset_h_
#define _Stroika_Foundation_Characters_Charset_h_ 1

#include "../StroikaPreComp.h"

#include <memory>
#include <string>

/**
 *  \file
 *
 */

namespace Stroika::Foundation::Characters {

    using namespace std;

    class String;

    /**
     *  This is NOT a widely used concept anylonger. Its largely been replaced by UNICODE.
     *  But - it IS used in HTTP, and MIME, so its pretty important to continue supporting.
     *      https://www.w3.org/International/articles/http-charset/index#charset
     * 
     *  Information tagged with a Charset is used to select a CodeCvt object, which maps (typically from)
     *  that character set to UNICODE.
     * 
     *  See https://www.iana.org/assignments/character-sets/character-sets.xhtml
     * 
     *  \note charset names are intrinsically compared case insensitively (@todo find reference for this but empirically its clear).
     */
    class Charset {

    public:
        Charset (const Charset&) = default;
        Charset (const std::string& charsetName);
        Charset (const std::string_view& charsetName);
        Charset (const String& charsetName);

    public:
        nonvirtual operator String () const;

    public:
        nonvirtual string AsNarrowSDKString () const;

    public:
        nonvirtual strong_ordering operator<=> (const Charset& rhs) const;
        nonvirtual bool            operator== (const Charset& rhs) const;

    private:
        struct Rep_;
        shared_ptr<Rep_> fRep_;
    };

    /**
     *  See https://www.iana.org/assignments/character-sets/character-sets.xhtml
     */
    namespace WellKnownCharsets {

        /**
         */
        static inline const Charset kUS_ASCII{"US-ASCII"sv};

        /** 
         *  HTTP 1.1 says that the default charset is ISO-8859-1. 
         */
        static inline const Charset kISO_8859_1{"ISO-8859-1"sv};

        /**
         */
        static inline const Charset kISO_Latin1{kISO_8859_1};

        /**
         */
        static inline const Charset kUTF8{"UTF-8"sv};
    }

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Charset.inl"

#endif /*_Stroika_Foundation_Characters_Charset_h_*/
