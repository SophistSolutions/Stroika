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

    /**
     */
    struct Stats {
        /**
         *  \brief compressed size / src data size
         *  
         *      0 is complete compression, and 1.0 is same size as original (can possibly be > 1.0)
         */
        optional<float> fCompression;
    };

    /**
     *  API NOT synchronized - use one at a time
     * 
     *  Reason Transform API not re-entrant is so you can optionally accumulate state, and return it only at the end.
     *  Would need to hook that state into Stream - or into instance of compressor - and that's intrinsically non-re-entrant.
     * 
     *  Could have done API so that inputStream was passed into New(). Didn't see any strong reason one way or the other.
     *  Could have named 'Transform' operator()() or used std::function - but again - no strong reasons one way or the other.
     * 
     *  \note - this API properly respects Stream 'blocking' - so if its upstream 'src' is not at EOF, attempts to read the
     *        result input stream may block or throw, or return nullopt (depending on which input stream api is called on the
     *        transformed stream).
     */
    struct IRep {
        /**
         *  \req not ongoing transform on this instance. 
         *  If re-used, last transform must be completed, meaning EOF returned, or exception thrown.
         */
        virtual InputStream::Ptr<byte> Transform (const InputStream::Ptr<byte>& src) = 0;
        /**
         * return null (depends on implementation) or stats about current ongoing or last completed transformation.
         */
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

        Ptr& operator= (const Ptr&) = default;
        Ptr& operator= (Ptr&&)      = default;

        /**
         */
        nonvirtual InputStream::Ptr<byte> Transform (const InputStream::Ptr<byte>& src);
        nonvirtual BLOB                   Transform (const BLOB& src);

        /**
         */
        nonvirtual optional<Stats> GetStats () const;
    };

    namespace Compress {
        /**
         */
        struct Options {
            /**
             * 0 is least, and 1.0 is most compression/highest compression
             */
            optional<float> fCompressionLevel;
        };
    }
    namespace Decompress {
        /**
         */
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
