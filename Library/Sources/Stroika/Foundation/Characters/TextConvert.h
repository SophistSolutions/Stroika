/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
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
 *      you have no, or limited knowlege of the code page. This contains logic to help with that case.
 * 
 *  @todo - this will replace the (probably to be deprecated) CodePage module - at least much/most of it.
 */

namespace Stroika::Foundation::Characters {

    using namespace std;

    /**
     *  \flag used to indicate if ByteOrderMark should be included (in other Stroika modules).
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
     *  Max size of span returned by GetByteOrderMark ()
     */
    constexpr size_t kMaxBOMSize = 3;

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
     *  \req into.size () >= SizeOfByteOrderMark (e)
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
