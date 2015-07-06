/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_iostream_TextInputStreamFromIStreamAdapter_h_
#define _Stroika_Foundation_Streams_iostream_TextInputStreamFromIStreamAdapter_h_ 1

#include    "../../StroikaPreComp.h"

#include    <istream>
#include    <mutex>

#include    "../../Configuration/Common.h"
#include    "../../Memory/SmallStackBuffer.h"

#include    "../InputStream.h"



/**
 * TODO:
 *          o   Consider mixing in Seekable here. Would be easy
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Streams {
            namespace   iostream {


                /**
                 *
                 */
                class   TextInputStreamFromIStreamAdapter : public InputStream<Characters::Character> {
                public:
                    TextInputStreamFromIStreamAdapter (wistream& originalStream);
                private:
                    class   Rep_;
                };


            }
        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#endif  /*_Stroika_Foundation_Streams_iostream_TextInputStreamFromIStreamAdapter_h_*/
