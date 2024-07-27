/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_Compression_Zip_Reader_h_
#define _Stroika_Foundation_DataExchange_Compression_Zip_Reader_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/DataExchange/Compression/Reader.h"

/// FILE DEPRECATED

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 *
 * TODO:
 *      @todo   http://stroika-bugs.sophists.com/browse/STK-609 - Redo the Compression Reader code to use a factory style like used in the rest of Stroika - ::New - instead of Compress methods??? needs some thought...
 */

namespace Stroika::Foundation::DataExchange::Compression::Zip {
#if qHasFeature_ZLib
    /**
     *  \par Example Usage
     *      \code
     *          Memory::BLOB  kSample1_   =     Memory::BLOB::Hex ("aa1234abcd01010102030405");
     *          Memory::BLOB  compressed  =     Compression::Zip::Reader{}.Compress (kSample1_);
     *          Assert (kSample1_ == Compression::Zip::Reader{}.Decompress (compressed));
     *      \endcode
     */
    class [[deprecated ("Since Stroika v3.0d7 - use Deflate::(De-)Compress::New")]] Reader : public DataExchange::Compression::Reader {
    public:
        Reader ();
    };
#endif

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#endif /*_Stroika_Foundation_DataExchange_Compression_Zip_Reader_h_*/
