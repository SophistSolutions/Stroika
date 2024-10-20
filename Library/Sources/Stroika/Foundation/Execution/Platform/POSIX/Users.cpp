/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include <unistd.h>

#include "Stroika/Foundation/Execution/Exceptions.h"
#include "Stroika/Foundation/Memory/StackBuffer.h"

#include "Users.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Execution;
using namespace Stroika::Foundation::Execution::Platform::POSIX;

using Characters::String;
using Memory::StackBuffer;

/*
 ********************************************************************************
 ************************ Platform::POSIX::UserName2UID *************************
 ********************************************************************************
 */
uid_t Platform::POSIX::UserName2UID (const String& name)
{
    size_t bufsize = ::sysconf (_SC_GETPW_R_SIZE_MAX);
    if (bufsize == -1) { /* Value was indeterminate */
        bufsize = 16384; /* Should be more than enough */
    }
    StackBuffer<char> buf{Memory::eUninitialized, bufsize};

    struct passwd  pwd {};
    struct passwd* result = nullptr;
    int            err    = ::getpwnam_r (name.AsNarrowSDKString ().c_str (), &pwd, buf.data (), bufsize, &result);
    if (err < 0) [[unlikely]] {
        ThrowPOSIXErrNo (err);
    }
    if (result == nullptr) [[unlikely]] {
        Execution::Throw (RuntimeErrorException{"No such username"sv});
    }
    return pwd.pw_uid;
}

/*
 ********************************************************************************
 ********************** Platform::POSIX::uid_t2UserName *************************
 ********************************************************************************
 */
String Platform::POSIX::uid_t2UserName (uid_t uid)
{
    size_t bufsize = ::sysconf (_SC_GETPW_R_SIZE_MAX);
    if (bufsize == -1) { /* Value was indeterminate */
        bufsize = 16384; /* Should be more than enough */
    }
    StackBuffer<char> buf{Memory::eUninitialized, bufsize};

    struct passwd  pwd {};
    struct passwd* result = nullptr;
    int            err    = ::getpwuid_r (uid, &pwd, buf.data (), bufsize, &result);
    if (err < 0) {
        ThrowPOSIXErrNo (err);
    }
    if (result == nullptr) {
        static const auto kException_ = RuntimeErrorException{"No such username"sv};
        Execution::Throw (kException_);
    }
    return String::FromSDKString (pwd.pw_name);
}

/*
 ********************************************************************************
 ****************************** Platform::POSIX::GetUID *************************
 ********************************************************************************
 */
uid_t Platform::POSIX::GetUID ()
{
    return ::getuid ();
}

/*
 ********************************************************************************
 ********************* Platform::POSIX::GetEffectiveUID *************************
 ********************************************************************************
 */
uid_t Platform::POSIX::GetEffectiveUID ()
{
    return ::geteuid ();
}
