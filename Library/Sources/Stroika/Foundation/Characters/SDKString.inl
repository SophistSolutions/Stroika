/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_SDKString_inl_
#define _Stroika_Foundation_Characters_SDKString_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "../Containers/Common.h"

#include "CodePage.h"

namespace Stroika::Foundation::Execution {
    [[noreturn]] void ThrowSystemErrNo ();
    [[noreturn]] void ThrowSystemErrNo (int sysErr);
}

namespace Stroika::Foundation::Characters {

    /*
     ********************************************************************************
     ***************************** Characters::SDKString2Narrow *********************
     ********************************************************************************
     */
    inline string SDKString2Narrow (const SDKString& s)
    {
        if constexpr (same_as<SDKChar, char>) {
            return s;
        }
        else {
#if qPlatform_Windows
            static constexpr DWORD kFLAGS_ = 0; // WC_ERR_INVALID_CHARS doesn't work (https://learn.microsoft.com/en-us/windows/win32/api/stringapiset/nf-stringapiset-widechartomultibyte), so must use lpUsedDefaultChar
            int stringLength = ::WideCharToMultiByte (CP_ACP, kFLAGS_, s.c_str (), static_cast<int> (s.length ()), nullptr, 0, nullptr, nullptr);
            if (stringLength == 0 and s.length () != 0) {
                Execution::ThrowSystemErrNo ();
            }
            string result;
            result.resize (stringLength);
            BOOL usedDefaultChar{false};
            Verify (::WideCharToMultiByte (CP_ACP, kFLAGS_, s.c_str (), static_cast<int> (s.length ()), Containers::Start (result),
                                           stringLength, nullptr, &usedDefaultChar) == stringLength);
            if (usedDefaultChar) {
                Execution::ThrowSystemErrNo (ERROR_NO_UNICODE_TRANSLATION);
            }
            return result;
#else
            AssertNotImplemented ();
#endif
        }
    }
    inline string SDKString2Narrow (const SDKString& s, AllowMissingCharacterErrorsFlag)
    {
        if constexpr (same_as<SDKChar, char>) {
            return s;
        }
        else {
#if qPlatform_Windows
            static constexpr DWORD kFLAGS_ = 0;
            int stringLength = ::WideCharToMultiByte (CP_ACP, kFLAGS_, s.c_str (), static_cast<int> (s.length ()), nullptr, 0, nullptr, nullptr);
            if (stringLength == 0 and s.length () != 0) {
                Execution::ThrowSystemErrNo ();
            }
            string result;
            result.resize (stringLength);
            Verify (::WideCharToMultiByte (CP_ACP, kFLAGS_, s.c_str (), static_cast<int> (s.length ()), Containers::Start (result),
                                           stringLength, nullptr, nullptr) == stringLength);
            return result;
#else
            AssertNotImplemented ();
#endif
        }
    }

    /*
     ********************************************************************************
     ***************************** Characters::Narrow2SDKString *********************
     ********************************************************************************
     */
    inline SDKString Narrow2SDKString (const span<const char>& s)
    {
        if constexpr (same_as<SDKChar, char>) {
            return SDKString{s.data (), s.data () + s.size ()};
        }
        else {
#if qPlatform_Windows
            static constexpr DWORD kFLAGS_ = MB_ERR_INVALID_CHARS;
            int stringLength = ::MultiByteToWideChar (CP_ACP, kFLAGS_, s.data (), static_cast<int> (s.length ()), nullptr, 0);
            if (stringLength == 0 and s.length () != 0) {
                Execution::ThrowSystemErrNo ();
            }
            SDKString result;
            result.resize (stringLength);
            Verify (::MultiByteToWideChar (CP_ACP, kFLAGS_, s.data (), static_cast<int> (s.length ()), Containers::Start (result),
                                           stringLength) == stringLength);
            return result;
#else
            AssertNotImplemented ();
#endif
        }
    }
   inline SDKString Narrow2SDKString (const string& s)
    {
        if constexpr (same_as<SDKChar, char>) {
            return s;
        }
        else {
#if qPlatform_Windows
            static constexpr DWORD kFLAGS_ = MB_ERR_INVALID_CHARS;
            int stringLength = ::MultiByteToWideChar (CP_ACP, kFLAGS_, s.c_str (), static_cast<int> (s.length ()), nullptr, 0);
            if (stringLength == 0 and s.length () != 0) {
                Execution::ThrowSystemErrNo ();
            }
            SDKString result;
            result.resize (stringLength);
            Verify (::MultiByteToWideChar (CP_ACP, kFLAGS_, s.c_str (), static_cast<int> (s.length ()), Containers::Start (result),
                                           stringLength) == stringLength);
            return result;
#else
            AssertNotImplemented ();
#endif
        }
    }
    /*
     ********************************************************************************
     ******************************* Characters::SDKString2Wide *********************
     ********************************************************************************
     */
#if qPlatform_Windows
    inline wstring SDKString2Wide (const SDKString& s) { return s; }
     inline   wstring SDKString2Wide (const span<const SDKChar>& s) { return wstring{s}; }
#endif

    /*
     ********************************************************************************
     ******************************* Characters::WideString2SDK *********************
     ********************************************************************************
     */
#if qPlatform_Windows
    inline SDKString WideString2SDK (const wstring& s) { return s; }
    inline SDKString WideString2SDK (const span<const SDKChar>& s) { return SDKString{s}; }
#endif



    inline wstring NarrowSDKString2Wide (span<const char> s)
    {
        return SDKString2Wide (Narrow2SDKString (s));
    }
    inline wstring NarrowSDKString2Wide (const string& s)
    {
        return SDKString2Wide (Narrow2SDKString (s));
    }

    /// <summary>
    /// DEPRECATED BELOW...
    /// </summary>

    DISABLE_COMPILER_MSC_WARNING_START (4996);
    DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Wdeprecated-declarations\"");
    DISABLE_COMPILER_CLANG_WARNING_START ("clang diagnostic ignored \"-Wdeprecated-declarations\"");
    [[deprecated (
        "Since Stroika v3.0d1 use String::FromNarrowSDKString (s).As<wstring> () - less efficent but this is never used")]] inline wstring
    NarrowSDKStringToWide (const string& s)
    {
#ifdef CP_ACP
        Assert (CP_ACP == 0);
#else
        const unsigned char CP_ACP = 0;
#endif
        return NarrowStringToWide (s, CP_ACP);
    }
    [[deprecated ("Since Stroika v3.0d1 use String (s).AsNarrowSDKString () - less efficent but this is never used")]] inline string
    WideStringToNarrowSDKString (const wstring& ws)
    {
#ifdef CP_ACP
        Assert (CP_ACP == 0);
#else
        const unsigned char CP_ACP = 0;
#endif
        return WideStringToNarrow (ws, CP_ACP);
    }
    [[deprecated (
        "Since Stroika v3.0d1 use String::FromSDKString (s).AsNarrowSDKString () - less efficent but this is never used")]] inline string
    SDKString2NarrowSDK (const SDKString& s)
    {
#if qTargetPlatformSDKUseswchar_t
        return WideStringToNarrowSDKString (s);
#else
        return s;
#endif
    }
    [[deprecated (
        "Since Stroika v3.0d1 use String::FromNarrowSDKString (s).AsSDKString () - less efficent but this is never used")]] inline SDKString
    NarrowSDK2SDKString (const string& s)
    {
#if qTargetPlatformSDKUseswchar_t
        return NarrowSDKStringToWide (s);
#else
        return s;
#endif
    }
    [[deprecated ("Since Stroika v3.0d1 use String{s}.AsSDKString () - less efficent but this is never used")]] inline SDKString
    Wide2SDKString (const wstring& s)
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
