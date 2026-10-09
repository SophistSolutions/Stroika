/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include <cstdio>

#include "Stroika/Foundation/Characters/CString/Utilities.h"
#include "Stroika/Foundation/DataExchange/Encoding/Hex.h"

#include "Format.h"

using namespace Stroika::Foundation;

string Cryptography::Private_::mkArrayFmt_ (const uint8_t* start, const uint8_t* end)
{
    return DataExchange::Encoding::Hex::Encode (as_bytes (span{start, end}));
}

string Cryptography::Private_::mkFmt_ (unsigned int n)
{
    char b[1024];
    b[0] = '\0';
    ::snprintf (b, size (b), "0x%u", n);
    return b;
}

string Cryptography::Private_::mkFmt_ (unsigned long n)
{
    char b[1024];
    b[0] = '\0';
    ::snprintf (b, size (b), "0x%lu", n);
    return b;
}

string Cryptography::Private_::mkFmt_ (unsigned long long n)
{
    char b[1024];
    b[0] = '\0';
    ::snprintf (b, size (b), "0x%llu", n);
    return b;
}
