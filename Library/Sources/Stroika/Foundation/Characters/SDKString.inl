/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_SDKString_inl_
#define _Stroika_Foundation_Characters_SDKString_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#include "CodePage.h"

namespace Stroika::Foundation::Characters {

    DISABLE_COMPILER_MSC_WARNING_START (4996);
    DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Wdeprecated-declarations\"");
    DISABLE_COMPILER_CLANG_WARNING_START ("clang diagnostic ignored \"-Wdeprecated-declarations\"");
    [[deprecated ("Since Stroika v3.0d1 use String::FromNarrowSDKString (s).As<wstring> () - less efficent but this is never used")]] inline wstring NarrowSDKStringToWide (const string& s)
    {
        return NarrowStringToWide (s, GetDefaultSDKCodePage ());
    }
    [[deprecated ("Since Stroika v3.0d1 use String (s).AsNarrowSDKString () - less efficent but this is never used")]] inline string WideStringToNarrowSDKString (const wstring& ws)
    {
        return WideStringToNarrow (ws, GetDefaultSDKCodePage ());
    }
    [[deprecated ("Since Stroika v3.0d1 use String::FromSDKString (s).AsNarrowSDKString () - less efficent but this is never used")]] inline string SDKString2NarrowSDK (const SDKString& s)
    {
#if qTargetPlatformSDKUseswchar_t
        return WideStringToNarrowSDKString (s);
#else
        return s;
#endif
    }
    [[deprecated ("Since Stroika v3.0d1 use String::FromNarrowSDKString (s).AsSDKString () - less efficent but this is never used")]] inline SDKString NarrowSDK2SDKString (const string& s)
    {
#if qTargetPlatformSDKUseswchar_t
        return NarrowSDKStringToWide (s);
#else
        return s;
#endif
    }
    [[deprecated ("Since Stroika v3.0d1 use String::FromSDKString ().As<wstring> () - less efficent but this is never used")]] inline wstring SDKString2Wide (const SDKString& s)
    {
#if qTargetPlatformSDKUseswchar_t
        return s;
#else
        return NarrowSDKStringToWide (s);
#endif
    }
    [[deprecated ("Since Stroika v3.0d1 use String{s}.AsSDKString () - less efficent but this is never used")]] inline SDKString Wide2SDKString (const wstring& s)
    {
#if qTargetPlatformSDKUseswchar_t
        return s;
#else
        return WideStringToNarrowSDKString (s);
#endif
    }
    [[deprecated ("Since Stroika v3.0d1 - just use String::FromNarrowSDKString (s).AsSDKString ()")]] inline SDKString ToSDKString (const string& s)
    {
        return NarrowSDK2SDKString (s);
    }
    [[deprecated ("Since Stroika v3.0d1 - just use String(s).AsSDKString ()")]] inline SDKString ToSDKString (const wstring& s)
    {
        return Wide2SDKString (s);
    }
    DISABLE_COMPILER_MSC_WARNING_END (4996);
    DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Wdeprecated-declarations\"");
    DISABLE_COMPILER_CLANG_WARNING_END ("clang diagnostic ignored \"-Wdeprecated-declarations\"");

}

#endif /*_Stroika_Foundation_Characters_SDKString_inl_*/
