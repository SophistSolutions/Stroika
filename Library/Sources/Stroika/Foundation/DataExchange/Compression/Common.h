/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_Compression_Common_h_
#define _Stroika_Foundation_DataExchange_Compression_Common_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Memory/BLOB.h"
#include "Stroika/Foundation/Streams/InputStream.h"

/**
 *  \file
 */

namespace Stroika::Foundation::DataExchange::Compression {

    using namespace Streams;
    using Memory::BLOB;

    struct Stats {
        optional<float> fCompression; // 0 is complete compression, and 1.0 is same size as original
    };

    /**
     *  API NOT synchronized - use one at a time
     */
    struct IRep {
        // req not ongoing transform on this instance (if re-used, last transform must be completed, meaning EOF returned, or exception thrown
        virtual InputStream::Ptr<byte> Transform (const InputStream::Ptr<byte>& src) = 0;
        // return null (depends on implementation) or stats about current ongoing or last completed transformation.
        virtual optional<Stats> GetStats () const = 0;
    };
    /**
     */
    struct Ptr : shared_ptr<IRep> {
        Ptr ()               = default;
        Ptr (const Ptr&)     = default;
        Ptr (Ptr&&) noexcept = default;
        Ptr (const shared_ptr<IRep>& s);
        Ptr (shared_ptr<IRep>&& s) noexcept;
        nonvirtual InputStream::Ptr<byte> Transform (const InputStream::Ptr<byte>& src);
        nonvirtual BLOB                   Transform (const BLOB& src);
        nonvirtual optional<Stats> GetStats () const;
    };

    namespace Compress {
        struct Options {
            optional<float> fCompressionLevel; // 0 is least, and 1.0 is most compression/highest compression
        };
    }
    namespace Decompress {
        struct Options {};
    }

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Common.inl"

#endif /*_Stroika_Foundation_DataExchange_Compression_Common_h_*/
