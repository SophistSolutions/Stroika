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
#include "Stroika/Foundation/Memory/Optional.h"
#include "Stroika/Foundation/Memory/StackBuffer.h"

namespace Stroika::Foundation::Characters::Number {

    /*
     ********************************************************************************
     ****************************** SignificantFigures ******************************
     ********************************************************************************
     */
    constexpr SignificantFigures::SignificantFigures (unsigned int p)
        : fSignificantFigures_{p}
    {
    }
    constexpr SignificantFigures::SignificantFigures (FullFlag)
        : fSignificantFigures_{} // kFullPrecision
    {
    }
    template <floating_point T>
    constexpr auto SignificantFigures::GetEffectiveSignificantFigures () const -> RepType
    {
        return fSignificantFigures_.value_or (numeric_limits<T>::max_digits10);
    }
    template <IStdBasicStringCompatibleCharacter CHAR>
    constexpr auto SignificantFigures::Calculate (span<const CHAR> number) -> RepType
    {
        // two passes cuz eatLeadingZero depends on scientific, which we find at the end of the text
        bool dotPresent = false;
        bool scientific = false;
        for (const CHAR c : number) {
            switch (c) {
                // Some locales use . (most), but europe uses , for same purpose (and used in ISO-8601)
                case '.':
                case ',':
                    dotPresent = true;
                    break;
                case 'e':
                case 'E':
                    scientific = true;
                    break;
            }
        }
        RepType n{0};
        RepType trailingZeros{0};
        bool    eatLeadingZeros = not scientific;
        bool    gotoDone        = false;
        for (const CHAR c : number) {
            switch (c) {
                case '-':
                case '+':
                    break;
                case '0':
                    if (eatLeadingZeros) {
                        // munch
                    }
                    else {
                        ++n;
                    }
                    ++trailingZeros;
                    break;
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                case '8':
                case '9':
                    eatLeadingZeros = false;
                    trailingZeros   = 0;
                    ++n;
                    break;
                case '.':
                case ',':
                    trailingZeros = 0;
                    break;
                case 'e':
                case 'E':
                    gotoDone = true; // goto Done;   // FIX to use this when c++23
                    break;
            }
            if (gotoDone) {
                break;
            }
        }
        //    Done:
        if (dotPresent) {
            if (eatLeadingZeros) {
                // then the trailing zeros we counted were also leading 0, but wern't added to n so do so now
                n += trailingZeros;
            }
        }
        else {
            n -= trailingZeros;
        }
        return n;
    }

    /**
     *      From http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2017/n4659.pdf,
     *      init (basic_streambuf...) initializes precision to 6
     *      Stroika need not maintain that default here, but it seems a sensible one...
     * 
     *  \note defined here instead of header cuz needs to be after SignificantFigures::CTOR.
     */
    constexpr inline SignificantFigures SignificantFigures::kDefault{6};

    /**
     *  \brief Full precision here means enough digits so that when written out (serialized) - and read back in (deserialized)
     *         you get the exact same answer.
     * 
     *  \see  // https://stackoverflow.com/questions/22458355/what-is-the-purpose-of-max-digits10-and-how-is-it-different-from-digits10
     *        numeric_limits<T>::max_digits10
     * 
     *  \note defined here instead of header cuz needs to be after SignificantFigures::CTOR.
     */
    constexpr inline SignificantFigures SignificantFigures::kFullPrecision{SignificantFigures::FullFlag::eFull};

    /*
     ********************************************************************************
     ***************************** FloatToStringOptions *****************************
     ********************************************************************************
     */
    inline FloatToStringOptions::FloatToStringOptions (const locale& l)
        : fUseLocale_{l}
    {
    }
    constexpr FloatToStringOptions::FloatToStringOptions (Common::PredefinedLocale p)
        : fUseCurrentLocale_{p == Common::PredefinedLocale::eUseCurrentLocale}
    {
    }
    constexpr FloatToStringOptions::FloatToStringOptions (ios_base::fmtflags fmtFlags)
        : fFmtFlags_{fmtFlags}
    {
    }
    constexpr FloatToStringOptions::FloatToStringOptions (SignificantFigures precision)
        : fSignificantFigures_{precision}
    {
    }
    constexpr FloatToStringOptions::FloatToStringOptions (FloatFormatType scientificNotation)
        : fFloatFormat_{scientificNotation}
    {
    }
    DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Wdeprecated-declarations\"");
    DISABLE_COMPILER_CLANG_WARNING_START ("clang diagnostic ignored \"-Wdeprecated-declarations\"");
    DISABLE_COMPILER_MSC_WARNING_START (4996)
    constexpr FloatToStringOptions::FloatToStringOptions (TrimTrailingZerosType trimTrailingZeros)
        : fTrimTrailingZeros_{trimTrailingZeros == TrimTrailingZerosType::eTrimZeros}
    {
    }
    DISABLE_COMPILER_MSC_WARNING_END (4996)
    DISABLE_COMPILER_CLANG_WARNING_END ("clang diagnostic ignored \"-Wdeprecated-declarations\"");
    DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Wdeprecated-declarations\"");

    constexpr FloatToStringOptions::FloatToStringOptions (const FloatToStringOptions& b1, const FloatToStringOptions& b2)
        : FloatToStringOptions{b1}
    {
        Memory::CopyToIf (&fSignificantFigures_, b2.fSignificantFigures_);
        Memory::CopyToIf (&fFmtFlags_, b2.fFmtFlags_);
        fUseCurrentLocale_ = b2.fUseCurrentLocale_;
        Memory::CopyToIf (&fUseLocale_, b2.fUseLocale_);
        Memory::CopyToIf (&fTrimTrailingZeros_, b2.fTrimTrailingZeros_);
        Memory::CopyToIf (&fFloatFormat_, b2.fFloatFormat_);
    }
    template <typename... ARGS>
    constexpr FloatToStringOptions::FloatToStringOptions (const FloatToStringOptions& b1, const FloatToStringOptions& b2, ARGS&&... args)
        : FloatToStringOptions{FloatToStringOptions{b1, b2}, forward<ARGS> (args)...}
    {
    }
    constexpr optional<SignificantFigures> FloatToStringOptions::GetSignificantFigures () const
    {
        return fSignificantFigures_;
    }
    constexpr optional<bool> FloatToStringOptions::GetTrimTrailingZeros () const
    {
        return fTrimTrailingZeros_;
    }
    inline locale FloatToStringOptions::GetUseLocale () const
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
    inline bool FloatToStringOptions::GetUsingLocaleClassic () const
    {
        if (not fUseLocale_ and not fUseCurrentLocale_) {
            return true; // very common case
        }
        return GetUseLocale () == locale::classic ();
    }
    constexpr optional<FloatFormatType> FloatToStringOptions::GetFloatFormat () const
    {
        return fFloatFormat_;
    }
    constexpr optional<ios_base::fmtflags> FloatToStringOptions::GetIOSFmtFlags () const
    {
        return fFmtFlags_;
    }

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
        // s without the whitespace (Character::IsWhitespace) at either end - in place
        template <IUNICODECanUnambiguouslyConvertFrom CHAR_T>
        span<const CHAR_T> TrimWhitespace_ (span<const CHAR_T> s)
        {
            auto isWhitespace = [&] (size_t i) {
                return Character{static_cast<char32_t> (static_cast<make_unsigned_t<CHAR_T>> (s[i]))}.IsWhitespace ();
            };
            size_t b = 0;
            size_t e = s.size ();
            if constexpr (not same_as<CHAR_T, char8_t>) {
                // each whitespace character is one code unit - in UTF-16 too: none is past U+FFFF, so none is a surrogate pair
                while (b < e and isWhitespace (b)) {
                    ++b;
                }
                while (b < e and isWhitespace (e - 1)) {
                    --e;
                }
            }
            else {
                // UTF-8: past ASCII, whitespace (U+1680, U+2000 to U+3000) is 3 bytes - 1110xxxx 10xxxxxx 10xxxxxx - so each end is
                // checked for one of those as well
                auto whitespace3At = [&] (size_t i) {
                    auto u = [&] (size_t k) { return static_cast<char32_t> (s[k]); };
                    return (u (i) & 0xF0) == 0xE0 and (u (i + 1) & 0xC0) == 0x80 and (u (i + 2) & 0xC0) == 0x80 and
                           Character{static_cast<char32_t> (((u (i) & 0x0F) << 12) | ((u (i + 1) & 0x3F) << 6) | (u (i + 2) & 0x3F))}.IsWhitespace ();
                };
                while (b < e) {
                    if (isWhitespace (b)) {
                        ++b;
                    }
                    else if (e - b >= 3 and whitespace3At (b)) {
                        b += 3;
                    }
                    else {
                        break;
                    }
                }
                while (b < e) {
                    if (isWhitespace (e - 1)) {
                        --e;
                    }
                    else if (e - b >= 3 and whitespace3At (e - 3)) {
                        e -= 3;
                    }
                    else {
                        break;
                    }
                }
            }
            return s.subspan (b, e - b);
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
        inline Character DecimalPoint_ (const Common::LocaleSpecifier& l)
        {
            if (const Common::PredefinedLocale* p = get_if<Common::PredefinedLocale> (&l); p != nullptr and *p == Common::PredefinedLocale::eUseCLocale) {
                return Character{'.'};
            }
            return Character{use_facet<numpunct<wchar_t>> (Common::ResolveLocale (l)).decimal_point ()};
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
     ************************** Number::Parse (integral T) **************************
     ********************************************************************************
     */
    template <integral T, IUNICODECanUnambiguouslyConvertFrom CHAR_T>
    Common::StdCompat::expected<T, DataExchange::BadFormatException> Parse (span<const CHAR_T> s, const IntegerParseOptions& options)
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
            IntegerParseOptions o          = options;
            o.fIgnoreSurroundingWhitespace = false;
            return Parse<T> (Private_::TrimWhitespace_ (s), o);
        }
        if (options.fDigitSeparator) {
            // slow - it copies the text twice (into a String, then a string without the separators): a candidate to optimize, if a
            // separator is used where speed matters
            if constexpr (same_as<remove_cv_t<CHAR_T>, char>) {
                if (not Character::IsASCII (s)) {
                    return notAnInteger ();
                }
            }
            optional<string> c = Private_::ToCNumberText_ (String{s}, nullopt, options.fDigitSeparator);
            if (not c) {
                return notAnInteger ();
            }
            IntegerParseOptions o = options;
            o.fDigitSeparator     = nullopt;
            return Parse<T> (span<const char>{c->data (), c->size ()}, o);
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
    inline Common::StdCompat::expected<T, DataExchange::BadFormatException> Parse (STRINGISH_ARG&& s, const IntegerParseOptions& options)
    {
        using DecayedStringishArg = remove_cvref_t<STRINGISH_ARG>;
        if constexpr (is_convertible_v<DecayedStringishArg, string_view>) {
            string_view sv = s; // char text: not ASCII is an error, not a throw from String's constructor
            return Parse<T> (span{sv.data (), sv.size ()}, options);
        }
        else if constexpr (Common::IAnyOf<DecayedStringishArg, const char8_t*, const char16_t*, const char32_t*, const wchar_t*>) {
            return Parse<T> (span{s, CString::Length (s)}, options);
        }
        else if constexpr (same_as<DecayedStringishArg, String>) {
            Memory::StackBuffer<wchar_t> ignored;
            return Parse<T> (s.template GetData<wchar_t> (&ignored), options);
        }
        else {
            return Parse<T> (String{forward<STRINGISH_ARG> (s)}, options);
        }
    }

    /*
     ********************************************************************************
     *********************** Number::Parse (floating_point T) ***********************
     ********************************************************************************
     */
    template <floating_point T, IUNICODECanUnambiguouslyConvertFrom CHAR_T>
    Common::StdCompat::expected<T, DataExchange::BadFormatException> Parse (span<const CHAR_T> s, const FloatParseOptions& options)
    {
        using ERRT = Common::StdCompat::unexpected<DataExchange::BadFormatException>;
        auto error = [&] (string_view why) { return ERRT{DataExchange::BadFormatException{Private_::QuoteForParseError_ (s) + why}}; };
        if constexpr (same_as<remove_cv_t<CHAR_T>, char>) {
            if (not Character::IsASCII (s)) {
                return error (" is not a number"sv);
            }
        }
        if (options.fIgnoreSurroundingWhitespace) {
            FloatParseOptions o            = options;
            o.fIgnoreSurroundingWhitespace = false;
            return Parse<T> (Private_::TrimWhitespace_ (s), o);
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
    inline Common::StdCompat::expected<T, DataExchange::BadFormatException> Parse (STRINGISH_ARG&& s, const FloatParseOptions& options)
        requires (IConvertibleToString<STRINGISH_ARG> or is_convertible_v<STRINGISH_ARG, std::string>)
    {
        using DecayedStringishArg = remove_cvref_t<STRINGISH_ARG>;
        if constexpr (is_convertible_v<DecayedStringishArg, string_view>) {
            string_view sv = s; // char text: not ASCII is an error, not a throw from String's constructor
            return Parse<T> (span{sv.data (), sv.size ()}, options);
        }
        else if constexpr (Common::IAnyOf<DecayedStringishArg, const char8_t*, const char16_t*, const char32_t*, const wchar_t*>) {
            return Parse<T> (span{s, CString::Length (s)}, options);
        }
        else if constexpr (same_as<DecayedStringishArg, String>) {
            Memory::StackBuffer<wchar_t> ignored;
            return Parse<T> (s.template GetData<wchar_t> (&ignored), options);
        }
        else {
            return Parse<T> (String{forward<STRINGISH_ARG> (s)}, options);
        }
    }
    template <floating_point T>
    Common::StdCompat::expected<T, DataExchange::BadFormatException> Parse (const String& s, String* remainder)
    {
        RequireNotNull (remainder);
        using ERRT   = Common::StdCompat::unexpected<DataExchange::BadFormatException>;
        String text  = s; // s may be *remainder - as in Parse<double> (t, &t) - so changed below
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
