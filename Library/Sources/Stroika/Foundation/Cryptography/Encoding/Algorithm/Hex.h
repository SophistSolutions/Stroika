/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cryptography_Encoding_Algorithm_Hex_h_
#define _Stroika_Foundation_Cryptography_Encoding_Algorithm_Hex_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <span>
#include <string>
#include <string_view>

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Memory/BLOB.h"

/**
 *  \file
 *
 *  \note Code-Status:  <a href="Code-Status.md#Beta">Beta</a>
 */

/**
 *  Hex - base16 (RFC 4648, section 8): bytes as text, two hexadecimal digits a byte, and back.
 *
 *  Like Base64, VERY WEAK encryption (Cryptography's ReadMe.md): the bytes made opaque - to a parser, and to most readers -
 *  and kept intact through a channel that would mangle them; but secret from no one who recognizes it.
 *
 *  Memory::BLOB's AsHex and FromHex, and Cryptography::Format, use it.
 */
namespace Stroika::Foundation::Cryptography::Encoding::Algorithm::Hex {

    /**
     *  \brief The bytes as hex: two digits a byte, lower case - {0x29, 0x14} as "2914"
     *
     *  \par Example Usage
     *      \code
     *          EXPECT_EQ (Hex::Encode (BLOB{0x29, 0x14, 0x4a}), "29144a");
     *      \endcode
     */
    string Encode (span<const byte> from);
    string Encode (const Memory::BLOB& from); ///< \brief The bytes as hex: two digits a byte, lower case

    /**
     *  \brief Hex read back into bytes: digits of either case; spaces before a byte's two digits skipped (so "29 14 4a" too).
     *         Throws DataExchange::BadFormatException on a character not a hex digit, or a byte's second digit missing.
     *
     *  \par Example Usage
     *      \code
     *          EXPECT_EQ (Hex::Decode ("29144A"sv), (BLOB{0x29, 0x14, 0x4a}));
     *      \endcode
     */
    Memory::BLOB Decode (span<const char> s);
    Memory::BLOB Decode (const char* s);               ///< \brief Hex read back into bytes
    Memory::BLOB Decode (string_view s);               ///< \brief Hex read back into bytes
    Memory::BLOB Decode (const string& s);             ///< \brief Hex read back into bytes
    Memory::BLOB Decode (const Characters::String& s); ///< \brief Hex read back into bytes

}

#endif /*_Stroika_Foundation_Cryptography_Encoding_Algorithm_Hex_h_*/
