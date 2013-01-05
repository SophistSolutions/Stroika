/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_TString_inl_
#define _Stroika_Foundation_Characters_TString_inl_ 1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "CodePage.h"

namespace   Stroika {
    namespace   Foundation {
        namespace   Characters {




            inline  wstring NarrowSDKStringToWide (const string& s)
            {
                return NarrowStringToWide (s, GetDefaultSDKCodePage ());
            }
            inline  string  WideStringToNarrowSDKString (const wstring& ws)
            {
                return WideStringToNarrow (ws, GetDefaultSDKCodePage ());
            }


            inline  string  TString2NarrowSDK (const TString& s)
            {
#if     qTargetPlatformSDKUseswchar_t
                return WideStringToNarrowSDKString (s);
#else
                return s;
#endif
            }
            inline  TString NarrowSDK2TString (const string& s)
            {
#if     qTargetPlatformSDKUseswchar_t
                return NarrowSDKStringToWide (s);
#else
                return s;
#endif
            }



            inline  wstring TString2Wide (const TString& s)
            {
#if     qTargetPlatformSDKUseswchar_t
                return s;
#else
                return NarrowSDKStringToWide (s);
#endif
            }
            inline  TString Wide2TString (const wstring& s)
            {
#if     qTargetPlatformSDKUseswchar_t
                return s;
#else
                return WideStringToNarrowSDKString (s);
#endif
            }
            inline  TString ToTString (const string& s)
            {
                return NarrowSDK2TString (s);
            }
            inline  TString ToTString (const wstring& s)
            {
                return Wide2TString (s);
            }
        }
    }
}

#endif  /*_Stroika_Foundation_Characters_TString_inl_*/
