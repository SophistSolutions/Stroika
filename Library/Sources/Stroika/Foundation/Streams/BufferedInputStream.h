/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_BufferedInputStream_h_
#define _Stroika_Foundation_Streams_BufferedInputStream_h_    1

#include    "../StroikaPreComp.h"

#include    <memory>

#include    "../Configuration/Common.h"
#include    "InputStream.h"



/**
 *  \file
 *
 *  \version    <a href="code_status.html#Alpha-Late">Alpha-Late</a>
 *
 *      @todo   BufferedInputStream::IRep_ IMPLEMENTATION NEEDED. It does no buffering!
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
             *  @brief  BufferedInputStream is a BinaryInputStream which provides buffered access.
             *          This is useful if calls to the underling stream source can be expensive. This class
             *          loads chunks of the stream into memory, and reduces calls to the underlying stream.
             */
            template    <typename ELEMENT_TYPE>
            class   BufferedInputStream : public InputStream<ELEMENT_TYPE> {
            private:
                class   Rep_;
            public:
                BufferedInputStream (const InputStream<ELEMENT_TYPE>& realIn);
            };


        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "BufferedInputStream.inl"

#endif  /*_Stroika_Foundation_Streams_BufferedInputStream_h_*/
