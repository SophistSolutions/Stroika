/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    "../Characters/Format.h"

#include    "CheckedConverter.h"



using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Characters;
using   namespace   Stroika::Foundation::DataExchangeFormat;



template    <>
Characters::String  DataExchangeFormat::CheckedConverter<string, Characters::String, UTF8> (const string& from, const UTF8& extraData)
{
    // @todo no chekcing done yet...
    return String::FromUTF8 (from);
}

template    <>
string  DataExchangeFormat::CheckedConverter<Characters::String, string, ASCII> (const Characters::String& from, const ASCII& extraData)
{
    // @todo no chekcing done yet...
    return from.AsASCII ();
}
