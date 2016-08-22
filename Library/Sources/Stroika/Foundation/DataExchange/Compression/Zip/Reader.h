/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_Compression_Zip_Reader_h_
#define _Stroika_Foundation_DataExchange_Compression_Zip_Reader_h_   1

#include    "../../../StroikaPreComp.h"

#include    "../../../Streams/InputStream.h"

#include    "../Reader.h"


/**
 *  \file
 *
 * TODO:
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   DataExchange {
            namespace   Compression {
                namespace   Zip {


#if     qHasFeature_ZLib
                    /**
                     *  \par Example Usage
                     *      \code
                     *      Memory::BLOB  kSample1_   =     Memory::BLOB::Hex ("aa1234abcd01010102030405");
                     *      Memory::BLOB  compressed  =     Compression::Zip::Reader ().Compress (kSample1_);
                     *      Assert (kSample1_ == Compression::Zip::Reader ().Decompress (compressed));
                     *      \endcode
                     */
                    class Reader : public DataExchange::Compression::Reader {
                    public:
                        Reader ();
                    private:
                        class   Rep_;
                    };
#endif


                }
            }
        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#endif  /*_Stroika_Foundation_DataExchange_Compression_Zip_Reader_h_*/
