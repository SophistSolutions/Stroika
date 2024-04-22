/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */

namespace Stroika::Foundation::Execution {

    /*
     ********************************************************************************
     ***************************** GetCurrentProcessID ******************************
     ********************************************************************************
     */
    inline pid_t GetCurrentProcessID ()
    {
#if qPlatform_Windows
        return GetCurrentProcessId ();
#elif qPlatform_POSIX
        return getpid ();
#else
        AssertNotImplemented ();
#endif
    }

}
