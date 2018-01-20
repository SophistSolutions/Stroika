/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_SDKString_inl_
#define _Stroika_Foundation_Characters_SDKString_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "CodePage.h"

namespace Stroika {
    namespace Foundation {
        namespace Characters {

            inline wstring NarrowSDKStringToWide (const string& s)
            {
                return NarrowStringToWide (s, GetDefaultSDKCodePage ());
            }
            inline string WideStringToNarrowSDKString (const wstring& ws)
            {
                return WideStringToNarrow (ws, GetDefaultSDKCodePage ());
            }

            inline string SDKString2NarrowSDK (const SDKString& s)
            {
#if qTargetPlatformSDKUseswchar_t
                return WideStringToNarrowSDKString (s);
#else
                return s;
#endif
            }
            inline SDKString NarrowSDK2SDKString (const string& s)
            {
#if qTargetPlatformSDKUseswchar_t
                return NarrowSDKStringToWide (s);
#else
                return s;
#endif
            }

            inline wstring SDKString2Wide (const SDKString& s)
            {
#if qTargetPlatformSDKUseswchar_t
                return s;
#else
                return NarrowSDKStringToWide (s);
#endif
            }
            inline SDKString Wide2SDKString (const wstring& s)
            {
#if qTargetPlatformSDKUseswchar_t
                return s;
#else
                return WideStringToNarrowSDKString (s);
#endif
            }
            inline SDKString ToSDKString (const string& s)
            {
                return NarrowSDK2SDKString (s);
            }
            inline SDKString ToSDKString (const wstring& s)
            {
                return Wide2SDKString (s);
            }
        }
    }
}
#endif /*_Stroika_Foundation_Characters_SDKString_inl_*/
