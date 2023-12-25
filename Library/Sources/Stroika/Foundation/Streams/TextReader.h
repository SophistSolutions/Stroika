/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_TextReader_h_
#define _Stroika_Foundation_Streams_TextReader_h_ 1

#include "../StroikaPreComp.h"

#include <optional>

#include "../Characters/CodeCvt.h"
#include "../Characters/UTFConvert.h"

#include "InputStream.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Late</a>
 *
 *      @todo   https://stroika.atlassian.net/browse/STK-588 - TextReader should be seekable (maybe just optionally)
 *              INCOMPLETE IMPLEMENTATION - API probably OK.
 *
 *      @todo   https://stroika.atlassian.net/browse/STK-487 - TextReader should have CTOR taking String as argument (so can be seekable)
 *
 *      @todo   DOCUMENT why we put ReadLine etc in InputStreamPtr, instead of here. Gist of why - though more
 *              logical here - requires no state - and so more flexible there. May reconsider.
 *              -- LGP 2015-07-06
 *
 *      @todo   https://stroika.atlassian.net/browse/STK-611 - some cases of Execution::InternallySynchronized are AssertNotImplemented on TextReader and TextWriter
 */

namespace Stroika::Foundation::Memory {
    class BLOB;
}

namespace Stroika::Foundation::Streams {

    using Characters::Character;
}

namespace Stroika::Foundation::Streams::TextReader {
    /**
     *  \brief TextReader is an InputStream of Character, usually constructted wrapping some binary object or binary stream
     *
     *  \note   This was called TextInputStreamBinaryAdapter
     *
     *  \note   This is similar to the .net TextReader (https://msdn.microsoft.com/en-us/library/system.io.textreader(v=vs.110).aspx) except that
     *          much of the 'reading' API is baked into InputStream::Ptr<Character>.
     *
     *  \note   TextReader's are smart about not reading more than they need to from the source Stream (unless you make that stream buffered, in
     *          which case the buffering can cause it to read ahead)
     *
     *          But TextReader itself doesn't read ahead more than it needs to to complete requested methods.
     *
     *  \par Example Usage
     *      \code
     *          for (String line : TextReader::New (FileInputStream::New ("/tmp/foo")).ReadLines ()) {
     *          }
     *      \endcode
     *
     *  \par Example Usage
     *      \code
     *          Assert (TextReader::New (String{"hello world"}).ReadAll () == "hello world");
     *      \endcode
     *
     *  \note   Reading improperly encoded text may result in a RuntimeException indicating improperly encoded characters.
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety-For-Envelope-Plus-Must-Externally-Synchronize-Letter">C++-Standard-Thread-Safety-For-Envelope-Plus-Must-Externally-Synchronize-Letter</a>
     */
    using Ptr = InputStream::Ptr<Character>;

    /**
        * This flag controls whether the TextReader instance will try to read-ahead (typically in order to cache). This is generally
        * a good thing, but for some stream uses, its important to not read ahead - e.g. in the underlying binary stream contains multiple objects and we
        * are just reading one).
        *
        *  \note eReadAheadAllowed doesn't mean the underlying class actually WILL read ahead, only that it is permitted to.
        */
    enum class ReadAhead {
        eReadAheadNever,
        eReadAheadAllowed,
    };
    static constexpr ReadAhead eReadAheadNever   = ReadAhead::eReadAheadNever;
    static constexpr ReadAhead eReadAheadAllowed = ReadAhead::eReadAheadAllowed;

    /**
     * @todo DOCUMENT - NEED EXAMPLE - WHY???
     */
    static const Characters::UTFCodeConverter<Character> kDefaultUTFCoodeCoverter;

    /**
     */
    enum class AutomaticCodeCvtFlags {
        eReadBOMAndIfNotPresentUseUTF8,
        eReadBOMAndIfNotPresentUseCurrentLocale,

        eDEFAULT = eReadBOMAndIfNotPresentUseCurrentLocale
    };
    static constexpr AutomaticCodeCvtFlags eReadBOMAndIfNotPresentUseUTF8 = AutomaticCodeCvtFlags::eReadBOMAndIfNotPresentUseUTF8;
    static constexpr AutomaticCodeCvtFlags eReadBOMAndIfNotPresentUseCurrentLocale = AutomaticCodeCvtFlags::eReadBOMAndIfNotPresentUseCurrentLocale;

    /**
     *  Seekable defaults to the same value as that of the underlying stream wrapped.
     *  For the constructor taking const InputStream::Ptr<Character>& src, the seekability mimics that of the original source.
     *  Constructors taking a BLOB, the resulting stream will be seekable..
     *
     *  But when you specify it explicitly, the given value will be used.
     *
     *  \note Depending on the underlying source (e.g. binary stream) - maintaining seekability may be expensive in terms
     *        of memory usage.
     *
     *  \par Example Usage
     *      \code
     *          for (String line : TextReader::New (FileInputStream::New (kProcCPUInfoFileName_, FileInputStream::eNotSeekable)).ReadLines ()) {
     *              DbgTrace (L"***in Configuration::GetSystemConfiguration_CPU capture_ line=%s", line.c_str ());
     *          }
     *      \endcode
     */
    Ptr New (const Memory::BLOB& src, AutomaticCodeCvtFlags codeCvtFlags = AutomaticCodeCvtFlags::eDEFAULT);
    Ptr New (const Memory::BLOB& src, const Characters::CodeCvt<>& codeConverter);
    Ptr New (const InputStream::Ptr<byte>& src);
    Ptr New (const InputStream::Ptr<byte>& src, SeekableFlag seekable, ReadAhead readAhead = eReadAheadAllowed);
    Ptr New (const InputStream::Ptr<byte>& src, AutomaticCodeCvtFlags codeCvtFlags, ReadAhead readAhead = eReadAheadAllowed);
    Ptr New (const InputStream::Ptr<byte>& src, const Characters::CodeCvt<>& codeConverter, SeekableFlag seekable = SeekableFlag::eSeekable,
             ReadAhead readAhead = eReadAheadAllowed);
    Ptr New (const InputStream::Ptr<Character>& src);
    Ptr New (const Traversal::Iterable<Character>& src);

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "TextReader.inl"

#endif /*_Stroika_Foundation_Streams_TextReader_h_*/
