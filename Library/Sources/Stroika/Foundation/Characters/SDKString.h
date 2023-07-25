/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#ifndef _Stroika_Foundation_Configuration_SDKString_h_
#define _Stroika_Foundation_Configuration_SDKString_h_ 1

#include "../StroikaPreComp.h"

#include <string>

#include "SDKChar.h"

/**
 *
 */

namespace Stroika::Foundation::Characters {

    /**
     *  This is the kind of String passed to most platform APIs.
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
     *  On most platforms, this does nothing, but on Windows, it maps wstrings to string using code-page CP_ACP
     *  @todo add span<> overloads
     */
    string SDKString2Narrow (const SDKString& s);

    /**
     *  On most platforms, this does nothing, but on Windows, it maps strings to wstring using code-page CP_ACP
     *  @todo add span<> overloads
     */
    SDKString Narrow2SDKString (const string& s);

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "SDKString.inl"

#endif /*_Stroika_Foundation_Configuration_SDKString_h_*/
