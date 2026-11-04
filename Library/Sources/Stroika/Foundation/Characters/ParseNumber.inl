/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include <charconv>
#include <cmath>
#include <limits>
#include <string>
#include <string_view>

#include "Stroika/Foundation/Characters/CString/Utilities.h"
#include "Stroika/Foundation/Characters/FloatConversion.h"
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
        // nan, inf or infinity - in any case, with an optional sign - which ToFloat reads as NaN or infinity on purpose
        inline bool IsNamedFloat_ (const String& s, string_view name)
        {
            String t = (s.StartsWith ("+"sv) or s.StartsWith ("-"sv)) ? s.SubString (1) : s;
            return String::EqualsComparer{eCaseInsensitive}(t, String{name});
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
        if constexpr (same_as<remove_cv_t<CHAR_T>, char>) {
            if (not Character::IsASCII (s)) { // ToFloat requires ASCII char text
                return ERRT{DataExchange::BadFormatException{Private_::QuoteForParseError_ (s) + " is not a number"sv}};
            }
        }
        if (options.fIgnoreSurroundingWhitespace) {
            ParseFloatOptions o            = options;
            o.fIgnoreSurroundingWhitespace = false;
            String                       t = String{s}.Trim ();
            Memory::StackBuffer<wchar_t> buf;
            return ParseFloat<T> (t.GetData<wchar_t> (&buf), o);
        }
        T r = FloatConversion::Private_::ToFloat_<T> (s); // NaN if not a number, infinity if too large
        if (isnan (r) and not Private_::IsNamedFloat_ (String{s}, "nan"sv)) {
            return ERRT{DataExchange::BadFormatException{Private_::QuoteForParseError_ (s) + " is not a number"sv}};
        }
        if (isinf (r) and not Private_::IsNamedFloat_ (String{s}, "inf"sv) and not Private_::IsNamedFloat_ (String{s}, "infinity"sv)) {
            return ERRT{DataExchange::BadFormatException{Private_::QuoteForParseError_ (s) + " is out of range"sv}};
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
        using ERRT    = Common::StdCompat::unexpected<DataExchange::BadFormatException>;
        String text   = s; // s may be *remainder - as in ParseFloat (t, &t) - so changed by the call below
        T      r      = FloatConversion::Private_::ToFloat_<T> (text, remainder); // skips leading whitespace; what follows in *remainder
        String number = text.SubString (0, text.size () - remainder->size ()).Trim ();
        if (number.empty () or (isnan (r) and not Private_::IsNamedFloat_ (number, "nan"sv))) {
            return ERRT{DataExchange::BadFormatException{"\""sv + text.LimitLength (50) + "\" does not start with a number"sv}};
        }
        if (isinf (r) and not Private_::IsNamedFloat_ (number, "inf"sv) and not Private_::IsNamedFloat_ (number, "infinity"sv)) {
            return ERRT{DataExchange::BadFormatException{"\""sv + number.LimitLength (50) + "\" is out of range"sv}};
        }
        return r;
    }

}
