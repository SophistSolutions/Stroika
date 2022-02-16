/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include <charconv>
#include <cstdarg>
#include <cstdlib>
#include <iomanip>
#include <limits>
#include <sstream>

#include "../Characters/StringBuilder.h"
#include "../Characters/ToString.h"
#include "../Containers/Common.h"
#include "../Debug/Assertions.h"
#include "../Debug/Trace.h"
#include "../Debug/Valgrind.h"
#include "../Math/Common.h"

#include "FloatConversion.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Memory;

/*
 ********************************************************************************
 ******************** FloatConversion::ToStringOptions **************************
 ********************************************************************************
 */
FloatConversion::ToStringOptions::ToStringOptions (UseCurrentLocale)
    : fUseLocale_{locale{}}
{
}

String FloatConversion::ToStringOptions::ToString () const
{
    StringBuilder sb;
    sb += L"{";
    if (fPrecision_) {
        sb += L"Precision:" + Characters::ToString (*fPrecision_) + L",";
    }
    if (fFmtFlags_) {
        sb += L"Fmt-Flags:" + Characters::ToString ((int)*fFmtFlags_, ios_base::hex) + L",";
    }
    if (fUseLocale_) {
        sb += L"Use-Locale" + String::FromNarrowSDKString (fUseLocale_->name ()) + L",";
    }
    if (fTrimTrailingZeros_) {
        sb += L"Trim-Trailing-Zeros: " + Characters::ToString (*fTrimTrailingZeros_) + L",";
    }
    if (fFloatFormat_) {
        sb += L"Scientific-Notation: " + Characters::ToString ((int)*fFloatFormat_) + L",";
    }
    sb += L"}";
    return sb.str ();
}

namespace {
    inline char* mkFmtWithPrecisionArg_ (char* formatBufferStart, [[maybe_unused]] char* formatBufferEnd, char _Spec)
    {
        char* fmtPtr = formatBufferStart;
        *fmtPtr++    = '%';
        // include precision arg
        *fmtPtr++ = '.';
        *fmtPtr++ = '*';
        if (_Spec != '\0') {
            *fmtPtr++ = _Spec; // e.g. 'L' qualifier
        }
        *fmtPtr++ = 'g'; // format specifier
        *fmtPtr   = '\0';
        Require (fmtPtr < formatBufferEnd);
        return formatBufferStart;
    }

    template <typename FLOAT_TYPE>
    inline String Float2String_GenericCase_ (FLOAT_TYPE f, const FloatConversion::ToStringOptions& options)
    {
        Require (not isnan (f));
        Require (not isinf (f));

        // expensive to construct, and slightly cheaper to just use thread_local version of
        // the same stringstream each time (only one per thread can be in use)
        static thread_local stringstream s;
        static const ios_base::fmtflags  kDefaultIOSFmtFlags_ = s.flags (); // Just copy out of the first constructed stringstream

        s.str (string{});
        s.clear ();

        static const locale kCLocale_ = locale::classic ();
        s.imbue (options.GetUseLocale ().value_or (kCLocale_));

        //  must set explictly (even if defaulted)  because of the thread_local thing
        s.flags (options.GetIOSFmtFlags ().value_or (kDefaultIOSFmtFlags_));

        // todo must set default precision because of the thread_local thing
        unsigned int usePrecision = options.GetPrecision ().value_or (FloatConversion::ToStringOptions::kDefaultPrecision.fPrecision);
        s.precision (usePrecision);

        {
            optional<ios_base::fmtflags> useFloatField;
            switch (options.GetFloatFormat ().value_or (FloatConversion::FloatFormatType::eDEFAULT)) {
                case FloatConversion::FloatFormatType::eScientific:
                    useFloatField = ios_base::scientific;
                    break;
                case FloatConversion::FloatFormatType::eDefaultFloat:
                    break;
                case FloatConversion::FloatFormatType::eFixedPoint:
                    useFloatField = ios_base::fixed;
                    break;
                case FloatConversion::FloatFormatType::eAutomatic: {
                    bool useScientificNotation = abs (f) >= pow (10, usePrecision / 2) or (f != 0 and abs (f) < pow (10, -static_cast<int> (usePrecision) / 2)); // scientific preserves more precision - but non-scientific looks better
                    if (useScientificNotation) {
                        useFloatField = ios_base::scientific;
                    }
                } break;
                default:
                    RequireNotReached ();
                    break;
            }
            if (useFloatField) {
                s.setf (*useFloatField, ios_base::floatfield);
            }
            else {
                s.unsetf (ios_base::floatfield); // see std::defaultfloat - not same as ios_base::fixed
            }
        }

        s << f;

        String tmp = options.GetUseLocale () ? String::FromNarrowString (s.str (), *options.GetUseLocale ()) : String::FromASCII (s.str ());
        if (options.GetTrimTrailingZeros ().value_or (FloatConversion::ToStringOptions::kDefaultTrimTrailingZeros)) {
            Characters::FloatConversion::Private_::TrimTrailingZeros_ (&tmp);
        }
        return tmp;
    }
    template <typename FLOAT_TYPE>
    inline String Float2String_ (FLOAT_TYPE f, const FloatConversion::ToStringOptions& options)
    {
        switch (fpclassify (f)) {
            case FP_INFINITE: {
                Assert (isinf (f));
                Assert (!isnan (f));
                static const String kNEG_INF_STR_{L"-INF"sv};
                static const String kINF_STR_{L"INF"sv};
                return f > 0 ? kINF_STR_ : kNEG_INF_STR_;
            }
            case FP_NAN: {
                Assert (isnan (f));
                Assert (!isinf (f));
                static const String kNAN_STR_{L"NAN"sv};
                return kNAN_STR_;
            }
            default: {
                if constexpr (qCompilerAndStdLib_valgrind_fpclassify_check_Buggy && qDebug) {
                    if (Debug::IsRunningUnderValgrind ()) {
                        if (isinf (f)) {
                            DbgTrace ("fpclassify (%f) = %d", (double)f, fpclassify (f));
                            static const String kNEG_INF_STR_{L"-INF"sv};
                            static const String kINF_STR_{L"INF"sv};
                            return f > 0 ? kINF_STR_ : kNEG_INF_STR_;
                        }
                    }
                }
            }
        }
        Assert (!isnan (f));
        Assert (!isinf (f));
        return Float2String_GenericCase_<FLOAT_TYPE> (f, options);
    }
}

/*
 ********************************************************************************
 *********** FloatConversion::Private_::Legacy_Float2String_ ********************
 ********************************************************************************
 */
String Characters::FloatConversion::Private_::Legacy_Float2String_ (float f, const FloatConversion::ToStringOptions& options)
{
    return Float2String_<float> (f, options);
}
String Characters::FloatConversion::Private_::Legacy_Float2String_ (double f, const FloatConversion::ToStringOptions& options)
{
    return Float2String_<double> (f, options);
}
String Characters::FloatConversion::Private_::Legacy_Float2String_ (long double f, const FloatConversion::ToStringOptions& options)
{
    return Float2String_<long double> (f, options);
}

/*
 ********************************************************************************
 ********************************* String2Float *********************************
 ********************************************************************************
 */
namespace {
    template <typename RETURN_TYPE, typename FUNCTION>
    inline RETURN_TYPE String2Float_LegacyStr2DHelper_ (const String& s, FUNCTION F)
    {
        wchar_t*       e   = nullptr;
        const wchar_t* cst = s.c_str ();
        if (::iswspace (*cst)) {
            return Math::nan<RETURN_TYPE> (); // this was a bug in the 2.1b14 and earlier code according to docs - we should REJECT strings with leading space in this case
        }
        RETURN_TYPE d = F (cst, &e);
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
    template <typename RETURN_TYPE, typename FUNCTION>
    inline RETURN_TYPE String2Float_LegacyStr2DHelper_ (const String& s, String* remainder, FUNCTION F)
    {
        RequireNotNull (remainder);
        wchar_t*       e   = nullptr;
        const wchar_t* cst = s.c_str ();
        RETURN_TYPE    d   = F (cst, &e);
        *remainder         = e;
        return d;
    }

    template <typename RETURN_TYPE>
    RETURN_TYPE String2Float_LegacyStr2D_ (const String& s);
    template <>
    inline float String2Float_LegacyStr2D_ (const String& s)
    {
        return String2Float_LegacyStr2DHelper_<float> (s, wcstof);
    }
    template <>
    inline double String2Float_LegacyStr2D_ (const String& s)
    {
        return String2Float_LegacyStr2DHelper_<double> (s, wcstod);
    }
    template <>
    inline long double String2Float_LegacyStr2D_ (const String& s)
    {
        return String2Float_LegacyStr2DHelper_<long double> (s, wcstold);
    }

}

namespace Stroika::Foundation::Characters::FloatConversion::Private_ {
    template <>
    float ToFloat_Legacy_ (const String& s)
    {
        return String2Float_LegacyStr2D_<float> (s);
    }
    template <>
    double ToFloat_Legacy_ (const String& s)
    {
        return String2Float_LegacyStr2D_<double> (s);
    }
    template <>
    long double ToFloat_Legacy_ (const String& s)
    {
        return String2Float_LegacyStr2D_<long double> (s);
    }
}

namespace Stroika::Foundation::Characters::FloatConversion::Private_ {
    template <>
    float ToFloat_Legacy_ (const String& s, String* remainder)
    {
        return String2Float_LegacyStr2DHelper_<float> (s, remainder, wcstof);
    }
    template <>
    double ToFloat_Legacy_ (const String& s, String* remainder)
    {
        return String2Float_LegacyStr2DHelper_<double> (s, remainder, wcstod);
    }
    template <>
    long double ToFloat_Legacy_ (const String& s, String* remainder)
    {
        return String2Float_LegacyStr2DHelper_<long double> (s, remainder, wcstold);
    }
}
