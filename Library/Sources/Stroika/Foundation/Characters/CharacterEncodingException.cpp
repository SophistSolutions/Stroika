/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/Format.h"
#include "Stroika/Foundation/Characters/StringBuilder.h"

#include "CharacterEncodingException.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;

/*
 ********************************************************************************
 ********************** Characters::CharacterEncodingException ******************
 ********************************************************************************
 */
CharacterEncodingException::CharacterEncodingException (EncodingOrDecoding encodingOrDecoding, optional<size_t> atSourceOffset,
                                                        const optional<String>& encoding)
    : inherited{[=] () {
        StringBuilder sb;
        sb << (encodingOrDecoding == eEncoding ? "Encoding Exception: cannot construct code point for character"sv
                                               : "Decoding Exception: cannot decode bytes to character"sv);
        if (atSourceOffset) {
            sb << " at source offset "sv << *atSourceOffset;
        }
        if (encoding) {
            sb << " using "sv << *encoding << " encoding"sv;
        }
        sb << "."sv;
        return sb;
    }()}
    , fEncodingOrDecoding_{encodingOrDecoding}
    , fAtSourceOffset_{atSourceOffset}
    , fEncoding_{encoding}
{
}
