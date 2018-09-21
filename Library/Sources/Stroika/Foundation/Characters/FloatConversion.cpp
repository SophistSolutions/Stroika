/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include <cstdarg>
#include <cstdlib>
#include <iomanip>
#include <limits>
#include <sstream>

#include "../Characters/StringBuilder.h"
#include "../Characters/String_Constant.h"
#include "../Containers/Common.h"
#include "../Debug/Assertions.h"
#include "../Debug/Trace.h"
#include "../Math/Common.h"
#include "../Memory/SmallStackBuffer.h"
#include "CodePage.h"

#include "FloatConversion.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Memory;

/*
 ********************************************************************************
 ************************** Float2StringOptions *********************************
 ********************************************************************************
 */
Float2StringOptions::Float2StringOptions (UseCurrentLocale)
    : fUseLocale_ (locale{})
{
}

String Float2StringOptions::ToString () const
{
    StringBuilder sb;
    sb += L"{";
    if (fPrecision_) {
        sb += L"Precision:" + Characters::ToString ((int)*fPrecision_) + L",";
    }
    if (fFmtFlags_) {
        sb += L"Fmt-Flags:" + Characters::ToString ((int)*fFmtFlags_) + L",";
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
    inline void TrimTrailingZeros_ (String* strResult)
    {
        RequireNotNull (strResult);
        // strip trailing zeros - except for the last first one after the decimal point.
        // And don't do if ends with exponential notation e+40 shouldnt get shortned to e+4!
        bool hasE = strResult->Find ('e', CompareOptions::eCaseInsensitive).has_value ();
        //Assert (hasE == (strResult->find ('e') != String::npos or strResult->find ('E') != String::npos));
        if (not hasE) {
            size_t pastDot = strResult->find ('.');
            if (pastDot != String::npos) {
                pastDot++;
                size_t pPastLastZero = strResult->length ();
                for (; (pPastLastZero - 1) > pastDot; --pPastLastZero) {
                    if ((*strResult)[pPastLastZero - 1] != '0') {
                        break;
                    }
                }
                *strResult = strResult->SubString (0, pPastLastZero);
            }
        }
    }
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
        SmallStackBuffer<char> buf (SmallStackBuffer<char>::eUninitialized, sz);
        char                   format[100];
        int                    resultStrLen = ::snprintf (buf, buf.size (), mkFmtWithPrecisionArg_ (std::begin (format), std::end (format), is_same_v<FLOAT_TYPE, long double> ? 'L' : '\0'), (int)precision, f);
        Verify (resultStrLen > 0 and resultStrLen < static_cast<int> (sz));
        String tmp = String::FromASCII (buf.begin (), buf.begin () + resultStrLen);
        if (trimTrailingZeros) {
            TrimTrailingZeros_ (&tmp);
        }
        return tmp;
    }

    template <typename FLOAT_TYPE>
    inline String Float2String_GenericCase_ (FLOAT_TYPE f, const Float2StringOptions& options)
    {
        Require (not isnan (f));
        Require (not isinf (f));

        // expensive to construct, and slightly cheaper to just use thread_local version of
        // the same stringstream each time (only one per thread can be in use)
        static thread_local stringstream s;
        static const ios_base::fmtflags    kDefaultIOSFmtFlags_ = s.flags (); // Just copy out of the first constructed stringstream

        s.str (string ());
        s.clear ();

        static const locale kCLocale_ = locale::classic ();
        s.imbue (options.GetUseLocale ().value_or (kCLocale_));

        //  must set explictly (even if defaulted)  because of the thread_local thing
        s.flags (options.GetIOSFmtFlags ().value_or (kDefaultIOSFmtFlags_));

        // todo must set default precision because of the thread_local thing
        unsigned int usePrecision = options.GetPrecision ().value_or (kDefaultPrecision.fPrecision);
        s.precision (usePrecision);

        {
            optional<ios_base::fmtflags> useFloatField;
            switch (options.GetFloatFormat ().value_or (Float2StringOptions::FloatFormatType::eDEFAULT)) {
                case Float2StringOptions::FloatFormatType::eScientific:
                    useFloatField = ios_base::scientific;
                    break;
                case Float2StringOptions::FloatFormatType::eDefaultFloat:
                    break;
                case Float2StringOptions::FloatFormatType::eFixedPoint:
                    useFloatField = ios_base::fixed;
                    break;
                case Float2StringOptions::FloatFormatType::eAutomatic: {
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
        if (options.GetTrimTrailingZeros ().value_or (Float2StringOptions::kDefaultTrimTrailingZeros)) {
            TrimTrailingZeros_ (&tmp);
        }
        return tmp;
    }
    template <typename FLOAT_TYPE>
    inline String Float2String_ (FLOAT_TYPE f, const Float2StringOptions& options)
    {
        switch (fpclassify (f)) {
            case FP_INFINITE: {
                Assert (isinf (f));
                Assert (!isnan (f));
                static const String_Constant kNEG_INF_STR_{L"-INF"};
                static const String_Constant kINF_STR_{L"INF"};
                return f > 0 ? kINF_STR_ : kNEG_INF_STR_;
            }
            case FP_NAN: {
                Assert (isnan (f));
                Assert (!isinf (f));
                static const String_Constant kNAN_STR_{L"NAN"};
                return kNAN_STR_;
            }
        }
        Assert (!isnan (f));
        Assert (!isinf (f));

        if (not options.GetUseLocale ().has_value () and not options.GetIOSFmtFlags ().has_value () and not options.GetFloatFormat ().has_value ()) {
            auto result = Float2String_OptimizedForCLocaleAndNoStreamFlags_ (f, options.GetPrecision ().value_or (kDefaultPrecision.fPrecision), options.GetTrimTrailingZeros ().value_or (Float2StringOptions::kDefaultTrimTrailingZeros));
            Ensure (result == Float2String_GenericCase_<FLOAT_TYPE> (f, options));
            return result;
        }
        return Float2String_GenericCase_<FLOAT_TYPE> (f, options);
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
    template <typename RETURN_TYPE, typename FUNCTION>
    inline RETURN_TYPE String2Float_ (const String& s, FUNCTION F)
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
}

namespace Stroika::Foundation::Characters {
    template <>
    float String2Float (const String& s)
    {
        return String2Float_<float> (s, wcstof);
    }

    template <>
    double String2Float (const String& s)
    {
        return String2Float_<double> (s, wcstod);
    }

    template <>
    long double String2Float (const String& s)
    {
        return String2Float_<long double> (s, wcstold);
    }
}
