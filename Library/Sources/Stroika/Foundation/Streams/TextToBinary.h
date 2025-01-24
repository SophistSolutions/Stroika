/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2025.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_TextToBinary_h_
#define _Stroika_Foundation_Streams_TextToBinary_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/CodeCvt.h"
#include "Stroika/Foundation/Characters/TextConvert.h"
#include "Stroika/Foundation/Execution/Synchronized.h"
#include "Stroika/Foundation/Streams/OutputStream.h"

namespace Stroika::Foundation::Streams {
    using Characters::ByteOrderMark;
    using Characters::Character;
    using Characters::UnicodeExternalEncodings;
}

namespace Stroika::Foundation::Streams::TextToBinary {

    /**
     *  \brief Take some binary output stream, and make it look like an output stream of (UNICODE) characters (using New - argument - encoding).
     *
     *  Obviously todo this, there may be some character set mapping/conversion needed. The object
     *  takes constructor arguments to decide how this will he handled.
     *
     *  TextToBinary::Writer is not seekable. It's possible to implement, but complicated, and performance costly. Very unlikely
     *  to ever be useful.
     *
     *  \note   This API was called TextOutputStreamAdapter
     *
     *  \note   TextToBinary::Writer aggregates its owned sub-stream, so that a Close () on TextToBinary::Writer
     *          will Close that sub-stream.
     *
     *  Ptr is a copyable smart pointer to a TextToBinary::Writer stream.
     *
     *  \par Example Usage
     *      \code
     *          Streams::TextToBinary::Writer::Ptr         textOut = Streams::TextToBinary::Writer::New (out, UnicodeExternalEncodings::eUTF8, ByteOrderMark::eDoneInclude);
     *          textOut.Write ("{}\r\n"_f (headLine));
     *          ...
     *      \endcode
     *
     *  \par Example Usage
     *      \code
     *          Streams::TextToBinary::Writer::Ptr         textOut = Streams::TextToBinary::Writer::New (binOut);
     *          textOut.Write ("Hello World\n");
     *      \endcode
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety-For-Envelope-Plus-Must-Externally-Synchronize-Letter">C++-Standard-Thread-Safety-For-Envelope-Plus-Must-Externally-Synchronize-Letter</a>
     *
     * If TextToBinary::Writer given an OutStream<Bytes>, it maps the characters according to the given code page info (@todo improve so generic code page support).
     * If handled an OutputStream<Character> - it just passes through characters.
     *
     *  \par Example Usage
     *      \code
     *          Streams::TextToBinary::Writer::Ptr         textOut = Streams::TextToBinary::Writer::New (out, UnicodeExternalEncodings::eUTF8, ByteOrderMark::eInclude);
     *          textOut.Write ("{}\r\n"_f (headLine));
     *          ...
     *      \endcode
     *
     *  \par Example Usage
     *      \code
     *          Streams::TextToBinary::Writer::Ptr         textOut = Streams::TextToBinary::Writer::New (binOut);
     *          textOut.Write ("Hello World\n");
     *      \endcode
     */
    namespace Writer {

        /**
         *  \brief TextToBinary::Writer wrap some sink (typically a binary stream), and produce a text sink you can Write() text to
         */
        using Ptr = Streams::OutputStream::Ptr<Character>;

        /**
         */
        Ptr New (const Streams::OutputStream::Ptr<byte>& src, const Characters::CodeCvt<>& char2OutputConverter);
        Ptr New (const Streams::OutputStream::Ptr<byte>& src, UnicodeExternalEncodings e = UnicodeExternalEncodings::eDEFAULT,
                 ByteOrderMark bom = ByteOrderMark::eDontInclude);
        Ptr New (const Streams::OutputStream::Ptr<Character>& src);
        template <typename... ARGS>
        Ptr New (Execution::InternallySynchronized internallySynchronized, ARGS... args);

    }

    namespace Reader {

        /**
         *  \brief TextToBinary::Reader wrap some source of text, and produce an binary input stream you can read (converted) bytes from.
         */
        using Ptr = InputStream::Ptr<byte>;

        /**
         *  \brief Stream wrapper that takes an InputStream<Character> and transforms it into an
         *         InputStream<byte> (like TextToBinary::Writer does, but pull rather than push based).
         * 
         *  Draft implementation (not very performant, but doesn't seem used much and easy to tweak)
         * 
         * DOC CONNECTION TO TextToBinary::Writer and maybe share output/format flags?
         * 
         * @todo NOTE - this CURRENTLY HARDWIRES converting to UTF-8
         * 
         * @todo take optional CodeCvt argument, or things you would pass to CodeCvt (character coding) for what binary rep to create!
         * 
         *  WONT change this part of the API - just adding overloads, so OK to release as-is - 2023-07-10
         */
        Ptr New (const InputStream::Ptr<Character>& srcStream);
        Ptr New (const Traversal::Iterable<Character>& srcText);

    }

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "TextToBinary.inl"

#endif /*_Stroika_Foundation_Streams_TextToBinary_h_*/
