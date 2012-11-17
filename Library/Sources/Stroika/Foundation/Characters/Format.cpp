/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    <cstdarg>
#include    <cstdlib>
#include    <iomanip>
#include    <sstream>

#include    "../Containers/Common.h"
#include    "../Debug/Assertions.h"
#include    "../Debug/Trace.h"
#include    "../Math/Common.h"
#include    "../Memory/SmallStackBuffer.h"
#include    "CodePage.h"

#include    "Format.h"




using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Characters;
using   namespace   Stroika::Foundation::Memory;






/*
 ********************************************************************************
 ************************************* Format ***********************************
 ********************************************************************************
 */
string  Characters::FormatV (const char* format, va_list argsList)
{
    RequireNotNull (format);
    Memory::SmallStackBuffer<char, 10* 1024> msgBuf (10 * 1024);
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

string  Characters::Format (const char* format, ...)
{
    va_list     argsList;
    va_start (argsList, format);
    string  tmp = FormatV (format, argsList);
    va_end (argsList);
    return tmp;
}

wstring Characters::FormatV (const wchar_t* format, va_list argsList)
{
    RequireNotNull (format);
    Memory::SmallStackBuffer<wchar_t, 10* 1024>  msgBuf (10 * 1024);
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

wstring Characters::Format (const wchar_t* format, ...)
{
    va_list     argsList;
    va_start (argsList, format);
    wstring tmp = FormatV (format, argsList);
    va_end (argsList);
    return tmp;
}







/*
 ********************************************************************************
 ****************************** HexString2Int ***********************************
 ********************************************************************************
 */
unsigned int Characters::HexString2Int (const string& s)
{
    unsigned    long    l   =   strtoul (s.c_str (), nullptr, 16);
    if (l >= UINT_MAX) {
        return UINT_MAX;
    }
    return l;
}

unsigned int Characters::HexString2Int (const wchar_t* s)
{
    RequireNotNull (s);
    unsigned    long    l   =   wcstoul (s, nullptr, 16);
    if (l >= UINT_MAX) {
        return UINT_MAX;
    }
    return l;
}

unsigned int Characters::HexString2Int (const wstring& s)
{
    unsigned    long    l   =   wcstoul (s.c_str (), nullptr, 16);
    if (l >= UINT_MAX) {
        return UINT_MAX;
    }
    return l;
}

unsigned int Characters::HexString2Int (const String& s)
{
    unsigned    long    l   =   wcstoul (s.c_str (), nullptr, 16);
    if (l >= UINT_MAX) {
        return UINT_MAX;
    }
    return l;
}






/*
 ********************************************************************************
 ********************************* String2Int ***********************************
 ********************************************************************************
 */
int Characters::String2Int (const string& s)
{
    long    l   =   strtol (s.c_str (), nullptr, 10);
    if (l <= INT_MIN) {
        return INT_MIN;
    }
    if (l >= INT_MAX) {
        return INT_MAX;
    }
    return l;
}

int Characters::String2Int (const wstring& s)
{
    long    l   =   wcstol (s.c_str (), nullptr, 10);
    if (l <= INT_MIN) {
        return INT_MIN;
    }
    if (l >= INT_MAX) {
        return INT_MAX;
    }
    return l;
}

int Characters::String2Int (const wchar_t* s)
{
    RequireNotNull (s);
    long    l   =   wcstol (s, nullptr, 10);
    if (l <= INT_MIN) {
        return INT_MIN;
    }
    if (l >= INT_MAX) {
        return INT_MAX;
    }
    return l;
}

int Characters::String2Int (const String& s)
{
    long    l   =   wcstol (s.c_str (), nullptr, 10);
    if (l <= INT_MIN) {
        return INT_MIN;
    }
    if (l >= INT_MAX) {
        return INT_MAX;
    }
    return l;
}








/*
 ********************************************************************************
 ********************************* String2Float *********************************
 ********************************************************************************
 */
double  Characters::String2Float (const string& s)
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

double  Characters::String2Float (const wchar_t* s)
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

double  Characters::String2Float (const wstring& s)
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

double  Characters::String2Float (const String& s)
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









/*
 ********************************************************************************
 ********************************* Float2String *********************************
 ********************************************************************************
 */
wstring Characters::Float2String (double f, unsigned int precision)
{
    if (std::isnan (f)) {
        return wstring ();
    }
    stringstream s;
    s << setprecision (precision) << f;
    wstring r = ASCIIStringToWide (s.str ());
    return r;
}







