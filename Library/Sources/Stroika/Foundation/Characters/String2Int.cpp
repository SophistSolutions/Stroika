/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include <cstdlib>

#include "../Containers/Common.h"
#include "../Debug/Assertions.h"
#include "../Debug/Trace.h"
#include "../Math/Common.h"

#include "String2Int.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;

/*
 ********************************************************************************
 ****************************** HexString2Int ***********************************
 ********************************************************************************
 */
unsigned int Characters::HexString2Int (const String& s)
{
    Memory::StackBuffer<wchar_t> sPossibleBackingStore;
    unsigned long                l = wcstoul (get<0> (s.c_str (&sPossibleBackingStore)), nullptr, 16);
    if (l >= numeric_limits<unsigned int>::max ()) {
        return numeric_limits<unsigned int>::max ();
    }
    return l;
}

/*
 ********************************************************************************
 ********************************* String2Int ***********************************
 ********************************************************************************
 */
long long int Characters::Private_::String2Int_ (const String& s)
{
    Memory::StackBuffer<wchar_t> possibleBackingStore;
    wchar_t*                     endptr = nullptr;
    unsigned long long int       l      = wcstoll (get<0> (s.c_str (&possibleBackingStore)), &endptr, 10);
    return *endptr == '\0' ? l : 0; // weird but I document default is zero if failed to fully parse
}

/*
 ********************************************************************************
 ******************************** String2UInt ***********************************
 ********************************************************************************
 */
unsigned long long int Characters::Private_::String2UInt_ (const String& s)
{
    Memory::StackBuffer<wchar_t> possibleBackingStore;
    wchar_t*                     endptr = nullptr;
    long long int                l      = wcstoull (get<0> (s.c_str (&possibleBackingStore)), &endptr, 10);
    return *endptr == '\0' ? l : 0; // weird but I document default is zero if failed to fully parse
}
