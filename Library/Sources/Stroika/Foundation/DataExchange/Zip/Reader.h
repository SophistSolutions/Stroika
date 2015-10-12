/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_Zip_ArchiveReader_h_
#define _Stroika_Foundation_DataExchange_Zip_ArchiveReader_h_   1

#include    "../../StroikaPreComp.h"

#include    "../../Streams/InputStream.h"

#include    "../ArchiveReader.h"


/**
 *  \file
 *
 * TODO:
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   DataExchange {
            namespace   Zip {


#if     qHasFeature_ZLib
                /**
                 *  @todo add example usage (and docs)
                 */
                class ArchiveReader : public DataExchange::ArchiveReader {
                public:
                    ArchiveReader (const Streams::InputStream<Memory::Byte>& in);
                private:
                    class   Rep_;
                };
#endif


            }
        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#endif  /*_Stroika_Foundation_DataExchange_Zip_ArchiveReader_h_*/
