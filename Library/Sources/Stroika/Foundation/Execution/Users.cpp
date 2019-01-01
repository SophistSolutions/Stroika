/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#include "../StroikaPreComp.h"

#if qPlatform_Windows
#include <Windows.h>
#endif

#include "../Characters/SDKChar.h"
#if qPlatform_Windows
#include "../Execution/Platform/Windows/Exception.h"
#endif
#include "../Memory/SmallStackBuffer.h"

#if qPlatform_POSIX
#include "Platform/POSIX/Users.h"
#endif

#include "Users.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Execution;

using Characters::String;
using Memory::SmallStackBuffer;
using Memory::SmallStackBufferCommon;

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
    SmallStackBuffer<Characters::SDKChar> buf (SmallStackBufferCommon::eUninitialized, sz + 1);
    Execution::ThrowIfFalseGetLastError (::GetUserNameEx (useFormat, buf, &sz));
    return String::FromSDKString (buf);
#elif qPlatform_Windows
    ULONG sz = 0;
    ::GetUserName (nullptr, &sz);
    SmallStackBuffer<Characters::SDKChar> buf (SmallStackBufferCommon::eUninitialized, sz + 1);
    Execution::Platform::Windows::ThrowIfFalseGetLastError (::GetUserName (buf, &sz));
    return String::FromSDKString (buf);
#elif qPlatform_POSIX
    return Platform::POSIX::uid_t2UserName (Platform::POSIX::GetUID ());
#else
#endif
}
