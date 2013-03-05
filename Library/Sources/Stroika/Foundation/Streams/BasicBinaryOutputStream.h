/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_BasicBinaryOutputStream_h_
#define _Stroika_Foundation_Streams_BasicBinaryOutputStream_h_  1

#include    "../StroikaPreComp.h"

#include    <mutex>
#include    <vector>

#include    "../Configuration/Common.h"
#include    "../Memory/SmallStackBuffer.h"

#include    "BinaryOutputStream.h"
#include    "Seekable.h"


/*
 *  \file
 *      @todo   This would be a good candidate class to rewrite using new ChunkedArray class (when I implement it)
 *              based on Led chunked arrays).
 *
 */




namespace   Stroika {
    namespace   Foundation {
        namespace   Streams {


            /**
             *  \brief  Simplest to use BinaryOutputStream; BasicBinaryOutputStream can be written to, and then the BLOB of data retrieved.
             *
             *  BasicBinaryOutputStream is threadsafe - meaning Write() can safely be called from
             *  multiple threads at a time freely, and the results are interleaved without corruption. There is no guarantee
             *  of ordering, but one will finish before the next starts writing (so if they are writing structured messages,
             *  those will remain in-tact).
             *
             *  BasicBinaryOutputStream is Seekable.
             */
            class   BasicBinaryOutputStream : public BinaryOutputStream {
            private:
                class   IRep_;
            public:
                BasicBinaryOutputStream ();
            };


        }
    }
}

#endif  /*_Stroika_Foundation_Streams_BasicBinaryOutputStream_h_*/





/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "BasicBinaryOutputStream.inl"
