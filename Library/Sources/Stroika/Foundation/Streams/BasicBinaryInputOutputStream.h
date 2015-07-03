/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_BasicBinaryInputOutputStream_h_
#define _Stroika_Foundation_Streams_BasicBinaryInputOutputStream_h_  1

#include    "../StroikaPreComp.h"

#include    <mutex>
#include    <vector>

#include    "../Configuration/Common.h"
#include    "../Memory/BLOB.h"

#include    "BinaryInputOutputStream.h"
#include    "MemoryStream.h"
#include    "Seekable.h"



#pragma message ("Warning: BasicBinaryInputOutputStream FILE DEPRECATED")


namespace   Stroika {
    namespace   Foundation {
        namespace   Streams {


            using BasicBinaryInputOutputStream = MemoryStream<Memory::Byte>;


        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
//#include    "BasicBinaryInputOutputStream.inl"

#endif  /*_Stroika_Foundation_Streams_BasicBinaryInputOutputStream_h_*/
