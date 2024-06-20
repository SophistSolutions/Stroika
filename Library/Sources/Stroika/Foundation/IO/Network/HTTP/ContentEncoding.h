/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_HTTP_ContentEncoding_h_
#define _Stroika_Foundation_IO_Network_HTTP_ContentEncoding_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Containers/Sequence.h"
#include "Stroika/Foundation/DataExchange/Atom.h"

/**
 */

namespace Stroika::Foundation::IO::Network::HTTP {

    using Characters::String;
    using Containers::Sequence;

    /**
     *  \brief Content coding values indicate an encoding transformation that has been or can be applied to an entity. Content codings are primarily used to allow a document to be compressed or otherwise usefully transformed without losing the identity of its underlying media type and without loss of information. Frequently, the entity is stored in coded form, transmitted directly, and only decoded by the recipient.
     * 
     *  All content-coding values are case-insensitive. 
     * 
     *  HTTP/1.1 uses content-coding values in the Accept-Encoding (section 14.3) and Content-Encoding (section 14.11) header fields. 
     * 
     *  \see https://www.w3.org/Protocols/rfc2616/rfc2616-sec3.html#sec3.5
     */
    struct ContentEncoding {
    public:
        /**
         */
        using AtomType = DataExchange::Atom<DataExchange::AtomManager_CaseInsensitive>;

    public:
        /**
         */
        ContentEncoding (AtomType a);
        template <Characters::IConvertibleToString STRING_LIKE>
        ContentEncoding (STRING_LIKE&& name);

    public:
        nonvirtual bool operator== (const ContentEncoding& rhs) const = default;

    public:
        /**
         *  \note  - though three way comparable, ordering is NOT alphabetical
         */
        nonvirtual auto operator<=> (const ContentEncoding& rhs) const = default;

    public:
        /**
         */
        template <typename T>
        nonvirtual String As () const
            requires (same_as<T, String>);

    public:
        /**
         *  @see Characters::ToString ();
         */
        nonvirtual String ToString () const;

    public:
        /**
         *  compress The encoding format produced by the common UNIX file compression program "compress". This format is an adaptive Lempel-Ziv-Welch coding (LZW).
         */
        static const ContentEncoding kCompress;

        /**
         *  deflate The "zlib" format defined in RFC 1950 [31] in combination with the "deflate" compression mechanism described in RFC 1951 [29].
         */
        static const ContentEncoding kDeflate;

        /**
         *  gzip An encoding format produced by the file compression program "gzip" (GNU zip) as described in RFC 1952 [25]. This format is a Lempel-Ziv coding (LZ77) with a 32 bit CRC
         */
        static const ContentEncoding kGZip;

        /**
         * identity The default (identity) encoding; the use of no transformation whatsoever. This content-coding is used only in the Accept- Encoding header, and SHOULD NOT be used in the Content-Encoding header.
         */
        static const ContentEncoding kIdentity;

    private:
        AtomType fRep_;
    };

    /**
     */
    class ContentEncodings : public Sequence<ContentEncoding> {
    public:
        ContentEncodings (const Traversal::Iterable<ContentEncoding>& src);
        ContentEncodings (ContentEncoding tc);

    public:
        nonvirtual ContentEncodings& operator= (const ContentEncodings& rhs) = default;

    public:
        /**
         */
        template <typename T>
        nonvirtual String As () const;

    public:
        /**
         */
        static ContentEncodings Parse (const String& headerValue);
    };
    template <>
    String ContentEncodings::As<String> () const;

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "ContentEncoding.inl"

#endif /*_Stroika_Foundation_IO_Network_HTTP_ContentEncoding_h_*/
