/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_BasicBinaryOutputStream_h_
#define _Stroika_Foundation_Streams_BasicBinaryOutputStream_h_  1

#include    "../StroikaPreComp.h"

#include    <mutex>
#include    <vector>

#include    "../Configuration/Common.h"
#include    "../Memory/BLOB.h"

#include    "BinaryOutputStream.h"
#include    "Seekable.h"
#include    "MemoryStream.h"



/*
 *  \file
 *
 *  \version    <a href="code_status.html#Alpha-Early">Alpha-Early</a>
 *
 *      @todo   This would be a good candidate class to rewrite using new Sequence_ChunkedArray
 *              class (when I implement it) based on Led chunked arrays).
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Streams {

            //@todo pragma obsolete??

            using BasicBinaryOutputStream = MemoryStream<Memory::Byte>;


        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
//#include    "BasicBinaryOutputStream.inl"

#endif  /*_Stroika_Foundation_Streams_BasicBinaryOutputStream_h_*/
