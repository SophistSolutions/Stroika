/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_iostream_TextOutputStreamFromOStreamAdapter_h_
#define _Stroika_Foundation_Streams_iostream_TextOutputStreamFromOStreamAdapter_h_ 1

#include    "../../StroikaPreComp.h"

#include    <istream>
#include    <mutex>

#include    "../../Configuration/Common.h"
#include    "../../Memory/SmallStackBuffer.h"

#include    "OutputStreamFromStdOStream.h"


#pragma message ("Warning: TextOutputStreamFromOStreamAdapter FILE DEPRECATED - use OutputStreamFromStdOStream")


namespace   Stroika {
    namespace   Foundation {
        namespace   Streams {
            namespace   iostream {


                using TextOutputStreamFromOStreamAdapter = OutputStreamFromStdOStream<Characters::Character>;


            }
        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#endif  /*_Stroika_Foundation_Streams_iostream_TextOutputStreamFromOStreamAdapter_h_*/
