/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#include "../../../StroikaPreComp.h"

#include <unistd.h>

#include "../../../Characters/String_Constant.h"
#include "../../../Memory/SmallStackBuffer.h"
#include "../../Exceptions.h"

#include "Users.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Execution;
using namespace Stroika::Foundation::Execution::Platform::POSIX;

using Characters::String;
using Characters::String_Constant;
using Memory::SmallStackBuffer;
using Memory::SmallStackBufferCommon;

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
    SmallStackBuffer<char> buf (SmallStackBufferCommon::eUninitialized, bufsize);

    struct passwd pwd {
    };
    struct passwd* result = nullptr;
    int            err    = getpwnam_r (name.AsNarrowSDKString ().c_str (), &pwd, buf, bufsize, &result);
    if (err < 0)
        [[UNLIKELY_ATTR]]
        {
            ThrowPOSIXErrNo (err);
        }
    if (result == nullptr)
        [[UNLIKELY_ATTR]]
        {
            Execution::Throw (Exception (L"No such username"sv));
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
    SmallStackBuffer<char> buf (SmallStackBufferCommon::eUninitialized, bufsize);

    struct passwd pwd {
    };
    struct passwd* result = nullptr;
    int            err    = ::getpwuid_r (uid, &pwd, buf, bufsize, &result);
    if (err < 0) {
        ThrowPOSIXErrNo (err);
    }
    if (result == nullptr) {
        Execution::Throw (Exception (L"No such username"sv));
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
