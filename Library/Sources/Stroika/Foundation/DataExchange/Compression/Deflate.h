/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_Compression_Deflate_h_
#define _Stroika_Foundation_DataExchange_Compression_Deflate_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/DataExchange/Compression/Common.h"

/**
 *  \file
 * 
 *  https://en.wikipedia.org/wiki/Deflate
 *
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 */

namespace Stroika::Foundation::DataExchange::Compression::Deflate {

    /**
     *  Check at compile time if deflate is supported.
     */
    constexpr bool kSupported =
#if qHasFeature_ZLib
        true
#else
        false
#endif
        ;

    /**
     *  \par Example Usage
     *      \code
     *          Memory::BLOB  kSample1_   =     Memory::BLOB::Hex ("aa1234abcd01010102030405");
     *          Memory::BLOB  compressed  =     Compression::Deflate::Compress::New ().Transform (kSample1_);
     *          Assert (kSample1_ == Compression::Compression::Deflate::Decompress::New ().Transform (compressed));
     *      \endcode
     */
    namespace Compress {
        using Compression::Compress::Options;
        Ptr New (const Options& o = {});
    }
    namespace Decompress {
        using Compression::Decompress::Options;
        Ptr New (const Options& o = {});
    }

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#endif /*_Stroika_Foundation_DataExchange_Compression_Deflate_h_*/
