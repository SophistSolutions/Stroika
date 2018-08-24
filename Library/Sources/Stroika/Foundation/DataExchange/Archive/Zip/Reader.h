/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_Archive_Zip_Reader_h_
#define _Stroika_Foundation_DataExchange_Archive_Zip_Reader_h_ 1

#include "../../../StroikaPreComp.h"

#include "../../../Streams/InputStream.h"

#include "../Reader.h"

/**
 *  \file
 *
 * TODO:
 */

namespace Stroika::Foundation::DataExchange::Archive::Zip {

#if qHasFeature_ZLib
    /**
     *  @todo add example usage (and docs)
     */
    class Reader : public DataExchange::Archive::Reader {
    public:
        Reader (const Streams::InputStream<std::byte>::Ptr& in);

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

#endif /*_Stroika_Foundation_DataExchange_Archive_Zip_Reader_h_*/
