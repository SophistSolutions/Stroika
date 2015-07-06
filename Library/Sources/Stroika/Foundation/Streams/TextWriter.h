/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_TextWriter_h_
#define _Stroika_Foundation_Streams_TextWriter_h_ 1

#include    "../StroikaPreComp.h"

#include    "OutputStream.h"
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
             *  @todo CLEANUP DOCS - EXPLAIN WHY TAKES BIN OR TEXT STREAM - PASSTHROUGH TEXTSTREAM AND MAP/ENCODE BINSTREAM.
             *
             *  \brief Take some binary output stream, and make it look like an output stream of (UNICODE) characters.
             *
             *  Obviously todo this, there may be some character set mapping/conversion needed. The object
             *  takes constructor arguments to decide how this will he handled.
             *
             *  TextWriter is Seekable iff its constructed with a BinaryOutputStream which is seekable.
             *
             *
             *  WAS CALLED TextOutputStreamBinaryAdapter
             */
            class   TextWriter : public TextOutputStream {
            private:
                using   inherited = TextOutputStream;

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
                TextWriter (const OutputStream<Memory::Byte>& src, Format format = Format::eUTF8);
                TextWriter (const TextOutputStream& src);

            private:
                class   Seekable_UTF8_Rep_;
                class   Seekable_WCharT_Rep_;
                class   UnSeekable_UTF8_Rep_;
                class   UnSeekable_WCharT_Rep_;
            private:
                shared_ptr<_IRep> mk_ (const OutputStream<Memory::Byte>& src, Format format);
            };


        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "TextWriter.inl"

#endif  /*_Stroika_Foundation_Streams_TextWriter_h_*/
