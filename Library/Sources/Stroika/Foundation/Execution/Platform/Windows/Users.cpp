/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include <Windows.h>

#include "Exception.h"

#include "Users.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Execution;
using namespace Stroika::Foundation::Execution::Platform::Windows;

using Characters::SDKChar;
using Characters::String;

/*
 ********************************************************************************
 *********************** Platform::Windows::SID22UserName ***********************
 ********************************************************************************
 */
String Platform::Windows::SID22UserName (PSID sid)
{
    SID_NAME_USE iUse{};
    SDKChar      name[1024];
    SDKChar      domain[1024];
    DWORD        nameLen{static_cast<DWORD> (Memory::NEltsOf (name))};
    DWORD        domainLen{static_cast<DWORD> (Memory::NEltsOf (domain))};
    ThrowIfZeroGetLastError (::LookupAccountSid (nullptr, sid, name, &nameLen, domain, &domainLen, &iUse));
    if (domainLen == 0) {
        return String::FromSDKString (name);
    }
    else {
        return String::FromSDKString (name) + "@"sv + String::FromSDKString (domain);
    }
}
