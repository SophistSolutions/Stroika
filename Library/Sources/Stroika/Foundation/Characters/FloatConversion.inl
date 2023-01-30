/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_FloatConversion_inl_
#define _Stroika_Foundation_Characters_FloatConversion_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include <charconv>
#include <sstream>

#include "../Containers/Common.h"
#include "../Memory/Optional.h"
#include "../Memory/Span.h"
#include "../Memory/StackBuffer.h"

#include "CString/Utilities.h"
#include "UTFConvert.h"

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
    inline ToStringOptions::ToStringOptions (const locale& l)
        : fUseLocale_{l}
    {
    }
    constexpr ToStringOptions::ToStringOptions (PredefinedLocale p)
        : fUseCurrentLocale_{p == PredefinedLocale::eUseCurrentLocale}
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
        fUseCurrentLocale_ = b2.fUseCurrentLocale_;
        Memory::CopyToIf (&fUseLocale_, b2.fUseLocale_);
        Memory::CopyToIf (&fTrimTrailingZeros_, b2.fTrimTrailingZeros_);
        Memory::CopyToIf (&fFloatFormat_, b2.fFloatFormat_);
    }
    template <typename... ARGS>
    inline ToStringOptions::ToStringOptions (const ToStringOptions& b1, const ToStringOptions& b2, ARGS&&... args)
        : ToStringOptions{ToStringOptions{b1, b2}, forward<ARGS> (args)...}
    {
    }
    inline optional<unsigned int> ToStringOptions::GetPrecision () const { return fPrecision_; }
    inline optional<bool>         ToStringOptions::GetTrimTrailingZeros () const { return fTrimTrailingZeros_; }
    inline locale                 ToStringOptions::GetUseLocale () const
    {
        if (fUseCurrentLocale_) {
            return locale{};
        }
        if (fUseLocale_) {
            return *fUseLocale_;
        }
        static const locale kCLocale_ = locale::classic ();
        return kCLocale_;
    }
    inline bool ToStringOptions::GetUsingLocaleClassic () const
    {
        if (not fUseLocale_ and not fUseCurrentLocale_) {
            return true; // very common case
        }
        return GetUseLocale () == locale::classic ();
    }
    inline optional<FloatFormatType>    ToStringOptions::GetFloatFormat () const { return fFloatFormat_; }
    inline optional<ios_base::fmtflags> ToStringOptions::GetIOSFmtFlags () const { return fFmtFlags_; }

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
                    ++pastDot;
                    size_t len           = strResult->length ();
                    size_t pPastLastZero = len;
                    for (; (pPastLastZero - 1) > pastDot; --pPastLastZero) {
                        if ((*strResult)[pPastLastZero - 1] != '0') {
                            break;
                        }
                    }
                    if (len != pPastLastZero) [[unlikely]] { // check common case of no change, but this substring and assign already pretty optimized (not sure helps performance)
                        *strResult = strResult->SubString (0, pPastLastZero);
                    }
                }
            }
        }
    }

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
                    DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Wrestrict\""); //  /usr/include/x86_64-linux-gnu/bits/string_fortified.h:29:33: warning: 'void* __builtin_memcpy(void*, const void*, long unsigned int)' accessing 18446744073709551612 bytes at offsets 8 and 0 overlaps 9223372036854775801 bytes at offset -9223372036854775805 [-Wrestrict]
                    DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Wstringop-overflow=\"");
                    DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Wstringop-overflow\""); // desperation try to silience
                    (void)::memcpy (tmp.begin (), start, len * sizeof (wchar_t));
                    DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Wstringop-overflow\"");
                    DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Wstringop-overflow=\"");
                    DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Wrestrict\"");
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
        template <typename FLOAT_TYPE>
        inline String ToString_OptimizedForCLocaleAndNoStreamFlags_ (FLOAT_TYPE f, int precision)
        {
            using namespace Memory;
            size_t            sz = precision + 100; // I think precision is enough
            StackBuffer<char> buf{Memory::eUninitialized, sz};
            ptrdiff_t         resultStrLen;
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
                resultStrLen =
                    ::snprintf (buf.data (), buf.size (),
                                mkFmtWithPrecisionArg_ (std::begin (format), std::end (format), is_same_v<FLOAT_TYPE, long double> ? 'L' : '\0'),
                                (int)precision, f);
            }
            else {
                // THIS #if test should NOT be needed but g++ 9 didn't properly respect if constexpr (link errors)
#if !qCompilerAndStdLib_to_chars_FP_Buggy
                // empirically, on MSVC, this is much faster (appears 3x apx faster) -- LGP 2021-11-04
                resultStrLen = to_chars (buf.begin (), buf.end (), f, chars_format::general, precision).ptr - buf.begin ();
#endif
            }
            Verify (resultStrLen > 0 and resultStrLen < static_cast<int> (sz));
            return String{Memory::ConstSpan (span{buf.data (), static_cast<size_t> (resultStrLen)})};
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

            s.imbue (options.GetUseLocale ());

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

            return options.GetUsingLocaleClassic () ? String{s.str ()} : String::FromNarrowString (s.str (), options.GetUseLocale ());
        }
    }

    namespace Private_ {
        template <typename FLOAT_TYPE>
        String ToString_String_Implementation_ (FLOAT_TYPE f, const ToStringOptions& options)
        {
            auto result = (options.GetUsingLocaleClassic () and not options.GetIOSFmtFlags () and not options.GetFloatFormat ())
                              ? Private_::ToString_OptimizedForCLocaleAndNoStreamFlags_ (
                                    f, options.GetPrecision ().value_or (ToStringOptions::kDefaultPrecision.fPrecision))
                              : Private_::ToString_GeneralCase_ (f, options);
            if (options.GetTrimTrailingZeros ().value_or (ToStringOptions::kDefaultTrimTrailingZeros)) {
                TrimTrailingZeros_ (&result);
            }
            Ensure (String::EqualsComparer{CompareOptions::eCaseInsensitive}(result, Legacy_Float2String_ (f, options)));
            return result;
        }
    }

    namespace Private_ {
        //
        // New span/c++20 version of 'ToFloat_Legacy_'
        // This RESPECTS THE CURRENT LOCALE
        // this private function allows nullptr remainder, and behaves somewhat differntly if null or not (unlike PUBLIC API)
        // Roughly (ignoring precision and characterset)
        //      Calls strtod
        //      Strtod Skips leading whitespace, but this routine does NOT allow that
        //      if remainder==null, rejects if not all characters eaten (else returns how many eaten in remainder)
        //
        // Note - inlined though long, because most of the logic gets compiled out depending on (template or actual)
        // parameters, and want to assure those 'seen' by optimizer so most of the code eliminated.
        //
        template <typename T, Character_IsUnicodeCodePointOrPlainChar CHAR_T>
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
#if qCompilerAndStdLib_spanOfContainer_Buggy
            Memory::CopySpanData (srcSpan, span{srcBufWithNul.data (), srcBufWithNul.size ()});
#else
            Memory::CopySpanData (srcSpan, span{srcBufWithNul});
#endif
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
            T d; // intentionally uninitialized - set below
            static_assert (is_same_v<T, float> or is_same_v<T, double> or is_same_v<T, long double>);
            if constexpr (sizeof (CHAR_T) == 1) {
                if constexpr (is_same_v<T, float>) {
                    d = ::strtof (reinterpret_cast<const char*> (si), const_cast<char**> (reinterpret_cast<const char**> (&ri)));
                }
                else if constexpr (is_same_v<T, double>) {
                    d = ::strtod (reinterpret_cast<const char*> (si), const_cast<char**> (reinterpret_cast<const char**> (&ri)));
                }
                else if constexpr (is_same_v<T, long double>) {
                    d = ::strtold (reinterpret_cast<const char*> (si), const_cast<char**> (reinterpret_cast<const char**> (&ri)));
                }
                else {
                    AssertNotReached ();
                }
            }
            else if constexpr (sizeof (CHAR_T) == sizeof (wchar_t)) {
                if constexpr (is_same_v<T, float>) {
                    d = ::wcstof (reinterpret_cast<const wchar_t*> (si), const_cast<wchar_t**> (reinterpret_cast<const wchar_t**> (&ri)));
                }
                else if constexpr (is_same_v<T, double>) {
                    d = ::wcstod (reinterpret_cast<const wchar_t*> (si), const_cast<wchar_t**> (reinterpret_cast<const wchar_t**> (&ri)));
                }
                else if constexpr (is_same_v<T, long double>) {
                    d = ::wcstold (reinterpret_cast<const wchar_t*> (si), const_cast<wchar_t**> (reinterpret_cast<const wchar_t**> (&ri)));
                }
                else {
                    AssertNotReached ();
                }
            }
            else {
                // must utf convert to wchar_t which we support
                Memory::StackBuffer<wchar_t> wideBuf{Memory::eUninitialized, UTFConverter::ComputeTargetBufferSize<wchar_t> (srcSpan)};
#if qCompilerAndStdLib_spanOfContainer_Buggy
                span<const wchar_t> wideSpan = UTFConverter::kThe.ConvertSpan (srcSpan, span{wideBuf.data (), wideBuf.size ()});
#else
                span<const wchar_t> wideSpan = UTFConverter::kThe.ConvertSpan (srcSpan, span{wideBuf});
#endif
                if (remainder == nullptr) {
                    d = ToFloat_RespectingLocale_<T, wchar_t> (wideSpan, nullptr);
                }
                else {
                    // do the conversion using wchar_t, and then map back the resulting remainder offset
                    span<const wchar_t>::iterator wideRemainder;
                    d  = ToFloat_RespectingLocale_<T> (wideSpan, &wideRemainder);
                    ri = UTFConverter::kThe.ConvertOffset<CHAR_T> (wideSpan, wideRemainder - wideSpan.begin ()) + si;
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

        // @todo LOSE THIS CODE SOON -LGP 2022-12-28- after testing a bit more to see asserts not triggering. Better to add regtests or move
        // this logic into REgtest file.

        // Version of code from pre-Stroika 2.1b14 (so b13 or earlier roughly)
        template <typename T = double>
        T ToFloat_Legacy_ (const String& s);
        template <typename T = double>
        T ToFloat_Legacy_ (const String& s, String* remainder);

        // Once I test a bit - I think we can lose these (CPP file) implementations --LGP 2022-12-27
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
    template <>
    inline string ToString (float f, const ToStringOptions& options)
    {
        // @todo improve performance for this case
        Require (options.GetUsingLocaleClassic ());
        return Private_::ToString_String_Implementation_ (f, options).AsASCII ();
    }
    template <>
    inline string ToString (double f, const ToStringOptions& options)
    {
        // @todo improve performance for this case
        Require (options.GetUsingLocaleClassic ());
        return Private_::ToString_String_Implementation_ (f, options).AsASCII ();
    }
    template <>
    inline string ToString (long double f, const ToStringOptions& options)
    {
        // @todo improve performance for this case
        Require (options.GetUsingLocaleClassic ());
        return Private_::ToString_String_Implementation_ (f, options).AsASCII ();
    }
    template <>
    inline wstring ToString (float f, const ToStringOptions& options)
    {
        // @todo improve performance for this case
        Require (options.GetUsingLocaleClassic ());
        return Private_::ToString_String_Implementation_ (f, options).As<wstring> ();
    }
    template <>
    inline wstring ToString (double f, const ToStringOptions& options)
    {
        // @todo improve performance for this case
        Require (options.GetUsingLocaleClassic ());
        return Private_::ToString_String_Implementation_ (f, options).As<wstring> ();
    }
    template <>
    inline wstring ToString (long double f, const ToStringOptions& options)
    {
        // @todo improve performance for this case
        Require (options.GetUsingLocaleClassic ());
        return Private_::ToString_String_Implementation_ (f, options).As<wstring> ();
    }

    /*
     ********************************************************************************
     *************************** FloatConversion::ToFloat ***************************
     ********************************************************************************
     */
    template <typename T, Character_Compatible CHAR_T>
    T ToFloat (span<const CHAR_T> s)
    {
        if constexpr (is_same_v<remove_cv_t<CHAR_T>, char>) {
            Require (Character::IsASCII (s));
        }
        /*
         *  Most of the time we can do this very efficiently, because there are just ascii characters.
         *  Else, fallback on algorithm that understands full unicode character set, and locales
         */
        Memory::StackBuffer<char> asciiS;
        if (Character::AsASCIIQuietly (s, &asciiS)) {
            T result; // intentionally uninitialized
            if constexpr (qCompilerAndStdLib_to_chars_FP_Buggy or qCompilerAndStdLib_from_chars_and_tochars_FP_Precision_Buggy) {
#if qCompilerAndStdLib_spanOfContainer_Buggy
                result = Private_::ToFloat_RespectingLocale_<T> (span<const char>{asciiS.data (), asciiS.size ()}, nullptr);
#else
                result                       = Private_::ToFloat_RespectingLocale_<T> (span<const char>{asciiS}, nullptr);
#endif
            }
            else {
                auto b = asciiS.begin ();
                auto e = asciiS.end ();
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
            }
            if constexpr (ConvertibleToString<decltype (s)>) {
                Ensure (Math::NearlyEquals (Private_::ToFloat_Legacy_<T> (String{s}), Private_::ToFloat_RespectingLocale_<T> (s, nullptr)));
                Ensure (Math::NearlyEquals (Private_::ToFloat_Legacy_<T> (String{s}), result));
            }
            return result;
        }
        if constexpr (ConvertibleToString<decltype (s)>) {
            Ensure (Math::NearlyEquals (Private_::ToFloat_Legacy_<T> (String{s}), Private_::ToFloat_RespectingLocale_<T> (s, nullptr)));
        }
        return Private_::ToFloat_RespectingLocale_<T> (s, nullptr); // fallback for non-ascii strings to old code
    }
    template <typename T, Character_Compatible CHAR_T>
    T ToFloat (span<const CHAR_T> s, typename span<const CHAR_T>::iterator* remainder)
    {
        RequireNotNull (remainder); // use other overload if 'null'
        if constexpr (is_same_v<remove_cv_t<CHAR_T>, char>) {
            Require (Character::IsASCII (s));
        }
        /*
         *  Most of the time we can do this very efficiently, because there are just ascii characters.
         *  Else, fallback on algorithm that understands full unicode character set.
         */
        if constexpr (not qCompilerAndStdLib_to_chars_FP_Buggy and not qCompilerAndStdLib_from_chars_and_tochars_FP_Precision_Buggy) {
            Memory::StackBuffer<char> asciiS;
            if (Character::AsASCIIQuietly (s, &asciiS)) {
                T     result; // intentionally uninitialized
                char* b = asciiS.begin ();
                char* e = asciiS.end ();
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
#if qDebug
                String legacyRemainer;
                Ensure (Math::NearlyEquals (Private_::ToFloat_Legacy_<T> (String{s}, &legacyRemainer), result));
#endif
                *remainder = s.begin () + UTFConverter::kThe.ConvertOffset<CHAR_T> (
                                              span{reinterpret_cast<const char8_t*> (b), reinterpret_cast<const char8_t*> (e)}, ptr - b);
                Assert (*remainder <= s.end ());
                // todo fix so can do this - Assert ((legacyRemainer == String{*remainder, s.end ()}));
                return result;
            }
        }
        return Private_::ToFloat_RespectingLocale_<T> (s, remainder); // fallback for non-ascii strings to strtod etc code
    }
    template <typename T, typename STRINGISH_ARG>
    inline T ToFloat (STRINGISH_ARG&& s)
        requires (ConvertibleToString<STRINGISH_ARG> or is_convertible_v<STRINGISH_ARG, std::string>)
    {
        using DecayedStringishArg = remove_cvref_t<STRINGISH_ARG>;
        if constexpr (is_same_v<DecayedStringishArg, const char*> or is_same_v<DecayedStringishArg, const char8_t*> or
                      is_same_v<DecayedStringishArg, const char16_t*> or is_same_v<DecayedStringishArg, const char32_t*> or
                      is_same_v<DecayedStringishArg, const wchar_t*>) {
            return ToFloat<T> (span{s, CString::Length (s)});
        }
        else if constexpr (is_same_v<DecayedStringishArg, String>) {
            Memory::StackBuffer<wchar_t> ignored; // todo optimize for keep ascii case
            auto                         sp = s.template GetData<wchar_t> (&ignored);
            return ToFloat<T> (sp);
        }
        else if constexpr (is_convertible_v<DecayedStringishArg, std::string>) {
            string ss = s;
            return ToFloat<T> (span{ss.data (), ss.size ()});
        }
        else {
            return ToFloat<T> (String{forward<STRINGISH_ARG> (s)});
        }
    }
    template <typename T>
    inline T ToFloat (const String& s, String* remainder)
    {
        RequireNotNull (remainder);
        Memory::StackBuffer<wchar_t>  ignored;
        span<const wchar_t>           srcSpan = s.GetData (&ignored);
        span<const wchar_t>::iterator tmpRemainder;
        auto                          result = ToFloat<T> (srcSpan, &tmpRemainder);
        *remainder                           = String{srcSpan.subspan (tmpRemainder - srcSpan.begin ())};
        return result;
    }

    //////////////// DEPRECATED STUFF BELOW

    template <typename T>
    [[deprecated ("Since Stroika v3.0d1 - use span overload")]] inline T ToFloat (const wchar_t* start, const wchar_t* end, const wchar_t** remainder)
    {
        Require (start <= end);
        RequireNotNull (remainder);
        T result; // intentionally uninitialized
        if constexpr (qCompilerAndStdLib_to_chars_FP_Buggy or qCompilerAndStdLib_from_chars_and_tochars_FP_Precision_Buggy) {
            result = Private_::ToFloat_ViaStrToD_<T> (start, end, remainder);
        }
        else {
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
    template <typename T = double>
    [[deprecated ("Since Stroika v3.0d1 - use span overload")]] inline T ToFloat (const char* start, const char* end)
    {
        return ToFloat<T> (span{start, end});
    }
    template <typename T = double>
    [[deprecated ("Since Stroika v3.0d1 - use span overload")]] inline T ToFloat (const wchar_t* start, const wchar_t* end)
    {
        return ToFloat<T> (span{start, end});
    }

}

#endif /*_Stroika_Foundation_Characters_FloatConversion_inl_*/
