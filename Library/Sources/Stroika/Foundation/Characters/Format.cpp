/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    <cstdarg>
#include    <cstdlib>
#include    <iomanip>
#include    <limits>
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
DISABLE_COMPILER_MSC_WARNING_START(6262)
String Characters::FormatV (const wchar_t* format, va_list argsList)
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
    return String (msgBuf);
}
DISABLE_COMPILER_MSC_WARNING_END(6262)



/*
********************************************************************************
************************************* Format ***********************************
********************************************************************************
*/
String Characters::Format (const wchar_t* format, ...)
{
    va_list     argsList;
    va_start (argsList, format);
    String tmp = FormatV (format, argsList);
    va_end (argsList);
    return tmp;
}







/*
 ********************************************************************************
 ****************************** HexString2Int ***********************************
 ********************************************************************************
 */
unsigned int Characters::HexString2Int (const String& s)
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
long long int Characters::Private_::String2Int_ (const String& s)
{
    using   std::numeric_limits;
    unsigned    long long int    l   =   wcstoll (s.c_str (), nullptr, 10);
    return l;
}




/*
 ********************************************************************************
 ********************************* String2UInt ***********************************
 ********************************************************************************
 */
unsigned long long int Characters::Private_::String2UInt_ (const String& s)
{
    using   std::numeric_limits;
    long long int    l   =   wcstoull (s.c_str (), nullptr, 10);
    return l;
}






/*
 ********************************************************************************
 ********************************* String2Float *********************************
 ********************************************************************************
 */
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
namespace {
    template    <typename FLOAT_TYPE>
    inline  String Float2String_ (FLOAT_TYPE f, const Float2StringOptions& options)
    {
        if (std::isnan (f)) {
            return String ();
        }
        stringstream s;
        if (options.fUseLocale.IsPresent ()) {
            s.imbue (*options.fUseLocale);
        }
        else {
            static  locale  kCLocale_ = locale ("C");
            s.imbue (kCLocale_);
        }
        if (options.fPrecision.IsPresent ()) {
            s << setprecision (*options.fPrecision);
        }
        if (options.fFmtFlags.IsPresent ()) {
            s << setiosflags (*options.fFmtFlags);
        }
        s << f;
        String tmp = String::FromAscii (s.str ());
        if (options.fTrimTrailingZeros) {
            // strip trailing zeros - except for the last first one after the decimal point
            size_t pastDot = tmp.find ('.');
            if (pastDot != String::npos) {
                pastDot++;
                size_t pPastLastZero = tmp.length ();
                for (; (pPastLastZero - 1) > pastDot; --pPastLastZero) {
                    if (tmp[pPastLastZero - 1] != '0') {
                        break;
                    }
                }
                tmp = tmp.SubString (0, pPastLastZero);
            }
        }
        return tmp;
    }

}
String Characters::Float2String (float f, const Float2StringOptions& options)
{
    return Float2String_<long double> (f, options);
}

String Characters::Float2String (double f, const Float2StringOptions& options)
{
    return Float2String_<long double> (f, options);
}

String Characters::Float2String (long double f, const Float2StringOptions& options)
{
    return Float2String_<long double> (f, options);
}





/*
 ********************************************************************************
 *************************** StripTrailingCharIfAny *****************************
 ********************************************************************************
 */
String Characters::StripTrailingCharIfAny (const String& s, wchar_t c)
{
    if (s.size () > 0 and s[s.size () - 1] == c) {
        String  tmp =   s;
        tmp.erase (tmp.size () - 1);
        return tmp;
    }
    return s;
}

