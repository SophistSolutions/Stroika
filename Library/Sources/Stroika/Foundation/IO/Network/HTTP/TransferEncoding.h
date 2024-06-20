/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_HTTP_TransferEncoding_h_
#define _Stroika_Foundation_IO_Network_HTTP_TransferEncoding_h_ 1

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
     *  \brief Transfer-Encoding is a hop-by-hop header, that is applied to a message between two nodes, not to a resource itself. Each segment of a multi-node connection can use different Transfer-Encoding values. If you want to compress data over the whole connection, use the end-to-end Content-Encoding header instead
     * 
     *  TransferEncoding is typically identity (in which case omitted) or chucked (@see https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers/Transfer-Encoding)
     *
     *  This is (probably mostly) obsoleted in HTTP 2.0(@todo read up); and we don't currently intend to support compress etc here 
     *  @see https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers/Content-Encoding
     */
    struct TransferEncoding {
    public:
        /**
         */
        using AtomType = DataExchange::Atom<DataExchange::AtomManager_CaseInsensitive>;

    public:
        /**
         */
        TransferEncoding (AtomType a);
        template <Characters::IConvertibleToString STRING_LIKE>
        TransferEncoding (STRING_LIKE&& name);

    public:
        nonvirtual bool operator== (const TransferEncoding& rhs) const = default;

    public:
        /**
         *  \note  - though three way comparable, ordering is NOT alphabetical
         */
        nonvirtual auto operator<=> (const TransferEncoding& rhs) const = default;

    public:
        /**
         *  @see Characters::ToString ();
         */
        nonvirtual String ToString () const;

    public:
        /**
         * identity The default (identity) encoding; the use of no transformation whatsoever. This content-coding is used only in the Accept- Encoding header, and SHOULD NOT be used in the Content-Encoding header.
         */
        static const TransferEncoding kChunked;

        /**
         *  compress The encoding format produced by the common UNIX file compression program "compress". This format is an adaptive Lempel-Ziv-Welch coding (LZW).
         */
        static const TransferEncoding kCompress;

        /**
         *  deflate The "zlib" format defined in RFC 1950 [31] in combination with the "deflate" compression mechanism described in RFC 1951 [29].
         */
        static const TransferEncoding kDeflate;

        /**
         *  gzip An encoding format produced by the file compression program "gzip" (GNU zip) as described in RFC 1952 [25]. This format is a Lempel-Ziv coding (LZ77) with a 32 bit CRC
         */
        static const TransferEncoding kGZip;

        /**
         * identity The default (identity) encoding; the use of no transformation whatsoever. This content-coding is used only in the Accept- Encoding header, and SHOULD NOT be used in the Content-Encoding header.
         */
        static const TransferEncoding kIdentity;

    private:
        AtomType fRep_;
    };

    /**
     */
    class TransferEncodings : public Sequence<TransferEncoding> {
    public:
        TransferEncodings (const Traversal::Iterable<TransferEncoding>& src);
        TransferEncodings (TransferEncoding tc);

    public:
        nonvirtual TransferEncodings& operator= (const TransferEncodings& rhs) = default;

    public:
        /**
         */
        template <typename T>
        nonvirtual String As () const;

    public:
        /**
         */
        static TransferEncodings Parse (const String& headerValue);
    };
    template <>
    String TransferEncodings::As<String> () const;

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "TransferEncoding.inl"

#endif /*_Stroika_Foundation_IO_Network_HTTP_TransferEncoding_h_*/
