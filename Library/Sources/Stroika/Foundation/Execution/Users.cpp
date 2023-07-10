/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#include "../StroikaPreComp.h"

#if qPlatform_Windows
#include <Windows.h>
#endif

#include "../Characters/SDKChar.h"
#if qPlatform_Windows
#include "../Execution/Platform/Windows/Exception.h"
#endif
#include "../Memory/StackBuffer.h"

#if qPlatform_POSIX
#include "Platform/POSIX/Users.h"
#endif

#include "Users.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Execution;

using Characters::String;
using Memory::StackBuffer;

/*
 ********************************************************************************
 ************************ Execution::GetCurrentUserName *************************
 ********************************************************************************
 */
String Execution::GetCurrentUserName ([[maybe_unused]] UserNameFormat format)
{
#if qPlatform_Windows && 0
    EXTENDED_NAME_FORMAT useFormat = NameDisplay;
    ULONG                sz        = 0;
    ::GetUserNameEx (useFormat, nullptr, &sz);
    StackBuffer<Characters::SDKChar> buf{Memory::eUninitialized, sz + 1};
    Execution::ThrowIfZeroGetLastError (::GetUserNameEx (useFormat, buf, &sz));
    return String::FromSDKString (buf);
#elif qPlatform_Windows
    ULONG sz = 0;
    ::GetUserName (nullptr, &sz);
    StackBuffer<Characters::SDKChar> buf{Memory::eUninitialized, sz + 1};
    Execution::Platform::Windows::ThrowIfZeroGetLastError (::GetUserName (buf.data (), &sz));
    return String::FromSDKString (buf);
#elif qPlatform_POSIX
    return Platform::POSIX::uid_t2UserName (Platform::POSIX::GetUID ());
#else
#endif
}
