/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_Compression_ZStd_h_
#define _Stroika_Foundation_DataExchange_Compression_ZStd_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/DataExchange/Compression/Common.h"

/**
 *  \file
 * 
 *  https://en.wikipedia.org/wiki/Zstd
 *
 *  \note Code-Status:  <a href="Code-Status.md#Beta">Beta</a>
 */

namespace Stroika::Foundation::DataExchange::Compression::ZStd {

    /**
     *  Check at compile time if ZStd is supported.
     */
    constexpr bool kSupported =
#if qStroika_HasComponent_zstd
        true
#else
        false
#endif
        ;

    /**
     *  \par Example Usage
     *      \code
     *          Memory::BLOB  kSample1_   =     Memory::BLOB::Hex ("aa1234abcd01010102030405");
     *          Memory::BLOB  compressed  =     Compression::ZStd::Compress::New ().Transform (kSample1_);
     *          Assert (kSample1_ == Compression::Compression::ZStd::Decompress::New ().Transform (compressed));
     *      \endcode
     * 
     *  \note if not kSupported, these 'New ()' functions just throw FeatureNotSupportedException{}
     */
    namespace Compress {
        /**
         * @brief  As of Stroika v3.0d22 - options ignored (and no api to train/generate dictionary)
         *
         *  many other options to consider allowing here
         */
        struct Options : Compression::Compress::Options {

            /**
             * @brief  defaults to ZSTD_CStreamInSize
             */
            optional<size_t> fInBufSize;

            /**
             * @brief  defaults to ZSTD_CStreamOutSize
             */
            optional<size_t> fOutBufSize;

            /**
             */
            optional<Memory::BLOB> fDictionary;

            /**
             * if true, flush after each write (useful for streaming), else ??? define rules for ZSTD_e_flush vs ZSTD_e_continue - NYI
             */
            optional<bool> fFlushEachWrite;

            /**
             */
            optional<unsigned int> fThreads;
        };
        Ptr New (const Options& o = {});
    }
    namespace Decompress {
        /**
         * @brief  As of Stroika v3.0d22 - options ignored
         */
        struct Options : Compression::Compress::Options {
            /**
             * @brief  defaults to ZSTD_DStreamInSize
             */
            optional<size_t> fInBufSize;

            /**
             * @brief  defaults to ZSTD_DStreamOutSize
             */
            optional<size_t> fOutBufSize;

            /**
             */
            optional<Memory::BLOB> fDictionary;

            /**
             */
            optional<size_t> fMaxMemory;

            /**
             */
            optional<unsigned int> fThreads;
        };
        Ptr New (const Options& o = {});
    }

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#endif /*_Stroika_Foundation_DataExchange_Compression_ZStd_h_*/
