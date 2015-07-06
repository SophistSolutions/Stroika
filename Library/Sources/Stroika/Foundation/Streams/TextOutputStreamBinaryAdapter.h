/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_TextOutputStreamBinaryAdapter_h_
#define _Stroika_Foundation_Streams_TextOutputStreamBinaryAdapter_h_ 1

#include    "../StroikaPreComp.h"

#include    <string>

#include    "../Memory/BLOB.h"

#include    "OutputStream.h"
#include    "TextOutputStream.h"

#include    "TextWriter.h"


#pragma message ("Warning: TextOutputStreamBinaryAdapter FILE DEPRECATED - use TextWriter")


namespace   Stroika {
    namespace   Foundation {
        namespace   Streams {


            using TextOutputStreamBinaryAdapter = TextWriter;



        }
    }
}
#endif  /*_Stroika_Foundation_Streams_TextOutputStreamBinaryAdapter_h_*/
