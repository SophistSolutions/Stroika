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
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 */

namespace Stroika::Foundation::DataExchange::Compression::Deflate {
#if qHasFeature_ZLib
    /**
     *  \par Example Usage
     *      \code
     *          Memory::BLOB  kSample1_   =     Memory::BLOB::Hex ("aa1234abcd01010102030405");
     *          Memory::BLOB  compressed  =     Compression::Deflate::Compress::New ().Transform (kSample1_);
     *          Assert (kSample1_ == Compression::Compression::Deflate::Decompress::New ().Transform (compressed));
     *      \endcode
     */
    namespace Compress {
        struct Options : Compression::Compress::Options {};
        Ptr New (const Options& o = {});
    }
    namespace Decompress {
        struct Options : Compression::Decompress::Options {};
        Ptr New (const Options& o = {});
    }
#endif

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#endif /*_Stroika_Foundation_DataExchange_Compression_Deflate_h_*/
