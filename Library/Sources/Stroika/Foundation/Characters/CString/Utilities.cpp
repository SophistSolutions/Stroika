/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#include    "../../StroikaPreComp.h"

#include    <iomanip>
#include    <sstream>

#include    "../../Math/Common.h"
#include    "../../Memory/SmallStackBuffer.h"
#include    "../CodePage.h"

#include    "Utilities.h"



using   namespace   Stroika;
using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Characters;
using   namespace   Stroika::Foundation::Characters::CString;




/*
 ********************************************************************************
 ************************************* Format ***********************************
 ********************************************************************************
 */
string  Characters::CString::FormatV (const char* format, va_list argsList)
{
    RequireNotNull (format);
    Memory::SmallStackBuffer<char, 10 * 1024> msgBuf (10 * 1024);
#if     qSupportValgrindQuirks
    // Makes little sense - even msgBuf[0] not sufficient - but this silences lots of warnings.
    // -- LGP 2012-05-19
    memset (msgBuf, 0, sizeof (msgBuf[0]) * msgBuf.GetSize());
#endif
#if     __STDC_WANT_SECURE_LIB__
    while (::vsnprintf_s (msgBuf, msgBuf.GetSize (), msgBuf.GetSize () - 1, format, argsList) < 0) {
        msgBuf.GrowToSize (msgBuf.GetSize () * 2);
    }
#else
    while (::vsnprintf (msgBuf, msgBuf.GetSize (), format, argsList) < 0) {
        msgBuf.GrowToSize (msgBuf.GetSize () * 2);
    }
#endif
    Assert (::strlen (msgBuf) < msgBuf.GetSize ());
    return string (msgBuf);
}

string  Characters::CString::Format (const char* format, ...)
{
    va_list     argsList;
    va_start (argsList, format);
    string  tmp = FormatV (format, argsList);
    va_end (argsList);
    return tmp;
}

DISABLE_COMPILER_MSC_WARNING_START(6262)
wstring Characters::CString::FormatV (const wchar_t* format, va_list argsList)
{
    RequireNotNull (format);
    Memory::SmallStackBuffer<wchar_t, 10 * 1024>  msgBuf (10 * 1024);
    const   wchar_t*    useFormat   =   format;
#if     !qStdLibSprintfAssumesPctSIsWideInFormatIfWideFormat
    wchar_t     newFormat[5 * 1024];
    {
        size_t  origFormatLen   =   wcslen (format);
        Require (origFormatLen < NEltsOf (newFormat) / 2);  // just to be sure safe - this is already crazy-big for format string...
        // Could use Memory::SmallStackBuffer<> but I doubt this will ever get triggered
        bool lookingAtFmtCvt = false;
        size_t  newFormatIdx    =   0;
        for (size_t i = 0; i < origFormatLen; ++i) {
            if (lookingAtFmtCvt) {
                switch (format[i]) {
                    case    '%': {
                            lookingAtFmtCvt = false;
                        }
                        break;
                    case    's': {
                            newFormat[newFormatIdx] = 'l';
                            newFormatIdx++;
                        }
                        break;
                    case    '.': {
                            // could still be part for format string
                        }
                        break;
                    default: {
                            if (isdigit (format[i])) {
                                // could still be part for format string
                            }
                            else {
                                lookingAtFmtCvt = false;    // DONE
                            }
                        }
                        break;
                }
            }
            else {
                if (format[i] == '%') {
                    lookingAtFmtCvt = true;
                }
            }
            newFormat[newFormatIdx] = format[i];
            newFormatIdx++;
        }
        Assert (newFormatIdx >= origFormatLen);
        if (newFormatIdx > origFormatLen) {
            newFormat[newFormatIdx] = '\0';
            useFormat   =   newFormat;
        }
    }
#endif

#if     qSupportValgrindQuirks
    // Makes little sense - even msgBuf[0] not sufficient - but this silences lots of warnings.
    // -- LGP 2012-05-19
    memset (msgBuf, 0, sizeof (msgBuf[0]) * msgBuf.GetSize());
#endif

    // Assume only reason for failure is not enuf bytes, so allocate more.
    // If I'm wrong, we'll just runout of memory and throw out...
    while (::vswprintf (msgBuf, msgBuf.GetSize (), useFormat, argsList) < 0) {
        msgBuf.GrowToSize (msgBuf.GetSize () * 2);
    }
    Assert (::wcslen (msgBuf) < msgBuf.GetSize ());
    return wstring (msgBuf);
}
DISABLE_COMPILER_MSC_WARNING_END(6262)


wstring Characters::CString::Format (const wchar_t* format, ...)
{
    va_list     argsList;
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
namespace   {
    template    <typename STRING>
    inline  STRING  LimitLength_HLPR (const STRING& str, size_t maxLen, bool keepLeft, const STRING& kELIPSIS)
    {
        if (str.length () <= maxLen) {
            return str;
        }
        size_t  useLen  =   maxLen;
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
string  Characters::CString::LimitLength (const string& str, size_t maxLen, bool keepLeft)
{
    return LimitLength_HLPR<string> (str, maxLen, keepLeft, "...");
}

wstring Characters::CString::LimitLength (const wstring& str, size_t maxLen, bool keepLeft)
{
    return LimitLength_HLPR<wstring> (str, maxLen, keepLeft, L"...");
}








/*
 ********************************************************************************
 *************************** StripTrailingCharIfAny *****************************
 ********************************************************************************
 */
namespace   {
    template    <typename STRING>
    inline  STRING  StripTrailingCharIfAny_HLPR (const STRING& str, typename STRING::value_type c)
    {
        if (str.size () > 0 and str[str.size () - 1] == c) {
            STRING  tmp =   str;
            tmp.erase (tmp.size () - 1);
            return tmp;
        }
        return str;
    }
}

string  Characters::CString::StripTrailingCharIfAny (const string& s, char c)
{
    return StripTrailingCharIfAny_HLPR (s, c);
}

wstring Characters::CString::StripTrailingCharIfAny (const wstring& s, wchar_t c)
{
    return StripTrailingCharIfAny_HLPR (s, c);
}






/*
 ********************************************************************************
 ********************************* Float2String *********************************
 ********************************************************************************
 */
wstring Characters::CString::Float2String (double f, unsigned int precision)
{
    if (std::isnan (f)) {
        return wstring ();
    }
    stringstream s;
    s.imbue (locale ("C"));
    s << setprecision (precision) << f;
    wstring r = ASCIIStringToWide (s.str ());
    return r;
}









/*
 ********************************************************************************
 ****************************** HexString2Int ***********************************
 ********************************************************************************
 */
unsigned int Characters::CString::HexString2Int (const string& s)
{
    using   std::numeric_limits;
    unsigned    long    l   =   strtoul (s.c_str (), nullptr, 16);
    if (l >= numeric_limits<unsigned int>::max ()) {
        return numeric_limits<unsigned int>::max ();
    }
    return l;
}

unsigned int Characters::CString::HexString2Int (const wchar_t* s)
{
    using   std::numeric_limits;
    RequireNotNull (s);
    unsigned    long    l   =   wcstoul (s, nullptr, 16);
    if (l >= numeric_limits<unsigned int>::max ()) {
        return numeric_limits<unsigned int>::max ();
    }
    return l;
}

unsigned int Characters::CString::HexString2Int (const wstring& s)
{
    using   std::numeric_limits;
    unsigned    long    l   =   wcstoul (s.c_str (), nullptr, 16);
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
    using   std::numeric_limits;
#if     qCompilerAndStdLib_Supports_strtoll
    long long int    l   =   strtoll (s.c_str (), nullptr, 10);
#else
    long long int    l   =   _strtoi64 (s.c_str (), nullptr, 10);
#endif
    // nothing needed todo to pin the value to min/max
    return l;
}

long long int Characters::CString::Private_::String2Int_ (const wstring& s)
{
    using   std::numeric_limits;
#if     qCompilerAndStdLib_Supports_strtoll
    unsigned    long long int    l   =   wcstoll (s.c_str (), nullptr, 10);
#else
    unsigned    long long int    l   =   _wcstoi64 (s.c_str (), nullptr, 10);
#endif
    return l;
}




/*
 ********************************************************************************
 ********************************* String2UInt ***********************************
 ********************************************************************************
 */
unsigned long long int Characters::CString::Private_::String2UInt_ (const string& s)
{
    using   std::numeric_limits;
#if     qCompilerAndStdLib_Supports_strtoll
    unsigned    long long int    l   =   strtoull (s.c_str (), nullptr, 10);
#else
    unsigned    long long int    l   =   _strtoui64 (s.c_str (), nullptr, 10);
#endif
    // nothing needed todo to pin the value to min/max
    return l;
}

unsigned long long int Characters::CString::Private_::String2UInt_ (const wstring& s)
{
    using   std::numeric_limits;
#if     qCompilerAndStdLib_Supports_strtoll
    long long int    l   =   wcstoull (s.c_str (), nullptr, 10);
#else

    long long int    l   =   _wcstoui64 (s.c_str (), nullptr, 10);
#endif
    return l;
}








/*
 ********************************************************************************
 ********************************* String2Float *********************************
 ********************************************************************************
 */
double  Characters::CString::String2Float (const string& s)
{
    char*   e   =   nullptr;
    double  d   =   strtod (s.c_str (), &e);
    if (d == 0) {
        if (s.c_str () == e) {
            return Math::nan ();
        }
    }
    return d;
}

double  Characters::CString::String2Float (const wchar_t* s)
{
    RequireNotNull (s);
    wchar_t*    e   =   nullptr;
    double  d   =   wcstod (s, &e);
    if (d == 0) {
        if (s == e) {
            return Math::nan ();
        }
    }
    return d;
}

double  Characters::CString::String2Float (const wstring& s)
{
    wchar_t*    e   =   nullptr;
    double  d   =   wcstod (s.c_str (), &e);
    if (d == 0) {
        if (s.c_str () == e) {
            return Math::nan ();
        }
    }
    return d;
}
