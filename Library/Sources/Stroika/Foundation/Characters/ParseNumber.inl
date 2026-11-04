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
    Common::StdCompat::expected<T, DataExchange::BadFormatException> ParseInteger (span<const CHAR_T> s, unsigned int radix)
    {
        Require (2 <= radix and radix <= 36);
        using ERRT = Common::StdCompat::unexpected<DataExchange::BadFormatException>;
        Memory::StackBuffer<char> ascii;
        if (Character::AsASCIIQuietly (s, &ascii)) {
            const char* b                = ascii.begin ();
            const char* e                = ascii.end ();
            bool        signSkipped      = false;
            bool        negativeUnsigned = false;
            if (b != e and *b == '+') {
                signSkipped = true; // from_chars takes no +
                ++b;
            }
            else if (b != e and *b == '-' and unsigned_integral<T>) {
                signSkipped = negativeUnsigned = true; // nor - for an unsigned type
                ++b;
            }
            if (b != e and not(signSkipped and (*b == '+' or *b == '-'))) { // a sign at most once (from_chars takes a signed type's -)
                T r{};
                auto [ptr, ec] = from_chars (b, e, r, static_cast<int> (radix));
                if (ptr == e and ec == errc{} and (not negativeUnsigned or r == 0)) {
                    return r;
                }
                if (ptr == e and (ec == errc{} or ec == errc::result_out_of_range)) {
                    return ERRT{DataExchange::BadFormatException{Private_::QuoteForParseError_ (s) + " is out of range ("sv +
                                                                 String{to_string (+numeric_limits<T>::min ())} + " to "sv +
                                                                 String{to_string (+numeric_limits<T>::max ())} + ")"sv}};
                }
            }
        }
        return ERRT{DataExchange::BadFormatException{
            Private_::QuoteForParseError_ (s) +
            (radix == 10 ? String{" is not an integer"sv} : " is not a base-"sv + String{to_string (radix)} + " integer"sv)}};
    }
    template <integral T, IConvertibleToString STRINGISH_ARG>
    inline Common::StdCompat::expected<T, DataExchange::BadFormatException> ParseInteger (STRINGISH_ARG&& s, unsigned int radix)
    {
        using DecayedStringishArg = remove_cvref_t<STRINGISH_ARG>;
        if constexpr (is_convertible_v<DecayedStringishArg, string_view>) {
            string_view sv = s; // char text: not ASCII is an error, not a throw from String's constructor
            return ParseInteger<T> (span{sv.data (), sv.size ()}, radix);
        }
        else if constexpr (Common::IAnyOf<DecayedStringishArg, const char8_t*, const char16_t*, const char32_t*, const wchar_t*>) {
            return ParseInteger<T> (span{s, CString::Length (s)}, radix);
        }
        else if constexpr (same_as<DecayedStringishArg, String>) {
            Memory::StackBuffer<wchar_t> ignored;
            return ParseInteger<T> (s.template GetData<wchar_t> (&ignored), radix);
        }
        else {
            return ParseInteger<T> (String{forward<STRINGISH_ARG> (s)}, radix);
        }
    }

    /*
     ********************************************************************************
     ********************************** ParseFloat **********************************
     ********************************************************************************
     */
    template <floating_point T, IUNICODECanUnambiguouslyConvertFrom CHAR_T>
    Common::StdCompat::expected<T, DataExchange::BadFormatException> ParseFloat (span<const CHAR_T> s)
    {
        using ERRT = Common::StdCompat::unexpected<DataExchange::BadFormatException>;
        if constexpr (same_as<remove_cv_t<CHAR_T>, char>) {
            if (not Character::IsASCII (s)) { // ToFloat requires ASCII char text
                return ERRT{DataExchange::BadFormatException{Private_::QuoteForParseError_ (s) + " is not a number"sv}};
            }
        }
        T r = ToFloat<T> (s); // NaN if not a number, infinity if too large
        if (isnan (r) and not Private_::IsNamedFloat_ (String{s}, "nan"sv)) {
            return ERRT{DataExchange::BadFormatException{Private_::QuoteForParseError_ (s) + " is not a number"sv}};
        }
        if (isinf (r) and not Private_::IsNamedFloat_ (String{s}, "inf"sv) and not Private_::IsNamedFloat_ (String{s}, "infinity"sv)) {
            return ERRT{DataExchange::BadFormatException{Private_::QuoteForParseError_ (s) + " is out of range"sv}};
        }
        return r;
    }
    template <floating_point T, typename STRINGISH_ARG>
    inline Common::StdCompat::expected<T, DataExchange::BadFormatException> ParseFloat (STRINGISH_ARG&& s)
        requires (IConvertibleToString<STRINGISH_ARG> or is_convertible_v<STRINGISH_ARG, std::string>)
    {
        using DecayedStringishArg = remove_cvref_t<STRINGISH_ARG>;
        if constexpr (is_convertible_v<DecayedStringishArg, string_view>) {
            string_view sv = s; // char text: not ASCII is an error, not a throw from String's constructor
            return ParseFloat<T> (span{sv.data (), sv.size ()});
        }
        else if constexpr (Common::IAnyOf<DecayedStringishArg, const char8_t*, const char16_t*, const char32_t*, const wchar_t*>) {
            return ParseFloat<T> (span{s, CString::Length (s)});
        }
        else if constexpr (same_as<DecayedStringishArg, String>) {
            Memory::StackBuffer<wchar_t> ignored;
            return ParseFloat<T> (s.template GetData<wchar_t> (&ignored));
        }
        else {
            return ParseFloat<T> (String{forward<STRINGISH_ARG> (s)});
        }
    }
    template <floating_point T>
    Common::StdCompat::expected<T, DataExchange::BadFormatException> ParseFloat (const String& s, String* remainder)
    {
        RequireNotNull (remainder);
        using ERRT    = Common::StdCompat::unexpected<DataExchange::BadFormatException>;
        T      r      = ToFloat<T> (s, remainder); // skips leading whitespace; what follows the number in *remainder
        String number = s.SubString (0, s.size () - remainder->size ()).Trim ();
        if (number.empty () or (isnan (r) and not Private_::IsNamedFloat_ (number, "nan"sv))) {
            return ERRT{DataExchange::BadFormatException{"\""sv + s.LimitLength (50) + "\" does not start with a number"sv}};
        }
        if (isinf (r) and not Private_::IsNamedFloat_ (number, "inf"sv) and not Private_::IsNamedFloat_ (number, "infinity"sv)) {
            return ERRT{DataExchange::BadFormatException{"\""sv + number.LimitLength (50) + "\" is out of range"sv}};
        }
        return r;
    }

}
