/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_String2Int_h_
#define _Stroika_Foundation_Characters_String2Int_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/String.h"

_DeprecatedFile_ ("DEPRECATED String2Int.h in v3.0d26 - use ParseNumber.h (ParseInteger)");

namespace Stroika::Foundation::Characters {

    template <integral T = int, IUNICODECodePoint CHAR_T>
    [[deprecated ("Since Stroika v3.0d26 use ParseInteger<T> (s) (ParseNumber.h) - .value_or (0) for this lenient behavior")]] T
    String2Int (span<const CHAR_T> s);
    template <integral T = int, IConvertibleToString STRINGISH_ARG>
    [[deprecated ("Since Stroika v3.0d26 use ParseInteger<T> (s) (ParseNumber.h) - .value_or (0) for this lenient behavior")]] T
    String2Int (STRINGISH_ARG&& s);

    [[deprecated ("Since Stroika v3.0d26 use ParseInteger<unsigned int> (s, {.fRadix = 16, .fAcceptRadixPrefix = true}) "
                  "(ParseNumber.h)")]] unsigned int
    HexString2Int (const String& s);

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "String2Int.inl"

#endif /*_Stroika_Foundation_Characters_String2Int_h_*/
