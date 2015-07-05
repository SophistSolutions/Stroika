/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_TextOutputStreamBinaryAdapter_h_
#define _Stroika_Foundation_Streams_TextOutputStreamBinaryAdapter_h_ 1

#include    "../StroikaPreComp.h"

#include    <string>

#include    "../Memory/BLOB.h"

#include    "BinaryOutputStream.h"
#include    "TextOutputStream.h"



/**
 *  \file
 *
 *  TODO:
 *      @todo   Extend format with eUTF16... and eUTF32, and note that wWCharT could be same as eUTF16/32 depending
 *              on system defaults.
 *
 *      @todo   Seekable case NYI
 *
 *      @todo   Seekable. Should this be seekable? Take CTOR param indicating if seekable? Maybe also conditional
 *              on if arg is seekable? (documentd, but unsure)
 *
 *                  Clarify behavior if src (CTOR ARG) stream is or is not seekable. And also what about if src is not
 *                  seeked to beginning for BOM? Clarify exact semantics!!!
 *
 *      @todo   Thread/Locking...
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Streams {


            /**
             *  \brief Take some binary output stream, and make it look like an output stream of (UNICODE) characters.
             *
             *  Obviously todo this, there may be some character set mapping/conversion needed. The object
             *  takes constructor arguments to decide how this will he handled.
             *
             *  TextOutputStreamBinaryAdapter is Seekable iff its constructed with a BinaryOutputStream which is seekable.
             */
            class   TextOutputStreamBinaryAdapter : public TextOutputStream {
            public:
                enum    class   Format : uint8_t    {
                    eUTF8WithBOM        =   1,
                    eUTF8WithoutBOM     =   2,
                    eUTF8               =   eUTF8WithBOM,

                    eWCharTWithBOM      =   3,
                    eWCharTWithoutBOM   =   4,
                    eWCharT             =   eWCharTWithBOM,

                };
            public:
                TextOutputStreamBinaryAdapter (const BinaryOutputStream& src, Format format = Format::eUTF8);

            private:
                class   Seekable_UTF8_Rep_;
                class   Seekable_WCharT_Rep_;
                class   UnSeekable_UTF8_Rep_;
                class   UnSeekable_WCharT_Rep_;
            private:
                shared_ptr<_IRep> mk_ (const BinaryOutputStream& src, Format format);
            };


        }
    }
}
#endif  /*_Stroika_Foundation_Streams_TextOutputStreamBinaryAdapter_h_*/
