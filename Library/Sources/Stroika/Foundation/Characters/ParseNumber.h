/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_ParseNumber_h_
#define _Stroika_Foundation_Characters_ParseNumber_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <span>

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Common/StdCompat.h"
#include "Stroika/Foundation/DataExchange/BadFormatException.h"

/**
 *  Strict text-to-number conversion: ParseInteger and ParseFloat. Text that is not a number of the type asked for - or is out of
 *  its range - is an error, reported in the expected<> returned, never a default value (String2Int's 0, ToFloat's NaN or infinity).
 *
 *  \note A header of its own - not part of String2Int.h or FloatConversion.h - because DataExchange::BadFormatException's header
 *        reaches FloatConversion.h (through Execution/Exceptions.h, Format.h and ToString.h); nothing else in Characters includes it.
 */

namespace Stroika::Foundation::Characters {

    /**
     *  \brief How ParseInteger reads its text: by default strictly - just the number, in base 10
     *
     *  Each option makes it accept more; the default accepts the least, so a lenient read says so where it is called.
     *
     *  \par Example Usage
     *      \code
     *          ParseInteger<unsigned int> ("0x1A", {.fRadix = 16, .fAcceptRadixPrefix = true});     // 26
     *          ParseInteger<int> (" 42 ", {.fIgnoreSurroundingWhitespace = true});                  // 42
     *      \endcode
     *
     *  \note Not here yet - to add when a caller needs one: a digit separator or grouping (1'000, 1_000, 1,000); a remainder overload
     *        of ParseInteger, reading the number at the start of longer text (as ParseFloat's) - say "0 kB".
     */
    struct ParseIntegerOptions {
        /**
         *  2 to 36; for a radix over 10, letters for the digits past 9, in either case.
         */
        unsigned int fRadix{10};

        /**
         *  Also accept the radix's usual prefix, after any sign, in either case: 0x for 16, 0o for 8, 0b for 2 (other radixes have none) -
         *  as IntegerToStringOptions::fEmitRadixPrefix writes it (NumberFormat.h).
         *
         *  Off by default: the radix is the caller's, so a prefix could only repeat or contradict it, and text formats differ on which
         *  they use (0x, #, $, &H...). Even on, the radix is never guessed from the text, as C's strtol (base 0) guesses it - a leading
         *  0 meaning octal, so "010" is 8, is a known source of bugs; a leading 0 is always just a digit.
         */
        bool fAcceptRadixPrefix{false};

        /**
         *  Ignore whitespace before and after the number.
         */
        bool fIgnoreSurroundingWhitespace{false};
    };

    /**
     *  \brief The whole of s as an integer of type T: text that is not entirely such a number, or is out of T's range, is an error
     *
     *  s is just the number: an optional + or - sign, then digits in options.fRadix - no whitespace, no radix prefix, no digit
     *  separators, nothing after (unless options say otherwise). The same text parses the same in any locale (this uses
     *  std::from_chars).
     *
     *  The error - a DataExchange::BadFormatException, a runtime error: bad data, not a bad call - quotes s, and says whether it is
     *  not an integer or out of T's range. ThrowIfFailed () throws it; value_or (0) gives String2Int's lenient behavior instead.
     *
     *  \par Example Usage
     *      \code
     *          uint16_t port = ThrowIfFailed (ParseInteger<uint16_t> (portText));   // throws if portText is not a port number
     *          int      n    = ParseInteger ("33aaa"sv).value_or (0);                // 0 - not an integer
     *          if (auto p = ParseInteger<uint16_t> (portText)) {
     *              use (*p);
     *          }
     *          else {
     *              cerr << Characters::ToString (p.error ()) << endl;               // "70000" is out of range (0 to 65535)
     *          }
     *      \endcode
     *
     *  \pre 2 <= options.fRadix and options.fRadix <= 36
     */
    template <integral T = int, IUNICODECanUnambiguouslyConvertFrom CHAR_T>
    Common::StdCompat::expected<T, DataExchange::BadFormatException> ParseInteger (span<const CHAR_T> s, const ParseIntegerOptions& options = {});
    template <integral T = int, IConvertibleToString STRINGISH_ARG>
    Common::StdCompat::expected<T, DataExchange::BadFormatException> ParseInteger (STRINGISH_ARG&& s, const ParseIntegerOptions& options = {});

    /**
     *  \brief How ParseFloat reads its text: by default strictly - just the number
     *
     *  \note Not here yet - to add when a caller needs one: a locale (a decimal comma); a digit separator or grouping (1,000.5);
     *        refusing nan and inf as text (accepted now, as ToFloat accepted them).
     */
    struct ParseFloatOptions {
        /**
         *  Ignore whitespace before and after the number.
         */
        bool fIgnoreSurroundingWhitespace{false};
    };

    /**
     *  \brief The whole of s as a floating-point number of type T: text that is not entirely a number, or is too large for T, is an
     *  error
     *
     *  s is just the number, as ToFloat reads it - an optional sign, digits with an optional decimal point and exponent - or nan, inf
     *  or infinity (in any case, with an optional sign), which give NaN or infinity as such. No whitespace, nothing after (unless
     *  options say otherwise).
     *
     *  The error, as ParseInteger's: "is not a number" or "is out of range". value_or (NAN) gives ToFloat's lenient behavior for
     *  text that is not a number.
     *
     *  The overload with a remainder reads the number at the start of s (after any whitespace) and puts what follows it in
     *  *remainder - for a number within longer text; its error is that s "does not start with a number". s may be *remainder, to
     *  read one number after another.
     *
     *  \par Example Usage
     *      \code
     *          double d = ThrowIfFailed (ParseFloat (text));
     *          String rest;
     *          double e = ThrowIfFailed (ParseFloat ("3.5 apples"_k, &rest));     // 3.5, and rest is " apples"
     *      \endcode
     *
     *  \pre remainder != nullptr
     */
    template <floating_point T = double, IUNICODECanUnambiguouslyConvertFrom CHAR_T>
    Common::StdCompat::expected<T, DataExchange::BadFormatException> ParseFloat (span<const CHAR_T> s, const ParseFloatOptions& options = {});
    template <floating_point T = double, typename STRINGISH_ARG>
    Common::StdCompat::expected<T, DataExchange::BadFormatException> ParseFloat (STRINGISH_ARG&& s, const ParseFloatOptions& options = {})
        requires (IConvertibleToString<STRINGISH_ARG> or is_convertible_v<STRINGISH_ARG, std::string>);
    template <floating_point T = double>
    Common::StdCompat::expected<T, DataExchange::BadFormatException> ParseFloat (const String& s, String* remainder);

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "ParseNumber.inl"

#endif /*_Stroika_Foundation_Characters_ParseNumber_h_*/
