/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_HTTP_ContentCoding_h_
#define _Stroika_Foundation_IO_Network_HTTP_ContentCoding_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/DataExchange/Atom.h"

/**
 */

namespace Stroika::Foundation::IO::Network::HTTP {

    using Characters::String;

    /**
     *  \brief Content coding values indicate an encoding transformation that has been or can be applied to an entity. Content codings are primarily used to allow a document to be compressed or otherwise usefully transformed without losing the identity of its underlying media type and without loss of information. Frequently, the entity is stored in coded form, transmitted directly, and only decoded by the recipient.
     * 
     *  All content-coding values are case-insensitive. 
     * 
     *  HTTP/1.1 uses content-coding values in the Accept-Encoding (section 14.3) and Content-Encoding (section 14.11) header fields. 
     * 
     *  \see https://www.w3.org/Protocols/rfc2616/rfc2616-sec3.html#sec3.5
     */
    struct ContentCoding {
    public:
        /**
         */
        using AtomType = DataExchange::Atom<DataExchange::AtomManager_CaseInsensitive>;

    public:
        /**
         */
        ContentCoding (AtomType a);
        template <Characters::IConvertibleToString STRING_LIKE>
        ContentCoding (STRING_LIKE&& name);

    public:
        bool operator== (const ContentCoding& rhs) const = default;

    public:
        /**
         *  \note  - though three way comparable, ordering is NOT alphabetical
         */
        auto operator<=> (const ContentCoding& rhs) const = default;

    public:
        /**
         *  @see Characters::ToString ();
         */
        nonvirtual String ToString () const;

    public:
        /**
         *  compress The encoding format produced by the common UNIX file compression program "compress". This format is an adaptive Lempel-Ziv-Welch coding (LZW).
         */
        static const ContentCoding kCompress;

        /**
         *  deflate The "zlib" format defined in RFC 1950 [31] in combination with the "deflate" compression mechanism described in RFC 1951 [29].
         */
        static const ContentCoding kDeflate;

        /**
         *  gzip An encoding format produced by the file compression program "gzip" (GNU zip) as described in RFC 1952 [25]. This format is a Lempel-Ziv coding (LZ77) with a 32 bit CRC
         */
        static const ContentCoding kGZip;

        /**
         * identity The default (identity) encoding; the use of no transformation whatsoever. This content-coding is used only in the Accept- Encoding header, and SHOULD NOT be used in the Content-Encoding header.
         */
        static const ContentCoding kIdentity;

    private:
        AtomType fRep_;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "ContentCoding.inl"

#endif /*_Stroika_Foundation_IO_Network_HTTP_ContentCoding_h_*/
