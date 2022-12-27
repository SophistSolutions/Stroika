/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_String2Int_h_
#define _Stroika_Foundation_Characters_String2Int_h_ 1

#include "../StroikaPreComp.h"

#include "String.h"

/**
 * TODO:
 * 
 *      @todo   SEE https://stroika.atlassian.net/browse/STK-768 - major refactor of this module
 * 
 *      @todo   Add Int2String () module? Like FloatConversion::ToString, and this String2Int?
 *
 *      @todo   DOCUMENT BEHAVIOR OF STRING2INT() for bad strings. What does it do?
 *              AND SIMILARPT FOR hexString2Int. And for both - probably rewrite to use strtoul/strtol etc
 *
 *      @todo   Same changes to HexString2Int() as we did with String2Int() - template on return value.
 *              Or maybe get rid of HexString2Int () - and just have optional radix param?
 *
 *      @todo   Consdier if we should have variants of these funtions taking a locale, or
 *              always using C/currnet locale. For the most part - I find it best to use the C locale.
 *              But DOCUMENT in all cases!!! And maybe implement variants...
 *
 */

namespace Stroika::Foundation::Characters {

    /**
     *  Convert the given decimal-format integral string to any integer type
     *  (e.g. signed char, unsigned short int, long long int, uint32_t etc).
     *
     *  String2Int will return 0 if no valid parse, and numeric_limits<T>::min on underflow,
     *  numeric_limits<T>::max on overflow.
     *      @todo consider if this is unwise - it seems we ought to throw? Or have a variant
     *              perhaps that does no throw?
     *
     *              CONSIDER!
     *
     *  @see strtoll(), or @see wcstoll (). This is a simple wrapper on strtoll() / wcstoll ().
     *  strtoll() is more flexible. This is merely meant to be an often convenient wrapper.
     *  Use strtoll etc directly to see if the string parsed properly.
     * 
     *  \req
     *      No leading or trailing whitespace in string argument (unlike strcoll/wstrcoll). (new requirement since Stroika 2.1b14)
     *
     *  \par Example Usage
     *      \code
     *          uint32_t    n1      =   String2Int<uint32_t> (L"33");
     *          int         n2      =   String2Int (L"33");
     *          int         n3      =   String2Int (L"33aaa");  // invalid parse, so returns zero!
     *      \endcode
     * 
     *  TODO:
     *      @todo MAYBE replace this with String2IntQuietly () - just always mapping to a default (and maybe make that default a parameter).
     *            and otherwise THROW on invalid.
     */
    template <typename T = int, Character_IsUnicodeCodePoint CHAR_T>
    T String2Int (span<const CHAR_T> s);
    template <typename T = int>
    T String2Int (const String& s);
    template <typename T = int, ConvertibleToString STRINGISH_ARG>
    T String2Int (STRINGISH_ARG&& s)
        requires (not is_same_v<remove_cvref_t<STRINGISH_ARG>, String>);
    template <typename T = int>
    [[deprecated ("Since Stroika v3.0d1, use span{} overload")]] T String2Int (const wchar_t* start, const wchar_t* end)
    {
        return String2Int (span<const wchar_t>{start, end});
    }

    /**
     *  Convert the given hex-format string to an unsigned integer.
     *  String2Int will return 0 if no valid parse, and UINT_MAX on overflow.
     *
     *  @see strtoul(), or @see wcstol (). This is a simple wrapper on strtoul() or wcstoul().
     *  strtoul() etc are more flexible. This is merely meant to be an often convenient wrapper.
     *  Use strtoul etc directly to see if the string parsed properly.
     */
    unsigned int HexString2Int (const String& s);

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "String2Int.inl"

#endif /*_Stroika_Foundation_Characters_String2Int_h_*/
