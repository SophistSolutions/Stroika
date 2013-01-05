/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_BufferedBinaryInputStream_h_
#define _Stroika_Foundation_Streams_BufferedBinaryInputStream_h_    1

#include    "../StroikaPreComp.h"

#include    <memory>

#include    "../Configuration/Common.h"
#include    "BinaryInputStream.h"




/**
 *  \file
 *
 *      @todo   Seekable! Must think out if this should mixin Seekable or not. I THINK it must be.
 *              I THINK if one SEEKS this, but the underlying
 *              class doesn't supprot seeking, we MAY need to either BUFFER MORE, or throw not supported.
 *              BufferedBinaryInputStream::IRep_ IMPLEMENTATION NEEDED
 *
 *      @todo   Add tuning parameters, such as buffer size
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Streams {

            /**
             *  @brief  BufferedBinaryInputStream is a BinaryInputStream which provides buffered access.
             *          This is useful if calls to the underling stream source can be expensive. This class
             *          loads chunks of the stream into memory, and reduces calls to the underlying stream.
             */
            class   BufferedBinaryInputStream : public BinaryInputStream {
            private:
                class   IRep_;
            public:
                BufferedBinaryInputStream (const BinaryInputStream& realIn);
            };

        }
    }
}
#endif  /*_Stroika_Foundation_Streams_BufferedBinaryInputStream_h_*/





/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "BufferedBinaryInputStream.inl"
