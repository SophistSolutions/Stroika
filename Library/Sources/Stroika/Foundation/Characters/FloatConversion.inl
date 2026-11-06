/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include <charconv>
#include <sstream>

#include "Stroika/Foundation/Characters/CString/Utilities.h"
#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/Characters/UTFConvert.h"
#include "Stroika/Foundation/Common/StdCompat.h"
#include "Stroika/Foundation/Containers/Common.h"
#include "Stroika/Foundation/Math/Common.h"
#include "Stroika/Foundation/Memory/Common.h"
#include "Stroika/Foundation/Memory/Optional.h"
#include "Stroika/Foundation/Memory/StackBuffer.h"

namespace Stroika::Foundation::Characters::FloatConversion {

    namespace Private_ {
        // TEMPLATE version of wcstof(etc) - to make easier to call from generic algorithm
        template <typename T>
        T CStr2FloatType_ (const wchar_t* s, wchar_t** e);
        template <typename T>
        T CStr2FloatType_ (const char* s, char** e);
        template <>
        inline float CStr2FloatType_ (const wchar_t* s, wchar_t** e)
        {
            return ::wcstof (s, e);
        }
        template <>
        inline double CStr2FloatType_ (const wchar_t* s, wchar_t** e)
        {
            return wcstod (s, e);
        }
        template <>
        inline long double CStr2FloatType_ (const wchar_t* s, wchar_t** e)
        {
            return wcstold (s, e);
        }
        template <>
        inline float CStr2FloatType_ (const char* s, char** e)
        {
            return ::strtof (s, e);
        }
        template <>
        inline double CStr2FloatType_ (const char* s, char** e)
        {
            return strtod (s, e);
        }
        template <>
        inline long double CStr2FloatType_ (const char* s, char** e)
        {
            return strtold (s, e);
        }
    }

    namespace Private_ {
        template <typename RETURN_TYPE>
        RETURN_TYPE ToFloat_ViaStrToD_ (const char* start, const char* end, const char** remainder)
        {
            Require (start <= end);
            if (start == end) {
                if (remainder != nullptr) {
                    *remainder = start;
                }
                return Math::nan<RETURN_TYPE> ();
            }
            else {
                char*       e   = nullptr;
                const char* cst = start;

                if (remainder == nullptr) {
                    // REJECT strings with leading space in this case - must match exactly
                    if (::isspace (*cst)) {
                        return Math::nan<RETURN_TYPE> ();
                    }
                }

                Memory::StackBuffer<char> tmp;
                if (*end != '\0') {
                    // remap addresses - copying to a temporary buffer, so we can nul-terminate string passed to strtod (etc)
                    size_t len = end - start;
                    tmp.GrowToSize (len + 1);
                    (void)::memcpy (tmp.begin (), start, len);
                    cst      = tmp.begin ();
                    tmp[len] = '\0';
                }
                RETURN_TYPE d = CStr2FloatType_<RETURN_TYPE> (cst, &e);
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
        template <typename RETURN_TYPE>
        RETURN_TYPE ToFloat_ViaStrToD_ (const wchar_t* start, const wchar_t* end, const wchar_t** remainder)
        {
            Require (start <= end);
            if (start == end) {
                if (remainder != nullptr) {
                    *remainder = start;
                }
                return Math::nan<RETURN_TYPE> ();
            }
            else {
                wchar_t*       e   = nullptr;
                const wchar_t* cst = start;

                if (remainder == nullptr) {
                    // REJECT strings with leading space in this case - must match exactly
                    if (::iswspace (*cst)) {
                        return Math::nan<RETURN_TYPE> ();
                    }
                }

                Memory::StackBuffer<wchar_t> tmp;
                if (*end != '\0') {
                    // remap addresses - copying to a temporary buffer, so we can nul-terminate string passed to strtod (etc)
                    size_t len = end - start;
                    tmp.GrowToSize (len + 1);
                    (void)::memcpy (tmp.begin (), start, len * sizeof (wchar_t));
                    cst      = tmp.begin ();
                    tmp[len] = '\0';
                }
                RETURN_TYPE d = CStr2FloatType_<RETURN_TYPE> (cst, &e);
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
        //
        // This RESPECTS THE CURRENT LOCALE
        // this private function allows nullptr remainder, and behaves somewhat differently if null or not (unlike PUBLIC API)
        // Roughly (ignoring precision and character set)
        //      Calls strtod
        //      Strtod Skips leading whitespace, but this routine does NOT allow that
        //      if remainder==null, rejects if not all characters eaten (else returns how many eaten in remainder)
        //
        // Note - inlined though long, because most of the logic gets compiled out depending on (template or actual)
        // parameters, and want to assure those 'seen' by optimizer so most of the code eliminated.
        //
        template <typename T, IUNICODECanUnambiguouslyConvertFrom CHAR_T>
        inline T ToFloat_RespectingLocale_ (const span<const CHAR_T> srcSpan, typename span<const CHAR_T>::iterator* remainder)
        {
            if (srcSpan.empty ()) {
                if (remainder != nullptr) {
                    *remainder = srcSpan.begin ();
                }
                return Math::nan<T> ();
            }

            // because strtod, etc, require a NUL-terminated string, and span doesn't generally provide one,
            // we must copy to a temporary buffer (not super costly, especially since this isn't the main
            // approach tried typically)
            Memory::StackBuffer<CHAR_T> srcBufWithNul{Memory::eUninitialized, srcSpan.size () + 1};
            Memory::CopyBytes (srcSpan, span{srcBufWithNul});
            srcBufWithNul[srcSpan.size ()] = '\0';
            const CHAR_T* si               = srcBufWithNul.begin ();
            const CHAR_T* ei               = srcBufWithNul.end () - 1; // buf nul-terminated, but dont treat the NUL as part of our length
            const CHAR_T* ri               = ei;

            // since strtod skips leading whitespace, prevent that
            if (remainder == nullptr) {
                bool isSpace;
                if constexpr (sizeof (CHAR_T) == 1) {
                    isSpace = std::isspace (*si);
                }
                else if constexpr (sizeof (CHAR_T) == sizeof (wchar_t)) {
                    isSpace = std::iswspace (*si);
                }
                else {
                    isSpace = std::iswspace (static_cast<wint_t> (*si)); // not sure how to check without complex conversion logic
                }
                if (isSpace) {
                    if (remainder != nullptr) {
                        *remainder = srcSpan.begin ();
                    }
                    return Math::nan<T> ();
                }
            }
            Stroika_ATTRIBUTE_INDETERMINATE T d; //  - set below
            static_assert (same_as<T, float> or same_as<T, double> or same_as<T, long double>);
            if constexpr (sizeof (CHAR_T) == 1) {
                if constexpr (same_as<T, float>) {
                    d = ::strtof (reinterpret_cast<const char*> (si), const_cast<char**> (reinterpret_cast<const char**> (&ri)));
                }
                else if constexpr (same_as<T, double>) {
                    d = ::strtod (reinterpret_cast<const char*> (si), const_cast<char**> (reinterpret_cast<const char**> (&ri)));
                }
                else if constexpr (same_as<T, long double>) {
                    d = ::strtold (reinterpret_cast<const char*> (si), const_cast<char**> (reinterpret_cast<const char**> (&ri)));
                }
                else {
                    AssertNotReached ();
                }
            }
            else if constexpr (sizeof (CHAR_T) == sizeof (wchar_t)) {
                if constexpr (same_as<T, float>) {
                    d = ::wcstof (reinterpret_cast<const wchar_t*> (si), const_cast<wchar_t**> (reinterpret_cast<const wchar_t**> (&ri)));
                }
                else if constexpr (same_as<T, double>) {
                    d = ::wcstod (reinterpret_cast<const wchar_t*> (si), const_cast<wchar_t**> (reinterpret_cast<const wchar_t**> (&ri)));
                }
                else if constexpr (same_as<T, long double>) {
                    d = ::wcstold (reinterpret_cast<const wchar_t*> (si), const_cast<wchar_t**> (reinterpret_cast<const wchar_t**> (&ri)));
                }
                else {
                    AssertNotReached ();
                }
            }
            else {
                // must utf convert to wchar_t which we support
                Memory::StackBuffer<wchar_t> wideBuf{Memory::eUninitialized, UTFConvert::ComputeTargetBufferSize<wchar_t> (srcSpan)};
                span<const wchar_t>          wideSpan = UTFConvert::kThe.ConvertSpan (srcSpan, span{wideBuf});
                if (remainder == nullptr) {
                    d = ToFloat_RespectingLocale_<T, wchar_t> (wideSpan, nullptr);
                }
                else {
                    // do the conversion using wchar_t, and then map back the resulting remainder offset
                    span<const wchar_t>::iterator wideRemainder;
                    d  = ToFloat_RespectingLocale_<T> (wideSpan, &wideRemainder);
                    ri = UTFConvert::kThe.ConvertOffset<CHAR_T> (wideSpan, wideRemainder - wideSpan.begin ()) + si;
                }
            }
            if (remainder == nullptr) {
                // if no remainder argument, we require all the text is 'eaten'
                if (ri != ei) {
                    d = Math::nan<T> ();
                }
            }
            else {
                *remainder = srcSpan.begin () + (ri - si);
            }
            return d;
        }

    }

    DISABLE_COMPILER_MSC_WARNING_START (4996)
    DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Wdeprecated-declarations\"");
    DISABLE_COMPILER_CLANG_WARNING_START ("clang diagnostic ignored \"-Wdeprecated-declarations\"");

    /*
     ********************************************************************************
     ********************** FloatConversion::ToStringOptions ************************
     ********************************************************************************
     */
    constexpr ToStringOptions::ToStringOptions (Common::PredefinedLocale p)
        : fPunctuation_{Number::PunctuationSpecifier{p}}
    {
    }
    inline ToStringOptions::ToStringOptions (const locale& l)
        : fPunctuation_{Number::Punctuation::FromLocale (l)}
    {
    }
    constexpr ToStringOptions::ToStringOptions (const Number::Punctuation& p)
        : fPunctuation_{Number::PunctuationSpecifier{p}}
    {
    }
    constexpr ToStringOptions::ToStringOptions (ios_base::fmtflags fmtFlags)
        : fFmtFlags_{fmtFlags}
    {
    }
    constexpr ToStringOptions::ToStringOptions (Number::SignificantFigures precision)
        : fSignificantFigures_{precision}
    {
    }
    constexpr ToStringOptions::ToStringOptions (FloatFormatType floatFormat)
        : fFloatFormat_{floatFormat}
    {
    }
    constexpr ToStringOptions::ToStringOptions (TrimTrailingZerosType trimTrailingZeros)
        : fTrimTrailingZeros_{trimTrailingZeros == TrimTrailingZerosType::eTrimZeros}
    {
    }
    constexpr ToStringOptions::ToStringOptions (const ToStringOptions& b1, const ToStringOptions& b2)
        : ToStringOptions{b1}
    {
        Memory::CopyToIf (&fSignificantFigures_, b2.fSignificantFigures_);
        Memory::CopyToIf (&fFmtFlags_, b2.fFmtFlags_);
        Memory::CopyToIf (&fPunctuation_, b2.fPunctuation_);
        Memory::CopyToIf (&fTrimTrailingZeros_, b2.fTrimTrailingZeros_);
        Memory::CopyToIf (&fFloatFormat_, b2.fFloatFormat_);
    }
    template <typename... ARGS>
    constexpr ToStringOptions::ToStringOptions (const ToStringOptions& b1, const ToStringOptions& b2, ARGS&&... args)
        : ToStringOptions{ToStringOptions{b1, b2}, forward<ARGS> (args)...}
    {
    }
    constexpr ToStringOptions::operator Number::FloatToStringOptions () const
    {
        using Number::Notation;
        using Number::Precision;
        Number::SignificantFigures sf   = fSignificantFigures_.value_or (Number::SignificantFigures{});
        bool                       full = sf == Number::SignificantFigures::kFullPrecision;
        unsigned int    n = max (sf.GetEffectiveSignificantFigures<double> (), 1u); // as many decimal places as kFullPrecision was
        FloatFormatType floatFormat = fFloatFormat_.value_or (FloatFormatType::eDEFAULT);
        if (fTrimTrailingZeros_ == true) {
            if (floatFormat == FloatFormatType::eFixedPoint) {
                floatFormat = FloatFormatType::eFixedPointWithWhitespaceTrimmed;
            }
            else if (floatFormat == FloatFormatType::eScientific) {
                floatFormat = FloatFormatType::eScientificWithWhitespaceTrimmed;
            }
        }
        Number::FloatToStringOptions r{.fPunctuation = fPunctuation_.value_or (Number::Punctuation{})};
        switch (floatFormat) {
            case FloatFormatType::eFixedPoint:
                r.fNotation  = Notation::ePlain;
                r.fPrecision = Precision::FixedFraction (n);
                break;
            case FloatFormatType::eFixedPointWithWhitespaceTrimmed:
                r.fNotation  = Notation::ePlain;
                r.fPrecision = Precision::MinMaxFraction (0, n);
                break;
            case FloatFormatType::eScientific:
                r.fNotation  = Notation::eScientific;
                r.fPrecision = full ? Precision::ShortestRoundTrip () : Precision::FixedSignificantDigits (n);
                break;
            case FloatFormatType::eScientificWithWhitespaceTrimmed:
                r.fNotation  = Notation::eScientific;
                r.fPrecision = full ? Precision::ShortestRoundTrip () : Precision::MinMaxSignificantDigits (min (2u, n), n);
                break;
            case FloatFormatType::eStandard:
                r.fNotation  = Notation::ePlain;
                r.fPrecision = sf;
                break;
            default: // eDefaultFloat, eAutomaticScientific
                r.fNotation  = Notation::eGeneral;
                r.fPrecision = sf;
                break;
        }
        return r;
    }
    constexpr optional<Number::SignificantFigures> ToStringOptions::GetSignificantFigures () const
    {
        return fSignificantFigures_;
    }
    constexpr optional<bool> ToStringOptions::GetTrimTrailingZeros () const
    {
        return fTrimTrailingZeros_;
    }
    inline Number::Punctuation ToStringOptions::GetPunctuation () const
    {
        return fPunctuation_ ? Number::ResolvePunctuation (*fPunctuation_) : Number::Punctuation{};
    }
    inline locale ToStringOptions::GetUseLocale () const
    {
        return GetPunctuation ().ToLocale ();
    }
    inline bool ToStringOptions::GetUsingLocaleClassic () const
    {
        return GetPunctuation () == Number::Punctuation{};
    }
    constexpr optional<FloatFormatType> ToStringOptions::GetFloatFormat () const
    {
        return fFloatFormat_;
    }
    constexpr optional<ios_base::fmtflags> ToStringOptions::GetIOSFmtFlags () const
    {
        return fFmtFlags_;
    }
    inline String ToStringOptions::ToString () const
    {
        return static_cast<Number::FloatToStringOptions> (*this).ToString ();
    }

    /*
     ********************************************************************************
     ************************** FloatConversion::ToString ***************************
     ********************************************************************************
     */
    template <Common::IAnyOf<String, string, wstring> STRING_TYPE, floating_point FLOAT_TYPE>
    inline STRING_TYPE ToString (FLOAT_TYPE f, const ToStringOptions& options)
    {
        Number::FloatToStringOptions o = options;
        if constexpr (same_as<STRING_TYPE, String>) {
            return Characters::ToString (f, o);
        }
        else if constexpr (same_as<STRING_TYPE, string>) {
            Require (o.GetPunctuation () == Characters::Number::Punctuation{});
            return Characters::ToString (f, o).AsASCII ();
        }
        else if constexpr (same_as<STRING_TYPE, wstring>) {
            Require (o.GetPunctuation () == Characters::Number::Punctuation{});
            return Characters::ToString (f, o).template As<wstring> ();
        }
    }
    DISABLE_COMPILER_MSC_WARNING_END (4996)
    DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Wdeprecated-declarations\"");
    DISABLE_COMPILER_CLANG_WARNING_END ("clang diagnostic ignored \"-Wdeprecated-declarations\"");

    /*
     ********************************************************************************
     *************************** FloatConversion::ToFloat ***************************
     ********************************************************************************
     */
    namespace Private_ {
        // ToFloat's implementation - and Number::Parse's, so not deprecated with ToFloat
        template <floating_point T, IUNICODECanUnambiguouslyConvertFrom CHAR_T>
        T ToFloat_ (span<const CHAR_T> s)
        {
            if constexpr (same_as<remove_cv_t<CHAR_T>, char>) {
                Require (Character::IsASCII (s));
            }
            /*
         *  Most of the time we can do this very efficiently, because there are just ascii characters.
         *  Else, fallback on algorithm that understands full unicode character set, and locales
         */
            Memory::StackBuffer<char> asciiS;
            if (Character::AsASCIIQuietly (s, &asciiS)) {
                Stroika_ATTRIBUTE_INDETERMINATE T result;
#if defined(__cpp_lib_to_chars) and not qCompilerAndStdLib_from_chars_and_tochars_FP_Precision_Buggy
#if qCompilerAndStdLib_from_chars_reads_past_end_Buggy
                asciiS.push_back (0);
#endif
                auto b = asciiS.begin ();
                auto e = asciiS.end ();
#if qCompilerAndStdLib_from_chars_reads_past_end_Buggy
                e--;
#endif
                if (b != e and *b == '+') [[unlikely]] {
                    ++b; // "the plus sign is not recognized outside of the exponent (only the minus sign is permitted at the beginning)" from https://en.cppreference.com/w/cpp/utility/from_chars
                }
                auto [ptr, ec] = from_chars (b, e, result);
                if (ec == errc::result_out_of_range) [[unlikely]] {
                    result = *b == '-' ? -numeric_limits<T>::infinity () : numeric_limits<T>::infinity ();
                }
                else if (ec != std::errc{} or ptr != e) [[unlikely]] {
                    //result = Math::nan<T> (); "if # is 100,1 - in funny locale - may need to use legacy algorithm
                    // @todo ADD OPTION arg to ToFloat so we know if C-Locale so can just return NAN here!...
                    result = Private_::ToFloat_RespectingLocale_<T> (s, nullptr);
                }
#else
                result = Private_::ToFloat_RespectingLocale_<T> (span<const char>{asciiS}, nullptr);
#endif
                return result;
            }
            return Private_::ToFloat_RespectingLocale_<T> (s, nullptr); // fallback for non-ascii strings to old code
        }
        template <floating_point T, IUNICODECanUnambiguouslyConvertFrom CHAR_T>
        T ToFloat_ (span<const CHAR_T> s, typename span<const CHAR_T>::iterator* remainder)
        {
            RequireNotNull (remainder); // use other overload if 'null'
            if constexpr (same_as<remove_cv_t<CHAR_T>, char>) {
                Require (Character::IsASCII (s));
            }
            /*
         *  Most of the time we can do this very efficiently, because there are just ascii characters.
         *  Else, fallback on algorithm that understands full unicode character set.
         */
#if defined(__cpp_lib_to_chars) and not qCompilerAndStdLib_from_chars_and_tochars_FP_Precision_Buggy
            Memory::StackBuffer<char> asciiS;
            if (Character::AsASCIIQuietly (s, &asciiS)) {
                Stroika_ATTRIBUTE_INDETERMINATE T result;
                char*                             b = asciiS.begin ();
                char*                             e = asciiS.end ();
                if (b != e and *b == '+') [[unlikely]] {
                    ++b; // "the plus sign is not recognized outside of the exponent (only the minus sign is permitted at the beginning)" from https://en.cppreference.com/w/cpp/utility/from_chars
                }
                auto [ptr, ec] = from_chars (b, e, result);
                if (ec == errc::result_out_of_range) [[unlikely]] {
                    result = *b == '-' ? -numeric_limits<T>::infinity () : numeric_limits<T>::infinity ();
                }
                else if (ec != std::errc{} or ptr != e) [[unlikely]] {
                    //result = Math::nan<T> (); "if # is 100,1 - in funny locale - may need to use legacy algorithm
                    // @todo ADD OPTION arg to ToFloat so we know if C-Locale so can just return NAN here!...
                    typename span<const CHAR_T>::iterator tmpI;
                    result = Private_::ToFloat_RespectingLocale_<T> (s, &tmpI);
                    ptr    = tmpI - s.begin () + b;
                }
                *remainder = s.begin () + UTFConvert::kThe.ConvertOffset<CHAR_T> (
                                              span{reinterpret_cast<const char8_t*> (b), reinterpret_cast<const char8_t*> (e)}, ptr - b);
                Assert (*remainder <= s.end ());
                return result;
            }
#endif
            return Private_::ToFloat_RespectingLocale_<T> (s, remainder); // fallback for non-ascii strings to strtod etc code
        }
        template <floating_point T>
        T ToFloat_ (const String& s, String* remainder)
        {
            RequireNotNull (remainder);
            Memory::StackBuffer<wchar_t>  ignored;
            span<const wchar_t>           srcSpan = s.GetData (&ignored);
            span<const wchar_t>::iterator tmpRemainder;
            auto                          result = ToFloat_<T> (srcSpan, &tmpRemainder);
            *remainder                           = String{srcSpan.subspan (tmpRemainder - srcSpan.begin ())};
            return result;
        }

    }
    template <floating_point T, IUNICODECanUnambiguouslyConvertFrom CHAR_T>
    inline T ToFloat (span<const CHAR_T> s)
    {
        return Private_::ToFloat_<T> (s);
    }
    template <floating_point T, IUNICODECanUnambiguouslyConvertFrom CHAR_T>
    inline T ToFloat (span<const CHAR_T> s, typename span<const CHAR_T>::iterator* remainder)
    {
        return Private_::ToFloat_<T> (s, remainder);
    }
    template <floating_point T, typename STRINGISH_ARG>
    inline T ToFloat (STRINGISH_ARG&& s)
        requires (IConvertibleToString<STRINGISH_ARG> or is_convertible_v<STRINGISH_ARG, std::string>)
    {
        using DecayedStringishArg = remove_cvref_t<STRINGISH_ARG>;
        if constexpr (same_as<DecayedStringishArg, const char*> or same_as<DecayedStringishArg, const char8_t*> or
                      same_as<DecayedStringishArg, const char16_t*> or same_as<DecayedStringishArg, const char32_t*> or
                      same_as<DecayedStringishArg, const wchar_t*>) {
            return Private_::ToFloat_<T> (span{s, CString::Length (s)});
        }
        else if constexpr (same_as<DecayedStringishArg, String>) {
            Memory::StackBuffer<wchar_t> ignored; // todo optimize for keep ascii case
            auto                         sp = s.template GetData<wchar_t> (&ignored);
            return Private_::ToFloat_<T> (sp);
        }
        else if constexpr (is_convertible_v<DecayedStringishArg, std::string>) {
            string ss = s;
            return Private_::ToFloat_<T> (span<const char>{ss.data (), ss.size ()});
        }
        else {
            String                       ss{forward<STRINGISH_ARG> (s)};
            Memory::StackBuffer<wchar_t> ignored;
            return Private_::ToFloat_<T> (ss.GetData<wchar_t> (&ignored));
        }
    }
    template <floating_point T>
    inline T ToFloat (const String& s, String* remainder)
    {
        return Private_::ToFloat_<T> (s, remainder);
    }

    //////////////// DEPRECATED STUFF BELOW

    template <typename T>
    [[deprecated ("Since Stroika v3.0d1 - use span overload")]] inline T ToFloat (const wchar_t* start, const wchar_t* end, const wchar_t** remainder)
    {
        Require (start <= end);
        RequireNotNull (remainder);
        Stroika_ATTRIBUTE_INDETERMINATE T result;
#if defined(__cpp_lib_to_chars) and not qCompilerAndStdLib_from_chars_and_tochars_FP_Precision_Buggy
        /*
         *  Most of the time we can do this very efficiently, because there are just ascii characters.
         *  Else, fallback on older algorithm that understands full unicode character set.
         */
        Memory::StackBuffer<char> asciiS;
        if (Character::AsASCIIQuietly (span{start, end}, &asciiS)) {
            auto b         = asciiS.begin ();
            auto originalB = b; // needed to properly compute remainder
            auto e         = asciiS.end ();
            if (remainder != nullptr) [[unlikely]] {
                // in remainder mode we skip leading whitespace
                while (b != e and iswspace (*b)) [[unlikely]] {
                    ++b;
                }
            }
            if (b != e and *b == '+') [[unlikely]] {
                ++b; // "the plus sign is not recognized outside of the exponent (only the minus sign is permitted at the beginning)" from https://en.cppreference.com/w/cpp/utility/from_chars
            }

            auto [ptr, ec] = from_chars (b, e, result);
            if (ec == errc::result_out_of_range) [[unlikely]] {
                return *b == '-' ? -numeric_limits<T>::infinity () : numeric_limits<T>::infinity ();
            }
            // if error - return nan
            if (ec != std::errc{}) [[unlikely]] {
                result = Math::nan<T> ();
            }
            // If asked to return remainder do so.
            // If NOT asked to return remainder, treat not using the entire string as forcing result to be a Nan (invalid parse of number of not the whole thing is a number)
            if (remainder == nullptr) [[likely]] {
                if (ptr != e) [[unlikely]] {
                    result = Math::nan<T> ();
                }
            }
            else {
                *remainder = ptr - originalB + start; // adjust in case we remapped data
            }
        }
        else {
            result = Private_::ToFloat_ViaStrToD_<T> (start, end, remainder);
        }
#else
        result = Private_::ToFloat_ViaStrToD_<T> (start, end, remainder);
#endif
        return result;
    }
    template <typename T = double>
    [[deprecated ("Since Stroika v3.0d1 - use span overload")]] inline T ToFloat (const char* start, const char* end)
    {
        return Private_::ToFloat_<T> (span{start, end});
    }
    template <typename T = double>
    [[deprecated ("Since Stroika v3.0d1 - use span overload")]] inline T ToFloat (const wchar_t* start, const wchar_t* end)
    {
        return Private_::ToFloat_<T> (span{start, end});
    }

}
