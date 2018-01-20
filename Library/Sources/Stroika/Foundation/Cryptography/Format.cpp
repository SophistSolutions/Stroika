/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include <cstdio>

#include "../Characters/CString/Utilities.h"

#include "Format.h"

using namespace Stroika::Foundation;

#if qCompilerAndStdLib_glibc_stdfunctionmapping_Buggy
namespace std {
    using ::snprintf;
}
#endif

string Cryptography::Private_::mkArrayFmt_ (const uint8_t* start, const uint8_t* end)
{
    string result;
    size_t N = end - start;
    result.reserve (2 * N);
    for (const uint8_t* i = start; i != end; ++i) {
        char b[10];
        b[0] = '\0';
        std::snprintf (b, NEltsOf (b), "%02x", *i);
        result += b;
    }
    return result;
}

string Cryptography::Private_::mkFmt_ (unsigned int n)
{
    char b[1024];
    b[0] = '\0';
    std::snprintf (b, NEltsOf (b), "0x%u", n);
    return b;
}

string Cryptography::Private_::mkFmt_ (unsigned long n)
{
    char b[1024];
    b[0] = '\0';
    std::snprintf (b, NEltsOf (b), "0x%lu", n);
    return b;
}

string Cryptography::Private_::mkFmt_ (unsigned long long n)
{
    char b[1024];
    b[0] = '\0';
    std::snprintf (b, NEltsOf (b), "0x%llu", n);
    return b;
}
