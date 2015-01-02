/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#include    "../../../StroikaPreComp.h"

#include    "../../../Characters/Format.h"
#include    "../../../Execution/Exceptions.h"

#include    "Methods.h"

using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Characters;
using   namespace   Stroika::Foundation::IO;
using   namespace   Stroika::Foundation::IO::Network;
using   namespace   Stroika::Foundation::IO::Network::HTTP;



extern  const   wchar_t Methods::kGet[]         =   L"GET";
extern  const   wchar_t Methods::kPut[]         =   L"PUT";
extern  const   wchar_t Methods::kPost[]        =   L"POST";
extern  const   wchar_t Methods::kDelete[]      =   L"DELETE";
extern  const   wchar_t Methods::kOptions[]     =   L"OPTIONS";


