/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_BasicBinaryInputStream_h_
#define _Stroika_Foundation_Streams_BasicBinaryInputStream_h_  1

#include    "../StroikaPreComp.h"

#include    <mutex>
#include    <vector>

#include    "../Configuration/Common.h"
#include    "../Memory/SmallStackBuffer.h"

#include    "InputStream.h"
#include    "Seekable.h"
#include    "MemoryStream.h"


#pragma message ("Warning: BasicBinaryInputStream FILE DEPRECATED")

/**
 *  \file
 *
 *  \version    <a href="code_status.html#Alpha-Early">Alpha-Early</a>
 *
 *      @todo   Should add Close () method. Any subsequent calls to this stream - would fail?
 *
 *              (maybe close ignored).
 *
 *              If we allow for that - we may need to have check method - isOpen?. So maybe best to
 *              have flush/close allowed, and anything else generate an assert error?
 *
 *      @todo   Re-implement using atomics to avoid critical section (cheaper).
 *
 *      @todo   #if  0 BasicBinaryInputStream CTOR -- see why not working on gcc 4.7.2
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Streams {

//@todo pragma obsolete??

            using BasicBinaryInputStream = MemoryStream<Memory::Byte>;


        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
//#include    "BasicBinaryInputStream.inl"

#endif  /*_Stroika_Foundation_Streams_BasicBinaryInputStream_h_*/
