/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_TextWriter_h_
#define _Stroika_Foundation_Streams_TextWriter_h_ 1

#include    "../StroikaPreComp.h"

#include    "OutputStream.h"



/**
 *  \file
 *
 *  TODO:
 *
 *      @todo   Sterl comments:
 *          class   TextOutputStreamBinaryAdapter : public TextOutputStream {
 *          public:
 *              enum    class   Format : uint8_t    {
 *                  eUTF8WithBOM        =   1,
 * ...
 *                  eWCharT             =   eWChar
 *
 *              CHANGE DEFAULT TO NOT WITH BOM..
 *              (I’m NOT SURE ABOUT THIS CHANGE)
 *              Probably REPLACE the 'Format' stuff with the same sort of parameters we passed to our older code converison code,
 *              either a locale, or a code page, and a separate overload with flags for BOM.
 *
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
 *      @todo   Consider adding locale feature. if you use narrow string (char* or string) it uses associated
 *              locale. If none, use global locale? Could use codepage instead of locale, but propba
 *              best to have one notion and extract that codepage from teh given (or global) locale.(ONLY FOR STREAM OF CHARCTETSD?? Maybe just in WRITER)
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
             *  TextWriter is Seekable iff its constructed with a OutputStream<> which is seekable.
             *
             *  \npte   This API was called TextOutputStreamAdapter
             */
            class   TextWriter : public OutputStream<Characters::Character> {
            private:
                using   inherited = OutputStream<Characters::Character>;

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
                /**
                 * IF TextWriter given an OutStream<Bytes>, it maps the characters according to the given code page info (@todo improve so generic code page support).
                 * If handled an OutputStream<Character> - it just passes through characters.
                 */
                TextWriter (const OutputStream<Memory::Byte>& src, Format format = Format::eUTF8);
                TextWriter (const OutputStream<Characters::Character>& src);

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
