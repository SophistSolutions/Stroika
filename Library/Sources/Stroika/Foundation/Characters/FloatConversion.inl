/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_FloatConversion_inl_
#define _Stroika_Foundation_Characters_FloatConversion_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include <charconv>

#include "../Containers/Common.h"
#include "../Memory/Optional.h"
#include "../Memory/SmallStackBuffer.h"

namespace Stroika::Foundation::Characters {

    /*
     ********************************************************************************
     *************** FloatConversion::ToStringOptions::Precision ********************
     ********************************************************************************
     */
    constexpr FloatConversion::ToStringOptions::Precision::Precision (unsigned int p)
        : fPrecision{p}
    {
    }
    constexpr inline const FloatConversion::ToStringOptions::Precision FloatConversion::ToStringOptions::kDefaultPrecision{6};

    /*
     ********************************************************************************
     ********************* FloatConversion::ToStringOptions *************************
     ********************************************************************************
     */
    constexpr FloatConversion::ToStringOptions::ToStringOptions (UseCLocale)
    {
    }
    inline FloatConversion::ToStringOptions::ToStringOptions (const locale& l)
        : fUseLocale_{l}
    {
    }
    constexpr FloatConversion::ToStringOptions::ToStringOptions (ios_base::fmtflags fmtFlags)
        : fFmtFlags_{fmtFlags}
    {
    }
    constexpr FloatConversion::ToStringOptions::ToStringOptions (Precision precision)
        : fPrecision_{precision.fPrecision}
    {
    }
    constexpr FloatConversion::ToStringOptions::ToStringOptions (FloatFormatType scientificNotation)
        : fFloatFormat_{scientificNotation}
    {
    }
    constexpr FloatConversion::ToStringOptions::ToStringOptions (TrimTrailingZerosType trimTrailingZeros)
        : fTrimTrailingZeros_{trimTrailingZeros == TrimTrailingZerosType::eTrim}
    {
    }
    inline FloatConversion::ToStringOptions::ToStringOptions (const ToStringOptions& b1, const ToStringOptions& b2)
        : Float2StringOptions{b1}
    {
        Memory::CopyToIf (&fPrecision_, b2.fPrecision_);
        Memory::CopyToIf (&fFmtFlags_, b2.fFmtFlags_);
        Memory::CopyToIf (&fUseLocale_, b2.fUseLocale_);
        Memory::CopyToIf (&fTrimTrailingZeros_, b2.fTrimTrailingZeros_);
        Memory::CopyToIf (&fFloatFormat_, b2.fFloatFormat_);
    }
    template <typename... ARGS>
    inline FloatConversion::ToStringOptions::ToStringOptions (const ToStringOptions& b1, const ToStringOptions& b2, ARGS&&... args)
        : ToStringOptions{ToStringOptions{b1, b2}, forward<ARGS> (args)...}
    {
    }
    inline optional<unsigned int> FloatConversion::ToStringOptions::GetPrecision () const
    {
        return fPrecision_;
    }
    inline optional<bool> FloatConversion::ToStringOptions::GetTrimTrailingZeros () const
    {
        return fTrimTrailingZeros_;
    }
    inline optional<locale> FloatConversion::ToStringOptions::GetUseLocale () const
    {
        return fUseLocale_;
    }
    inline optional<FloatConversion::ToStringOptions::FloatFormatType> FloatConversion::ToStringOptions::GetFloatFormat () const
    {
        return fFloatFormat_;
    }
    inline optional<ios_base::fmtflags> FloatConversion::ToStringOptions::GetIOSFmtFlags () const
    {
        return fFmtFlags_;
    }

    namespace Private_ {
        String      Legacy_Float2String_ (float f, const FloatConversion::ToStringOptions& options);
        String      Legacy_Float2String_ (double f, const FloatConversion::ToStringOptions& options);
        String      Legacy_Float2String_ (long double f, const FloatConversion::ToStringOptions& options);
        inline void TrimTrailingZeros_ (String* strResult)
        {
            // @todo THIS could be more efficeint. We should KNOW case of the 'e' and maybe able to tell/avoid looking based on args to String2Float
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
        template <typename FLOAT_TYPE>
        inline String ToString_OptimizedForCLocaleAndNoStreamFlags_ (FLOAT_TYPE f, int precision)
        {
            using namespace Memory;
            size_t                 sz = precision + 100; // I think precision is enough
            SmallStackBuffer<char> buf{SmallStackBuffer<char>::eUninitialized, sz};
            ptrdiff_t              resultStrLen;
            if constexpr (qCompilerAndStdLib_to_chars_FP_Buggy) {
                auto mkFmtWithPrecisionArg_ = [] (char* formatBufferStart, [[maybe_unused]] char* formatBufferEnd, char _Spec) -> char* {
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
                };
                char format[100]; // intentionally uninitialized, cuz filled in with mkFmtWithPrecisionArg_
                resultStrLen = ::snprintf (buf, buf.size (), mkFmtWithPrecisionArg_ (std::begin (format), std::end (format), is_same_v<FLOAT_TYPE, long double> ? 'L' : '\0'), (int)precision, f);
            }
            else {
                // THIS #if test should NOT be needed but g++ 9 didn't properly respect if constexpr
#if !qCompilerAndStdLib_to_chars_FP_Buggy
                // empirically, on MSVC, this is much faster (appears 3x apx faster) -- LGP 2021-11-04
                resultStrLen = to_chars (buf.begin (), buf.end (), f, chars_format::general, precision).ptr - buf.begin ();
#endif
            }
            Verify (resultStrLen > 0 and resultStrLen < static_cast<int> (sz));
            return String::FromASCII (buf.begin (), buf.begin () + resultStrLen);
        }
        template <typename FLOAT_TYPE>
        String ToString_GeneralCase_ (FLOAT_TYPE f, const FloatConversion::ToStringOptions& options)
        {
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

            return options.GetUseLocale () ? String::FromNarrowString (s.str (), *options.GetUseLocale ()) : String::FromASCII (s.str ());
        }
        template <typename FLOAT_TYPE>
        String ToString_String_Implementation_ (FLOAT_TYPE f, const FloatConversion::ToStringOptions& options)
        {
            auto result =
                (not options.GetUseLocale () and not options.GetIOSFmtFlags () and not options.GetFloatFormat ())
                    ? Private_::ToString_OptimizedForCLocaleAndNoStreamFlags_ (f, options.GetPrecision ().value_or (FloatConversion::ToStringOptions::kDefaultPrecision.fPrecision))
                    : ToString_GeneralCase_ (f, options);
            if (options.GetTrimTrailingZeros ().value_or (Float2StringOptions::kDefaultTrimTrailingZeros)) {
                TrimTrailingZeros_ (&result);
            }
            Ensure (String::EqualsComparer{CompareOptions::eCaseInsensitive}(result, Legacy_Float2String_ (f, options)));
            return result;
        }
    }

    /*
     ********************************************************************************
     ************************** FloatConversion::ToString ***************************
     ********************************************************************************
     */
    template <>
    inline String FloatConversion::ToString (float f, const ToStringOptions& options)
    {
        return Private_::ToString_String_Implementation_ (f, options);
    }
    template <>
    inline String FloatConversion::ToString (double f, const ToStringOptions& options)
    {
        return Private_::ToString_String_Implementation_ (f, options);
    }
    template <>
    inline String FloatConversion::ToString (long double f, const ToStringOptions& options)
    {
        return Private_::ToString_String_Implementation_ (f, options);
    }

    /*
     ********************************************************************************
     ********************************* String2Float *********************************
     ********************************************************************************
     */
    template <typename T>
    T String2Float (const wchar_t* start, const wchar_t* end)
    {
        Require (start != nullptr and end != nullptr and start != end);
        if constexpr (qCompilerAndStdLib_to_chars_FP_Buggy or qCompilerAndStdLib_from_chars_and_tochars_FP_Precision_Buggy) {
            return String2Float (String{start, end});
        }
        else {
            /*
             *  Most of the time we can do this very efficiently, because there are just ascii characters.
             *  Else, fallback on older algorithm that understands full unicode character set.
             */
            Memory::SmallStackBuffer<char> asciiS;
            if (String::AsASCIIQuietly (start, end, &asciiS)) {
                T    r; // intentionally uninitialized
                auto b = asciiS.begin ();
                auto e = asciiS.end ();
                if (b != e and *b == '+') {
                    b++; // "the plus sign is not recognized outside of the exponent (only the minus sign is permitted at the beginning)" from https://en.cppreference.com/w/cpp/utility/from_chars
                }
                auto [ptr, ec] = from_chars (b, e, r);
                if (ec == errc::result_out_of_range) [[UNLIKELY_ATTR]] {
                    return *b == '-' ? -numeric_limits<T>::infinity () : numeric_limits<T>::infinity ();
                }
                // if error or trailing crap - return nan
                auto result = (ec == std::errc () and ptr == e) ? r : Math::nan<T> ();
                Ensure (result == String2Float (String{start, end})); // test backward compat with old algorithm --LGP 2021-11-08
                return result;
            }
            else {
                return String2Float (String{start, end});
            }
        }
    }

}

#endif /*_Stroika_Foundation_Characters_FloatConversion_inl_*/
