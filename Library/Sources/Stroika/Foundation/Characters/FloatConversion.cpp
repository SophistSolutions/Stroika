/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    <cstdarg>
#include    <cstdlib>
#include    <iomanip>
#include    <limits>
#include    <sstream>

#include    "../Containers/Common.h"
#include    "../Characters/String_Constant.h"
#include    "../Debug/Assertions.h"
#include    "../Debug/Trace.h"
#include    "../Math/Common.h"
#include    "../Memory/SmallStackBuffer.h"
#include    "CodePage.h"

#include    "FloatConversion.h"




using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Characters;
using   namespace   Stroika::Foundation::Memory;





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
            static  const   String_Constant     kNAN_STR_ { L"NAN" };
            return kNAN_STR_;
        }
        if (std::isinf (f)) {
            static  const   String_Constant     kNEG_INF_STR_   { L"-INF" };
            static  const   String_Constant     kINF_STR_       { L"INF" };
            return f > 0 ? kINF_STR_ : kNEG_INF_STR_;
        }
        stringstream s;
        if (options.fUseLocale.IsPresent ()) {
            s.imbue (*options.fUseLocale);
        }
        else {
            static  const   locale  kCLocale_ = locale::classic ();
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
 ********************************* String2Float *********************************
 ********************************************************************************
 */
namespace {
    template    <typename RETURN_TYPE, typename FUNCTION>
    inline  RETURN_TYPE  String2Float_ (const String& s, FUNCTION F)
    {
        wchar_t*        e   = nullptr;
        const wchar_t*  cst = s.c_str ();
#if     qCompilerAndStdLib_strtof_NAN_ETC_Buggy
        if (s == L"INF" or s == L"+INF" or s == L"INFINITY" or s == L"+INFINITY") {
            return numeric_limits<RETURN_TYPE>::infinity ();
        }
        if (s == L"-INF" or s == L"-INFINITY") {
            return -numeric_limits<RETURN_TYPE>::infinity ();
        }
#endif
        RETURN_TYPE  d = F (cst, &e);
        // if trailing crap - return nan
        if (*e != '\0') {
            return Math::nan<RETURN_TYPE> ();
        }
        if (d == 0) {
            if (cst == e) {
                return Math::nan<RETURN_TYPE> ();
            }
        }
        return d;
    }
}


namespace   Stroika {
    namespace   Foundation {
        namespace   Characters {
            template    <>
            float  String2Float (const String& s)
            {
                return String2Float_<float> (s, wcstof);
            }

            template    <>
            double  String2Float (const String& s)
            {
                return String2Float_<double> (s, wcstod);
            }

            template    <>
            long double  String2Float (const String& s)
            {
                return String2Float_<long double> (s, wcstold);
            }
        }
    }
}
