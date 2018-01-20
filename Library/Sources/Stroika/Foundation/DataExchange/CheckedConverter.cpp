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
Characters::String DataExchange::CheckedConverter<Characters::String, UTF8, const string&> (const string& from, const UTF8& extraData)
{
    // @todo no chekcing done yet...
    return String::FromUTF8 (from);
}

template <>
Characters::String DataExchange::CheckedConverter<Characters::String, UTF8, string> (string from, const UTF8& extraData)
{
    // @todo no chekcing done yet...
    return String::FromUTF8 (from);
}

template <>
Characters::String DataExchange::CheckedConverter<Characters::String, UTF8, const char*> (const char* from, const UTF8& extraData)
{
    // @todo no chekcing done yet...
    return String::FromUTF8 (from);
}

template <>
Characters::String DataExchange::CheckedConverter<Characters::String, UTF8, char*> (char* from, const UTF8& extraData)
{
    // @todo no chekcing done yet...
    return String::FromUTF8 (from);
}

template <>
Characters::String DataExchange::CheckedConverter<Characters::String, ASCII, const string&> (const string& from, const ASCII& extraData)
{
    for (auto i = from.begin (); i != from.end (); ++i) {
        if (not isascii (*i)) {
            Execution::Throw (BadFormatException (String_Constant (L"Cannot coerce string to ASCII")));
        }
    }
    return ASCIIStringToWide (from);
}

template <>
Characters::String DataExchange::CheckedConverter<Characters::String, ASCII, string> (string from, const ASCII& extraData)
{
    return CheckedConverter<Characters::String, ASCII, const string&> (from, extraData);
}

template <>
Characters::String DataExchange::CheckedConverter<Characters::String, ASCII, const char*> (const char* from, const ASCII& extraData)
{
    RequireNotNull (from);
    for (auto i = from; *i != '\0'; ++i) {
        if (not isascii (*i)) {
            Execution::Throw (BadFormatException (String_Constant (L"Cannot coerce string to ASCII")));
        }
    }
    return ASCIIStringToWide (from);
}

template <>
Characters::String DataExchange::CheckedConverter<Characters::String, ASCII, char*> (char* from, const ASCII& extraData)
{
    RequireNotNull (from);
    for (auto i = from; *i != '\0'; ++i) {
        if (not isascii (*i)) {
            Execution::Throw (BadFormatException (String_Constant (L"Cannot coerce string to ASCII")));
        }
    }
    return ASCIIStringToWide (from);
}

template <>
string DataExchange::CheckedConverter<string, ASCII, const Characters::String&> (const Characters::String& from, const ASCII& extraData)
{
    // @todo no chekcing done yet...
    return from.AsASCII ();
}
