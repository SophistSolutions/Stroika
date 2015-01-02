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

#include    "../TextOutputStream.h"
#include    "../Seekable.h"



/**
 * TODO:
 *      @todo   Totally untested - just a draft implementations
 *
 *      @todo   Consider mixing in Seekable here. Would be easy
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Streams {
            namespace   iostream {


                /**
                 *
                 */
                class   TextOutputStreamFromOStreamAdapter : public TextOutputStream {
                private:
                    class   IRep_;
                public:
                    TextOutputStreamFromOStreamAdapter (wostream& originalStream);
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

#endif  /*_Stroika_Foundation_Streams_iostream_TextOutputStreamFromOStreamAdapter_h_*/
