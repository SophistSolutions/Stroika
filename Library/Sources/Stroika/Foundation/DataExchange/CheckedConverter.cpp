/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    "../Characters/Format.h"

#include    "CheckedConverter.h"



using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Characters;
using   namespace   Stroika::Foundation::DataExchange;



template    <>
Characters::String  DataExchange::CheckedConverter<Characters::String, UTF8, string> (const string& from, const UTF8& extraData)
{
    // @todo no chekcing done yet...
    return String::FromUTF8 (from);
}

template    <>
Characters::String  DataExchange::CheckedConverter<Characters::String, ASCII, string> (const string& from, const ASCII& extraData)
{
    for (auto i = from.begin (); i != from.end (); ++i) {
        if (not isascii (*i)) {
            Execution::DoThrow (BadFormatException ());
		}
    }
    return ASCIIStringToWide (from);
}

template    <>
string  DataExchange::CheckedConverter<string, ASCII, Characters::String> (const Characters::String& from, const ASCII& extraData)
{
    // @todo no chekcing done yet...
    return from.AsASCII ();
}
