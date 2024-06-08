/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_TextWriter_h_
#define _Stroika_Foundation_Streams_TextWriter_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/CodeCvt.h"
#include "Stroika/Foundation/Characters/TextConvert.h"
#include "Stroika/Foundation/Streams/OutputStream.h"

/**
 *  \file
 */

namespace Stroika::Foundation::Streams {
    using Characters::ByteOrderMark;
    using Characters::Character;
    using Characters::UnicodeExternalEncodings;
}
namespace Stroika::Foundation::Streams::TextWriter {

    /**
     *  \brief Take some binary output stream, and make it look like an output stream of (UNICODE) characters (using New - argument - encoding).
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
     *  Ptr is a copyable smart pointer to a TextWriter stream.
     *
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
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety-For-Envelope-Plus-Must-Externally-Synchronize-Letter">C++-Standard-Thread-Safety-For-Envelope-Plus-Must-Externally-Synchronize-Letter</a>
     */
    using Ptr = OutputStream::Ptr<Character>;

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
    Ptr New (const OutputStream::Ptr<byte>& src, Characters::CodeCvt<>&& char2OutputConverter);
    Ptr New (const OutputStream::Ptr<byte>& src, UnicodeExternalEncodings e = UnicodeExternalEncodings::eDEFAULT,
             ByteOrderMark bom = ByteOrderMark::eDontInclude);
    Ptr New (const OutputStream::Ptr<Character>& src);
    template <typename... ARGS>
    Ptr New (Execution::InternallySynchronized internallySynchronized, ARGS... args);

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "TextWriter.inl"

#endif /*_Stroika_Foundation_Streams_TextWriter_h_*/
