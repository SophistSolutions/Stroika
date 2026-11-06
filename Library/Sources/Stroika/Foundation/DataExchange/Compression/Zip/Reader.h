/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_Compression_Zip_Reader_h_
#define _Stroika_Foundation_DataExchange_Compression_Zip_Reader_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/DataExchange/Compression/Reader.h"

/// FILE DEPRECATED

/**
 *  \file
 *
 *  \note Code-Status:  <a href="Code-Status.md#Beta">Beta</a>
 *
 * TODO:
 *      @todo   https://github.com/SophistSolutions/Stroika/issues/745 (STK-609) - Redo the Compression Reader code to use a factory style like used in the rest of Stroika - ::New - instead of Compress methods??? needs some thought...
 */

namespace Stroika::Foundation::DataExchange::Compression::Zip {
#if qStroika_HasComponent_zlib
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
