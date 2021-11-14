/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include <charconv>
#include <cstdarg>
#include <cstdlib>
#include <iomanip>
#include <limits>
#include <sstream>
#if __cpp_lib_format
#include <format>
#endif

#include "../Characters/StringBuilder.h"
#include "../Characters/ToString.h"
#include "../Containers/Common.h"
#include "../Debug/Assertions.h"
#include "../Debug/Trace.h"
#include "../Debug/Valgrind.h"
#include "../Math/Common.h"
#include "../Memory/SmallStackBuffer.h"

#include "FloatConversion.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Memory;

/*
 ********************************************************************************
 **************************** FloatConversion::ToStringOptions *******************************
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

/*
  ********************************************************************************
  ********************************* Float2String *********************************
  ********************************************************************************
  */
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
    inline String Float2String_OptimizedForCLocaleAndNoStreamFlags_ (FLOAT_TYPE f, int precision, bool trimTrailingZeros)
    {
        Require (not isnan (f));
        Require (not isinf (f));
        size_t                 sz = precision + 100; // I think precision is enough
        SmallStackBuffer<char> buf{SmallStackBuffer<char>::eUninitialized, sz};

#if qCompilerAndStdLib_to_chars_FP_Buggy
        char format[100]; // intentionally uninitialized, cuz filled in with mkFmtWithPrecisionArg_
        int  resultStrLen = ::snprintf (buf, buf.size (), mkFmtWithPrecisionArg_ (std::begin (format), std::end (format), is_same_v<FLOAT_TYPE, long double> ? 'L' : '\0'), (int)precision, f);
#elif 1
        // empirically, on MSVC, this is much faster (appears 3x apx faster) -- LGP 2021-11-04
        ptrdiff_t resultStrLen = to_chars (buf.begin (), buf.end (), f, chars_format::general, precision).ptr - buf.begin ();
#elif __cpp_lib_format && 0
        // I read this was supposed to be faster, but empirically on windows seems much slower -- LGP 2021-11-04
        ptrdiff_t resultStrLen = format_to_n (buf.begin (), sz, is_same_v<FLOAT_TYPE, long double> ? "{:.{}Lg}"sv : "{:.{}g}"sv, f, precision).size;
#else
        char format[100]; // intentionally uninitialized, cuz filled in with mkFmtWithPrecisionArg_
        int  resultStrLen = ::snprintf (buf, buf.size (), mkFmtWithPrecisionArg_ (std::begin (format), std::end (format), is_same_v<FLOAT_TYPE, long double> ? 'L' : '\0'), (int)precision, f);
#endif
        Verify (resultStrLen > 0 and resultStrLen < static_cast<int> (sz));
        String tmp = String::FromASCII (buf.begin (), buf.begin () + resultStrLen);
        if (trimTrailingZeros) {
            Characters::Private_::TrimTrailingZeros_ (&tmp);
        }
        return tmp;
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
            switch (options.GetFloatFormat ().value_or (FloatConversion::ToStringOptions::FloatFormatType::eDEFAULT)) {
                case FloatConversion::ToStringOptions::FloatFormatType::eScientific:
                    useFloatField = ios_base::scientific;
                    break;
                case FloatConversion::ToStringOptions::FloatFormatType::eDefaultFloat:
                    break;
                case FloatConversion::ToStringOptions::FloatFormatType::eFixedPoint:
                    useFloatField = ios_base::fixed;
                    break;
                case FloatConversion::ToStringOptions::FloatFormatType::eAutomatic: {
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
            Characters::Private_::TrimTrailingZeros_ (&tmp);
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
#if qCompilerAndStdLib_valgrind_fpclassify_check_Buggy && qDebug
            default: {
                if (Debug::IsRunningUnderValgrind ()) {
                    if (isinf (f)) {
                        DbgTrace ("fpclassify (%f) = %d", (double)f, fpclassify (f));
                        static const String kNEG_INF_STR_{L"-INF"sv};
                        static const String kINF_STR_{L"INF"sv};
                        return f > 0 ? kINF_STR_ : kNEG_INF_STR_;
                    }
                }
            }
#endif
        }
        Assert (!isnan (f));
        Assert (!isinf (f));

        // Handle special cases as a performance optimization
        [[maybe_unused]] constexpr bool kUsePerformanceOptimizedCases_ = true;
        if constexpr (kUsePerformanceOptimizedCases_) {
            if (not options.GetUseLocale ().has_value () and not options.GetIOSFmtFlags ().has_value () and not options.GetFloatFormat ().has_value ()) {
                auto result = Float2String_OptimizedForCLocaleAndNoStreamFlags_ (f, options.GetPrecision ().value_or (FloatConversion::ToStringOptions::kDefaultPrecision.fPrecision), options.GetTrimTrailingZeros ().value_or (FloatConversion::ToStringOptions::kDefaultTrimTrailingZeros));
                Ensure (result == Float2String_GenericCase_<FLOAT_TYPE> (f, options));
                return result;
            }
        }
        return Float2String_GenericCase_<FLOAT_TYPE> (f, options);
    }
}
#if 0
String Characters::Float2String (float f, const FloatConversion::ToStringOptions& options)
{
    Assert (Float2String_<float> (f, options) == FloatConversion::ToString (f, options));
    return Float2String_<float> (f, options);
}

String Characters::Float2String (double f, const FloatConversion::ToStringOptions& options)
{
    Assert (Float2String_<double> (f, options) == FloatConversion::ToString (f, options));
    return Float2String_<double> (f, options);
}

String Characters::Float2String (long double f, const FloatConversion::ToStringOptions& options)
{
    Assert (String::EqualsComparer{CompareOptions::eCaseInsensitive}(Float2String_<long double> (f, options), FloatConversion::ToString (f, options)));
    return Float2String_<long double> (f, options);
}
#endif

/*
 ********************************************************************************
 *************************** Characters::Private_::Legacy_Float2String_ *********************************
 ********************************************************************************
 */

String Characters::Private_::Legacy_Float2String_ (float f, const FloatConversion::ToStringOptions& options)
{
    return Float2String_<float> (f, options);
}
String Characters::Private_::Legacy_Float2String_ (double f, const FloatConversion::ToStringOptions& options)
{
    return Float2String_<double> (f, options);
}
String Characters::Private_::Legacy_Float2String_ (long double f, const FloatConversion::ToStringOptions& options)
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
        RETURN_TYPE    d   = F (cst, &e);
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

    /**
     * Empirical test, 2021-11-05 on x64, windows, new laptop, vs2k19, showed about 30% speedup from this new logic.
     * And putting in assertions to assure same results.
    */
    template <typename RETURN_TYPE>
    RETURN_TYPE String2Float_NewLogic_ (const String& s)
    {
        Memory::SmallStackBuffer<char> asciiS;
        if (s.AsASCIIQuietly (&asciiS)) {
            RETURN_TYPE r; // intentionally uninitialized
            auto        b = asciiS.begin ();
            auto        e = asciiS.end ();
            if (b != e and *b == '+') {
                b++; // "the plus sign is not recognized outside of the exponent (only the minus sign is permitted at the beginning)" from https://en.cppreference.com/w/cpp/utility/from_chars
            }
            auto [ptr, ec] = from_chars (b, e, r);
            if (ec == errc::result_out_of_range) {
                return *b == '-' ? -numeric_limits<RETURN_TYPE>::infinity () : numeric_limits<RETURN_TYPE>::infinity ();
            }
            // if error or trailing crap - return nan
            return (ec == std::errc () and ptr == e) ? r : Math::nan<RETURN_TYPE> ();
        }
        else {
            return String2Float_LegacyStr2D_<RETURN_TYPE> (s);
        }
    }

}

namespace Stroika::Foundation::Characters {
    template <>
    float String2Float (const String& s)
    {
#if qCompilerAndStdLib_to_chars_FP_Buggy
        return String2Float_LegacyStr2D_<float> (s);
#else
        auto result = String2Float_NewLogic_<float> (s);
#if qDebug
        //static_assert (Math::nan<float> () != Math::nan<float> ());
        Ensure ((result == String2Float_LegacyStr2D_<float> (s)) or isnan (result));
#endif
        return result;
#endif
    }

    template <>
    double String2Float (const String& s)
    {
#if qCompilerAndStdLib_to_chars_FP_Buggy
        return String2Float_LegacyStr2D_<double> (s);
#else
        auto result = String2Float_NewLogic_<double> (s);
#if qDebug
        static_assert (Math::nan<double> () != Math::nan<double> ());
        Ensure ((result == String2Float_LegacyStr2D_<double> (s)) or isnan (result));
#endif
        return result;
#endif
    }

    template <>
    long double String2Float (const String& s)
    {
#if qCompilerAndStdLib_to_chars_FP_Buggy
        return String2Float_LegacyStr2D_<long double> (s);
#else
        auto result = String2Float_NewLogic_<long double> (s);
#if qDebug
        static_assert (Math::nan<long double> () != Math::nan<long double> ());
        Ensure ((result == String2Float_LegacyStr2D_<long double> (s)) or isnan (result));
#endif
        return result;
#endif
    }
}

namespace Stroika::Foundation::Characters {
    template <>
    float String2Float (const String& s, String* remainder)
    {
        return String2Float_LegacyStr2DHelper_<float> (s, remainder, wcstof);
    }

    template <>
    double String2Float (const String& s, String* remainder)
    {
        return String2Float_LegacyStr2DHelper_<double> (s, remainder, wcstod);
    }

    template <>
    long double String2Float (const String& s, String* remainder)
    {
        return String2Float_LegacyStr2DHelper_<long double> (s, remainder, wcstold);
    }
}
