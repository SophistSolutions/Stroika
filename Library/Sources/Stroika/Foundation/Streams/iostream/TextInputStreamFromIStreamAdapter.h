/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_iostream_TextInputStreamFromIStreamAdapter_h_
#define _Stroika_Foundation_Streams_iostream_TextInputStreamFromIStreamAdapter_h_ 1

#include    "../../StroikaPreComp.h"

#include    <istream>
#include    <mutex>

#include    "../../Configuration/Common.h"
#include    "../../Memory/SmallStackBuffer.h"

#include    "../TextInputStream.h"
#include    "../Seekable.h"



/*
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
                class   TextInputStreamFromIStreamAdapter : public TextInputStream {
                private:
                    class   IRep_;
                public:
                    TextInputStreamFromIStreamAdapter (wistream& originalStream);
                };

            }
        }
    }
}

#endif  /*_Stroika_Foundation_Streams_iostream_TextInputStreamFromIStreamAdapter_h_*/





/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
