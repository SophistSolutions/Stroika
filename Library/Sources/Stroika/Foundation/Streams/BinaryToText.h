/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_BinaryToText_h_
#define _Stroika_Foundation_Streams_BinaryToText_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <optional>

#include "Stroika/Foundation/Characters/CodeCvt.h"
#include "Stroika/Foundation/Characters/UTFConvert.h"
#include "Stroika/Foundation/Execution/Synchronized.h"
#include "Stroika/Foundation/Streams/InputStream.h"

/**
 *  \file
 *
 *  \note Code-Status:  <a href="Code-Status.md#Beta">Beta</a>
 */

namespace Stroika::Foundation::Memory {
    class BLOB;
}
namespace Stroika::Foundation::Streams {
    using Characters::Character;
}

namespace Stroika::Foundation::Streams::BinaryToText {

    /**
     *  \brief Decode by the text's byte order mark where it has one, and otherwise with fIfNone - as against a CodeCvt alone,
     *         which decodes with exactly that, byte order mark or not
     *
     *  Given neither, Reader::New and Convert decode by the byte order mark, and otherwise as UTF-8 - as Stroika writes text
     *  (@see Characters::ByteOrderMark).
     *
     *  \par Example Usage
     *      \code
     *          // a text file a Windows program may have written: as its byte order mark says, else in the ANSI code page
     *          String text = BinaryToText::Reader::New (FileInputStream::New (path), ByteOrderMarkOr{CodeCvt<>::PlatformDefault ()}).ReadAll ();
     *      \endcode
     */
    struct ByteOrderMarkOr {
        Characters::CodeCvt<> fIfNone;
    };

    /**
     *  How text was decoded with no CodeCvt named - @see ByteOrderMarkOr, and Reader::New (src) for the byte order mark, else UTF-8.
     */
    enum class [[deprecated ("Since Stroika v3.0d25 - use Reader::New (src) or Convert (src) - a byte order mark, else UTF-8 - or "
                             "ByteOrderMarkOr{codeCvt}")]] AutomaticCodeCvtFlags {
        eReadBOMAndIfNotPresentUseUTF8,
        eReadBOMAndIfNotPresentUseCurrentLocale,

        eDEFAULT = eReadBOMAndIfNotPresentUseUTF8
    };
    DISABLE_COMPILER_MSC_WARNING_START (4996)
    DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Wdeprecated-declarations\"");
    DISABLE_COMPILER_CLANG_WARNING_START ("clang diagnostic ignored \"-Wdeprecated-declarations\"");
    using AutomaticCodeCvtFlags::eReadBOMAndIfNotPresentUseCurrentLocale;
    using AutomaticCodeCvtFlags::eReadBOMAndIfNotPresentUseUTF8;
    DISABLE_COMPILER_MSC_WARNING_END (4996)
    DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Wdeprecated-declarations\"");
    DISABLE_COMPILER_CLANG_WARNING_END ("clang diagnostic ignored \"-Wdeprecated-declarations\"");

    /**
     *  \brief BinaryToText::Reader::Ptr is an InputStream::Ptr<Character>, usually constructed wrapping some binary object or binary stream
     *
     *  \note   This was called TextInputStreamBinaryAdapter
     *  \note   This was called TextReader
     *
     *  \note   This is similar to the .net TextReader (https://msdn.microsoft.com/en-us/library/system.io.textreader(v=vs.110).aspx) except that
     *          much of the 'reading' API is baked into InputStream::Ptr<Character>.
     *
     *  \note   BinaryToText::Reader's are smart about not reading more than they need to from the source Stream (unless you make that stream buffered, in
     *          which case the buffering can cause it to read ahead)
     *
     *          But BinaryToText::Reader itself doesn't read ahead more than it needs to to complete requested methods.
     *
     *  \par Example Usage
     *      \code
     *          for (String line : BinaryToText::Reader::New (FileInputStream::New ("/tmp/foo")).ReadLines ()) {
     *          }
     *      \endcode
     *
     *  \par Example Usage
     *      \code
     *          Assert (BinaryToText::Reader::New (String{"hello world"}).ReadAll () == "hello world");
     *      \endcode
     *
     *  \note   Reading improperly encoded text may result in a RuntimeException indicating improperly encoded characters.
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety-For-Envelope-Plus-Must-Externally-Synchronize-Letter">C++-Standard-Thread-Safety-For-Envelope-Plus-Must-Externally-Synchronize-Letter</a>
     *
     *  Seekable defaults to the same value as that of the underlying stream wrapped.
     *  For the constructor taking const InputStream::Ptr<Character>& src, the seekability mimics that of the original source.
     *  Constructors taking a BLOB, the resulting stream will be seekable..
     *
     *  But when you specify it explicitly, the given value will be used.
     *
     *  \note Depending on the underlying source (e.g. binary stream) - maintaining seekability may be expensive in terms
     *        of memory usage.
     */
    namespace Reader {

        /**
         *  \brief BinaryToText::Readers produce text in the form of an InputStream of 'Character' objects (so you might get the text with ReadAll()).
         */
        using Ptr = InputStream::Ptr<Character>;

        /**
         * This flag controls whether the (BinaryToText::Reader) TextReader instance will try to read-ahead (typically in order to cache). This is generally
         * a good thing, but for some stream uses, its important to not read ahead - e.g. in the underlying binary stream contains multiple objects and we
         * are just reading one).
         *
         *  \note eReadAheadAllowed doesn't mean the underlying class actually WILL read ahead, only that it is permitted to.
         */
        enum class ReadAhead {
            eReadAheadNever,
            eReadAheadAllowed,
        };
        using ReadAhead::eReadAheadAllowed;
        using ReadAhead::eReadAheadNever;

        /**
         *  \brief Create an InputStream::Ptr<Character> from the arguments (usually binary source) - which can be used to Read out the text as a string
         *
         *  \par Example Usage
         *      \code
         *          for (String line : BinaryToText::Reader::New (FileInputStream::New (kProcCPUInfoFileName_, FileInputStream::eNotSeekable)).ReadLines ()) {
         *              DbgTrace ("***in Common::GetSystemConfiguration_CPU capture_ line={}"_f, line);
         *          }
         *      \endcode
         *  
         *  \note New (const InputStream::Ptr<byte>& src,... overloads)
         *      o   Seekability
         *              o   if not specified, its copied from the src binary stream.
         *              o   New (const Traversal::Iterable<Character>& src) produces a seekable stream
         *      o   Decoding
         *              o   given a CodeCvt, with exactly that
         *              o   otherwise by its byte order mark, where src.IsSeekable () and it has one (a stream that cannot seek
         *                  is not looked at for one), and failing that with ByteOrderMarkOr's fIfNone, or as UTF-8
         */
        Ptr New (const InputStream::Ptr<byte>& src, optional<SeekableFlag> seekable = {}, ReadAhead readAhead = eReadAheadAllowed);
        Ptr New (const InputStream::Ptr<byte>& src, const ByteOrderMarkOr& ifNoByteOrderMark, optional<SeekableFlag> seekable = {},
                 ReadAhead readAhead = eReadAheadAllowed); ///< \brief ... by its byte order mark, else with ifNoByteOrderMark.fIfNone
        Ptr New (const InputStream::Ptr<byte>& src, const Characters::CodeCvt<>& codeConverter, optional<SeekableFlag> seekable = {},
                 ReadAhead readAhead = eReadAheadAllowed);   ///< \brief ... with exactly codeConverter
        Ptr New (const InputStream::Ptr<Character>& src);    ///< \brief ... text already
        Ptr New (const Traversal::Iterable<Character>& src); ///< \brief ... text already
        template <typename... ARGS>
        Ptr New (Execution::InternallySynchronized internallySynchronized, ARGS... args); ///< \brief ... internally synchronized
        DISABLE_COMPILER_MSC_WARNING_START (4996)
        DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Wdeprecated-declarations\"");
        DISABLE_COMPILER_CLANG_WARNING_START ("clang diagnostic ignored \"-Wdeprecated-declarations\"");
        [[deprecated ("Since Stroika v3.0d25 - use New (src, seekable, readAhead) - a byte order mark, else UTF-8 - or New (src, "
                      "ByteOrderMarkOr{codeCvt}, ...)")]] Ptr
        New (const InputStream::Ptr<byte>& src, optional<AutomaticCodeCvtFlags> codeCvtFlags, optional<SeekableFlag> seekable = {},
             ReadAhead readAhead = eReadAheadAllowed);
        DISABLE_COMPILER_MSC_WARNING_END (4996)
        DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Wdeprecated-declarations\"");
        DISABLE_COMPILER_CLANG_WARNING_END ("clang diagnostic ignored \"-Wdeprecated-declarations\"");

    }

    // note - @todo COULD do this as an OutStream::Ptr/Writer - but never found a need/use
    namespace Writer {
    }

    /**
     *  \brief convert BLOB to a String - decoded as Reader::New decodes: by its byte order mark, else as UTF-8 (or with
     *         ifNoByteOrderMark's), or with exactly codeConverter
     *
     *  \note shorthand for BinaryToText::Reader::New (src, ...).ReadAll ())
     *
     *  \note could have done with stream, but the API names suggest side-effect free, and that wouldn't be,
     *        since it seeks the src input stream.
     */
    Characters::String Convert (const Memory::BLOB& src);
    Characters::String Convert (const Memory::BLOB& src, const ByteOrderMarkOr& ifNoByteOrderMark);   ///< \brief convert BLOB to a String
    Characters::String Convert (const Memory::BLOB& src, const Characters::CodeCvt<>& codeConverter); ///< \brief convert BLOB to a String
    DISABLE_COMPILER_MSC_WARNING_START (4996)
    DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Wdeprecated-declarations\"");
    DISABLE_COMPILER_CLANG_WARNING_START ("clang diagnostic ignored \"-Wdeprecated-declarations\"");
    [[deprecated ("Since Stroika v3.0d25 - use Convert (src) - a byte order mark, else UTF-8 - or Convert (src, "
                  "ByteOrderMarkOr{codeCvt})")]] Characters::String
    Convert (const Memory::BLOB& src, optional<AutomaticCodeCvtFlags> codeCvtFlags);
    DISABLE_COMPILER_MSC_WARNING_END (4996)
    DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Wdeprecated-declarations\"");
    DISABLE_COMPILER_CLANG_WARNING_END ("clang diagnostic ignored \"-Wdeprecated-declarations\"");

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "BinaryToText.inl"

#endif /*_Stroika_Foundation_Streams_BinaryToText_h_*/
