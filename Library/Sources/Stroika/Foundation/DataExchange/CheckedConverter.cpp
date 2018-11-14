/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include "../Characters/Format.h"

#include "CheckedConverter.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::DataExchange;

template <>
String DataExchange::CheckedConverter<String, UTF8, string> (string from, [[maybe_unused]] const UTF8& extraData)
{
    // @todo no chekcing done yet...
    return String::FromUTF8 (from);
}

template <>
String DataExchange::CheckedConverter<String, UTF8, const char*> (const char* from, [[maybe_unused]] const UTF8& extraData)
{
    // @todo no checking done yet...
    return String::FromUTF8 (from);
}

template <>
String DataExchange::CheckedConverter<String, ASCII, string> (string from, [[maybe_unused]] const ASCII& extraData)
{
    for (auto i = from.begin (); i != from.end (); ++i) {
        if (not isascii (*i))
            [[UNLIKELY_ATTR]]
            {
                Execution::Throw (BadFormatException (String_Constant (L"Cannot coerce string to ASCII")));
            }
    }
    return ASCIIStringToWide (from);
}

template <>
String DataExchange::CheckedConverter<String, ASCII, const char*> (const char* from, [[maybe_unused]] const ASCII& extraData)
{
    RequireNotNull (from);
    for (auto i = from; *i != '\0'; ++i) {
        if (not isascii (*i))
            [[UNLIKELY_ATTR]]
            {
                Execution::Throw (BadFormatException (String_Constant (L"Cannot coerce string to ASCII")));
            }
    }
    return ASCIIStringToWide (from);
}

template <>
string DataExchange::CheckedConverter<string, ASCII, String> (String from, [[maybe_unused]] const ASCII& extraData)
{
    // @todo no chekcing done yet...
    return from.AsASCII ();
}
