/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_TextWriter_h_
#define _Stroika_Foundation_Streams_TextWriter_h_ 1

#include "../StroikaPreComp.h"

#include "InternallySyncrhonizedOutputStream.h"
#include "OutputStream.h"

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
 *              (I�m NOT SURE ABOUT THIS CHANGE)
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
 *              best to have one notion and extract that codepage from the given (or global) locale.(ONLY FOR STREAM OF CHARCTETSD?? Maybe just in WRITER)
 *
 */

namespace Stroika {
    namespace Foundation {
        namespace Streams {

            /**
             *  \brief Take some binary output stream, and make it look like an output stream of (UNICODE) characters.
             *
             *  Obviously todo this, there may be some character set mapping/conversion needed. The object
             *  takes constructor arguments to decide how this will he handled.
             *
             *  TextWriter is Seekable iff its constructed with a OutputStream<>::Ptr which is seekable.
             *
             *  \note   This API was called TextOutputStreamAdapter
             *
             *  \note   \em Thread-Safety   <a href="thread_safety.html#C++-Standard-Thread-Safety-Plus-Must-Externally-Synchronize-Letter">C++-Standard-Thread-Safety-Plus-Must-Externally-Synchronize-Letter</a>
             */
            class TextWriter : public OutputStream<Characters::Character> {
            public:
                TextWriter ()                  = delete;
                TextWriter (const TextWriter&) = delete;

            public:
                enum class Format : uint8_t {
                    eUTF8WithBOM    = 1,
                    eUTF8WithoutBOM = 2,
                    eUTF8           = eUTF8WithBOM,

                    eWCharTWithBOM    = 3,
                    eWCharTWithoutBOM = 4,
                    eWCharT           = eWCharTWithBOM,
                };

            public:
                class Ptr;

            public:
                /**
                 * IF TextWriter given an OutStream<Bytes>, it maps the characters according to the given code page info (@todo improve so generic code page support).
                 * If handled an OutputStream<Character> - it just passes through characters.
                 */
                static Ptr New (const OutputStream<Memory::Byte>::Ptr& src, Format format = Format::eUTF8);
                static Ptr New (const OutputStream<Characters::Character>::Ptr& src);
                static Ptr New (const TextWriter&) = delete;

            public:
                /**
                 */
                nonvirtual TextWriter& operator= (const TextWriter&) = delete;

            private:
                class Seekable_UTF8_Rep_;
                class Seekable_WCharT_Rep_;
                class UnSeekable_UTF8_Rep_;
                class UnSeekable_WCharT_Rep_;

            private:
                static shared_ptr<OutputStream<Characters::Character>::_IRep> mk_ (const OutputStream<Memory::Byte>::Ptr& src, Format format);
            };

            /**
             *  Ptr is a copyable smart pointer to a TextWriter stream.
             */
            class TextWriter::Ptr : public OutputStream<Characters::Character>::Ptr {
            private:
                using inherited = typename OutputStream<Characters::Character>::Ptr;

            public:
                /**
                &&&&&
                *  \par Example Usage
                *      \code
                *          InputStream<Byte>::Ptr in = ExternallyOwnedMemoryInputStream<Byte> (begin (buf), begin (buf) + nBytesRead);
                *      \endcode
                *
                *  \par Example Usage
                *      \code
                *          CallExpectingBinaryInputStreamPtr (ExternallyOwnedMemoryInputStream<Byte> (begin (buf), begin (buf) + nBytesRead))
                *      \endcode
                */
                Ptr ()                = default;
                Ptr (const Ptr& from) = default;
                Ptr (const OutputStream<Characters::Character>::Ptr& from);

            protected:
                Ptr (const shared_ptr<OutputStream<Characters::Character>::_IRep>& from);

            public:
                nonvirtual Ptr& operator= (const Ptr& rhs) = default;

            private:
                friend class TextWriter;
            };
        }
    }
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "TextWriter.inl"

#endif /*_Stroika_Foundation_Streams_TextWriter_h_*/
