/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#ifndef _Stroika_Foundation_Configuration_SDKString_h_
#define _Stroika_Foundation_Configuration_SDKString_h_ 1

#include "../StroikaPreComp.h"

#include <span>
#include <string>

#include "SDKChar.h"

/**
 *  TODO
 *      @todo CONSIDER directly documenting/supporting EUC - https://en.wikipedia.org/wiki/Extended_Unix_Code as alternate for non-unicode systems
 */

namespace Stroika::Foundation::Characters {

    /**
     *  This is the kind of String passed to most platform APIs.
     * 
     *  The easiest way to convert between a String and SDKString, is via the String class APIs:
     *  AsSDKString, AsNarrowSDKString, FromSDKString, FromNarrowSDKString.
     *
     *  For std::string (etc) interop, that works, but also @SDKString2Narrow and @Narrow2SDKString
     *
     *  @see SDKChar
     */
    using SDKString = basic_string<SDKChar>;

    /**
     *  SDKSTR is a macro to wrap constant string literals to get const SDKChar*
     */
#if qTargetPlatformSDKUseswchar_t
#define SDKSTR(x) L##x
#else
#define SDKSTR(x) x
#endif

    /**
     *  This flag ignores missing code points (when transforming from UNICODE to some character set that might not contain them),
     *  and does the best possible to map characters. Needed for things like translating a UNICODE error message to a locale{} 
     *  characterset which might not contain some of those UNICODE characters.
     */
    enum class AllowMissingCharacterErrorsFlag {
        eIgnoreErrors
    };

    /**
     *  On most platforms, this does nothing, but on Windows, it maps wstrings to string using code-page CP_ACP
     *  @todo add span<> overloads
     */
    string SDKString2Narrow (const SDKString& s);
    string SDKString2Narrow (const SDKString& s, AllowMissingCharacterErrorsFlag);

    /**
     *  On most platforms, this does nothing, but on Windows, it maps strings to wstring using code-page CP_ACP
     *  @todo add span<> overloads
     */
    SDKString Narrow2SDKString (span<const char> s);
    SDKString Narrow2SDKString (const string& s);

    /**
     */
    // @todo TONS OF CLENAUPS HERE - INCLUDING USING SPAN, and AVOIDING INTERMEDIATES NOT NEEDED...
    wstring NarrowSDKString2Wide (span<const char> s);
    wstring NarrowSDKString2Wide (const string& s);

    /**
     *  On Windows, this does nothing as SDKString==wstring, but on other platforms it follows the rules of SDKChar to map it to wstring.
     */
    wstring SDKString2Wide (span<const SDKChar> s);
    wstring SDKString2Wide (const SDKString& s);

    /**
     *  On Windows, this does nothing as SDKString==wstring, but on other platforms it follows the rules of SDKChar to map it from wstring.
     */
    SDKString WideString2SDK (span<const wchar_t> s);
    SDKString WideString2SDK (const wstring& s);

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "SDKString.inl"

#endif /*_Stroika_Foundation_Configuration_SDKString_h_*/
