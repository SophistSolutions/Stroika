/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
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

    /*
     ********************************************************************************
     ***************************** NarrowSDKStringToWide ****************************
     ********************************************************************************
     */
    inline wstring NarrowSDKStringToWide (const string& s)
    {
        return NarrowStringToWide (s, GetDefaultSDKCodePage ());
    }

    /*
     ********************************************************************************
     ************************ WideStringToNarrowSDKString ***************************
     ********************************************************************************
     */
    inline string WideStringToNarrowSDKString (const wstring& ws)
    {
        return WideStringToNarrow (ws, GetDefaultSDKCodePage ());
    }

    /*
     ********************************************************************************
     ***************************** SDKString2NarrowSDK ******************************
     ********************************************************************************
     */
    inline string SDKString2NarrowSDK (const SDKString& s)
    {
#if qTargetPlatformSDKUseswchar_t
        return WideStringToNarrowSDKString (s);
#else
        return s;
#endif
#if 0
        if constexpr (qTargetPlatformSDKUseswchar_t) {
            return WideStringToNarrowSDKString (s);
        }
        else {
            return s;
        }
#endif
    }

    /*
     ********************************************************************************
     ******************************** NarrowSDK2SDKString ***************************
     ********************************************************************************
     */
    inline SDKString NarrowSDK2SDKString (const string& s)
    {
#if qTargetPlatformSDKUseswchar_t
        return NarrowSDKStringToWide (s);
#else
        return s;
#endif
#if 0
        constexpr bool x = qTargetPlatformSDKUseswchar_t;
        if constexpr (x) {
            return NarrowSDKStringToWide (s);
        }
        else {
            return s;
        }
#endif
    }

    /*
     ********************************************************************************
     *********************************** SDKString2Wide *****************************
     ********************************************************************************
     */
    inline wstring SDKString2Wide (const SDKString& s)
    {
#if qTargetPlatformSDKUseswchar_t
        return s;
#else
        return NarrowSDKStringToWide (s);
#endif
#if 0
        if constexpr (qTargetPlatformSDKUseswchar_t) {
            return s;
        }
        else {
            return NarrowSDKStringToWide (s);
        }
#endif
    }

    /*
     ********************************************************************************
     ********************************** Wide2SDKString ******************************
     ********************************************************************************
     */
    inline SDKString Wide2SDKString (const wstring& s)
    {
#if qTargetPlatformSDKUseswchar_t
        return s;
#else
        return WideStringToNarrowSDKString (s);
#endif
#if 0
        if constexpr (qTargetPlatformSDKUseswchar_t) {
            return s;
        }
        else {
            return WideStringToNarrowSDKString (s);
        }
#endif
    }

    /*
     ********************************************************************************
     *********************************** ToSDKString ********************************
     ********************************************************************************
     */
    inline SDKString ToSDKString (const string& s)
    {
        return NarrowSDK2SDKString (s);
    }
    inline SDKString ToSDKString (const wstring& s)
    {
        return Wide2SDKString (s);
    }

}

#endif /*_Stroika_Foundation_Characters_SDKString_inl_*/
