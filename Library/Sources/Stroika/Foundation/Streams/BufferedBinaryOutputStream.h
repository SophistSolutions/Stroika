/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_BufferedBinaryOutputStream_h_
#define _Stroika_Foundation_Streams_BufferedBinaryOutputStream_h_   1

#include    "../StroikaPreComp.h"

#include    <vector>

#include    "../Configuration/Common.h"
#include    "BufferedOutputStream.h"




#pragma message ("Warning: BufferedBinaryOutputStream FILE DEPRECATED - use BufferedOutputStream<Memory::Byte>")



namespace   Stroika {
    namespace   Foundation {
        namespace   Streams {


            using BufferedBinaryOutputStream = BufferedOutputStream<Memory::Byte>;



        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#endif  /*_Stroika_Foundation_Streams_BufferedBinaryOutputStream_h_*/
