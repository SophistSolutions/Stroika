/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_TextInputStreamBinaryAdapter_h_
#define _Stroika_Foundation_Streams_TextInputStreamBinaryAdapter_h_ 1

#include    "../StroikaPreComp.h"

#include    <string>

#include    "../Memory/BLOB.h"

#include    "BinaryInputStream.h"
#include    "Seekable.h"
#include    "TextInputStream.h"



/**
 *  \file
 *
 *      @todo   Cruddy, draft (but technically I think correct) implementation.
 */

namespace   Stroika {
    namespace   Foundation {
        namespace   Streams {

            /**
             *
             */
            class   TextInputStreamBinaryAdapter : public TextInputStream {
            private:
                class   IRep_;
            public:
                TextInputStreamBinaryAdapter (BinaryInputStream src);
            };

        }
    }
}
#endif  /*_Stroika_Foundation_Streams_TextInputStreamBinaryAdapter_h_*/
