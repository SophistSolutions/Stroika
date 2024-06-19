/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */

namespace Stroika::Foundation::IO::Network::HTTP {

    /*
     ********************************************************************************
     ****************************** HTTP::TransferCodings ***************************
     ********************************************************************************
     */
    inline TransferEncodings::TransferEncodings (const Traversal::Iterable<TransferEncoding>& src)
        : Sequence<TransferEncoding>{src}
    {
    }
    inline TransferEncodings::TransferEncodings (TransferEncoding tc)
        : Sequence<TransferEncoding>{tc}
    {
    }

}

namespace Stroika::Foundation::Configuration {
    // These names are chosen to exactly match the ones described in https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers/Transfer-Encoding
    template <>
    constexpr EnumNames<IO::Network::HTTP::TransferEncoding> DefaultNames<IO::Network::HTTP::TransferEncoding>::k{{{
        {IO::Network::HTTP::TransferEncoding::eChunked, L"chunked"},
        {IO::Network::HTTP::TransferEncoding::eCompress, L"compress"},
        {IO::Network::HTTP::TransferEncoding::eDeflate, L"deflate"},
        {IO::Network::HTTP::TransferEncoding::eGZip, L"gzip"},
        {IO::Network::HTTP::TransferEncoding::eIdentity, L"identity"},
    }}};
}
