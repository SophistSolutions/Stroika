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

namespace Stroika::Foundation::Characters::FloatConversion {

    /*
     ********************************************************************************
     ************************ FloatConversion::Precision ****************************
     ********************************************************************************
     */
    constexpr Precision::Precision (unsigned int p)
        : fPrecision{p}
    {
    }

    /*
     ********************************************************************************
     ********************* FloatConversion::ToStringOptions *************************
     ********************************************************************************
     */
    constexpr inline const Precision ToStringOptions::kDefaultPrecision{6};
    constexpr ToStringOptions::ToStringOptions (UseCLocale)
    {
    }
    inline ToStringOptions::ToStringOptions (const locale& l)
        : fUseLocale_{l}
    {
    }
    constexpr ToStringOptions::ToStringOptions (ios_base::fmtflags fmtFlags)
        : fFmtFlags_{fmtFlags}
    {
    }
    constexpr ToStringOptions::ToStringOptions (Precision precision)
        : fPrecision_{precision.fPrecision}
    {
    }
    constexpr ToStringOptions::ToStringOptions (FloatFormatType scientificNotation)
        : fFloatFormat_{scientificNotation}
    {
    }
    constexpr ToStringOptions::ToStringOptions (TrimTrailingZerosType trimTrailingZeros)
        : fTrimTrailingZeros_{trimTrailingZeros == TrimTrailingZerosType::eTrim}
    {
    }
    inline ToStringOptions::ToStringOptions (const ToStringOptions& b1, const ToStringOptions& b2)
        : ToStringOptions{b1}
    {
        Memory::CopyToIf (&fPrecision_, b2.fPrecision_);
        Memory::CopyToIf (&fFmtFlags_, b2.fFmtFlags_);
        Memory::CopyToIf (&fUseLocale_, b2.fUseLocale_);
        Memory::CopyToIf (&fTrimTrailingZeros_, b2.fTrimTrailingZeros_);
        Memory::CopyToIf (&fFloatFormat_, b2.fFloatFormat_);
    }
    template <typename... ARGS>
    inline ToStringOptions::ToStringOptions (const ToStringOptions& b1, const ToStringOptions& b2, ARGS&&... args)
        : ToStringOptions{ToStringOptions{b1, b2}, forward<ARGS> (args)...}
    {
    }
    inline optional<unsigned int> ToStringOptions::GetPrecision () const
    {
        return fPrecision_;
    }
    inline optional<bool> ToStringOptions::GetTrimTrailingZeros () const
    {
        return fTrimTrailingZeros_;
    }
    inline optional<locale> ToStringOptions::GetUseLocale () const
    {
        return fUseLocale_;
    }
    inline optional<FloatFormatType> ToStringOptions::GetFloatFormat () const
    {
        return fFloatFormat_;
    }
    inline optional<ios_base::fmtflags> ToStringOptions::GetIOSFmtFlags () const
    {
        return fFmtFlags_;
    }

    namespace Private_ {
        // In CPP file
        String Legacy_Float2String_ (float f, const ToStringOptions& options);
        String Legacy_Float2String_ (double f, const ToStringOptions& options);
        String Legacy_Float2String_ (long double f, const ToStringOptions& options);
    }

    namespace Private_ {
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
                    size_t len           = strResult->length ();
                    size_t pPastLastZero = len;
                    for (; (pPastLastZero - 1) > pastDot; --pPastLastZero) {
                        if ((*strResult)[pPastLastZero - 1] != '0') {
                            break;
                        }
                    }
                    if (len != pPastLastZero) [[UNLIKELY_ATTR]] { // check common case of no change, but this substring and assign already pretty optimized (not sure helps performance)
                        *strResult = strResult->SubString (0, pPastLastZero);
                    }
                }
            }
        }
    }

    namespace Private_ {
        // TEMPLATE version of wcstof(etc) - to make easier to call from generic algorithm
        template <typename T>
        T wcstoFloatType_ (const wchar_t* s, wchar_t** e);
        template <>
        inline float wcstoFloatType_ (const wchar_t* s, wchar_t** e)
        {
            return ::wcstof (s, e);
        }
        template <>
        inline double wcstoFloatType_ (const wchar_t* s, wchar_t** e)
        {
            return wcstod (s, e);
        }
        template <>
        inline long double wcstoFloatType_ (const wchar_t* s, wchar_t** e)
        {
            return wcstold (s, e);
        }
    }

    namespace Private_ {
        template <typename RETURN_TYPE>
        RETURN_TYPE String2FloatViaStrToD_ (const wchar_t* start, const wchar_t* end, const wchar_t** remainder)
        {
            if (start == end) {
                if (remainder != nullptr) {
                    *remainder = start;
                }
                return Math::nan<RETURN_TYPE> ();
            }
            else {
                wchar_t*                          e   = nullptr;
                const wchar_t*                    cst = start;
                Memory::SmallStackBuffer<wchar_t> tmp;
                if (*end != '\0') {
                    // remap addresses - copying to a temporary buffer, so we can nul-terminate string passed to strtod (etc)
                    size_t len = end - start;
                    tmp.GrowToSize (len + 1);
                    (void)::memcpy (tmp.begin (), start, len * sizeof (wchar_t));
                    cst      = tmp.begin ();
                    tmp[len] = '\0';
                }
                RETURN_TYPE d = wcstoFloatType_<RETURN_TYPE> (cst, &e);
                // If asked to return remainder do so.
                // If NOT asked to return remainder, treat not using the entire string as forcing result to be a Nan (invalid parse of number of not the whole thing is a number)
                if (remainder == nullptr) {
                    if (e != end) {
                        d = Math::nan<RETURN_TYPE> ();
                    }
                }
                else {
                    *remainder = e - cst + start; // adjust in case we remapped data
                }
                return d;
            }
        }
    }

    namespace Private_ {
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
    }

    namespace Private_ {
        template <typename FLOAT_TYPE>
        String ToString_GeneralCase_ (FLOAT_TYPE f, const ToStringOptions& options)
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
            unsigned int usePrecision = options.GetPrecision ().value_or (ToStringOptions::kDefaultPrecision.fPrecision);
            s.precision (usePrecision);

            {
                optional<ios_base::fmtflags> useFloatField;
                switch (options.GetFloatFormat ().value_or (FloatFormatType::eDEFAULT)) {
                    case FloatFormatType::eScientific:
                        useFloatField = ios_base::scientific;
                        break;
                    case FloatFormatType::eDefaultFloat:
                        break;
                    case FloatFormatType::eFixedPoint:
                        useFloatField = ios_base::fixed;
                        break;
                    case FloatFormatType::eAutomatic: {
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
    }

    namespace Private_ {
        template <typename FLOAT_TYPE>
        String ToString_String_Implementation_ (FLOAT_TYPE f, const ToStringOptions& options)
        {
            auto result =
                (not options.GetUseLocale () and not options.GetIOSFmtFlags () and not options.GetFloatFormat ())
                    ? Private_::ToString_OptimizedForCLocaleAndNoStreamFlags_ (f, options.GetPrecision ().value_or (ToStringOptions::kDefaultPrecision.fPrecision))
                    : Private_::ToString_GeneralCase_ (f, options);
            if (options.GetTrimTrailingZeros ().value_or (ToStringOptions::kDefaultTrimTrailingZeros)) {
                TrimTrailingZeros_ (&result);
            }
            Ensure (String::EqualsComparer{CompareOptions::eCaseInsensitive}(result, Legacy_Float2String_ (f, options)));
            return result;
        }
    }

    namespace Private_ {
        // Version of code from pre-Stroika 2.1b14 (so b13 or earlier roughly)
        template <typename T = double>
        T ToFloat_Legacy_ (const String& s);
        template <typename T = double>
        T ToFloat_Legacy_ (const String& s, String* remainder);

        template <>
        float ToFloat_Legacy_ (const String& s);
        template <>
        double ToFloat_Legacy_ (const String& s);
        template <>
        long double ToFloat_Legacy_ (const String& s);
        template <>
        float ToFloat_Legacy_ (const String& s, String* remainder);
        template <>
        double ToFloat_Legacy_ (const String& s, String* remainder);
        template <>
        long double ToFloat_Legacy_ (const String& s, String* remainder);

    }

    /*
     ********************************************************************************
     ************************** FloatConversion::ToString ***************************
     ********************************************************************************
     */
    template <>
    inline String ToString (float f, const ToStringOptions& options)
    {
        return Private_::ToString_String_Implementation_ (f, options);
    }
    template <>
    inline String ToString (double f, const ToStringOptions& options)
    {
        return Private_::ToString_String_Implementation_ (f, options);
    }
    template <>
    inline String ToString (long double f, const ToStringOptions& options)
    {
        return Private_::ToString_String_Implementation_ (f, options);
    }

    /*
     ********************************************************************************
     ************************ FloatConversion::ToFloat ******************************
     ********************************************************************************
     */
    template <typename T>
    T ToFloat (const wchar_t* start, const wchar_t* end)
    {
        T result; // intentionally uninitialized
        if constexpr (qCompilerAndStdLib_to_chars_FP_Buggy or qCompilerAndStdLib_from_chars_and_tochars_FP_Precision_Buggy) {
            result = Private_::String2FloatViaStrToD_<T> (start, end, nullptr);
        }
        else {
            /*
             *  Most of the time we can do this very efficiently, because there are just ascii characters.
             *  Else, fallback on older algorithm that understands full unicode character set.
             */
            Memory::SmallStackBuffer<char> asciiS;
            if (String::AsASCIIQuietly (start, end, &asciiS)) {
                auto b = asciiS.begin ();
                auto e = asciiS.end ();
                if (b != e and *b == '+') {
                    b++; // "the plus sign is not recognized outside of the exponent (only the minus sign is permitted at the beginning)" from https://en.cppreference.com/w/cpp/utility/from_chars
                }
                auto [ptr, ec] = from_chars (b, e, result);
                if (ec == errc::result_out_of_range) [[UNLIKELY_ATTR]] {
                    return *b == '-' ? -numeric_limits<T>::infinity () : numeric_limits<T>::infinity ();
                }
                // if error or trailing crap - return nan
                if (ec != std::errc{} or ptr != e) {
                    result = Math::nan<T> ();
                }
            }
            else {
                result = Private_::String2FloatViaStrToD_<T> (start, end, nullptr);
            }
        }
        if constexpr (qDebug) {
            // test backward compat with old algorithm --LGP 2021-11-08
            if (isnan (result)) {
                Ensure (isnan (Private_::ToFloat_Legacy_<T> (String{start, end})));
            }
            else {
                Ensure (result == Private_::ToFloat_Legacy_<T> (String{start, end}));
            }
        }
        return result;
    }
    template <typename T>
    inline T ToFloat (const String& s)
    {
        auto [start, end] = s.GetData<wchar_t> ();
        return ToFloat<T> (start, end);
    }
    template <typename T>
    inline T ToFloat (const wchar_t* start, const wchar_t* end, const wchar_t** remainder)
    {
        RequireNotNull (remainder);
        T result; // intentionally uninitialized
        if constexpr (qCompilerAndStdLib_to_chars_FP_Buggy or qCompilerAndStdLib_from_chars_and_tochars_FP_Precision_Buggy) {
            result = Private_::String2FloatViaStrToD_<T> (start, end, remainder);
        }
        else {
            /*
             *  Most of the time we can do this very efficiently, because there are just ascii characters.
             *  Else, fallback on older algorithm that understands full unicode character set.
             */
            Memory::SmallStackBuffer<char> asciiS;
            if (String::AsASCIIQuietly (start, end, &asciiS)) {
                auto b = asciiS.begin ();
                auto e = asciiS.end ();
                if (b != e and *b == '+') {
                    b++; // "the plus sign is not recognized outside of the exponent (only the minus sign is permitted at the beginning)" from https://en.cppreference.com/w/cpp/utility/from_chars
                }
                auto [ptr, ec] = from_chars (b, e, result);
                if (ec == errc::result_out_of_range) [[UNLIKELY_ATTR]] {
                    return *b == '-' ? -numeric_limits<T>::infinity () : numeric_limits<T>::infinity ();
                }
                // if error or trailing crap - return nan
                if (ec != std::errc{} or ptr != e) {
                    result = Math::nan<T> ();
                }
            }
            else {
                result = Private_::String2FloatViaStrToD_<T> (start, end, remainder);
            }
        }
        if constexpr (qDebug) {
            // test backward compat with old algorithm --LGP 2021-11-15
            String tmpRem;
            if (isnan (result)) {
                Ensure (isnan (Private_::ToFloat_Legacy_<T> (String{start, end}, &tmpRem)));
                Ensure (tmpRem == *remainder);
            }
            else {
                Ensure (result == Private_::ToFloat_Legacy_<T> (String{start, end}, &tmpRem));
                Ensure (tmpRem == *remainder);
            }
        }
        return result;
    }
    template <typename T>
    inline T ToFloat (const String& s, String* remainder)
    {
        RequireNotNull (remainder);
        auto [start, end]           = s.GetData<wchar_t> ();
        const wchar_t* tmpRemainder = nullptr;
        auto           result       = ToFloat<T> (start, end, &tmpRemainder);
        AssertNotNull (tmpRemainder);
        *remainder = String{tmpRemainder};
        return result;
    }

}

namespace Stroika::Foundation::Characters {
    using Float2StringOptions [[deprecated ("Since Stroika 2.1b14, use FloatConversion::ToStringOptions")]] = FloatConversion::ToStringOptions;
    [[deprecated ("Since Stroika 2.1b14, use FloatConversion::ToString")]] inline String Float2String (float f, const FloatConversion::ToStringOptions& options = FloatConversion::ToStringOptions{})
    {
        return FloatConversion::ToString (f, options);
    }
    [[deprecated ("Since Stroika 2.1b14, use FloatConversion::ToString")]] inline String Float2String (double f, const FloatConversion::ToStringOptions& options = FloatConversion::ToStringOptions{})
    {
        return FloatConversion::ToString (f, options);
    }
    [[deprecated ("Since Stroika 2.1b14, use FloatConversion::ToString")]] inline String Float2String (long double f, const FloatConversion::ToStringOptions& options = FloatConversion::ToStringOptions{})
    {
        return FloatConversion::ToString (f, options);
    }
    template <typename T = double>
    [[deprecated ("Since Stroika 2.1b14, use FloatConversion::ToFloat")]] inline T String2Float (const wchar_t* start, const wchar_t* end)
    {
        return FloatConversion::ToFloat<T> (start, end);
    }
    template <typename T = double>
    [[deprecated ("Since Stroika 2.1b14, use FloatConversion::ToFloat")]] inline T String2Float (const String& s)
    {
        return FloatConversion::ToFloat<T> (s);
    }
    template <typename T = double>
    [[deprecated ("Since Stroika 2.1b14, use FloatConversion::ToFloat")]] inline T String2Float (const String& s, String* remainder)
    {
        return FloatConversion::ToFloat<T> (s, remainder);
    }
}

#endif /*_Stroika_Foundation_Characters_FloatConversion_inl_*/
