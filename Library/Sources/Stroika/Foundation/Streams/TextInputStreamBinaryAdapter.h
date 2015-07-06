/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_TextInputStreamBinaryAdapter_h_
#define _Stroika_Foundation_Streams_TextInputStreamBinaryAdapter_h_ 1

#include    "../StroikaPreComp.h"

#include    <string>

#include    "../Memory/BLOB.h"

#include    "InputStream.h"
#include    "TextReader.h"



#pragma message ("Warning: TextInputStreamBinaryAdapter FILE DEPRECATED - use TextReader")


namespace   Stroika {
    namespace   Foundation {
        namespace   Streams {


            using TextInputStreamBinaryAdapter = TextReader;


        }
    }
}
#endif  /*_Stroika_Foundation_Streams_TextInputStreamBinaryAdapter_h_*/
