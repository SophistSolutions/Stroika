/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
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
 *      @todo   https://stroika.atlassian.net/browse/STK-611 - some cases of Execution::InternallySyncrhonized are AssertNotImplemented on TextReader and TextWriter
 */

namespace Stroika::Foundation::Streams {

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
     *  \note   TextWriter aggregates its owned substream, so that a Close () on TextWriter
     *          will Close that substream.
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
         *
         *  \par Example Usage
         *      \code
         *          Streams::TextWriter::Ptr         textOut = Streams::TextWriter::New (out, Streams::TextWriter::Format::eUTF8WithoutBOM);
         *          textOut.Write (Characters::Format (L"%s\r\n", headLine.c_str ()));
         *          ...
         *      \endcode
         *
         *  \par Example Usage
         *      \code
         *          Streams::TextWriter::Ptr         textOut = Streams::TextWriter::New (binOut);
         *          textOut.Write (L"Hello World\n");
         *      \endcode
         */
        static Ptr New (const OutputStream<byte>::Ptr& src, Format format = Format::eUTF8);
        static Ptr New (const OutputStream<Characters::Character>::Ptr& src);
        static Ptr New (Execution::InternallySyncrhonized internallySyncrhonized, const OutputStream<byte>::Ptr& src, Format format = Format::eUTF8);
        static Ptr New (Execution::InternallySyncrhonized internallySyncrhonized, const OutputStream<Characters::Character>::Ptr& src);

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
        static shared_ptr<OutputStream<Characters::Character>::_IRep> mk_ (const OutputStream<byte>::Ptr& src, Format format);

    private:
        using InternalSyncRep_ = Streams::InternallySyncrhonizedOutputStream<Characters::Character, TextWriter, OutputStream<Characters::Character>::_IRep>;
    };

    /**
     *  Ptr is a copyable smart pointer to a TextWriter stream.
     */
    class TextWriter::Ptr : public OutputStream<Characters::Character>::Ptr {
    private:
        using inherited = typename OutputStream<Characters::Character>::Ptr;

    public:
        /**
         *  \par Example Usage
         *      \code
         *          Streams::TextWriter::Ptr         textOut = Streams::TextWriter::New (out, Streams::TextWriter::Format::eUTF8WithoutBOM);
         *          textOut.Write (Characters::Format (L"%s\r\n", headLine.c_str ()));
         *          ...
         *      \endcode
         *
         *  \par Example Usage
         *      \code
         *          Streams::TextWriter::Ptr         textOut = Streams::TextWriter::New (binOut);
         *          textOut.Write (L"Hello World\n");
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

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "TextWriter.inl"

#endif /*_Stroika_Foundation_Streams_TextWriter_h_*/
