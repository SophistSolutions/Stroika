/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_Archive_7z_Reader_h_
#define _Stroika_Foundation_DataExchange_Archive_7z_Reader_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Streams/InputStream.h"

#include "Stroika/Foundation/DataExchange/Archive/Reader.h"

/**
 *  \file
 *
 *  \note Code-Status:  <a href="Code-Status.md#Alpha">Alpha</a>
 */

namespace Stroika::Foundation::DataExchange::Archive::_7z {

#if qHasFeature_LZMA
    /**
     *  @todo add example usage (and docs)
     */
    class Reader : public DataExchange::Archive::Reader {
    public:
        Reader (const Streams::InputStream::Ptr<byte>& in);

    private:
        class Rep_;
    };
#endif

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#endif /*_Stroika_Foundation_DataExchange_Archive_7z_Reader_h_*/
