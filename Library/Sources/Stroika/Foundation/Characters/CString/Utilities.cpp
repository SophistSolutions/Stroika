/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include <cstdarg>
DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Wformat-truncation\""); // for g++-13 g++-release-sanitize_address_undefined x86_64-linux-gnu/bits/stdio2.h:68:36: warning: null format string [-Wformat-truncation=]
#include <cstdio>
DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Wformat-truncation\"");
#include <iomanip>
#include <sstream>

#if qCompilerAndStdLib_vswprintf_errantDependencyOnLocale_Buggy
#include <clocale>
#include <xlocale.h>
#endif

#if qCompilerAndStdLib_vswprintf_errantDependencyOnLocale_Buggy
#include "Stroika/Foundation/Execution/Finally.h"
#endif
#include "Stroika/Foundation/Characters/CodePage.h"
#include "Stroika/Foundation/Math/Common.h"
#include "Stroika/Foundation/Memory/StackBuffer.h"

#include "Utilities.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Characters::CString;

/*
 ********************************************************************************
 ************************* Characters::CString::FormatV *************************
 ********************************************************************************
 */
string Characters::CString::FormatV (const char* format, va_list argsList)
{
    RequireNotNull (format);
    Memory::StackBuffer<char, 10 * 1024> msgBuf{Memory::eUninitialized, 10 * 1024};
    // SUBTLE: va_list looks like it is passed by value, but its not really,
    // and vswprintf, at least on GCC munges it. So we must use va_copy() to do this safely
    // @see http://en.cppreference.com/w/cpp/utility/variadic/va_copy
    va_list argListCopy;
    va_copy (argListCopy, argsList);

#if __STDC_WANT_SECURE_LIB__
    while (::vsnprintf_s (msgBuf.data (), msgBuf.GetSize (), msgBuf.GetSize () - 1, format, argListCopy) < 0) {
        msgBuf.GrowToSize_uninitialized (msgBuf.GetSize () * 2);
        va_end (argListCopy);
        va_copy (argListCopy, argsList);
    }
#else
    while (vsnprintf (msgBuf.data (), msgBuf.GetSize (), format, argListCopy) < 0) {
        msgBuf.GrowToSize_uninitialized (msgBuf.GetSize () * 2);
        va_end (argListCopy);
        va_copy (argListCopy, argsList);
    }
#endif
    va_end (argListCopy);
    Assert (::strlen (msgBuf.data ()) < msgBuf.GetSize ());
    return string{msgBuf.data ()};
}

u8string Characters::CString::FormatV (const char8_t* format, va_list argsList)
{
    RequireNotNull (format);
    Memory::StackBuffer<char8_t, 10 * 1024> msgBuf{Memory::eUninitialized, 10 * 1024};
    // SUBTLE: va_list looks like it is passed by value, but its not really,
    // and vswprintf, at least on GCC munges it. So we must use va_copy() to do this safely
    // @see http://en.cppreference.com/w/cpp/utility/variadic/va_copy
    va_list argListCopy;
    va_copy (argListCopy, argsList);

#if __STDC_WANT_SECURE_LIB__
    while (::vsnprintf_s (reinterpret_cast<char*> (msgBuf.data ()), msgBuf.GetSize (), msgBuf.GetSize () - 1,
                          reinterpret_cast<const char*> (format), argListCopy) < 0) {
        msgBuf.GrowToSize_uninitialized (msgBuf.GetSize () * 2);
        va_end (argListCopy);
        va_copy (argListCopy, argsList);
    }
#else
    while (vsnprintf (reinterpret_cast<char*> (msgBuf.data ()), msgBuf.GetSize (), reinterpret_cast<const char*> (format), argListCopy) < 0) {
        msgBuf.GrowToSize_uninitialized (msgBuf.GetSize () * 2);
        va_end (argListCopy);
        va_copy (argListCopy, argsList);
    }
#endif
    va_end (argListCopy);
    Assert (::strlen (reinterpret_cast<char*> (msgBuf.data ())) < msgBuf.GetSize ());
    return u8string{msgBuf.data ()};
}

DISABLE_COMPILER_MSC_WARNING_START (6262)
#if qCompilerAndStdLib_arm_asan_FaultStackUseAfterScope_Buggy
Stroika_Foundation_Debug_ATTRIBUTE_NO_SANITIZE_ADDRESS
#endif
    wstring
    Characters::CString::FormatV (const wchar_t* format, va_list argsList)
{
    RequireNotNull (format);
    Memory::StackBuffer<wchar_t, 10 * 1024> msgBuf{Memory::eUninitialized, 10 * 1024};
    const wchar_t*                          useFormat = format;
    wchar_t                                 newFormat[5 * 1024];
    {
        size_t origFormatLen = ::wcslen (format);
        Require (origFormatLen < Memory::NEltsOf (newFormat) / 2); // just to be sure safe - this is already crazy-big for format string...
        // Could use Memory::StackBuffer<> but I doubt this will ever get triggered
        bool   lookingAtFmtCvt = false;
        size_t newFormatIdx    = 0;
        for (size_t i = 0; i < origFormatLen; ++i) {
            if (lookingAtFmtCvt) {
                switch (format[i]) {
                    case '%': {
                        lookingAtFmtCvt = false;
                    } break;
                    case 's': {
                        newFormat[newFormatIdx] = 'l';
                        ++newFormatIdx;
                    } break;
                    case '.': {
                        // could still be part for format string
                    } break;
                    default: {
                        if (isdigit (format[i])) {
                            // could still be part for format string
                        }
                        else {
                            lookingAtFmtCvt = false; // DONE
                        }
                    } break;
                }
            }
            else {
                if (format[i] == '%') {
                    lookingAtFmtCvt = true;
                }
            }
            newFormat[newFormatIdx] = format[i];
            ++newFormatIdx;
        }
        Assert (newFormatIdx >= origFormatLen);
        if (newFormatIdx > origFormatLen) {
            newFormat[newFormatIdx] = '\0';
            useFormat               = newFormat;
        }
    }

    // SUBTLE: va_list looks like it is passed by value, but its not really,
    // and vswprintf, at least on GCC munges it. So we must use va_copy() to do this safely
    // @see http://en.cppreference.com/w/cpp/utility/variadic/va_copy
    va_list argListCopy;
    va_copy (argListCopy, argsList);

#if qCompilerAndStdLib_vswprintf_errantDependencyOnLocale_Buggy
    locale_t                tmpLocale{};
    locale_t                prevLocale{};
    [[maybe_unused]] auto&& cleanup = Execution::Finally ([&tmpLocale, &prevLocale] () noexcept {
        if (prevLocale != nullptr) {
            ::uselocale (prevLocale);
        }
        if (tmpLocale != nullptr) {
            ::freelocale (tmpLocale);
        }
    });
#endif

    // Assume only reason for failure is not enuf bytes, so allocate more.
    // If I'm wrong, we'll just runout of memory and throw out...
    while (::vswprintf (msgBuf.data (), msgBuf.GetSize (), useFormat, argListCopy) < 0) {
#if qCompilerAndStdLib_vswprintf_errantDependencyOnLocale_Buggy
        if (errno == EILSEQ and tmpLocale == nullptr) {
            tmpLocale  = ::newlocale (LC_CTYPE, "en_US.UTF-8", NULL);
            prevLocale = ::uselocale (tmpLocale);
            continue;
        }
#endif
        msgBuf.GrowToSize_uninitialized (msgBuf.GetSize () * 2);
        va_end (argListCopy);
        va_copy (argListCopy, argsList);
    }
    va_end (argListCopy);
    Assert (::wcslen (msgBuf.data ()) < msgBuf.GetSize ());
    return wstring{msgBuf.data ()};
}
DISABLE_COMPILER_MSC_WARNING_END (6262)

/*
 ********************************************************************************
 ************************* Characters::CString::Format **************************
 ********************************************************************************
 */
string Characters::CString::Format (const char* format, ...)
{
    va_list argsList;
    va_start (argsList, format);
    string tmp = FormatV (format, argsList);
    va_end (argsList);
    return tmp;
}

u8string Characters::CString::Format (const char8_t* format, ...)
{
    va_list argsList;
    va_start (argsList, format);
    u8string tmp = FormatV (format, argsList);
    va_end (argsList);
    return tmp;
}

wstring Characters::CString::Format (const wchar_t* format, ...)
{
    va_list argsList;
    va_start (argsList, format);
    wstring tmp = FormatV (format, argsList);
    va_end (argsList);
    return tmp;
}

/*
 ********************************************************************************
 ********************************* LimitLength **********************************
 ********************************************************************************
 */
namespace {
    template <typename STRING>
    inline STRING LimitLength_HLPR_ (const STRING& str, size_t maxLen, bool keepLeft, const STRING& kELIPSIS)
    {
        if (str.length () <= maxLen) {
            return str;
        }
        size_t useLen = maxLen;
        if (useLen > kELIPSIS.length ()) {
            useLen -= kELIPSIS.length ();
        }
        else {
            useLen = 0;
        }
        if (keepLeft) {
            return str.substr (0, useLen) + kELIPSIS;
        }
        else {
            return kELIPSIS + str.substr (str.length () - useLen);
        }
    }
}
string Characters::CString::LimitLength (const string& str, size_t maxLen, bool keepLeft)
{
    return LimitLength_HLPR_<string> (str, maxLen, keepLeft, "...");
}

wstring Characters::CString::LimitLength (const wstring& str, size_t maxLen, bool keepLeft)
{
    return LimitLength_HLPR_<wstring> (str, maxLen, keepLeft, L"...");
}

/*
 ********************************************************************************
 *************************** StripTrailingCharIfAny *****************************
 ********************************************************************************
 */
namespace {
    template <typename STRING>
    inline STRING StripTrailingCharIfAny_HLPR_ (const STRING& str, typename STRING::value_type c)
    {
        if (str.size () > 0 and str[str.size () - 1] == c) {
            STRING tmp = str;
            tmp.erase (tmp.size () - 1);
            return tmp;
        }
        return str;
    }
}

string Characters::CString::StripTrailingCharIfAny (const string& s, char c)
{
    return StripTrailingCharIfAny_HLPR_ (s, c);
}

wstring Characters::CString::StripTrailingCharIfAny (const wstring& s, wchar_t c)
{
    return StripTrailingCharIfAny_HLPR_ (s, c);
}

/*
 ********************************************************************************
 ****************************** HexString2Int ***********************************
 ********************************************************************************
 */
unsigned int Characters::CString::HexString2Int (const string& s)
{
    unsigned long l = strtoul (s.c_str (), nullptr, 16);
    if (l >= numeric_limits<unsigned int>::max ()) {
        return numeric_limits<unsigned int>::max ();
    }
    return l;
}

unsigned int Characters::CString::HexString2Int (const wchar_t* s)
{
    RequireNotNull (s);
    unsigned long l = wcstoul (s, nullptr, 16);
    if (l >= numeric_limits<unsigned int>::max ()) {
        return numeric_limits<unsigned int>::max ();
    }
    return l;
}

unsigned int Characters::CString::HexString2Int (const wstring& s)
{
    unsigned long l = wcstoul (s.c_str (), nullptr, 16);
    if (l >= numeric_limits<unsigned int>::max ()) {
        return numeric_limits<unsigned int>::max ();
    }
    return l;
}

/*
 ********************************************************************************
 ********************************* String2Int ***********************************
 ********************************************************************************
 */
long long int Characters::CString::Private_::String2Int_ (const string& s)
{
    // nothing needed todo to pin the value to min/max
    return strtoll (s.c_str (), nullptr, 10);
}

long long int Characters::CString::Private_::String2Int_ (const wstring& s)
{
    unsigned long long int l = wcstoll (s.c_str (), nullptr, 10);
    return l;
}

/*
 ********************************************************************************
 ********************************* String2UInt ***********************************
 ********************************************************************************
 */
unsigned long long int Characters::CString::Private_::String2UInt_ (const string& s)
{
    // nothing needed todo to pin the value to min/max
    unsigned long long int l = strtoull (s.c_str (), nullptr, 10);
    return l;
}

unsigned long long int Characters::CString::Private_::String2UInt_ (const wstring& s)
{
    long long int l = wcstoull (s.c_str (), nullptr, 10);
    return l;
}
