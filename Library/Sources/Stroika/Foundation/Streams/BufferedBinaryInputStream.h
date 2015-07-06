/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
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
 *  \version    <a href="code_status.html#Alpha-Late">Alpha-Late</a>
 *
 *
 *      @todo   REDO as template so can support text AND binary streams.
 *
 *      @todo   BufferedBinaryInputStream::IRep_ IMPLEMENTATION NEEDED. It does no buffering!
 *
 *      @todo   Seekable! Must think out if this should mixin Seekable or not. I THINK it must be.
 *              I THINK if one SEEKS this, but the underlying
 *              class doesn't support seeking, we MAY need to either BUFFER MORE, or throw not supported.
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
            class   BufferedBinaryInputStream : public InputStream<Byte> {
            private:
                class   Rep_;
            public:
                BufferedBinaryInputStream (const InputStream<Byte>& realIn);
            };


        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "BufferedBinaryInputStream.inl"

#endif  /*_Stroika_Foundation_Streams_BufferedBinaryInputStream_h_*/
