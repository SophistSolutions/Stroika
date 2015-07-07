/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_BufferedBinaryInputStream_h_
#define _Stroika_Foundation_Streams_BufferedBinaryInputStream_h_    1

#include    "../StroikaPreComp.h"

#include    <memory>

#include    "../Configuration/Common.h"
#include    "InputStream.h"
#include    "BufferedInputStream.h"




#pragma message ("Warning: BufferedBinaryInputStream FILE DEPRECATED - use BufferedInputStream<Memory::Byte>")



namespace   Stroika {
    namespace   Foundation {
        namespace   Streams {


            using BufferedBinaryInputStream = BufferedInputStream<Memory::Byte>;



        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
//#include    "BufferedBinaryInputStream.inl"

#endif  /*_Stroika_Foundation_Streams_BufferedBinaryInputStream_h_*/
