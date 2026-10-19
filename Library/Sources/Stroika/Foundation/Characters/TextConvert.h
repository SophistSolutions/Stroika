/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_TextConvert_h_
#define _Stroika_Foundation_Characters_TextConvert_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <optional>
#include <span>

#include "Stroika/Foundation/Characters/CodeCvt.h"

/**
 *  \file
 *      Wrappers and extensions to the CodeCvt/std::codecvt<> logic. Often when reading/writing files (or a series of bytes)
 *      you have no, or limited knowledge of the code page. This contains logic to help with that case.
 * 
 *  @todo - this will replace the (probably to be deprecated) CodePage module - at least much/most of it.
 */

namespace Stroika::Foundation::Characters {

    using namespace std;

    /**
     *  \brief Whether text written as bytes begins with a byte order mark (BOM) - U+FEFF, encoded, saying which Unicode
     *         encoding (and byte order) follows
     *
     *  Whether one helps depends on the FORMAT written, not on where the bytes go (file, memory, network) - which a writer
     *  rarely knows anyway:
     *      o   Harmful - JSON (RFC 8259: implementations "MUST NOT add a byte order mark" to networked JSON), XML in UTF-8 (it
     *          declares its own encoding, UTF-8 by default), anything sent with a charset (HTTP's Content-Type), and any format
     *          parsed by a reader that does not expect one - a small device's parser, a regular expression, a shell
     *          script's first line. Unicode itself: for UTF-8, "neither required nor recommended".
     *      o   Helpful - plain text that declares no encoding (.txt, CSV, INI), read by a program that otherwise assumes a
     *          legacy code page: Excel opening a UTF-8 CSV is the common case.
     *      o   Needed - UTF-16 and UTF-32: XML requires one there (XML 1.0, section 4.3.3), and other text has no other way to
     *          give its byte order.
     *
     *  So Stroika:
     *      o   writes none by default - each writer, TextToBinary::Writer included, takes eDontInclude unless told otherwise
     *      o   reads one where it is present (ReadByteOrderMark, BinaryToText::Reader), and without one, and with no encoding
     *          named, takes the text as UTF-8 (@see Streams::BinaryToText::ByteOrderMarkOr)
     */
    enum class ByteOrderMark {
        eInclude,
        eDontInclude
    };

    /**
     *  returns the byte order mark for the given unicode encoding. Size is always <= kMaxBOMSize
     */
    constexpr span<const byte> GetByteOrderMark (UnicodeExternalEncodings e) noexcept;

    /**
     *  Max size of span returned by GetByteOrderMark () - UTF-32's
     */
    constexpr size_t kMaxBOMSize = 4;

    /**
     *  returns guessed encoding, and number of bytes consumed. If 'd' doesn't contain
     *  BOM (possible cuz not large enuf) - returns nullopt
     * 
     *  Pass in any size span, but recommended to use size kMaxBOMSize (less and you may miss some, more and wont use extra data).
     * 
     *  \par Example Usage:
     *      \code
     *          span<const byte> from = argument;
     *          if (optional<tuple<UnicodeExternalEncodings, size_t>> o = ReadByteOrderMark (from)) {
     *              return make_tuple (Characters::CodeCvt<Character> (get<0> (*o)), get<1> (*o));
     *          }
     *          else {
     *              return make_tuple (Characters::CodeCvt<Character> (UnicodeExternalEncodings::eDEFAULT), 0);
     *          }
     *      \endcode
     */
    constexpr optional<tuple<UnicodeExternalEncodings, size_t>> ReadByteOrderMark (span<const byte> d) noexcept;

    /**
     *  \pre into.size () >= SizeOfByteOrderMark (e)
     * 
     *  returns remaining span to write into (basically just into.subspan(SizeOfByteOrderMark (e))
     *  so caller can continue writing
     */
    span<byte> WriteByteOrderMark (UnicodeExternalEncodings e, span<byte> into);

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "TextConvert.inl"

#endif /*_Stroika_Foundation_Characters_TextConvert_h_*/
