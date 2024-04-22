/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_SDKString_inl_
#define _Stroika_Foundation_Characters_SDKString_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Stroika/Foundation/Characters/CodePage.h"
#include "Stroika/Foundation/Containers/Common.h"

namespace Stroika::Foundation::Execution {
    [[noreturn]] void ThrowSystemErrNo ();
    [[noreturn]] void ThrowSystemErrNo (int sysErr);
}

namespace Stroika::Foundation::Characters {

    /*
     ********************************************************************************
     ***************************** Characters::Narrow2SDK ***************************
     ********************************************************************************
     */
    inline SDKString Narrow2SDK (span<const char> s)
    {
#if qTargetPlatformSDKUseswchar_t
#if qPlatform_Windows
        static constexpr DWORD kFLAGS_      = MB_ERR_INVALID_CHARS;
        int                    stringLength = ::MultiByteToWideChar (CP_ACP, kFLAGS_, s.data (), static_cast<int> (s.size ()), nullptr, 0);
        if (stringLength == 0 and s.size () != 0) {
            Execution::ThrowSystemErrNo ();
        }
        SDKString result;
        result.resize (stringLength);
        Verify (::MultiByteToWideChar (CP_ACP, kFLAGS_, s.data (), static_cast<int> (s.size ()), Containers::Start (result), stringLength) == stringLength);
        return result;
#else
        AssertNotImplemented (); // nobody but windows uses wchar_t as far as I know
#endif
#else
        return SDKString{s.begin (), s.end ()};
#endif
    }
    inline SDKString Narrow2SDK (const string& s)
    {
#if qTargetPlatformSDKUseswchar_t
        return Narrow2SDK (span{s}); // delegate work
#else
        return s; // short-circuit so optimizer opportunity
#endif
    }
    inline SDKString Narrow2SDK (span<const char> s, AllowMissingCharacterErrorsFlag)
    {
#if qTargetPlatformSDKUseswchar_t
#if qPlatform_Windows
        static constexpr DWORD kFLAGS_      = 0;
        int                    stringLength = ::MultiByteToWideChar (CP_ACP, kFLAGS_, s.data (), static_cast<int> (s.size ()), nullptr, 0);
        if (stringLength == 0 and s.size () != 0) {
            Execution::ThrowSystemErrNo ();
        }
        SDKString result;
        result.resize (stringLength);
        Verify (::MultiByteToWideChar (CP_ACP, kFLAGS_, s.data (), static_cast<int> (s.size ()), Containers::Start (result), stringLength) == stringLength);
        return result;
#else
        AssertNotImplemented (); // nobody but windows uses wchar_t as far as I know
#endif
#else
        return SDKString{s.begin (), s.end ()};
#endif
    }
    inline SDKString Narrow2SDK (const string& s, [[maybe_unused]] AllowMissingCharacterErrorsFlag allow)
    {
#if qTargetPlatformSDKUseswchar_t
        return Narrow2SDK (span{s}, allow); // delegate work
#else
        return s; // short-circuit so optimizer opportunity
#endif
    }

    /*
     ********************************************************************************
     **************************** Characters::NarrowSDK2Wide ************************
     ********************************************************************************
     */
    inline wstring NarrowSDK2Wide (span<const char> s)
    {
        // No need to special case qTargetPlatformSDKUseswchar_t cuz SDK2Wide is a no-op inlinable in that case
        return SDK2Wide (Narrow2SDK (s));
    }
    inline wstring NarrowSDK2Wide (const string& s)
    {
        return NarrowSDK2Wide (span{s});
    }
    inline wstring NarrowSDK2Wide (span<const char> s, AllowMissingCharacterErrorsFlag allow)
    {
        // No need to special case qTargetPlatformSDKUseswchar_t cuz SDK2Wide is a no-op inlinable in that case
        return SDK2Wide (Narrow2SDK (s, allow), allow);
    }
    inline wstring NarrowSDK2Wide (const string& s, AllowMissingCharacterErrorsFlag allow)
    {
        return NarrowSDK2Wide (span{s}, allow);
    }

    /*
     ********************************************************************************
     ***************************** Characters::SDK2Narrow ***************************
     ********************************************************************************
     */
    inline string SDK2Narrow (span<const SDKChar> s)
    {
#if qTargetPlatformSDKUseswchar_t
#if qPlatform_Windows
        static constexpr DWORD kFLAGS_ = 0; // WC_ERR_INVALID_CHARS doesn't work (https://learn.microsoft.com/en-us/windows/win32/api/stringapiset/nf-stringapiset-widechartomultibyte), so must use lpUsedDefaultChar
        int stringLength = ::WideCharToMultiByte (CP_ACP, kFLAGS_, s.data (), static_cast<int> (s.size ()), nullptr, 0, nullptr, nullptr);
        if (stringLength == 0 and s.size () != 0) {
            Execution::ThrowSystemErrNo ();
        }
        string result;
        result.resize (stringLength);
        BOOL usedDefaultChar{false};
        Verify (::WideCharToMultiByte (CP_ACP, kFLAGS_, s.data (), static_cast<int> (s.size ()), Containers::Start (result), stringLength,
                                       nullptr, &usedDefaultChar) == stringLength);
        if (usedDefaultChar) {
            Execution::ThrowSystemErrNo (ERROR_NO_UNICODE_TRANSLATION);
        }
        return result;
#else
        AssertNotImplemented (); // nobody but windows uses wchar_t as far as I know
#endif
#else
        return string{s.begin (), s.end ()};
#endif
    }
    inline string SDK2Narrow (const SDKString& s)
    {
#if qTargetPlatformSDKUseswchar_t
        return SDK2Narrow (span{s}); // delegate work
#else
        return s; // short-circuit so optimizer opportunity
#endif
    }
    inline string SDK2Narrow (span<const SDKChar> s, AllowMissingCharacterErrorsFlag)
    {
#if qTargetPlatformSDKUseswchar_t
#if qPlatform_Windows
        static constexpr DWORD kFLAGS_ = 0; // NOTE NOT specifying WC_ERR_INVALID_CHARS so map bad/missing UNICODE characters to some system default char
        int stringLength = ::WideCharToMultiByte (CP_ACP, kFLAGS_, s.data (), static_cast<int> (s.size ()), nullptr, 0, nullptr, nullptr);
        if (stringLength == 0 and s.size () != 0) {
            Execution::ThrowSystemErrNo ();
        }
        string result;
        result.resize (stringLength);
        Verify (::WideCharToMultiByte (CP_ACP, kFLAGS_, s.data (), static_cast<int> (s.size ()), Containers::Start (result), stringLength,
                                       nullptr, nullptr) == stringLength);
        return result;
#else
        AssertNotImplemented ();
#endif
#else
        return string{s.begin (), s.end ()};
#endif
    }
    inline string SDK2Narrow (const SDKString& s, [[maybe_unused]] AllowMissingCharacterErrorsFlag allowMissing)
    {
#if qTargetPlatformSDKUseswchar_t
        return SDK2Narrow (span{s}, allowMissing); // delegate work
#else
        return s; // short-circuit so optimizer opportunity
#endif
    }

    /*
     ********************************************************************************
     ******************************* Characters::SDK2Wide ***************************
     ********************************************************************************
     */
#if qTargetPlatformSDKUseswchar_t
    inline wstring SDK2Wide (span<const SDKChar> s)
    {
        return wstring{s.begin (), s.end ()};
    }
#endif
    inline wstring SDK2Wide (const SDKString& s)
    {
#if qTargetPlatformSDKUseswchar_t
        return s; // short-circuit so optimizer opportunity
#else
        return SDK2Wide (span{s}); // delegate work
#endif
    }
#if qTargetPlatformSDKUseswchar_t
    inline wstring SDK2Wide (span<const SDKChar> s, AllowMissingCharacterErrorsFlag)
    {
        return wstring{s.begin (), s.end ()};
    }
#endif
    inline wstring SDK2Wide (const SDKString& s, [[maybe_unused]] AllowMissingCharacterErrorsFlag allow)
    {
#if qTargetPlatformSDKUseswchar_t
        return s; // short-circuit so optimizer opportunity
#else
        return SDK2Wide (span{s}, allow); // delegate work
#endif
    }

    /*
     ********************************************************************************
     ******************************* Characters::Wide2SDK ***************************
     ********************************************************************************
     */
#if qTargetPlatformSDKUseswchar_t
    inline SDKString Wide2SDK (span<const SDKChar> s)
    {
        return SDKString{s.begin (), s.end ()};
    }
#endif
    inline SDKString Wide2SDK (const wstring& s)
    {
#if qTargetPlatformSDKUseswchar_t
        return s; // short-circuit so optimizer opportunity
#else
        return Wide2SDK (span{s});
#endif
    }
#if qTargetPlatformSDKUseswchar_t
    inline SDKString Wide2SDK (span<const SDKChar> s, AllowMissingCharacterErrorsFlag)
    {
        return SDKString{s.begin (), s.end ()};
    }
#endif
    inline SDKString Wide2SDK (const wstring& s, [[maybe_unused]] AllowMissingCharacterErrorsFlag allow)
    {
#if qTargetPlatformSDKUseswchar_t
        return s; // short-circuit so optimizer opportunity
#else
        return Wide2SDK (span{s}, allow);
#endif
    }

    /// <summary>
    //////////////////////////// DEPRECATED BELOW.../////////////////////////////
    /// </summary>

    DISABLE_COMPILER_MSC_WARNING_START (4996);
    DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Wdeprecated-declarations\"");
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

}

#endif /*_Stroika_Foundation_Characters_SDKString_inl_*/
