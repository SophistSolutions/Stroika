/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    <cstdlib>

#include    "../Containers/Common.h"
#include    "../Debug/Assertions.h"
#include    "../Debug/Trace.h"
#include    "../Math/Common.h"
#include    "CodePage.h"

#include    "String2Int.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Characters;






/*
 ********************************************************************************
 ****************************** HexString2Int ***********************************
 ********************************************************************************
 */
unsigned int Characters::HexString2Int (const String& s)
{
    using   std::numeric_limits;
    unsigned    long    l   =   wcstoul (s.c_str (), nullptr, 16);
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
    using   std::numeric_limits;
    unsigned    long long int    l   =   wcstoll (s.c_str (), nullptr, 10);
    return l;
}




/*
 ********************************************************************************
 ******************************** String2UInt ***********************************
 ********************************************************************************
 */
unsigned long long int Characters::Private_::String2UInt_ (const String& s)
{
    using   std::numeric_limits;
    long long int    l   =   wcstoull (s.c_str (), nullptr, 10);
    return l;
}
