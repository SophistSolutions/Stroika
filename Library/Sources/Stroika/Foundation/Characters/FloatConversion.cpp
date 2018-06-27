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
Float2StringOptions::Float2StringOptions (UseCLocale)
{
    static const locale kCLocale_ = locale::classic ();
    fUseLocale_                   = kCLocale_;
}

Float2StringOptions::Float2StringOptions (UseCurrentLocale)
    : fUseLocale_ (locale ())
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
    template <typename FLOAT_TYPE>
    inline String Float2String_ (FLOAT_TYPE f, const Float2StringOptions& options)
    {
        if (std::isnan (f)) {
            static const String_Constant kNAN_STR_{L"NAN"};
            return kNAN_STR_;
        }
        else if (std::isinf (f)) {
            static const String_Constant kNEG_INF_STR_{L"-INF"};
            static const String_Constant kINF_STR_{L"INF"};
            return f > 0 ? kINF_STR_ : kNEG_INF_STR_;
        }
        stringstream s;

        static const locale kCLocale_ = locale::classic ();
        s.imbue (options.GetUseLocale ().value_or (kCLocale_));

        if (options.GetIOSFmtFlags ()) {
            s << setiosflags (*options.GetIOSFmtFlags ());
        }

        Optional<unsigned int> precision{options.GetPrecision ()};
        unsigned int           usePrecision = precision ? *precision : static_cast<unsigned int> (s.precision ());
        if (precision) {
            s << setprecision (usePrecision);
        }

        switch (options.GetFloatFormat ().value_or (Float2StringOptions::FloatFormatType::eDEFAULT)) {
            case Float2StringOptions::FloatFormatType::eScientific:
                s.setf (std::ios_base::scientific, std::ios_base::floatfield);
                break;
            case Float2StringOptions::FloatFormatType::eDefaultFloat:
                s.unsetf (std::ios_base::floatfield); // see std::defaultfloat - not same as std::ios_base::fixed
                break;
            case Float2StringOptions::FloatFormatType::eFixedPoint:
                s.setf (std::ios_base::fixed, std::ios_base::floatfield);
                break;
            case Float2StringOptions::FloatFormatType::eAutomatic: {
                bool useScientificNotation = abs (f) >= std::pow (10, usePrecision / 2) or (f != 0 and abs (f) < std::pow (10, -static_cast<int> (usePrecision) / 2)); // scientific preserves more precision - but non-scientific looks better
                if (useScientificNotation) {
                    s.setf (std::ios_base::scientific, std::ios_base::floatfield);
                }
                else {
                    s.unsetf (std::ios_base::floatfield); // see std::defaultfloat - not same as std::ios_base::fixed
                }
            } break;
            default:
                RequireNotReached ();
                break;
        }

        s << f;

        String tmp = options.GetUseLocale () ? String::FromNarrowString (s.str (), *options.GetUseLocale ()) : String::FromASCII (s.str ());

        bool useTrimTrailingZeros = options.GetTrimTrailingZeros ().value_or (true);
        if (useTrimTrailingZeros) {
            // strip trailing zeros - except for the last first one after the decimal point.
            // And don't do if ends with exponential notation e+40 shouldnt get shortned to e+4!
            bool hasE = tmp.find ('e') != String::npos or tmp.find ('E') != String::npos;
            if (not hasE) {
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

namespace Stroika {
    namespace Foundation {
        namespace Characters {
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
    }
}
