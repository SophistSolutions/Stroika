/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    "Tokenize.h"




using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Characters;
using   namespace   Stroika::Foundation::Memory;






template    <>
vector<String> Characters::Tokenize (const String& str, const String& delimiters)
{
    vector<wstring>     tmp =   Tokenize<wstring> (str.As<wstring> (), delimiters.As<wstring> ());
    vector<String>      r;
    r.reserve (tmp.size ());
    for (const wstring& w : tmp) {
        r.push_back (String (w));
    }
    return r;
}
