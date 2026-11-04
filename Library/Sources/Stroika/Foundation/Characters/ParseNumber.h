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
     *  \brief The whole of s as an integer of type T, in the given radix: text that is not entirely such a number, or is out of T's
     *  range, is an error
     *
     *  s is just the number: an optional + or - sign, then digits (for a radix over 10, letters for the digits past 9, either case) -
     *  no whitespace, no 0x prefix, nothing after. The same text parses the same in any locale (this uses std::from_chars).
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
     *  \pre 2 <= radix and radix <= 36
     */
    template <integral T = int, IUNICODECanUnambiguouslyConvertFrom CHAR_T>
    Common::StdCompat::expected<T, DataExchange::BadFormatException> ParseInteger (span<const CHAR_T> s, unsigned int radix = 10);
    template <integral T = int, IConvertibleToString STRINGISH_ARG>
    Common::StdCompat::expected<T, DataExchange::BadFormatException> ParseInteger (STRINGISH_ARG&& s, unsigned int radix = 10);

    /**
     *  \brief The whole of s as a floating-point number of type T: text that is not entirely a number, or is too large for T, is an
     *  error
     *
     *  s is just the number, as ToFloat reads it - an optional sign, digits with an optional decimal point and exponent - or nan, inf
     *  or infinity (in any case, with an optional sign), which give NaN or infinity as such. No whitespace, nothing after.
     *
     *  The error, as ParseInteger's: "is not a number" or "is out of range". value_or (NAN) gives ToFloat's lenient behavior for
     *  text that is not a number.
     *
     *  The overload with a remainder reads the number at the start of s (after any whitespace) and puts what follows it in
     *  *remainder - for a number within longer text; its error is that s "does not start with a number".
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
    Common::StdCompat::expected<T, DataExchange::BadFormatException> ParseFloat (span<const CHAR_T> s);
    template <floating_point T = double, typename STRINGISH_ARG>
    Common::StdCompat::expected<T, DataExchange::BadFormatException> ParseFloat (STRINGISH_ARG&& s)
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
