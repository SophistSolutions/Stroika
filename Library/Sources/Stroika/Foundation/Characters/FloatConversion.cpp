/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include <charconv>
#include <cstdarg>
#include <cstdlib>
#include <iomanip>
#include <limits>
#include <sstream>

#include "Stroika/Foundation/Characters/StringBuilder.h"
#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/Containers/Common.h"
#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/Debug/Valgrind.h"
#include "Stroika/Foundation/Math/Common.h"

#include "FloatConversion.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Memory;

/*
 ********************************************************************************
 ************************ FloatConversion::Precision ****************************
 ********************************************************************************
 */
String FloatConversion::Precision::ToString () const
{
    StringBuilder sb;
    sb << "{"sv;
    if (fPrecision) {
        sb << "Precision:"sv << *fPrecision;
    }
    else {
        sb << "FULL";
    }
    sb << "}"sv;
    return sb;
}

/*
 ********************************************************************************
 ******************** FloatConversion::ToStringOptions **************************
 ********************************************************************************
 */
String FloatConversion::ToStringOptions::ToString () const
{
    StringBuilder sb;
    sb << "{"sv;
    if (fPrecision_) {
        sb << "Precision:"sv << *fPrecision_ << ","sv;
    }
    if (fFmtFlags_) {
        sb << "Fmt-Flags:"sv << Characters::ToString ((int)*fFmtFlags_, ios_base::hex) << ","sv;
    }
    if (fUseLocale_) {
        sb << "Use-Locale"sv << String::FromNarrowSDKString (fUseLocale_->name ()) << ","sv;
    }
    if (fTrimTrailingZeros_) {
        sb << "Trim-Trailing-Zeros: "sv << *fTrimTrailingZeros_ << ","sv;
    }
    if (fFloatFormat_) {
        sb << "Scientific-Notation: "sv << (int)*fFloatFormat_ << ","sv;
    }
    sb << "}"sv;
    return sb;
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

        s.imbue (options.GetUseLocale ());

        //  must set explictly (even if defaulted)  because of the thread_local thing
        s.flags (options.GetIOSFmtFlags ().value_or (kDefaultIOSFmtFlags_));

        // todo must set default precision because of the thread_local thing
        unsigned int usePrecision =
            options.GetPrecision ().value_or (FloatConversion::ToStringOptions::kDefaultPrecision).GetEffectivePrecision<FLOAT_TYPE> ();
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
                case FloatConversion::FloatFormatType::eAutomaticScientific: {
                    bool useScientificNotation = abs (f) >= pow (10, usePrecision / 2) or
                                                 (f != 0 and abs (f) < pow (10, -static_cast<int> (usePrecision) / 2)); // scientific preserves more precision - but non-scientific looks better
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

        String tmp = options.GetUsingLocaleClassic () ? String{s.str ()} : String::FromNarrowString (s.str (), options.GetUseLocale ());
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
                static const String kNEG_INF_STR_{"-INF"_k};
                static const String kINF_STR_{"INF"_k};
                return f > 0 ? kINF_STR_ : kNEG_INF_STR_;
            }
            case FP_NAN: {
                Assert (isnan (f));
                Assert (!isinf (f));
                static const String kNAN_STR_{"NAN"_k};
                return kNAN_STR_;
            }
            default: {
                if constexpr (qCompilerAndStdLib_valgrind_fpclassify_check_Buggy && qStroika_Foundation_Debug_AssertionsChecked) {
                    if (Debug::IsRunningUnderValgrind ()) {
                        if (isinf (f)) {
                            DbgTrace ("fpclassify ({}) = {}"_f, (double)f, fpclassify (f));
                            static const String kNEG_INF_STR_{"-INF"_k};
                            static const String kINF_STR_{"INF"_k};
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
