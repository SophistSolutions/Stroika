/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_TextWriter_h_
#define _Stroika_Foundation_Streams_TextWriter_h_ 1

#include "../StroikaPreComp.h"

#include "../Characters/CodeCvt.h"
#include "../Characters/TextConvert.h"

#include "InternallySynchronizedOutputStream.h" // no need to include once we remove deprecated references to this
#include "OutputStream.h"

/**
 *  \file
 */

namespace Stroika::Foundation::Streams {

    using Characters::ByteOrderMark;
    using Characters::Character;
    using Characters::UnicodeExternalEncodings;

    /**
     *  \brief Take some binary output stream, and make it look like an output stream of (UNICODE) characters.
     *
     *  Obviously todo this, there may be some character set mapping/conversion needed. The object
     *  takes constructor arguments to decide how this will he handled.
     *
     *  TextWriter is not seekable. It's possible to implement, but complicated, and performance costly. Very unlikely
     *  to ever be useful.
     *
     *  \note   This API was called TextOutputStreamAdapter
     *
     *  \note   TextWriter aggregates its owned substream, so that a Close () on TextWriter
     *          will Close that substream.
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety-For-Envelope-Plus-Must-Externally-Synchronize-Letter">C++-Standard-Thread-Safety-For-Envelope-Plus-Must-Externally-Synchronize-Letter</a>
     */
    class TextWriter : public OutputStream<Character> {
    public:
        TextWriter ()                  = delete;
        TextWriter (const TextWriter&) = delete;

    public:
        class Ptr;

    public:
        /**
         * If TextWriter given an OutStream<Bytes>, it maps the characters according to the given code page info (@todo improve so generic code page support).
         * If handled an OutputStream<Character> - it just passes through characters.
         *
         *  \par Example Usage
         *      \code
         *          Streams::TextWriter::Ptr         textOut = Streams::TextWriter::New (out, UnicodeExternalEncodings::eUTF8, ByteOrderMark::eInclude);
         *          textOut.Write (Characters::Format (L"%s\r\n", headLine.c_str ()));
         *          ...
         *      \endcode
         *
         *  \par Example Usage
         *      \code
         *          Streams::TextWriter::Ptr         textOut = Streams::TextWriter::New (binOut);
         *          textOut.Write ("Hello World\n");
         *      \endcode
         */
        static Ptr New (const OutputStream<byte>::Ptr& src, Characters::CodeCvt<Character>&& char2OutputConverter);
        static Ptr New (const OutputStream<byte>::Ptr& src, UnicodeExternalEncodings e = UnicodeExternalEncodings::eDefault,
                        ByteOrderMark bom = ByteOrderMark::eDontInclude);
        static Ptr New (const OutputStream<Character>::Ptr& src);

    public:
        DISABLE_COMPILER_MSC_WARNING_START (4996); // DEPRECATED
        DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Wdeprecated-declarations\"");
        DISABLE_COMPILER_CLANG_WARNING_START ("clang diagnostic ignored \"-Wdeprecated-declarations\"");
        enum class [[deprecated ("Since Stroka v3.0d1, use UnicodeExternalEncodings overload")]] Format : uint8_t{
            eUTF8WithBOM = 1,   eUTF8WithoutBOM = 2,   eUTF8 = eUTF8WithBOM,
            eWCharTWithBOM = 3, eWCharTWithoutBOM = 4, eWCharT = eWCharTWithBOM,
        };
        [[deprecated ("Since Stroka v3.0d1, use UnicodeExternalEncodings overload")]] static Ptr New (const OutputStream<byte>::Ptr& src,
                                                                                                      Format format); // to be deprecated soon
        [[deprecated ("Since Stroka v3.0d1, just wrap in InternallySynchronizedOutputStream direclty if needed")]] static Ptr
        New (Execution::InternallySynchronized internallySynchronized, const OutputStream<byte>::Ptr& src, Format format = Format::eUTF8);
        [[deprecated ("Since Stroka v3.0d1, just wrap in InternallySynchronizedOutputStream direclty if needed")]] static Ptr
        New (Execution::InternallySynchronized internallySynchronized, const OutputStream<Character>::Ptr& src);
        DISABLE_COMPILER_MSC_WARNING_END (4996); // DEPRECATED
        DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Wdeprecated-declarations\"");
        DISABLE_COMPILER_CLANG_WARNING_END ("clang diagnostic ignored \"-Wdeprecated-declarations\"");

    public:
        /**
         */
        nonvirtual TextWriter& operator= (const TextWriter&) = delete;

    private:
        class UnSeekable_CodeCvt_Rep_;

    private:
        template <Characters ::IUNICODECanUnambiguouslyConvertFrom OUTPUT_CHAR_T>
        class UnSeekable_UTFConverter_Rep_;
    };

    /**
     *  Ptr is a copyable smart pointer to a TextWriter stream.
     */
    class TextWriter::Ptr : public OutputStream<Character>::Ptr {
    private:
        using inherited = typename OutputStream<Character>::Ptr;

    public:
        /**
         *  \par Example Usage
         *      \code
         *          Streams::TextWriter::Ptr         textOut = Streams::TextWriter::New (out, UnicodeExternalEncodings::eUTF8, ByteOrderMark::eDoneInclude);
         *          textOut.Write (Characters::Format (L"%s\r\n", headLine.c_str ()));
         *          ...
         *      \endcode
         *
         *  \par Example Usage
         *      \code
         *          Streams::TextWriter::Ptr         textOut = Streams::TextWriter::New (binOut);
         *          textOut.Write ("Hello World\n");
         *      \endcode
         */
        Ptr ()                = default;
        Ptr (const Ptr& from) = default;
        Ptr (const OutputStream<Character>::Ptr& from);

    protected:
        Ptr (const shared_ptr<OutputStream<Character>::_IRep>& from);

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
