/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include <cctype>
#include <charconv>
#include <cmath>
#include <limits>
#include <sstream>
#include <string>
#include <string_view>

#include "Stroika/Foundation/Characters/CString/Utilities.h"
#include "Stroika/Foundation/Memory/StackBuffer.h"

namespace Stroika::Foundation::Characters {

    namespace Private_ {
        // s, quoted, for an error message - shortened if long; char text that is not ASCII cannot be made a String, so is described
        template <IUNICODECanUnambiguouslyConvertFrom CHAR_T>
        String QuoteForParseError_ (span<const CHAR_T> s)
        {
            if constexpr (same_as<remove_cv_t<CHAR_T>, char>) {
                if (not Character::IsASCII (s)) {
                    return "(text that is not ASCII)"sv;
                }
            }
            return "\""sv + String{s}.LimitLength (50) + "\""sv;
        }
        // s, as ASCII, with each separator between two ASCII letters or digits removed, and decimalPoint (if given) made '.' - nullopt
        // if s has anything else that is not ASCII, a separator anywhere else, or a '.' that is not the decimal point
        inline optional<string> ToCNumberText_ (const String& s, optional<Character> decimalPoint, optional<Character> separator)
        {
            auto   isAlnum = [] (Character c) { return c.IsASCII () and isalnum (static_cast<unsigned char> (c.GetAsciiCode ())); };
            size_t n       = s.size ();
            string r;
            r.reserve (n);
            for (size_t i = 0; i < n; ++i) {
                Character c = s[i];
                if (separator and c == *separator) {
                    if (i == 0 or i + 1 == n or not isAlnum (s[i - 1]) or not isAlnum (s[i + 1])) {
                        return nullopt;
                    }
                }
                else if (decimalPoint and c == *decimalPoint) {
                    r += '.';
                }
                else if ((decimalPoint and c == '.') or not c.IsASCII ()) {
                    return nullopt;
                }
                else {
                    r += c.GetAsciiCode ();
                }
            }
            return r;
        }
        inline Character DecimalPoint_ (const variant<PredefinedLocale, locale>& l)
        {
            if (const PredefinedLocale* p = get_if<PredefinedLocale> (&l)) {
                return *p == PredefinedLocale::eUseCLocale ? Character{'.'} : Character{use_facet<numpunct<wchar_t>> (locale{}).decimal_point ()};
            }
            return Character{use_facet<numpunct<wchar_t>> (get<locale> (l)).decimal_point ()};
        }
        // the number at the start of s, in the "C" locale's format - whatever the global locale: its length (0 if s does not start with
        // one), its value in *result, and *outOfRange if it is too large for T. nan, inf and infinity - in any case, with an optional
        // sign - are read here, as whole words (from_chars would also take nan(chars))
        template <floating_point T>
        size_t ParseFloatPrefix_ (string_view s, T* result, bool* outOfRange)
        {
            *outOfRange     = false;
            size_t i        = 0;
            bool   negative = false;
            if (i < s.size () and (s[i] == '+' or s[i] == '-')) {
                negative = s[i] == '-';
                ++i;
            }
            auto isWord = [&] (string_view w) {
                if (s.size () - i < w.size ()) {
                    return false;
                }
                for (size_t k = 0; k < w.size (); ++k) {
                    if (tolower (static_cast<unsigned char> (s[i + k])) != w[k]) {
                        return false;
                    }
                }
                return i + w.size () == s.size () or not isalnum (static_cast<unsigned char> (s[i + w.size ()]));
            };
            if (isWord ("nan"sv)) {
                *result = numeric_limits<T>::quiet_NaN ();
                return i + 3;
            }
            for (string_view w : {"infinity"sv, "inf"sv}) {
                if (isWord (w)) {
                    *result = negative ? -numeric_limits<T>::infinity () : numeric_limits<T>::infinity ();
                    return i + w.size ();
                }
            }
            // the number's extent, in from_chars' decimal syntax (chars_format::general): digits with an optional '.' - at least one
            // digit - then an optional exponent; found here so that what reads it below is given just the number
            auto   isDigit = [&] (size_t k) { return k < s.size () and isdigit (static_cast<unsigned char> (s[k])); };
            size_t end     = i;
            size_t nDigits = 0;
            for (; isDigit (end); ++end, ++nDigits) {
            }
            if (end < s.size () and s[end] == '.') {
                for (++end; isDigit (end); ++end, ++nDigits) {
                }
            }
            if (nDigits == 0) {
                return 0;
            }
            if (end < s.size () and (s[end] == 'e' or s[end] == 'E')) {
                size_t k = end + 1;
                if (k < s.size () and (s[k] == '+' or s[k] == '-')) {
                    ++k;
                }
                if (isDigit (k)) {
                    for (end = k; isDigit (end); ++end) {
                    }
                }
            }
            string digits{s.substr (i, end - i)}; // a NUL after it, as some from_chars read past the end (qCompilerAndStdLib_from_chars_reads_past_end_Buggy)
            size_t used = digits.size ();
#if defined(__cpp_lib_to_chars) and not qCompilerAndStdLib_from_chars_and_tochars_FP_Precision_Buggy
            auto [ptr, ec] = from_chars (digits.data (), digits.data () + digits.size (), *result);
            if (ec == errc::invalid_argument) {
                return 0;
            }
            *outOfRange = ec == errc::result_out_of_range;
            used        = static_cast<size_t> (ptr - digits.data ());
#else
            // this standard library's from_chars cannot read floating point numbers: a stream in the "C" locale does instead. Given
            // just the number (libc++'s would read hex too), it fails only over range - on overflow, with the largest value
            // (libstdc++) or infinity (libc++). Underflow to zero is out of range too, though not every library fails for it.
            istringstream in{digits};
            in.imbue (locale::classic ());
            in >> *result;
            T    magnitude = abs (*result);
            bool nonZero   = string_view{digits}.substr (0, digits.find_first_of ("eE")).find_first_not_of ("0."sv) != string_view::npos;
#if qCompilerAndStdLib_num_get_subnormal_failbit_Buggy
            *outOfRange = (in.fail () and (isinf (magnitude) or magnitude == numeric_limits<T>::max ())) or (magnitude == 0 and nonZero);
#else
            *outOfRange = in.fail () or (magnitude == 0 and nonZero);
#endif
#endif
            if (negative) {
                *result = -*result;
            }
            return i + used;
        }
    }

    /*
     ********************************************************************************
     ********************************* ParseInteger *********************************
     ********************************************************************************
     */
    template <integral T, IUNICODECanUnambiguouslyConvertFrom CHAR_T>
    Common::StdCompat::expected<T, DataExchange::BadFormatException> ParseInteger (span<const CHAR_T> s, const ParseIntegerOptions& options)
    {
        const unsigned int radix = options.fRadix;
        Require (2 <= radix and radix <= 36);
        using ERRT        = Common::StdCompat::unexpected<DataExchange::BadFormatException>;
        auto notAnInteger = [&] () {
            return ERRT{DataExchange::BadFormatException{
                Private_::QuoteForParseError_ (s) +
                (radix == 10 ? String{" is not an integer"sv} : " is not a base-"sv + String{to_string (radix)} + " integer"sv)}};
        };
        if (options.fIgnoreSurroundingWhitespace) {
            if constexpr (same_as<remove_cv_t<CHAR_T>, char>) {
                if (not Character::IsASCII (s)) {
                    return notAnInteger ();
                }
            }
            ParseIntegerOptions o          = options;
            o.fIgnoreSurroundingWhitespace = false;
            String                       t = String{s}.Trim ();
            Memory::StackBuffer<wchar_t> buf;
            return ParseInteger<T> (t.GetData<wchar_t> (&buf), o);
        }
        if (options.fDigitSeparator) {
            if constexpr (same_as<remove_cv_t<CHAR_T>, char>) {
                if (not Character::IsASCII (s)) {
                    return notAnInteger ();
                }
            }
            optional<string> c = Private_::ToCNumberText_ (String{s}, nullopt, options.fDigitSeparator);
            if (not c) {
                return notAnInteger ();
            }
            ParseIntegerOptions o = options;
            o.fDigitSeparator     = nullopt;
            return ParseInteger<T> (span<const char>{c->data (), c->size ()}, o);
        }
        Memory::StackBuffer<char> ascii;
        if (Character::AsASCIIQuietly (s, &ascii)) {
            const char* b        = ascii.begin ();
            const char* e        = ascii.end ();
            bool        negative = b != e and *b == '-';
            if (b != e and (*b == '+' or *b == '-')) {
                ++b; // the sign is read here, so a prefix can follow it (and from_chars takes no +, nor - for an unsigned type)
            }
            if (options.fAcceptRadixPrefix and e - b > 2 and b[0] == '0') {
                char p = static_cast<char> (b[1] | 0x20); // in either case
                if ((radix == 16 and p == 'x') or (radix == 8 and p == 'o') or (radix == 2 and p == 'b')) {
                    b += 2;
                }
            }
            if (b != e and *b != '+' and *b != '-') { // a sign at most once
                using U = make_unsigned_t<T>;
                U magnitude{};
                auto [ptr, ec] = from_chars (b, e, magnitude, static_cast<int> (radix));
                if (ptr == e and ec == errc{}) {
                    // the sign applied here, so -128 fits an int8_t, though 128 does not
                    if constexpr (unsigned_integral<T>) {
                        if (not negative or magnitude == 0) {
                            return magnitude;
                        }
                    }
                    else {
                        constexpr U kMax = static_cast<U> (numeric_limits<T>::max ());
                        if (magnitude <= kMax) {
                            return negative ? static_cast<T> (-static_cast<T> (magnitude)) : static_cast<T> (magnitude);
                        }
                        if (negative and magnitude == kMax + 1u) {
                            return numeric_limits<T>::min ();
                        }
                    }
                }
                if (ptr == e and (ec == errc{} or ec == errc::result_out_of_range)) {
                    return ERRT{DataExchange::BadFormatException{Private_::QuoteForParseError_ (s) + " is out of range ("sv +
                                                                 String{to_string (+numeric_limits<T>::min ())} + " to "sv +
                                                                 String{to_string (+numeric_limits<T>::max ())} + ")"sv}};
                }
            }
        }
        return notAnInteger ();
    }
    template <integral T, IConvertibleToString STRINGISH_ARG>
    inline Common::StdCompat::expected<T, DataExchange::BadFormatException> ParseInteger (STRINGISH_ARG&& s, const ParseIntegerOptions& options)
    {
        using DecayedStringishArg = remove_cvref_t<STRINGISH_ARG>;
        if constexpr (is_convertible_v<DecayedStringishArg, string_view>) {
            string_view sv = s; // char text: not ASCII is an error, not a throw from String's constructor
            return ParseInteger<T> (span{sv.data (), sv.size ()}, options);
        }
        else if constexpr (Common::IAnyOf<DecayedStringishArg, const char8_t*, const char16_t*, const char32_t*, const wchar_t*>) {
            return ParseInteger<T> (span{s, CString::Length (s)}, options);
        }
        else if constexpr (same_as<DecayedStringishArg, String>) {
            Memory::StackBuffer<wchar_t> ignored;
            return ParseInteger<T> (s.template GetData<wchar_t> (&ignored), options);
        }
        else {
            return ParseInteger<T> (String{forward<STRINGISH_ARG> (s)}, options);
        }
    }

    /*
     ********************************************************************************
     ********************************** ParseFloat **********************************
     ********************************************************************************
     */
    template <floating_point T, IUNICODECanUnambiguouslyConvertFrom CHAR_T>
    Common::StdCompat::expected<T, DataExchange::BadFormatException> ParseFloat (span<const CHAR_T> s, const ParseFloatOptions& options)
    {
        using ERRT = Common::StdCompat::unexpected<DataExchange::BadFormatException>;
        auto error = [&] (string_view why) { return ERRT{DataExchange::BadFormatException{Private_::QuoteForParseError_ (s) + why}}; };
        if constexpr (same_as<remove_cv_t<CHAR_T>, char>) {
            if (not Character::IsASCII (s)) {
                return error (" is not a number"sv);
            }
        }
        if (options.fIgnoreSurroundingWhitespace) {
            ParseFloatOptions o            = options;
            o.fIgnoreSurroundingWhitespace = false;
            String                       t = String{s}.Trim ();
            Memory::StackBuffer<wchar_t> buf;
            return ParseFloat<T> (t.GetData<wchar_t> (&buf), o);
        }
        Character decimalPoint = Private_::DecimalPoint_ (options.fLocale);
        Require (not options.fDigitSeparator or *options.fDigitSeparator != decimalPoint);
        string text; // in the "C" locale's format
        if (decimalPoint == '.' and not options.fDigitSeparator) {
            Memory::StackBuffer<char> ascii;
            if (not Character::AsASCIIQuietly (s, &ascii)) {
                return error (" is not a number"sv);
            }
            text.assign (ascii.begin (), ascii.end ());
        }
        else if (optional<string> c = Private_::ToCNumberText_ (String{s}, decimalPoint, options.fDigitSeparator)) {
            text = move (*c);
        }
        else {
            return error (" is not a number"sv);
        }
        T    r{};
        bool outOfRange = false;
        if (text.empty () or Private_::ParseFloatPrefix_ (text, &r, &outOfRange) != text.size ()) {
            return error (" is not a number"sv);
        }
        if (outOfRange) {
            return error (" is out of range"sv);
        }
        return r;
    }
    template <floating_point T, typename STRINGISH_ARG>
    inline Common::StdCompat::expected<T, DataExchange::BadFormatException> ParseFloat (STRINGISH_ARG&& s, const ParseFloatOptions& options)
        requires (IConvertibleToString<STRINGISH_ARG> or is_convertible_v<STRINGISH_ARG, std::string>)
    {
        using DecayedStringishArg = remove_cvref_t<STRINGISH_ARG>;
        if constexpr (is_convertible_v<DecayedStringishArg, string_view>) {
            string_view sv = s; // char text: not ASCII is an error, not a throw from String's constructor
            return ParseFloat<T> (span{sv.data (), sv.size ()}, options);
        }
        else if constexpr (Common::IAnyOf<DecayedStringishArg, const char8_t*, const char16_t*, const char32_t*, const wchar_t*>) {
            return ParseFloat<T> (span{s, CString::Length (s)}, options);
        }
        else if constexpr (same_as<DecayedStringishArg, String>) {
            Memory::StackBuffer<wchar_t> ignored;
            return ParseFloat<T> (s.template GetData<wchar_t> (&ignored), options);
        }
        else {
            return ParseFloat<T> (String{forward<STRINGISH_ARG> (s)}, options);
        }
    }
    template <floating_point T>
    Common::StdCompat::expected<T, DataExchange::BadFormatException> ParseFloat (const String& s, String* remainder)
    {
        RequireNotNull (remainder);
        using ERRT   = Common::StdCompat::unexpected<DataExchange::BadFormatException>;
        String text  = s; // s may be *remainder - as in ParseFloat (t, &t) - so changed below
        size_t start = 0;
        while (start < text.size () and text[start].IsWhitespace ()) {
            ++start;
        }
        string ascii; // a number is all ASCII, so the ASCII text from there is enough to read it
        for (size_t i = start; i < text.size () and text[i].IsASCII (); ++i) {
            ascii += text[i].GetAsciiCode ();
        }
        T      r{};
        bool   outOfRange = false;
        size_t used       = Private_::ParseFloatPrefix_ (ascii, &r, &outOfRange);
        if (used == 0) {
            *remainder = text;
            return ERRT{DataExchange::BadFormatException{"\""sv + text.LimitLength (50) + "\" does not start with a number"sv}};
        }
        *remainder = text.SubString (start + used);
        if (outOfRange) {
            return ERRT{DataExchange::BadFormatException{"\""sv + String{ascii.substr (0, used)}.LimitLength (50) + "\" is out of range"sv}};
        }
        return r;
    }

}
