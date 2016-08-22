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
                     *  @todo add example usage (and docs)
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
