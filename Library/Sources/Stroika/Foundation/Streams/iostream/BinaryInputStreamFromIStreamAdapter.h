/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_iostream_BinaryInputStreamFromIStreamAdapter_h_
#define _Stroika_Foundation_Streams_iostream_BinaryInputStreamFromIStreamAdapter_h_ 1

#include    "../../StroikaPreComp.h"

#include    "InputStreamAdapter.h"



#pragma message ("Warning: BinaryInputStreamFromIStreamAdapter FILE DEPRECATED - use InputStreamAdapter")



namespace   Stroika {
    namespace   Foundation {
        namespace   Streams {
            namespace   iostream {


                using BinaryInputStreamFromIStreamAdapter = InputStreamAdapter<Memory::Byte>;


            }
        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#endif  /*_Stroika_Foundation_Streams_iostream_BinaryInputStreamFromIStreamAdapter_h_*/
