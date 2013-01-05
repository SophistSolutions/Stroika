/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#include    "../../../StroikaPreComp.h"

#include    "../../../Memory/SmallStackBuffer.h"
#include    "../../ErrNoException.h"

#include    "Users.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Execution;
using   namespace   Stroika::Foundation::Execution::Platform::POSIX;

using   Characters::String;



/*
 ********************************************************************************
 ************************ Platform::POSIX::UserName2UID *************************
 ********************************************************************************
 */
uid_t   Platform::POSIX::UserName2UID (const String& name)
{
    size_t  bufsize = sysconf (_SC_GETPW_R_SIZE_MAX);
    if (bufsize == -1) {          /* Value was indeterminate */
        bufsize = 16384;        /* Should be more than enough */
    }
    Memory::SmallStackBuffer<char>  buf (bufsize);

    struct passwd pwd;
    memset (&pwd, 0, sizeof (pwd));
    struct passwd* result   =   nullptr;
    int err = getpwnam_r (name.AsTString ().c_str (), &pwd, buf, bufsize, &result);
    if (err < 0) {
        errno_ErrorException::DoThrow (err);
    }
    if (result == nullptr) {
        Execution::DoThrow (StringException (L"No such username"));
    }
    return pwd.pw_uid;
}



/*
 ********************************************************************************
 ********************** Platform::POSIX::uid_t2UserName *************************
 ********************************************************************************
 */
String  Platform::POSIX::uid_t2UserName (uid_t uid)
{
    size_t  bufsize = sysconf (_SC_GETPW_R_SIZE_MAX);
    if (bufsize == -1) {          /* Value was indeterminate */
        bufsize = 16384;        /* Should be more than enough */
    }
    Memory::SmallStackBuffer<char>  buf (bufsize);

    struct passwd pwd;
    memset (&pwd, 0, sizeof (pwd));
    struct passwd* result   =   nullptr;
    int err = getpwuid_r (uid, &pwd, buf, bufsize, &result);
    if (err < 0) {
        errno_ErrorException::DoThrow (err);
    }
    if (result == nullptr) {
        Execution::DoThrow (StringException (L"No such username"));
    }
    return String::FromTString (pwd.pw_name);
}



/*
 ********************************************************************************
 ****************************** Platform::POSIX::GetUID *************************
 ********************************************************************************
 */
uid_t   Platform::POSIX::GetUID ()
{
    return getuid ();
}


/*
 ********************************************************************************
 ********************* Platform::POSIX::GetEffectiveUID *************************
 ********************************************************************************
 */
uid_t   Platform::POSIX::GetEffectiveUID ()
{
    return geteuid ();
}

